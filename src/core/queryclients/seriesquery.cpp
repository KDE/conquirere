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

#include "seriesquery.h"
#include "../library.h"
#include "../projectsettings.h"

#include "config/conquirere.h"
#include "config/bibglobals.h"

#include <Nepomuk2/Variant>
#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

#include <QtCore/QtConcurrentRun>
#include <QtCore/QFutureWatcher>

SeriesQuery::SeriesQuery(QObject *parent)
    : QueryClient(parent),
      m_newWatcher(0)
{
}

SeriesQuery::~SeriesQuery()
{
    m_newWatcher->stop();
    delete m_newWatcher;
}

void SeriesQuery::stopFetchData()
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

void SeriesQuery::startFetchData()
{
    stopFetchData();

    // keep track of newly added resources
    m_newWatcher = new Nepomuk2::ResourceWatcher(this);
    m_newWatcher->addType(Nepomuk2::Vocabulary::NBIB::Series());

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

    QFuture<QList<CachedRowEntry> > future = QtConcurrent::run(this, &SeriesQuery::initialQueryNepomuk);
    QFutureWatcher<QList<CachedRowEntry> > *futureWatcher = new QFutureWatcher<QList<CachedRowEntry> >();

    futureWatcher->setFuture(future);
    connect(futureWatcher, SIGNAL(finished()),this, SLOT(finishedInitialQuery()));
}

QList<CachedRowEntry> SeriesQuery::initialQueryNepomuk()
{
    QTime startTime = QTime::currentTime();

    QString hideTypes;
    // add a filter to hide several publication types
    foreach(int i, ConqSettings::hiddenNbibSeries()) {
        hideTypes.append(QString(" FILTER NOT EXISTS { ?r a <%1> . } ").arg(BibGlobals::SeriesTypeURL((BibGlobals::SeriesType)i).toString()));
    }

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
    QString query = QString::fromLatin1("select distinct ?r ?title ?star ?type ?content where { {"
                                        " { ?r a nbib:Series . "  + hideTypes.toLatin1() + " }"

                                        "OPTIONAL { ?r nao:prefLabel ?reviewed . }" //FIXME: add reviewed to query, implement it first. tagging?

                                        "OPTIONAL { ?r nie:title ?title . }"
                                        "OPTIONAL { ?r nao:numericRating ?star . }"

                                        "OPTIONAL { ?r nbib:seriesOf ?v2 . }"
                                        "OPTIONAL { ?v2 nie:title ?content . }"
                                        "OPTIONAL { ?r rdf:type ?type . }"
                                        "Filter (?type != rdfs:Resource)"
                                        "Filter (?type != nie:InformationElement)"

                                        // this might hide valid resources that are not further defined as book or some thing else. Won't happen often though
                                        // but this would double the number of results we need t oquery due to the ?type query
                                        "Filter (?type != nbib:Series)"
                                        + projectRelated.toLatin1() + " }" + projectTag.toLatin1() +
                                        "}");

    QList<CachedRowEntry> newCache = processQueryResults(query);

    QTime endTime = QTime::currentTime();
    kDebug() << "add" << newCache.size() << "entries after" << startTime.msecsTo(endTime) << "msec";

    return newCache;
}

void SeriesQuery::updateCacheEntry(const QUrl &uri, const QueryClient::UpdateType &updateType)
{
    QFuture<QList<CachedRowEntry> > future = QtConcurrent::run(this, &SeriesQuery::updateQueryNepomuk, uri);
    QFutureWatcher<QList<CachedRowEntry> > *updateQueryWatcher = new QFutureWatcher<QList<CachedRowEntry> >();

    updateQueryWatcher->setFuture(future);

    if(updateType == UPDATE_RESOURCE_DATA) {
        connect(updateQueryWatcher, SIGNAL(finished()),this, SLOT(finishedUpdateQuery()));
    }
    else {
        connect(updateQueryWatcher, SIGNAL(finished()),this, SLOT(finishedNewResourceQuery()));
    }
}

QList<CachedRowEntry> SeriesQuery::updateQueryNepomuk(const QUrl &uri)
{
    QTime startTime = QTime::currentTime();

    // first fetch all publications
    // this will lead to duplicates as we fetch for author names and types too
    // for each rdf:type and each connected author/publisher/editor we get the resource as result

    //If you update this, also update initialQueryNepomuk and processQueryResults and the enum ColumnList and the SeriesModel
    QString query = QString::fromLatin1("select distinct ?title ?star ?type ?content where {"
                                        "OPTIONAL { %1 nao:prefLabel ?reviewed . }" //FIXME: add reviewed to query, implement it first. tagging?

                                        "OPTIONAL { %1 nie:title ?title . }"
                                        "OPTIONAL { %1 nao:numericRating ?star . }"

                                        "OPTIONAL { %1 nbib:seriesOf ?v2 . }"
                                        "OPTIONAL { ?v2 nie:title ?content . }"
                                        "OPTIONAL { %1 rdf:type ?type . }"
                                        "}").arg( Soprano::Node::resourceToN3( uri ) );

    QList<CachedRowEntry> newCache = processQueryResults(query, uri);

    QTime endTime = QTime::currentTime();
    kDebug() << "update" << newCache.size() << "entries after" << startTime.msecsTo(endTime) << "msec";

    return newCache;
}

QList<CachedRowEntry> SeriesQuery::processQueryResults(const QString &query, const QUrl &uri)
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
                     << p.value("type").toString() << p.value("content").toString();
        }
        else {
            QString star = p.value("star").toString();
            if(!star.isEmpty())
                curEntry[Column_StarRate] = star;

            QString title = p.value("title").toString();
            if(!title.isEmpty())
                curEntry[Column_Title] = title;

            QString type = p.value("type").toString();
            if(!type.isEmpty() && !curEntry.at(Column_ResourceType).contains(type))
                curEntry[Column_ResourceType] = QString("%1;%2").arg(curEntry[Column_ResourceType]).arg(type);

            QString content = p.value("content").toString();
            if(!content.isEmpty()) {
                //create content for the HTMLDelegate looks a lot better when several entries are being displayed
                curEntry[Column_Content] = curEntry[Column_Content].append("&#8226; ");
                curEntry[Column_Content] = curEntry[Column_Content].append(content);
                curEntry[Column_Content] = curEntry[Column_Content].append("<br/>");
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
        cre.resourceType = detectResourceType(i.value().at(Column_ResourceType));
        newCache.append(cre);
    }

    return newCache;
}

QVariantList SeriesQuery::createDisplayData(const QStringList & item) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {

        case Column_ResourceType: {

            QList<QUrl> typeList;
            foreach(const QString s, item.at(i).split(";")) {
                typeList.append(QUrl(s));
            }
            BibGlobals::SeriesType type = BibGlobals::SeriesTypeFromUrl(typeList);
            QString typeSting = BibGlobals::SeriesTypeTranslation(type);

            newEntry = typeSting;

            break;
        }
        case Column_Content:
        {
            QString seriesContent = QLatin1String("<font size=\"85%\">");
            seriesContent.append( item.at(i) );

            seriesContent.chop(5); //last </br>
            seriesContent.append(QLatin1String("</font>"));

            newEntry = seriesContent;
            break;
        }
        case Column_StarRate:
        case Column_Title:
            newEntry = item.at(i);
            break;
        default:
            newEntry = QVariant();
        }

        displayList.append(newEntry);
    }

    return displayList;
}

QVariantList SeriesQuery::createDecorationData(const QStringList & item) const
{
    Q_UNUSED(item);

    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        decorationList.append(QVariant());
    }

    return decorationList;
}

uint SeriesQuery::detectResourceType(const QString &typeList) const
{
    QList<QUrl> urlList;
    foreach(const QString s, typeList.split(";")) {
        urlList.append(QUrl(s));
    }

    return (uint)BibGlobals::SeriesTypeFromUrl(urlList);
}
