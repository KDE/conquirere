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

#include "documentquery.h"
#include "../library.h"
#include "../projectsettings.h"

#include "globals.h"

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
#include <Nepomuk2/Vocabulary/NFO>
#include <Soprano/Vocabulary/NAO>

#include <QtCore/QRegExp>

DocumentQuery::DocumentQuery(QObject *parent) :
    QueryClient(parent)
{
}

void DocumentQuery::startFetchData()
{
    Nepomuk2::Query::OrTerm mainOrTerm;
    Nepomuk2::Query::AndTerm andTerm;

    Nepomuk2::Query::OrTerm subOrTerm;
    subOrTerm.addSubTerm( Nepomuk2::Query::ResourceTypeTerm( Nepomuk2::Vocabulary::NFO::PaginatedTextDocument() ) );
    subOrTerm.addSubTerm( Nepomuk2::Query::ResourceTypeTerm( Nepomuk2::Vocabulary::NFO::Presentation() ) );
    subOrTerm.addSubTerm( Nepomuk2::Query::ResourceTypeTerm( Nepomuk2::Vocabulary::NFO::MindMap() ) );
    subOrTerm.addSubTerm( Nepomuk2::Query::ResourceTypeTerm( Nepomuk2::Vocabulary::NFO::Spreadsheet() ) );

    andTerm.addSubTerm(subOrTerm);

    if(m_library->libraryType() == Library_Project) {
        Nepomuk2::Query::OrTerm orTerm;
        orTerm.addSubTerm( Nepomuk2::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
                                                           Nepomuk2::Query::ResourceTerm( m_library->settings()->projectTag() )));
        orTerm.addSubTerm( Nepomuk2::Query::ComparisonTerm( Soprano::Vocabulary::NAO::isRelated(),
                                                           Nepomuk2::Query::ResourceTerm(m_library->settings()->projectThing() )));
        andTerm.addSubTerm(orTerm);


        // now we also add all rescources with nfo:FileDataObject and isRelated() for the project too
        Nepomuk2::Query::AndTerm subAndTerm;
        subAndTerm.addSubTerm( Nepomuk2::Query::ResourceTypeTerm( Nepomuk2::Vocabulary::NFO::FileDataObject() ) );
        subAndTerm.addSubTerm( Nepomuk2::Query::ComparisonTerm( Soprano::Vocabulary::NAO::isRelated(),
                                                               Nepomuk2::Query::ResourceTerm(m_library->settings()->projectThing() )));

        mainOrTerm.addSubTerm(subAndTerm);
    }

    mainOrTerm.addSubTerm(andTerm);


    // build the query
    Nepomuk2::Query::Query query( mainOrTerm );
    m_queryClient->query(query);
}

void DocumentQuery::resourceChanged (const Nepomuk2::Resource &resource)
{
    if(!resource.hasType(Nepomuk2::Vocabulary::NFO::PaginatedTextDocument()))
        return;

    QList<CachedRowEntry> newCache;

    CachedRowEntry cre;
    cre.displayColums = createDisplayData(resource);
    cre.decorationColums = createDecorationData(resource);
    cre.resource = resource;
    newCache.append(cre);

    emit updateCacheEntries(newCache);
}

QVariantList DocumentQuery::createDisplayData(const Nepomuk2::Resource & res) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Title: {
            Nepomuk2::Resource publication = res.property(Nepomuk2::Vocabulary::NBIB::publishedAs()).toResource();

            if(!publication.isValid()) {
                newEntry = QVariant();
            }
            else {
                QString titleSting = publication.property(Nepomuk2::Vocabulary::NIE::title()).toString();
                newEntry = titleSting;
            }
            break;
        }
        case Column_Date: {
            QString dateString;
            dateString = res.property(Nepomuk2::Vocabulary::NIE::lastModified()).toString();

            if(dateString.isEmpty()) {
                dateString = res.property(Nepomuk2::Vocabulary::NIE::created()).toString();
            }
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
        case Column_Author: {
            Nepomuk2::Resource publication = res.property(Nepomuk2::Vocabulary::NBIB::publishedAs()).toResource();

            if(!publication.isValid()) {
                newEntry = QVariant();
            }
            else {
                QString authorSting;
                QList<Nepomuk2::Resource> authorList;

                authorList = publication.property(Nepomuk2::Vocabulary::NCO::creator()).toResourceList();

                foreach(const Nepomuk2::Resource & a, authorList) {
                    authorSting.append(a.genericLabel());
                    authorSting.append(QLatin1String("; "));
                }
                authorSting.chop(2);
                newEntry = authorSting;
            }
            break;
        }
        case Column_FileName: {
            QString filenameString = res.property(Nepomuk2::Vocabulary::NFO::fileName()).toString();

            newEntry = filenameString;
            break;
        }
        case Column_Folder: {
            QString folderString = res.property(Nepomuk2::Vocabulary::NIE::url()).toString();
            QString filenameString = res.property(Nepomuk2::Vocabulary::NFO::fileName()).toString();

            folderString.remove(filenameString);
            folderString.remove(QLatin1String("file://"));
            folderString.replace(QRegExp(QLatin1String("/home/\\w*/")), QLatin1String("~/"));

            newEntry = folderString;
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

QVariantList DocumentQuery::createDecorationData(const Nepomuk2::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Publication: {
            Nepomuk2::Resource file = res.property(Nepomuk2::Vocabulary::NBIB::publishedAs()).toResource();
            if(file.isValid()) {
                newEntry = KIcon(QLatin1String("bookmarks-organize"));
            }
            break;
        }
        case Column_Reviewed:
            newEntry = KIcon(QLatin1String("dialog-ok-apply"));
            break;
        default:
            newEntry = QVariant();
        }

        decorationList.append(newEntry);
    }

    return decorationList;
}
