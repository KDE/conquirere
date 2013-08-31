/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "notequery.h"

#include "../library.h"
#include "../projectsettings.h"

#include <Nepomuk2/Variant>

#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include <Nepomuk2/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

#include <QtCore/QtConcurrentRun>
#include <QtCore/QFutureWatcher>

#include <KDE/KDebug>

NoteQuery::NoteQuery(QObject *parent)
    : QueryClient(parent),
      m_newWatcher(0)
{
}

NoteQuery::~NoteQuery()
{
    m_newWatcher->stop();
    delete m_newWatcher;
}

void NoteQuery::stopFetchData()
{
    if(m_newWatcher) {
        m_newWatcher->stop();
        delete m_newWatcher;
        m_newWatcher = 0;
    }

    if(m_resourceWatcher) {
        m_resourceWatcher->stop();
        delete m_resourceWatcher;
        m_resourceWatcher = 0;
    }
}

void NoteQuery::startFetchData()
{
    stopFetchData();

    // keep track of newly added resources
    m_newWatcher = new Nepomuk2::ResourceWatcher(this);
    m_newWatcher->addType(Nepomuk2::Vocabulary::PIMO::Note());

    //DEPRECATED: remove library dependency for queryclient subclasses
    if(m_library->libraryType() == BibGlobals::Library_Project) {
        m_newWatcher->addProperty(Soprano::Vocabulary::NAO::isRelated());
        connect(m_newWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
                this, SLOT(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)) );
    }

    connect(m_newWatcher, SIGNAL(resourceCreated(Nepomuk2::Resource,QList<QUrl>)),
            this, SLOT(resourceCreated(Nepomuk2::Resource,QList<QUrl>)) );

    m_newWatcher->start();

    // create the resource watcher that will keep track of changes in the existing data
    m_resourceWatcher = new Nepomuk2::ResourceWatcher(this);

    connect(m_resourceWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)) );

    connect(m_resourceWatcher, SIGNAL(resourceTypeAdded(Nepomuk2::Resource,Nepomuk2::Types::Class)),
            this, SLOT(resourceTypeChanged(Nepomuk2::Resource,Nepomuk2::Types::Class)) );

    connect(m_resourceWatcher, SIGNAL(resourceTypeRemoved(Nepomuk2::Resource,Nepomuk2::Types::Class)),
            this, SLOT(resourceTypeChanged(Nepomuk2::Resource,Nepomuk2::Types::Class)) );

    connect(m_resourceWatcher, SIGNAL(resourceRemoved(QUrl,QList<QUrl>)),
            this, SLOT(resourceRemoved(QUrl,QList<QUrl>)) );

    QFuture<QList<CachedRowEntry> > future = QtConcurrent::run(this, &NoteQuery::initialQueryNepomuk);
    QFutureWatcher<QList<CachedRowEntry> > *futureWatcher = new QFutureWatcher<QList<CachedRowEntry> >();

    futureWatcher->setFuture(future);
    connect(futureWatcher, SIGNAL(finished()),this, SLOT(finishedInitialQuery()));
}

QList<CachedRowEntry> NoteQuery::initialQueryNepomuk()
{
    QTime startTime = QTime::currentTime();

    // helping string to filter for all documents that are related to the current project
    QString projectRelated;
    QString projectTag;
    if(m_library->libraryType() == BibGlobals::Library_Project) {
        projectRelated = QString("?r nao:isRelated  <%1> .").arg(m_library->settings()->projectThing().uri().toString());
        projectTag = QString("UNION { ?r nao:hasTag  <%1> . }").arg(m_library->settings()->projectTag().uri().toString() );
    }

    // first fetch all series
    // this will lead to duplicates as we fetch for seriesOf publication names and types too
    // for each rdf:type and each connected publicaion (seriesOf) we get the resource as result
    QString query = QString::fromLatin1("select distinct ?r ?title ?star ?date ?tags where { {"
                                        "?r a pimo:Note . "

                                        "OPTIONAL { ?r nao:prefLabel ?reviewed . }" //FIXME: add reviewed to query, implement it first. tagging?

                                        "OPTIONAL { ?r nie:title ?title . }"
                                        "OPTIONAL { ?r nao:numericRating ?star . }"
                                        "OPTIONAL { ?r nao:lastModified ?date . }"
                                        "OPTIONAL { ?r nao:hasTag ?t . }"
                                        "OPTIONAL { ?t nao:prefLabel ?tag . }"
                                        + projectRelated.toLatin1() + " }" + projectTag.toLatin1() +

                                        "}");

    QList<CachedRowEntry> newCache = processQueryResults(query);

    QTime endTime = QTime::currentTime();
    kDebug() << "add" << newCache.size() << "entries after" << startTime.msecsTo(endTime) << "msec";

    return newCache;
}

void NoteQuery::updateCacheEntry(const QUrl &uri, const QueryClient::UpdateType &updateType)
{
    QFuture<QList<CachedRowEntry> > future = QtConcurrent::run(this, &NoteQuery::updateQueryNepomuk, uri);
    QFutureWatcher<QList<CachedRowEntry> > *updateQueryWatcher = new QFutureWatcher<QList<CachedRowEntry> >();

    updateQueryWatcher->setFuture(future);

    if(updateType == UPDATE_RESOURCE_DATA) {
        connect(updateQueryWatcher, SIGNAL(finished()),this, SLOT(finishedUpdateQuery()));
    }
    else {
        connect(updateQueryWatcher, SIGNAL(finished()),this, SLOT(finishedNewResourceQuery()));
    }
}

QList<CachedRowEntry> NoteQuery::updateQueryNepomuk(const QUrl &uri)
{
    QTime startTime = QTime::currentTime();

    // first fetch all publications
    // this will lead to duplicates as we fetch for author names and types too
    // for each rdf:type and each connected author/publisher/editor we get the resource as result

    //If you update this, also update initialQueryNepomuk and processQueryResults and the enum ColumnList and the NoteModel
    QString query = QString::fromLatin1("select distinct ?title ?star ?date ?tags where {"
                                        "OPTIONAL { %1 nao:prefLabel ?reviewed . }" //FIXME: add reviewed to query, implement it first. tagging?

                                        "OPTIONAL { %1 nie:title ?title . }"
                                        "OPTIONAL { %1 nao:numericRating ?star . }"
                                        "OPTIONAL { %1 nao:lastModified ?date . }"
                                        "OPTIONAL { %1 nao:hasTag ?t . }"
                                        "OPTIONAL { ?t nao:prefLabel ?tag . }"
                                        "}").arg( Soprano::Node::resourceToN3( uri ) );

    QList<CachedRowEntry> newCache = processQueryResults(query, uri);

    QTime endTime = QTime::currentTime();
    kDebug() << "update" << newCache.size() << "entries after" << startTime.msecsTo(endTime) << "msec";

    return newCache;
}

QList<CachedRowEntry> NoteQuery::processQueryResults(const QString &query, const QUrl &uri)
{
    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    // combine all search results again, so we get a just a single resource with a list of all authors and the list of types
    // instead of many resources with all types again
    QMap<QString, QStringList> resultList;
    while( it.next() ) {
        Soprano::BindingSet p = it.current();

        // get either a new entry or what we have inserted beforehand
        QStringList curEntry = resultList.value( p.value("r").toString(), QStringList());

        // now set / add each queried value into the StringList
        if (curEntry.isEmpty() ) {

            curEntry << p.value("star").toString() << p.value("title").toString()
                     << p.value("date").toString() << p.value("tag").toString();
        }
        else {
            QString star = p.value("star").toString();
            if(!star.isEmpty())
                curEntry[Column_StarRate] = star;

            QString title = p.value("title").toString();
            if(!title.isEmpty())
                curEntry[Column_Title] = title;

            QString date = p.value("date").toString();
            if( !date.isEmpty() )
                curEntry[Column_Date] = date;

            QString tag = p.value("tag").toString();
            if(!tag.isEmpty() && !curEntry[Column_Tags] .contains(tag)) {
                curEntry[Column_Tags] = curEntry[Column_Tags].append("; ");
                curEntry[Column_Tags] = curEntry[Column_Tags].append(tag);
            }
        }

        // and save the result back into the map
        if(p.value("r").isEmpty()) {
            Q_ASSERT(!uri.isEmpty());
            resultList.insert(uri.toString(), curEntry);
        }
        else {
            resultList.insert(p.value("r").toString(), curEntry);
        }
    }

    // now create the cache entries from all returned search results
    QList<CachedRowEntry> newCache;
    QMapIterator<QString, QStringList> i(resultList);
    while (i.hasNext()) {
        i.next();

        CachedRowEntry cre;
        cre.uri = QUrl( i.key() );
        cre.displayColums = createDisplayData(i.value());
        cre.decorationColums = createDecorationData(i.value());
        cre.resource = Nepomuk2::Resource::fromResourceUri( cre.uri  );
        cre.timestamp = QDateTime::currentDateTime();
        cre.resourceType = 0; //UNUSED
        newCache.append(cre);
    }

    return newCache;
}

QVariantList NoteQuery::createDisplayData(const QStringList & item) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {

        case Column_Date: {

            QDateTime date = QDateTime::fromString( item.at(Column_Date), Qt::ISODate);
            if(date.isValid()) {
                newEntry = date.toString("dd.MM.yyyy");
            }

            break;
        }
        case Column_StarRate:
        case Column_Title:
        case Column_Tags:
            newEntry = item.at(i);
            break;
        default:
            newEntry = QVariant();
        }

        displayList.append(newEntry);
    }

    return displayList;
}

QVariantList NoteQuery::createDecorationData(const QStringList & item) const
{
    Q_UNUSED(item);

    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        decorationList.append(QVariant());
    }

    return decorationList;
}
