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
#include <Nepomuk2/Query/ResourceTerm>
#include <Nepomuk2/Query/AndTerm>
#include <Nepomuk2/Query/OrTerm>
#include <Nepomuk2/Query/ResourceTypeTerm>
#include <Nepomuk2/Query/ComparisonTerm>
#include <Nepomuk2/Query/NegationTerm>

#include "nbib.h"
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>

ReferenceQuery::ReferenceQuery(QObject *parent)
    : QueryClient(parent)
{
}

void ReferenceQuery::startFetchData()
{
    Nepomuk2::Query::AndTerm andTerm;
    andTerm.addSubTerm( Nepomuk2::Query::ResourceTypeTerm( Nepomuk2::Vocabulary::NBIB::Reference() ) );

    /*
    //BUG this is something nepomuk does not like ...
    // added workaround in queryclient.cpp
//    Nepomuk2::Query::AndTerm hideOrTerm;
    foreach(int i, ConqSettings::hiddenNbibPublications()) {
        Nepomuk2::Query::Term hiddenPublicationTerm = Nepomuk2::Query::NegationTerm::negateTerm( Nepomuk2::Query::ResourceTypeTerm( BibEntryTypeURL.at(i) ) );
        Nepomuk2::Query::ComparisonTerm pubTypeTerm = Nepomuk2::Query::ComparisonTerm( Nepomuk2::Vocabulary::NBIB::publication(),
                                                                                     hiddenPublicationTerm);
        hideOrTerm.addSubTerm(pubTypeTerm);
    }

    if( !ConqSettings::hiddenNbibPublications().isEmpty())
        andTerm.addSubTerm(hideOrTerm);
        */

    if(m_library->libraryType() == Library_Project) {
        Nepomuk2::Query::OrTerm orTerm;
        orTerm.addSubTerm( Nepomuk2::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
                                                           Nepomuk2::Query::ResourceTerm( m_library->settings()->projectTag() ) ));
        orTerm.addSubTerm( Nepomuk2::Query::ComparisonTerm( Soprano::Vocabulary::NAO::isRelated(),
                                                            Nepomuk2::Query::ResourceTerm(m_library->settings()->projectThing()) ) );
        andTerm.addSubTerm(orTerm);
    }

    // build the query
    Nepomuk2::Query::Query query( andTerm );
    m_queryClient->query(query);
}

void ReferenceQuery::resourceChanged (const Nepomuk2::Resource &resource)
{
    if(!resource.hasType(Nepomuk2::Vocabulary::NBIB::Reference()))
        return;

    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
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
