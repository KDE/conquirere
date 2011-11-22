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

#include <Nepomuk/Variant>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>

#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

NoteQuery::NoteQuery(QObject *parent) :
    QueryClient(parent)
{
}

void NoteQuery::startFetchData()
{
    Nepomuk::Query::AndTerm andTerm;

    andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::PIMO::Note() ) );

    if(m_library->libraryType() == Library_Project) {
        Nepomuk::Query::OrTerm orTerm;
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
                                                           Nepomuk::Query::ResourceTerm( m_library->pimoTag() ) ));
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
                                                            Nepomuk::Query::ResourceTerm(m_library->pimoLibrary()) ) );
        andTerm.addSubTerm(orTerm);
    }

    // build the query
    Nepomuk::Query::Query query( andTerm );
    m_queryClient->query(query);
}

void NoteQuery::resourceChanged (const Nepomuk::Resource &resource)
{
    if(!resource.hasType(Nepomuk::Vocabulary::PIMO::Note()))
        return;

    //qDebug() << "QueryClient::resourceChanged without ResourceWatcher";
    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
}

QVariantList NoteQuery::createDisplayData(const Nepomuk::Resource & res) const
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
            Nepomuk::Resource note = res.property(Nepomuk::Vocabulary::PIMO::groundingOccurrence()).toResource();

            QString dateSting = note.property(Soprano::Vocabulary::NAO::lastModified()).toString();
            if(dateSting.isEmpty()) {
                dateSting = res.property(Soprano::Vocabulary::NAO::created()).toString();
            }
            newEntry = dateSting;
            break;
        }
        case Column_Tags: {
            QString tagString;
            QList<Nepomuk::Resource> tagList = res.property(Soprano::Vocabulary::NAO::hasTag()).toResourceList();

            foreach(const Nepomuk::Resource & nr, tagList) {
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

QVariantList NoteQuery::createDecorationData(const Nepomuk::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        decorationList.append(QVariant());
    }

    return decorationList;
}

