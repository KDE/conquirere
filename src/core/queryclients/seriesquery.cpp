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

#include "../../globals.h"

#include <Nepomuk/Variant>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

SeriesQuery::SeriesQuery(QObject *parent)
    : QueryClient(parent)
{
}

void SeriesQuery::startFetchData()
{
    Nepomuk::Query::AndTerm andTerm;

    andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Series() ) );

    if(m_library->libraryType() == Library_Project) {
        Nepomuk::Query::OrTerm orTerm;
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
                                                           Nepomuk::Query::ResourceTerm( m_library->settings()->projectTag() )));
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
                                                            Nepomuk::Query::ResourceTerm(m_library->settings()->projectThing() )));
        andTerm.addSubTerm(orTerm);
    }

    // build the query
    Nepomuk::Query::Query query( andTerm );
    m_queryClient->query(query);
}

void SeriesQuery::resourceChanged (const Nepomuk::Resource &resource)
{
    if(!resource.hasType(Nepomuk::Vocabulary::NBIB::Series()))
        return;

    qDebug() << "SeriesQuery::resourceChanged";
    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
}

QVariantList SeriesQuery::createDisplayData(const Nepomuk::Resource & res) const
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
            QString titleSting = res.property(Nepomuk::Vocabulary::NIE::title()).toString();

            newEntry = titleSting;
            break;
        }
        case Column_StarRate: {
            int rating = res.property(Soprano::Vocabulary::NAO::numericRating()).toInt();

            newEntry = rating;
            break;
        }
        case Column_Content: {
            QList<Nepomuk::Resource> isSeriesOf = res.property(Nepomuk::Vocabulary::NBIB::seriesOf()).toResourceList();

            //create content for the HTMLDelegate looks a lot better when several entries are being displayed
            QString seriesContent = QLatin1String("<font size=\"85%\">");
            foreach(const Nepomuk::Resource &r, isSeriesOf) {
                seriesContent.append("&#8226; ");
                seriesContent.append(r.property(Nepomuk::Vocabulary::NIE::title()).toString());
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

QVariantList SeriesQuery::createDecorationData(const Nepomuk::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        decorationList.append(QVariant());
    }

    return decorationList;
}
