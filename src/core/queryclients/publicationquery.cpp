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

#include "publicationquery.h"
#include "../library.h"
#include "../projectsettings.h"

#include "globals.h"

#include "config/conquirere.h"

#include <KDE/KIcon>

#include <Nepomuk2/Variant>
#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/ResourceWatcher>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>

#include <QtCore/QtConcurrentRun>

#include <KDE/KDebug>

PublicationQuery::PublicationQuery(QObject *parent)
    : QueryClient(parent)
{
}

PublicationQuery::~PublicationQuery()
{
    m_newWatcher->stop();
    delete m_newWatcher;
}

void PublicationQuery::startFetchData()
{
    // keep track of newly added resources
    m_newWatcher = new Nepomuk2::ResourceWatcher(this);
    m_newWatcher->addType(Nepomuk2::Vocabulary::NBIB::Publication());

    if(m_library->libraryType() == Library_Project) {
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

    QFuture<QList<CachedRowEntry> > future = QtConcurrent::run(this, &PublicationQuery::queryNepomuk);
    m_futureWatcher = new QFutureWatcher<QList<CachedRowEntry> >();

    m_futureWatcher->setFuture(future);
    connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(finishedQuery()));
}

QList<CachedRowEntry> PublicationQuery::queryNepomuk()
{
    QTime startTime = QTime::currentTime();

    QString hideTypes;
    // add a filter to hide several publication types
    foreach(int i, ConqSettings::hiddenNbibPublications()) {
        hideTypes.append(QString(" FILTER NOT EXISTS { ?r a <%1> . } ").arg(BibEntryTypeURL.at(i).toString()));
    }

    // helping string to filter for all documents that are related to the current project
    QString projectRelated;
    QString projectTag;
    if(m_library->libraryType() == Library_Project) {
        projectRelated = QString("?r nao:isRelated  <%1> .").arg(m_library->settings()->projectThing().uri().toString());
        projectTag = QString("UNION { ?r nao:hasTag  <%1> . }").arg(m_library->settings()->projectTag().uri().toString() );
    }

    // first fetch all publications
    // this will lead to duplicates as we fetch for author names and types too
    // for each rdf:type and each connected author/publisher/editor we get the resource as result
    QString query = QString::fromLatin1("select distinct ?r ?title ?date ?star ?type ?creator ?citekey ?publisher ?editor ?reviewed ?file where { {"
                                        " { ?r a nbib:Publication . "  + hideTypes.toLatin1() + " }"
                                        "OPTIONAL { ?r rdf:type ?type . }"
                                        "OPTIONAL { ?r nie:title ?title . }"
                                        "OPTIONAL { ?r nie:publicationDate ?date . }"
                                        "OPTIONAL { ?r nao:numericRating ?star . }"

                                        "OPTIONAL { ?r nbib:isPulicationOf ?file . }"
                                        "OPTIONAL { ?r nao:prefLabel ?reviewed . }" //FIXME: add reviewed to query, implement it first. tagging?

                                        "OPTIONAL { ?r nco:creator ?r1 . }"
                                        "OPTIONAL { ?r1 nco:fullname ?creator . }"

                                        "OPTIONAL { ?r nco:publisher ?r2 . }"
                                        "OPTIONAL { ?r2 nco:fullname ?publisher . }"

                                        "OPTIONAL { ?r nbib:editor ?r3 . }"
                                        "OPTIONAL { ?r3 nco:fullname ?editor . }"

                                        "OPTIONAL { ?r nbib:reference ?r4 . }"
                                        "OPTIONAL { ?r4 nbib:citeKey ?citekey . }"

                                        "Filter (?type != rdfs:Resource)"
                                        "Filter (?type != nie:InformationElement)"

                                        // this might hide valid resources that are not further defined as book or some thing else. Won't happen often though
                                        // but this would double the number of results we need to query due to the ?type query
                                        //"Filter (?type != nbib:Publication)"
                                        + projectRelated.toLatin1() + " }" + projectTag.toLatin1() +
                                        "}");

    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    // combine all search results again, so we get just a single resource with a list of all authors and the list of types
    // instead of many resources with all types again
    QMap<QString, QStringList> resultList;
    while( it.next() ) {
        Soprano::BindingSet p = it.current();

        // get either a new entry or what we have inserted beforehand
        QStringList curEntry = resultList.value( p.value("r").toString(), QStringList());

        // now set / add each queried value into the StringList
        if (curEntry.isEmpty() ) {

            curEntry << p.value("star").toString() << p.value("reviewed").toString() << p.value("file").toString()
                     << p.value("citekey").toString() << p.value("type").toString() << p.value("creator").toString()
                     << p.value("title").toString() << p.value("date").toString() << p.value("publisher").toString()
                     << p.value("editor").toString();
        }
        else {
            QString type = p.value("type").toString();
            if(!type.isEmpty() && !curEntry.at(Column_ResourceType).contains(type))
                curEntry.replace(Column_ResourceType, QString("%1;%2").arg(curEntry.at(Column_ResourceType)).arg(type) );

            QString creator = p.value("creator").toString();
            if(!creator.isEmpty() && !curEntry.at(Column_Author).contains(creator))
                curEntry.replace(Column_Author, QString("%1; %2").arg(curEntry.at(Column_Author)).arg(creator) );

            QString editor = p.value("editor").toString();
            if(!editor.isEmpty() && !curEntry.at(Column_Editor).contains(editor))
                curEntry[Column_Editor] == QString("%1; %2").arg(curEntry.at(Column_Editor)).arg(editor);

            QString publisher = p.value("publisher").toString();
            if(!publisher.isEmpty() && !curEntry.at(Column_Publisher).contains(publisher))
                curEntry[Column_Publisher] == QString("%1; %2").arg(curEntry.at(Column_Publisher)).arg(publisher);

            QString citekey = p.value("citekey").toString();
            if(!citekey.isEmpty() && !curEntry.at(Column_CiteKey).contains(citekey))
                curEntry[Column_CiteKey] == QString("%1; %2").arg(curEntry.at(Column_CiteKey)).arg(citekey);
        }

        // and save the result back into the map
        resultList.insert(p.value("r").toString(), curEntry);
    }

    // now create the cache entries from all returned search results
    QList<CachedRowEntry> newCache;
    QMapIterator<QString, QStringList> i(resultList);
    while (i.hasNext()) {
        i.next();

        CachedRowEntry cre;
        cre.displayColums = createDisplayData(i.value());
        cre.decorationColums = createDecorationData(i.value());
        cre.resource = Nepomuk2::Resource::fromResourceUri( KUrl( i.key() ) );
        cre.timestamp = QDateTime::currentDateTime();
        newCache.append(cre);

        m_resourceWatcher->addResource( cre.resource );
    }

    QTime endTime = QTime::currentTime();
    kDebug() << "add" << newCache.size() << "entries after" << startTime.msecsTo(endTime) << "msec";

    return newCache;
}

void PublicationQuery::finishedQuery()
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

QVariantList PublicationQuery::createDisplayData(const QStringList & item) const
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
            BibEntryType type = BibEntryTypeFromUrl(typeList);
            QString typeSting = BibEntryTypeTranslation.at(type);

            newEntry = typeSting;

            break;
        }
        case Column_Date: {

            QDateTime date = QDateTime::fromString( item.at(Column_Date), Qt::ISODate);
            if(date.isValid()) {
                newEntry = date.toString("dd.MM.yyyy");
            }

            break;
        }
        case Column_StarRate:
        case Column_CiteKey:
        case Column_Author:
        case Column_Title:
        case Column_Publisher:
        case Column_Editor:
            newEntry = item.at(i);
            break;
        default:
            newEntry = QVariant();
        }

        displayList.append(newEntry);
    }

    return displayList;

}

QVariantList PublicationQuery::createDecorationData(const QStringList & item) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_FileAvailable:
        {
            QString file = item.at(i);
            if(!file.isNull()) {
                newEntry = KIcon(QLatin1String("bookmarks-organize"));
            }
            break;
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

QVariantList PublicationQuery::createDisplayData(const Nepomuk2::Resource & res) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_ResourceType: {
            BibEntryType type = BibEntryTypeFromUrl(res);
            QString typeSting = BibEntryTypeTranslation.at(type);

            newEntry = typeSting;
            break;
        }
        case Column_Author: {
            QString authorSting;
            QList<Nepomuk2::Resource> authorList = res.property(Nepomuk2::Vocabulary::NCO::creator()).toResourceList();

            foreach(const Nepomuk2::Resource & a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            newEntry = authorSting;
            break;
        }
        case Column_Title: {
            QString titleSting = res.property(Nepomuk2::Vocabulary::NIE::title()).toString();

            newEntry = titleSting;
            break;
        }
        case Column_Date: {
            QString dateString = res.property(Nepomuk2::Vocabulary::NBIB::publicationDate()).toString();

            QDateTime date = QDateTime::fromString(dateString, Qt::ISODate);
            if(date.isValid()) {
                newEntry = date.toString("dd.MM.yyyy");
            }
            else {
                newEntry = dateString;
            }

            break;
        }
        case Column_Editor: {
            QString editorSting;
            QList<Nepomuk2::Resource> editorList = res.property(Nepomuk2::Vocabulary::NBIB::editor()).toResourceList();

            foreach(const Nepomuk2::Resource & a, editorList) {
                editorSting.append(a.genericLabel());
                editorSting.append(QLatin1String("; "));
            }
            editorSting.chop(2);

            newEntry = editorSting;
            break;
        }
        case Column_Publisher: {
            QString publisherSting;
            QList<Nepomuk2::Resource> publisherList = res.property(Nepomuk2::Vocabulary::NCO::publisher()).toResourceList();

            foreach(const Nepomuk2::Resource & a, publisherList) {
                publisherSting.append(a.genericLabel());
                publisherSting.append(QLatin1String("; "));
            }
            publisherSting.chop(2);

            newEntry = publisherSting;
            break;
        }
        case Column_CiteKey: {
            QString citekeySting;
            QList<Nepomuk2::Resource> refs = res.property(Nepomuk2::Vocabulary::NBIB::reference()).toResourceList();

            foreach(const Nepomuk2::Resource & r, refs) {
                QString citykey = r.property(Nepomuk2::Vocabulary::NBIB::citeKey()).toString();
                if(citykey.isEmpty()) {
                    citykey = i18nc("indicates that the citekey for a reference is not known","unknown Citekey");
                }
                citekeySting.append(citykey);
                citekeySting.append(QLatin1String("; "));
            }
            citekeySting.chop(2);

            newEntry = citekeySting;
            break;
        }
        case Column_StarRate: {
            int rating = res.property(Soprano::Vocabulary::NAO::numericRating()).toInt();

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

QVariantList PublicationQuery::createDecorationData(const Nepomuk2::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_FileAvailable:
        {
            Nepomuk2::Resource file = res.property(Nepomuk2::Vocabulary::NBIB::isPublicationOf()).toResource();
            if(file.isValid()) {
                newEntry = KIcon(QLatin1String("bookmarks-organize"));
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
