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

#include "eventquery.h"

#include "../library.h"
#include "../projectsettings.h"

#include <Nepomuk2/Variant>

#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NCAL>
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>

#include <KDE/KIcon>

#include <QtCore/QtConcurrentRun>

#include <KDE/KDebug>

EventQuery::EventQuery(QObject *parent)
    : QueryClient(parent)
{
}

EventQuery::~EventQuery()
{
    m_newWatcher->stop();
    delete m_newWatcher;
}

void EventQuery::startFetchData()
{
    // keep track of newly added resources
    m_newWatcher = new Nepomuk2::ResourceWatcher(this);

    m_newWatcher->addType(Nepomuk2::Vocabulary::NCAL::Event());
    m_newWatcher->addProperty(Nepomuk2::Vocabulary::NBIB::eventPublication());

    if(m_library->libraryType() == BibGlobals::Library_Project) {
        m_newWatcher->addProperty(Soprano::Vocabulary::NAO::isRelated());
    }
    connect(m_newWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)) );

    connect(m_newWatcher, SIGNAL(resourceCreated(Nepomuk2::Resource,QList<QUrl>)),
            this, SLOT(resourceCreated(Nepomuk2::Resource,QList<QUrl>)) );

    m_newWatcher->start();

    // create the resource watcher that will keep track of changes in the existing data
    //FIXME: Why does the resource watcher for events also call propertyChange for documents?
    m_resourceWatcher = new Nepomuk2::ResourceWatcher(this);

    connect(m_resourceWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)) );

    connect(m_resourceWatcher, SIGNAL(resourceTypeAdded(Nepomuk2::Resource,Nepomuk2::Types::Class)),
            this, SLOT(resourceTypeChanged(Nepomuk2::Resource,Nepomuk2::Types::Class)) );

    connect(m_resourceWatcher, SIGNAL(resourceTypeRemoved(Nepomuk2::Resource,Nepomuk2::Types::Class)),
            this, SLOT(resourceTypeChanged(Nepomuk2::Resource,Nepomuk2::Types::Class)) );

    connect(m_resourceWatcher, SIGNAL(resourceRemoved(QUrl,QList<QUrl>)),
            this, SLOT(resourceRemoved(QUrl,QList<QUrl>)) );

    QFuture<QList<CachedRowEntry> > future = QtConcurrent::run(this, &EventQuery::queryNepomuk);
    m_futureWatcher = new QFutureWatcher<QList<CachedRowEntry> >();

    m_futureWatcher->setFuture(future);
    connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(finishedQuery()));
}

QList<CachedRowEntry> EventQuery::queryNepomuk()
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
    // this will lead to duplicates as we fetch for nbib:eventpublication names
    // each connected publicaion we get the resource as result
    QString query = QString::fromLatin1("select distinct ?r ?title ?star ?date ?publication where { {"
                                        "?r a ncal:Event . "
                                        "?r nbib:eventpublication ?pub ."

                                        "OPTIONAL { ?pub nie:title ?publication . }"

                                        "OPTIONAL { ?r nie:title ?title . }"
                                        "OPTIONAL { ?r nao:numericRating ?star . }"
                                        "OPTIONAL { ?r ncal:date ?date . }"
                                        + projectRelated.toLatin1() + " }" + projectTag.toLatin1() +

                                        "}");

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
                     << p.value("date").toString() << p.value("publication").toString() << p.value("r").toString();
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

            QString publication = p.value("publication").toString();
            if(!publication.isEmpty() && !curEntry[Column_Publication] .contains(publication)) {

                //create content for the HTMLDelegate looks a lot better when several entries are being displayed
                curEntry[Column_Publication] = curEntry[Column_Publication].append("&#8226; ");
                curEntry[Column_Publication] = curEntry[Column_Publication].append(publication);
                curEntry[Column_Publication] = curEntry[Column_Publication].append("<br/>");
            }
        }

        // and save the result back into the map
        resultList.insert(p.value("r").toString(), curEntry);
    }

    // now create the cache entries from all returned search results
    QList<CachedRowEntry> newCache;
    QMapIterator<QString, QStringList> i(resultList);
    while (i.hasNext()) {
        i.next();

        // create the cache entries for each search result
        CachedRowEntry cre;
        cre.displayColums = createDisplayData(i.value());
        cre.decorationColums = createDecorationData(i.value());
        cre.resource = Nepomuk2::Resource::fromResourceUri( KUrl( i.key() ) );
        cre.timestamp = QDateTime::currentDateTime();
        cre.resourceType = detectResourceType(cre.resource);
        newCache.append(cre);

        m_resourceWatcher->addResource( cre.resource );
    }

    QTime endTime = QTime::currentTime();
    kDebug() << "add" << newCache.size() << "entries after" << startTime.msecsTo(endTime) << "msec";

    return newCache;
}

void EventQuery::finishedQuery()
{
    QList<CachedRowEntry> results = m_futureWatcher->future().result();

    // add all results to the ResourceWatcher
    foreach(const CachedRowEntry &cre, results) {
        m_resourceWatcher->addResource( cre.resource );
    }

    emit newCacheEntries(results);

    //don't start the watcher if we have no resources to watch
    // will be started from the queryclient.h when updateResource inserts new items
    if( !m_resourceWatcher->resources().isEmpty()) {
        m_resourceWatcher->start();
    }

    emit queryFinished();

    delete m_futureWatcher;
    m_futureWatcher = 0;
}

QVariantList EventQuery::createDisplayData(const QStringList & item) const
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
        case Column_Publication:
        {
            QString publicationsContent = QLatin1String("<font size=\"85%\">");
            publicationsContent.append( item.at(i) );

            publicationsContent.chop(5); //last </br>
            publicationsContent.append(QLatin1String("</font>"));

            newEntry = publicationsContent;
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

QVariantList EventQuery::createDecorationData(const QStringList & item) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        //TODO: find out if event is also available in akonadi
        decorationList.append(QVariant());
    }

    return decorationList;
}

QVariantList EventQuery::createDisplayData(const Nepomuk2::Resource & resource) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Publication: {
            QList<Nepomuk2::Resource> publicationList = resource.property(Nepomuk2::Vocabulary::NBIB::eventPublication()).toResourceList();

            QString pubString;
            if(!publicationList.isEmpty()) {
                //create content for the HTMLDelegate looks a lot better when several publication are being displayed
                pubString = QLatin1String("<font size=\"85%\">");
                foreach(const Nepomuk2::Resource & r, publicationList) {
                    pubString.append("&#8226; ");
                    pubString.append( r.property(Nepomuk2::Vocabulary::NIE::title()).toString() );
                    pubString.append("<br/>");
                }
                pubString.chop(5);
                pubString.append(QLatin1String("</font>"));
            }

            newEntry = pubString;
            break;
        }
        case Column_Title: {
            QString titleSting = resource.property(Nepomuk2::Vocabulary::NIE::title()).toString();

            newEntry = titleSting;
            break;
        }
        case Column_Date: {
            QString dateString = resource.property(Nepomuk2::Vocabulary::NCAL::date()).toString();

            QDateTime date = QDateTime::fromString(dateString, Qt::ISODate);
            if(date.isValid()) {
                newEntry = date.toString("dd.MM.yyyy");
            }
            else {
                newEntry = dateString;
            }
            break;
        }
        case Column_StarRate: {
            int rating = resource.property(Soprano::Vocabulary::NAO::numericRating()).toInt();

            newEntry = rating;
            break;
        }
        default:
            newEntry = QVariant();
        }

        displayList.append(newEntry);
    }

    return displayList;
}

QVariantList EventQuery::createDecorationData(const Nepomuk2::Resource & resource) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
//        switch(i) {
//        case Column_Akonadi: {
//            //TODO: find out if event is also available in akonadi
//            Nepomuk2::Resource event = resource.property(Nepomuk2::Vocabulary::PIMO::groundingOccurrence()).toResource();
//            if(resource.hasType(Nepomuk2::Vocabulary::NCAL::Event()) || event.isValid()) {
//                newEntry = KIcon(QLatin1String("akonadi"));
//            }
//        }
//        }
    decorationList.append(newEntry);

    }

    return decorationList;
}

uint EventQuery::detectResourceType(const Nepomuk2::Resource & res) const
{
    Q_UNUSED(res)
    return 0;
}
