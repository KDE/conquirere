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

#include "nbibio/conquirere.h"

#include <KDE/KIcon>

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

PublicationQuery::PublicationQuery(QObject *parent)
    : QueryClient(parent)
{
}

void PublicationQuery::startFetchData()
{
    Nepomuk2::Query::AndTerm andTerm;

    andTerm.addSubTerm( Nepomuk2::Query::ResourceTypeTerm( Nepomuk2::Vocabulary::NBIB::Publication() ) );

    foreach(int i, ConqSettings::hiddenNbibPublications()) {
        Nepomuk2::Query::Term hiddenPublicationTerm = Nepomuk2::Query::NegationTerm::negateTerm(Nepomuk2::Query::ResourceTypeTerm( BibEntryTypeURL.at(i) ));
        andTerm.addSubTerm(hiddenPublicationTerm);
    }

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

void PublicationQuery::resourceChanged (const Nepomuk2::Resource &resource)
{
    if(!resource.hasType(Nepomuk2::Vocabulary::NBIB::Publication()))
        return;

    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
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
