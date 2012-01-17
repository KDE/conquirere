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

#include "../../globals.h"

#include <KDE/KIcon>

#include "nbibio/conquirere.h"

#include <Nepomuk/Variant>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/NegationTerm>

#include "nbib.h"
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>

ReferenceQuery::ReferenceQuery(QObject *parent)
    : QueryClient(parent)
{
}

void ReferenceQuery::startFetchData()
{
    Nepomuk::Query::AndTerm andTerm;
    Nepomuk::Query::AndTerm hideOrTerm;

    andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Reference() ) );

    foreach(int i, ConqSettings::hidenNbibPublications()) {
        Nepomuk::Query::Term hiddenPublicationTerm = Nepomuk::Query::NegationTerm::negateTerm( Nepomuk::Query::ResourceTypeTerm( BibEntryTypeURL.at(i) ) );
        Nepomuk::Query::ComparisonTerm pubTypeTerm = Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::NBIB::publication(),
                                                                                     hiddenPublicationTerm);

        hideOrTerm.addSubTerm(pubTypeTerm);
    }
    andTerm.addSubTerm(hideOrTerm);

    if(m_library->libraryType() == Library_Project) {
        Nepomuk::Query::OrTerm orTerm;
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
                                                           Nepomuk::Query::ResourceTerm( m_library->settings()->projectTag() ) ));
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
                                                            Nepomuk::Query::ResourceTerm(m_library->settings()->projectThing()) ) );
        andTerm.addSubTerm(orTerm);
    }

    // build the query
    Nepomuk::Query::Query query( andTerm );
    m_queryClient->query(query);
}

void ReferenceQuery::resourceChanged (const Nepomuk::Resource &resource)
{
    if(!resource.hasType(Nepomuk::Vocabulary::NBIB::Reference()))
        return;

    qDebug() << "ReferenceQuery::resourceChanged";
    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
}

QVariantList ReferenceQuery::createDisplayData(const Nepomuk::Resource & res) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);
    Nepomuk::Resource publication = res.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();

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
            QList<Nepomuk::Resource> authorList = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();

            foreach(const Nepomuk::Resource & a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            newEntry = authorSting;
            break;
        }
        case Column_Title: {
            QString titleSting = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();

            newEntry = titleSting;
            break;
        }
        case Column_Date: {
            QString dateSting = publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();

            newEntry = dateSting;
            break;
        }
        case Column_Editor: {
            QString authorSting;
            QList<Nepomuk::Resource> authorList = publication.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();

            foreach(const Nepomuk::Resource & a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            newEntry = authorSting;
            break;
        }
        case Column_Publisher: {
            QString authorSting;
            QList<Nepomuk::Resource> authorList = publication.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();

            foreach(const Nepomuk::Resource & a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            newEntry = authorSting;
            break;
        }
        case Column_CiteKey: {
            QString citekeySting = res.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();

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

QVariantList ReferenceQuery::createDecorationData(const Nepomuk::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_FileAvailable:
        {
            Nepomuk::Resource publication = res.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
            Nepomuk::Resource file = publication.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResource();
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
