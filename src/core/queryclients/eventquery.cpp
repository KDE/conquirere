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

#include "../../globals.h"

#include <Nepomuk/Variant>
#include <Nepomuk/Thing>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>

#include "nbib.h"
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NCAL>
#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>

#include <KDE/KIcon>

EventQuery::EventQuery(QObject *parent)
    : QueryClient(parent)
{
}

void EventQuery::startFetchData()
{
    Nepomuk::Query::AndTerm andTerm;

    Nepomuk::Query::OrTerm orTerm;
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::PIMO::Event() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NCAL::Event() ) );
    andTerm.addSubTerm(orTerm);

    if(m_library->libraryType() == Library_Project) {
        Nepomuk::Query::OrTerm orTerm;
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
                                                           Nepomuk::Query::ResourceTerm( m_library->pimoTag() )));
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
                                                            Nepomuk::Query::ResourceTerm(m_library->pimoLibrary()) ) );
        andTerm.addSubTerm(orTerm);
    }

    // build the query
    Nepomuk::Query::Query query( andTerm );
    m_queryClient->query(query);
}

void EventQuery::resourceChanged (const Nepomuk::Resource &resource)
{
    if(!resource.hasType(Nepomuk::Vocabulary::PIMO::Event() ) ||
       !resource.hasType(Nepomuk::Vocabulary::NCAL::Event() ))
        return;

    qDebug() << "EventQuery::resourceChanged";
    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
}

QVariantList EventQuery::createDisplayData(const Nepomuk::Resource & res) const
{
    Nepomuk::Resource event;
    Nepomuk::Thing thing;
    if(res.hasType(Nepomuk::Vocabulary::NCAL::Event())) {
        event = res;
        thing = event.pimoThing();
    }
    else {
        event = res.property(Nepomuk::Vocabulary::PIMO::groundingOccurrence()).toResource();
        thing = res;
    }

    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Publication: {
            QList<Nepomuk::Resource> publicationList = thing.property(Nepomuk::Vocabulary::NBIB::eventPublication()).toResourceList();

            QString pubString;
            if(!publicationList.isEmpty()) {
                //create content for the HTMLDelegate looks a lot better when several publication are beeing displayed
                pubString = QLatin1String("<font size=\"85%\">");
                foreach(const Nepomuk::Resource & r, publicationList) {
                    pubString.append("&#8226; ");
                    pubString.append( r.property(Nepomuk::Vocabulary::NIE::title()).toString() );
                    pubString.append("<br/>");
                }
                pubString.chop(5);
                pubString.append(QLatin1String("</font>"));
            }

            newEntry = pubString;
            break;
        }
        case Column_Title: {
            QString titleSting = event.property(Nepomuk::Vocabulary::NIE::title()).toString();
            if(titleSting.isEmpty()) {
                titleSting = thing.property(Nepomuk::Vocabulary::NIE::title()).toString();
            }

            newEntry = titleSting;
            break;
        }
        case Column_Date: {
            QString dateSting = event.property(Nepomuk::Vocabulary::PIMO::dtstart()).toString();

            newEntry = dateSting;
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

QVariantList EventQuery::createDecorationData(const Nepomuk::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Akonadi: {
            Nepomuk::Resource event = res.property(Nepomuk::Vocabulary::PIMO::groundingOccurrence()).toResource();
            if(res.hasType(Nepomuk::Vocabulary::NCAL::Event()) || event.isValid()) {
                newEntry = KIcon(QLatin1String("akonadi"));
            }
        }
        }

        decorationList.append(newEntry);
    }

    return decorationList;
}
