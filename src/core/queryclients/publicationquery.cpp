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

#include "../../globals.h"

#include <KDE/KIcon>

#include <Nepomuk/Variant>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>

#include "nbib.h"
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>

PublicationQuery::PublicationQuery(QObject *parent)
    : QueryClient(parent)
{
}

void PublicationQuery::startFetchData()
{
    Nepomuk::Query::AndTerm andTerm;

    andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Publication() ) );

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

void PublicationQuery::resourceChanged (const Nepomuk::Resource &resource)
{
    if(!resource.hasType(Nepomuk::Vocabulary::NBIB::Publication()))
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

QVariantList PublicationQuery::createDisplayData(const Nepomuk::Resource & res) const
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
            QList<Nepomuk::Resource> authorList = res.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();

            foreach(const Nepomuk::Resource & a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            newEntry = authorSting;
            break;
        }
        case Column_Title: {
            QString titleSting = res.property(Nepomuk::Vocabulary::NIE::title()).toString();

            newEntry = titleSting;
            break;
        }
        case Column_Date: {
            QString dateSting = res.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();

            newEntry = dateSting;
            break;
        }
        case Column_Editor: {
            QString editorSting;
            QList<Nepomuk::Resource> editorList = res.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();

            foreach(const Nepomuk::Resource & a, editorList) {
                editorSting.append(a.genericLabel());
                editorSting.append(QLatin1String("; "));
            }
            editorSting.chop(2);

            newEntry = editorSting;
            break;
        }
        case Column_Publisher: {
            QString publisherSting;
            QList<Nepomuk::Resource> publisherList = res.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();

            foreach(const Nepomuk::Resource & a, publisherList) {
                publisherSting.append(a.genericLabel());
                publisherSting.append(QLatin1String("; "));
            }
            publisherSting.chop(2);

            newEntry = publisherSting;
            break;
        }
        case Column_CiteKey: {
            QString citekeySting;
            QList<Nepomuk::Resource> refs = res.property(Nepomuk::Vocabulary::NBIB::reference()).toResourceList();

            foreach(const Nepomuk::Resource & r, refs) {
                QString citykey = r.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
                if(citykey.isEmpty()) {
                    citykey = i18n("unknown Citekey");
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

QVariantList PublicationQuery::createDecorationData(const Nepomuk::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_FileAvailable:
        {
            Nepomuk::Resource file = res.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResource();
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
