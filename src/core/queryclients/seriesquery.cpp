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

#include "globals.h"

#include "nbibio/conquirere.h"

#include <Nepomuk2/Variant>
#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

SeriesQuery::SeriesQuery(QObject *parent)
    : QueryClient(parent)
{
}

SeriesQuery::~SeriesQuery()
{
    m_newWatcher->stop();
}

void SeriesQuery::startFetchData()
{
    // keep track of newly added resources
    m_newWatcher = new Nepomuk2::ResourceWatcher(this);
    m_newWatcher->addType(Nepomuk2::Vocabulary::NBIB::Series());

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
        hideTypes.append(QString(" FILTER NOT EXISTS { ?r a <%1> . } ").arg(BibEntryTypeURL.at(i).toString()));
    }

    QTime t1 = QTime::currentTime();

    // first fetch all series
    // this will lead to duplicates as we fetch for seriesOf publication names and types too
    // for each rdf:type and each connected publicaion (seriesOf) we get the resource as result
    QString query = QString::fromLatin1("select distinct ?r ?title ?star ?type ?content where {"
                                        " { ?r a nbib:Series . "  + hideTypes.toLatin1() + " }"

                                        "OPTIONAL { ?r nao:prefLabel ?reviewed . }" //FIXME: add reviewed to query, implement it first. tagging?

                                        "OPTIONAL { ?r nie:title ?title . }"
                                        "OPTIONAL { ?r nao:numericRating ?star . }"

                                        "OPTIONAL { ?r nbib:seriesOf ?v2 . }"
                                        "OPTIONAL { ?v2 nie:title ?content . }"
                                        "OPTIONAL { ?r rdf:type ?type . }"
                                        "Filter (?type != rdfs:Resource)"
                                        "Filter (?type != nie:InformationElement)"
                                        //this might hide valid resources that are not further defined as book or some thing else. Won't happen often though
                                        // but this would double the number of results we need t oquery due to the ?type query
                                        "Filter (?type != nbib:Series)"
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
    Nepomuk2::Query::AndTerm andTerm;

    if(m_library->libraryType() == Library_Project) {
        Nepomuk2::Query::OrTerm orTerm;
        orTerm.addSubTerm( Nepomuk2::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
                                                           Nepomuk2::Query::ResourceTerm( m_library->settings()->projectTag() )));
        orTerm.addSubTerm( Nepomuk2::Query::ComparisonTerm( Soprano::Vocabulary::NAO::isRelated(),
                                                            Nepomuk2::Query::ResourceTerm(m_library->settings()->projectThing() )));
        andTerm.addSubTerm(orTerm);
    }
    */
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
            SeriesType type = SeriesTypeFromUrl(typeList);
            QString typeSting = SeriesTypeTranslation.at(type);

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
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        decorationList.append(QVariant());
    }

    return decorationList;
}

QVariantList SeriesQuery::createDisplayData(const Nepomuk2::Resource & res) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_ResourceType: {

            SeriesType type = SeriesTypeFromUrl(res);
            QString typeSting = SeriesTypeTranslation.at(type);

            newEntry = typeSting;
            break;
        }
        case Column_Title: {
            QString titleSting = res.property(Nepomuk2::Vocabulary::NIE::title()).toString();

            newEntry = titleSting;
            break;
        }
        case Column_StarRate: {
            int rating = res.property(Soprano::Vocabulary::NAO::numericRating()).toInt();

            newEntry = rating;
            break;
        }
        case Column_Content: {
            QList<Nepomuk2::Resource> isSeriesOf = res.property(Nepomuk2::Vocabulary::NBIB::seriesOf()).toResourceList();

            //create content for the HTMLDelegate looks a lot better when several entries are being displayed
            QString seriesContent = QLatin1String("<font size=\"85%\">");
            foreach(const Nepomuk2::Resource &r, isSeriesOf) {
                seriesContent.append("&#8226; ");
                seriesContent.append(r.property(Nepomuk2::Vocabulary::NIE::title()).toString());
                seriesContent.append("<br/>");
            }
            seriesContent.chop(5);
            seriesContent.append(QLatin1String("</font>"));

            newEntry = seriesContent;
            break;
        }
        default:
            newEntry = QVariant();
        }

        displayList.append(newEntry);
    }

    return displayList;
}

QVariantList SeriesQuery::createDecorationData(const Nepomuk2::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        decorationList.append(QVariant());
    }

    return decorationList;
}
