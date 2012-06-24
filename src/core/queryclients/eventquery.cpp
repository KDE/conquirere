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

#include "globals.h"

#include <Nepomuk2/Variant>
#include <Nepomuk2/Thing>
#include <Nepomuk2/Query/ResourceTerm>
#include <Nepomuk2/Query/AndTerm>
#include <Nepomuk2/Query/OrTerm>
#include <Nepomuk2/Query/ResourceTypeTerm>
#include <Nepomuk2/Query/ComparisonTerm>

#include "nbib.h"
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCAL>
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>

#include <KDE/KIcon>

EventQuery::EventQuery(QObject *parent)
    : QueryClient(parent)
{
}

void EventQuery::startFetchData()
{
    Nepomuk2::Query::AndTerm andTerm;

    Nepomuk2::Query::OrTerm orTerm;
    orTerm.addSubTerm( Nepomuk2::Query::ResourceTypeTerm( Nepomuk2::Vocabulary::PIMO::Event() ) );
    orTerm.addSubTerm( Nepomuk2::Query::ResourceTypeTerm( Nepomuk2::Vocabulary::NCAL::Event() ) );
    andTerm.addSubTerm(orTerm);

    if(m_library->libraryType() == Library_Project) {
        Nepomuk2::Query::OrTerm orTerm;
        orTerm.addSubTerm( Nepomuk2::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
                                                           Nepomuk2::Query::ResourceTerm( m_library->settings()->projectTag() )));
        orTerm.addSubTerm( Nepomuk2::Query::ComparisonTerm( Soprano::Vocabulary::NAO::isRelated(),
                                                            Nepomuk2::Query::ResourceTerm(m_library->settings()->projectThing() )));
        andTerm.addSubTerm(orTerm);
    }

    // build the query
    Nepomuk2::Query::Query query( andTerm );
    m_queryClient->query(query);
}

void EventQuery::resourceChanged (const Nepomuk2::Resource &resource)
{
    if(!resource.hasType(Nepomuk2::Vocabulary::PIMO::Event() ) ||
       !resource.hasType(Nepomuk2::Vocabulary::NCAL::Event() ))
        return;

    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
}

QVariantList EventQuery::createDisplayData(const Nepomuk2::Resource & res) const
{
    Nepomuk2::Resource event;
    Nepomuk2::Thing thing;
    if(res.hasType(Nepomuk2::Vocabulary::NCAL::Event())) {
        event = res;
        thing = event.pimoThing();
    }
    else {
        event = res.property(Nepomuk2::Vocabulary::PIMO::groundingOccurrence()).toResource();
        thing = res;
    }

    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Publication: {
            QList<Nepomuk2::Resource> publicationList = thing.property(Nepomuk2::Vocabulary::NBIB::eventPublication()).toResourceList();

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
            QString titleSting = event.property(Nepomuk2::Vocabulary::NIE::title()).toString();
            if(titleSting.isEmpty()) {
                titleSting = thing.property(Nepomuk2::Vocabulary::NIE::title()).toString();
            }

            newEntry = titleSting;
            break;
        }
        case Column_Date: {
            QString dateString = event.property(Nepomuk2::Vocabulary::PIMO::dtstart()).toString();
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
        case Column_StarRate: {
            int rating = thing.property(Soprano::Vocabulary::NAO::numericRating()).toInt();

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

QVariantList EventQuery::createDecorationData(const Nepomuk2::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Akonadi: {
            Nepomuk2::Resource event = res.property(Nepomuk2::Vocabulary::PIMO::groundingOccurrence()).toResource();
            if(res.hasType(Nepomuk2::Vocabulary::NCAL::Event()) || event.isValid()) {
                newEntry = KIcon(QLatin1String("akonadi"));
            }
        }
        }

        decorationList.append(newEntry);
    }

    return decorationList;
}
