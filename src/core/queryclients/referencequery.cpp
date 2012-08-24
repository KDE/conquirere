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

#include "referencequery.h"

#include "../library.h"
#include "../projectsettings.h"

#include "globals.h"

#include <KDE/KIcon>

#include "nbibio/conquirere.h"

#include <Nepomuk2/Variant>

#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include "nbib.h"
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>

ReferenceQuery::ReferenceQuery(QObject *parent)
    : QueryClient(parent)
{
}

ReferenceQuery::~ReferenceQuery()
{
    m_newWatcher->stop();
}

void ReferenceQuery::startFetchData()
{
    // keep track of newly added resources
    m_newWatcher = new Nepomuk2::ResourceWatcher(this);
    m_newWatcher->addType(Nepomuk2::Vocabulary::NBIB::Reference());

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

    QString hideTypes;
    // add a filter to hide several publication types
    foreach(int i, ConqSettings::hiddenNbibPublications()) {
        hideTypes.append(QString(" FILTER NOT EXISTS { ?pub a <%1> . } ").arg(BibEntryTypeURL.at(i).toString()));
    }

    QTime t1 = QTime::currentTime();

    // first fetch all publications
    // this will lead to duplicates as we fetch for author names and types too
    // for each rdf:type and each connected author/publisher/editor we get the resource as result
    QString query = QString::fromLatin1("select distinct ?r ?title ?date ?star ?type ?creator ?citekey ?publisher ?editor ?reviewed ?file where {"
                                        " ?r a nbib:Reference . "
                                        " { ?r nbib:publication ?pub ."  + hideTypes.toLatin1() + " }"

                                        "OPTIONAL { ?pub nao:numericRating ?star . }"
                                        "OPTIONAL { ?pub nie:title ?title . }"
                                        "OPTIONAL { ?r nbib:citeKey ?citekey . }"


                                        "OPTIONAL { ?pub nie:publicationDate ?date . }"

                                        "OPTIONAL { ?pub nbib:isPulicationOf ?file . }"
                                        "OPTIONAL { ?r nao:prefLabel ?reviewed . }" //FIXME: add reviewed to query, implement it first. tagging?

                                        "OPTIONAL { ?pub nco:creator ?r1 . }"
                                        "OPTIONAL { ?r1 nco:fullname ?creator . }"

                                        "OPTIONAL { ?pub nco:publisher ?r2 . }"
                                        "OPTIONAL { ?r2 nco:fullname ?publisher . }"

                                        "OPTIONAL { ?pub nbib:editor ?r3 . }"
                                        "OPTIONAL { ?r3 nco:fullname ?editor . }"

                                        "OPTIONAL { ?pub rdf:type ?type . }"
                                        "Filter (?type != rdfs:Resource)"
                                        "Filter (?type != nie:InformationElement)"
                                        //this might hide valid resources that are not further defined as book or some thing else. Won't happen often though
                                        // but this would double the number of results we need t oquery due to the ?type query
                                        "Filter (?type != nbib:Publication)"
                                        "}");

    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    QTime t2 = QTime::currentTime();
    kDebug() << "###### search finished ########## after" << t1.msecsTo(t2) << "msecs";

    // combine all search results again, so we get a just a single resource with a list of all authors and the list of types
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

    QTime t3 = QTime::currentTime();
    kDebug() << "###### prefilter Finished ########## after" << t2.msecsTo(t3) << "msec";

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
        newCache.append(cre);

        m_resourceWatcher->addResource( cre.resource );
    }

    QTime t4 = QTime::currentTime();
    kDebug() << "add ########## " << newCache.size() << " ############## entires after" << t3.msecsTo(t4) << "msec. total" << t1.msecsTo(t4) << "msec";

    emit newCacheEntries(newCache);

    m_resourceWatcher->start();

    emit queryFinished();
    /*
    if(m_library->libraryType() == Library_Project) {
        Nepomuk2::Query::OrTerm orTerm;
        orTerm.addSubTerm( Nepomuk2::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
                                                           Nepomuk2::Query::ResourceTerm( m_library->settings()->projectTag() ) ));
        orTerm.addSubTerm( Nepomuk2::Query::ComparisonTerm( Soprano::Vocabulary::NAO::isRelated(),
                                                            Nepomuk2::Query::ResourceTerm(m_library->settings()->projectThing()) ) );
        andTerm.addSubTerm(orTerm);
    }
    */
}

QVariantList ReferenceQuery::createDisplayData(const QStringList & item) const
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

QVariantList ReferenceQuery::createDecorationData(const QStringList & item) const
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

QVariantList ReferenceQuery::createDisplayData(const Nepomuk2::Resource & res) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);
    Nepomuk2::Resource publication = res.property(Nepomuk2::Vocabulary::NBIB::publication()).toResource();

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_ResourceType: {

            BibEntryType type = BibEntryTypeFromUrl(publication);
            QString typeSting = BibEntryTypeTranslation.at(type);

            newEntry = typeSting;
            break;
        }
        case Column_Author: {
            QString authorSting;
            QList<Nepomuk2::Resource> authorList = publication.property(Nepomuk2::Vocabulary::NCO::creator()).toResourceList();

            foreach(const Nepomuk2::Resource & a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            newEntry = authorSting;
            break;
        }
        case Column_Title: {
            QString titleSting = publication.property(Nepomuk2::Vocabulary::NIE::title()).toString();

            newEntry = titleSting;
            break;
        }
        case Column_Date: {
            QString dateString = publication.property(Nepomuk2::Vocabulary::NBIB::publicationDate()).toString();
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
        case Column_Editor: {
            QString authorSting;
            QList<Nepomuk2::Resource> authorList = publication.property(Nepomuk2::Vocabulary::NBIB::editor()).toResourceList();

            foreach(const Nepomuk2::Resource & a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            newEntry = authorSting;
            break;
        }
        case Column_Publisher: {
            QString authorSting;
            QList<Nepomuk2::Resource> authorList = publication.property(Nepomuk2::Vocabulary::NCO::publisher()).toResourceList();

            foreach(const Nepomuk2::Resource & a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            newEntry = authorSting;
            break;
        }
        case Column_CiteKey: {
            QString citekeySting = res.property(Nepomuk2::Vocabulary::NBIB::citeKey()).toString();

            newEntry = citekeySting;
            break;
        }
        case Column_StarRate: {
            int rating = publication.property(Soprano::Vocabulary::NAO::numericRating()).toInt();

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

QVariantList ReferenceQuery::createDecorationData(const Nepomuk2::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_FileAvailable:
        {
            Nepomuk2::Resource publication = res.property(Nepomuk2::Vocabulary::NBIB::publication()).toResource();
            Nepomuk2::Resource file = publication.property(Nepomuk2::Vocabulary::NBIB::isPublicationOf()).toResource();
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
