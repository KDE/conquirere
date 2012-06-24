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
#include <Nepomuk2/Query/ResourceTerm>
#include <Nepomuk2/Query/AndTerm>
#include <Nepomuk2/Query/OrTerm>
#include <Nepomuk2/Query/ResourceTypeTerm>
#include <Nepomuk2/Query/ComparisonTerm>

#include <Nepomuk2/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

NoteQuery::NoteQuery(QObject *parent)
    : QueryClient(parent)
{
}

void NoteQuery::startFetchData()
{
    Nepomuk2::Query::AndTerm andTerm;

    andTerm.addSubTerm( Nepomuk2::Query::ResourceTypeTerm( Nepomuk2::Vocabulary::PIMO::Note() ) );

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

void NoteQuery::resourceChanged (const Nepomuk2::Resource &resource)
{
    if(!resource.hasType(Nepomuk2::Vocabulary::PIMO::Note()))
        return;

    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
}

QVariantList NoteQuery::createDisplayData(const Nepomuk2::Resource & res) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Title: {
            QString titleSting = res.property(Soprano::Vocabulary::NAO::prefLabel()).toString();

            newEntry = titleSting;
            break;
        }
        case Column_Date: {
            Nepomuk2::Resource note = res.property(Nepomuk2::Vocabulary::PIMO::groundingOccurrence()).toResource();

            QString dateString = note.property(Soprano::Vocabulary::NAO::lastModified()).toString();
            if(dateString.isEmpty()) {
                dateString = res.property(Soprano::Vocabulary::NAO::created()).toString();
            }

            QDateTime date = QDateTime::fromString(dateString, Qt::ISODate);
            if(date.isValid()) {
                newEntry = date.toString("dd.MM.yyyy hh:mm:ss");
            }
            else {
                newEntry = dateString;
            }
            break;
        }
        case Column_Tags: {
            QString tagString;
            QList<Nepomuk2::Resource> tagList = res.property(Soprano::Vocabulary::NAO::hasTag()).toResourceList();

            foreach(const Nepomuk2::Resource & nr, tagList) {
                tagString.append(nr.property(Soprano::Vocabulary::NAO::prefLabel()).toString());
                tagString.append(QLatin1String("; "));
            }
            tagString.chop(2);
            newEntry = tagString;
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

QVariantList NoteQuery::createDecorationData(const Nepomuk2::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        decorationList.append(QVariant());
    }

    return decorationList;
}

