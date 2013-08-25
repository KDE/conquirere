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

#include "documentquery.h"
#include "../library.h"
#include "../projectsettings.h"
#include "config/conquirere.h"

#include <KDE/KIcon>

#include <Nepomuk2/Variant>

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NFO>
#include <Soprano/Vocabulary/NAO>

#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include <QtCore/QRegExp>
#include <QtCore/QtConcurrentRun>

#include <KDE/KDebug>

DocumentQuery::DocumentQuery(QObject *parent)
    : QueryClient(parent),
      m_newWatcher(0)
{
}

DocumentQuery::~DocumentQuery()
{
    m_newWatcher->stop();
    delete m_newWatcher;
}

void DocumentQuery::stopFetchData()
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

void DocumentQuery::startFetchData()
{
    stopFetchData();

    // keep track of newly added resources
    m_newWatcher = new Nepomuk2::ResourceWatcher(this);
    m_newWatcher->addType(Nepomuk2::Vocabulary::NFO::PaginatedTextDocument());
    m_newWatcher->addType(Nepomuk2::Vocabulary::NFO::Spreadsheet());
    m_newWatcher->addType(Nepomuk2::Vocabulary::NFO::MindMap());

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

    QFuture<QList<CachedRowEntry> > future = QtConcurrent::run(this, &DocumentQuery::queryNepomuk);
    m_futureWatcher = new QFutureWatcher<QList<CachedRowEntry> >();

    m_futureWatcher->setFuture(future);
    connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(finishedQuery()));
}

QList<CachedRowEntry> DocumentQuery::queryNepomuk()
{

    QTime startTime = QTime::currentTime();

    // helping string to filter for all documents that are related to the current project
    QString projectRelated;
    QString projectTag;
    QString restrictedFolders;
    if(m_library->libraryType() == BibGlobals::Library_Project) {
        projectRelated = QString("?r nao:isRelated  <%1> .").arg(m_library->settings()->projectThing().uri().toString());
        projectTag = QString("UNION { ?r nao:hasTag  <%1> . }").arg(m_library->settings()->projectTag().uri().toString() );
    }
    else {
        QStringList folderWhiteList = ConqSettings::systemLibraryWhiteList();
        if(!folderWhiteList.isEmpty()) {
            restrictedFolders  = QString("FILTER regex(?folder, \"(%1)\" ) .").arg(folderWhiteList.join(QLatin1String("|")));
        }
    }

    //helping string to restrict the document mimetypes based on nepomuks NFO classification
    QStringList allowedNepomukTypes;
    if(ConqSettings::showDocumentsType()) {
        allowedNepomukTypes << QLatin1String("nfo:Document");
    }
    if(ConqSettings::showPaginatedTextDocumentType()) {
        allowedNepomukTypes << QLatin1String("nfo:PaginatedTextDocument");
    }
    if(ConqSettings::showPlainTextDocumentsType()) {
        allowedNepomukTypes << QLatin1String("nfo:PlainTextDocument");
    }
    if(ConqSettings::showMindMapType()) {
        allowedNepomukTypes << QLatin1String("nfo:MindMap");
    }
    if(ConqSettings::showSpreadsheetType()) {
        allowedNepomukTypes << QLatin1String("nfo:Spreadsheet");
    }

    QString filterForTypes;

    if( !allowedNepomukTypes.isEmpty() ) {
        filterForTypes = QString("FILTER(?v1 in (%1)) .").arg(allowedNepomukTypes.join(QLatin1String(", ")));
    }
    else {
        qWarning() << "No nepomuk document types specified in the configuration file.";
    }

    // first fetch all publications
    // this will lead to duplicates as we fetch for author names and types too
    // for each rdf:type and each connected author/publisher/editor we get the resource as result
    QString query = QString::fromLatin1("select distinct ?r ?title ?star ?publication ?date ?folder ?reviewed where { {"
                                        "?r a ?v1 ."
                                        + filterForTypes.toLatin1() +

                                        "OPTIONAL { ?r nao:prefLabel ?reviewed . }" //FIXME: add reviewed to query, implement it first. tagging?

                                        "OPTIONAL { ?r nie:title ?title . }"
                                        "OPTIONAL { ?r nao:numericRating ?star . }"
                                        "OPTIONAL { ?r nie:lastModified ?date . }"
                                        "?r nie:url ?folder ."
                                        + restrictedFolders.toLatin1() +
                                        "OPTIONAL { ?r nbib:publishedAs ?publication . }"
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

            curEntry << p.value("star").toString() << p.value("reviewed").toString() << p.value("publication").toString()
                     << p.value("title").toString() << p.value("folder").toString() << p.value("date").toString();
        }
        else {
            QString star = p.value("star").toString();
            if(!star.isEmpty())
                curEntry[Column_StarRate] = star;

            QString reviewed = p.value("reviewed").toString();
            if(!reviewed.isEmpty())
                curEntry[Column_Reviewed] = reviewed;

            QString publication = p.value("publication").toString();
            if(!publication.isEmpty())
                curEntry[Column_Publication] = publication;

            QString title = p.value("title").toString();
            if(!title.isEmpty())
                curEntry[Column_Title] = title;

            QString folder = p.value("folder").toString();
            if(!folder.isEmpty())
                curEntry[Column_Folder] = folder;

            QString date = p.value("date").toString();
            if(!date.isEmpty())
                curEntry[Column_Date] = date;
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

void DocumentQuery::finishedQuery()
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

QVariantList DocumentQuery::createDisplayData(const QStringList & item) const
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
        case Column_FileName:
        {
            KUrl fn( item.at(Column_Folder));
            newEntry = fn.fileName();
            break;
        }
        case Column_Folder: {
            QString folderString = item.at(Column_Folder);
            KUrl fn( item.at(Column_Folder));
            QString filenameString = fn.fileName();

            folderString.remove(filenameString);
            folderString.remove(QLatin1String("file://"));
            folderString.replace(QRegExp(QLatin1String("/home/\\w*/")), QLatin1String("~/"));

            newEntry = folderString;
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

QVariantList DocumentQuery::createDecorationData(const QStringList & item) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Publication: {
            if( !item.at(Column_Publication).isEmpty()) {
                newEntry = KIcon(QLatin1String("bookmarks-organize"));
            }
            else {
                newEntry = QVariant();
            }
            break;
        }
        case Column_Reviewed:
        {
            newEntry = KIcon(QLatin1String("dialog-ok-apply"));
            break;
        }
        default:
            newEntry = QVariant();
        }

        decorationList.append(newEntry);
    }

    return decorationList;
}


QVariantList DocumentQuery::createDisplayData(const Nepomuk2::Resource & res) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Title: {
            Nepomuk2::Resource publication = res.property(Nepomuk2::Vocabulary::NBIB::publishedAs()).toResource();

            if(!publication.isValid()) {
                newEntry = QVariant();
            }
            else {
                QString titleSting = publication.property(Nepomuk2::Vocabulary::NIE::title()).toString();
                newEntry = titleSting;
            }
            break;
        }
        case Column_Date: {
            QString dateString;
            dateString = res.property(Nepomuk2::Vocabulary::NIE::lastModified()).toString();

            if(dateString.isEmpty()) {
                dateString = res.property(Nepomuk2::Vocabulary::NIE::created()).toString();
            }
            dateString.remove('Z');

            QDateTime date = QDateTime::fromString(dateString, Qt::ISODate);
            if(date.isValid()) {
                newEntry = date.toString("dd.MM.yyyy");
            }
            else {
                newEntry = dateString;
            }
            break;
        }
        case Column_FileName: {
            QString filenameString = res.property(Nepomuk2::Vocabulary::NFO::fileName()).toString();

            newEntry = filenameString;
            break;
        }
        case Column_Folder: {
            QString folderString = res.property(Nepomuk2::Vocabulary::NIE::url()).toString();
            QString filenameString = res.property(Nepomuk2::Vocabulary::NFO::fileName()).toString();

            folderString.remove(filenameString);
            folderString.remove(QLatin1String("file://"));
            folderString.replace(QRegExp(QLatin1String("/home/\\w*/")), QLatin1String("~/"));

            newEntry = folderString;
            break;
        }
        case Column_StarRate: {
            int rating = res.rating();
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

QVariantList DocumentQuery::createDecorationData(const Nepomuk2::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Publication: {
            Nepomuk2::Resource file = res.property(Nepomuk2::Vocabulary::NBIB::publishedAs()).toResource();
            if(file.isValid()) {
                newEntry = KIcon(QLatin1String("bookmarks-organize"));
            }
            break;
        }
        case Column_Reviewed:
            newEntry = KIcon(QLatin1String("dialog-ok-apply"));
            break;
        default:
            newEntry = QVariant();
        }

        decorationList.append(newEntry);
    }

    return decorationList;
}

uint DocumentQuery::detectResourceType(const Nepomuk2::Resource & res) const
{
    Q_UNUSED(res);
    return 0;
}
