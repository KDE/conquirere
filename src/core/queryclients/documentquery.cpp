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
#include "library.h"

#include "globals.h"

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
#include <Nepomuk/Vocabulary/NFO>
#include <Soprano/Vocabulary/NAO>

#include <QtCore/QRegExp>

DocumentQuery::DocumentQuery(QObject *parent) :
    QueryClient(parent)
{
}

void DocumentQuery::startFetchData()
{
    Nepomuk::Query::OrTerm mainOrTerm;
    Nepomuk::Query::AndTerm andTerm;

    andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::PaginatedTextDocument() ) );

    if(m_library->libraryType() == Library_Project) {
        Nepomuk::Query::OrTerm orTerm;
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
                                                           Nepomuk::Query::ResourceTerm( m_library->pimoTag() ) ));
        orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
                                                           Nepomuk::Query::ResourceTerm(m_library->pimoLibrary()) ) );
        andTerm.addSubTerm(orTerm);


        // now we also add all rescources with nfo:FileDataObject and isRelated() for the project too
        Nepomuk::Query::AndTerm subAndTerm;
        subAndTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::FileDataObject() ) );
        subAndTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
                                                               Nepomuk::Query::ResourceTerm(m_library->pimoLibrary()) ) );

        mainOrTerm.addSubTerm(subAndTerm);
    }

    mainOrTerm.addSubTerm(andTerm);

    // build the query
    Nepomuk::Query::Query query( mainOrTerm );
    m_queryClient->query(query);
}

void DocumentQuery::resourceChanged (const Nepomuk::Resource &resource)
{
    if(!resource.hasType(Nepomuk::Vocabulary::NFO::PaginatedTextDocument()))
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

QVariantList DocumentQuery::createDisplayData(const Nepomuk::Resource & res) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Title: {
            Nepomuk::Resource publication = res.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();

            if(!publication.isValid()) {
                newEntry = QVariant();
            }
            else {
                QString titleSting = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
                newEntry = titleSting;
            }
            break;
        }
        case Column_Date: {
            QString dateSting;
            dateSting = res.property(Nepomuk::Vocabulary::NIE::lastModified()).toString();

            if(dateSting.isEmpty()) {
                dateSting = res.property(Nepomuk::Vocabulary::NIE::created()).toString();
            }
            newEntry = dateSting;
            break;
        }
        case Column_Author: {
            Nepomuk::Resource publication = res.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();

            if(!publication.isValid()) {
                newEntry = QVariant();
            }
            else {
                QString authorSting;
                QList<Nepomuk::Resource> authorList;

                authorList = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();

                foreach(const Nepomuk::Resource & a, authorList) {
                    authorSting.append(a.genericLabel());
                    authorSting.append(QLatin1String("; "));
                }
                authorSting.chop(2);
                newEntry = authorSting;
            }
            break;
        }
        case Column_FileName: {
            QString filenameString = res.property(Nepomuk::Vocabulary::NFO::fileName()).toString();

            newEntry = filenameString;
            break;
        }
        case Column_Folder: {
            QString folderString = res.property(Nepomuk::Vocabulary::NIE::url()).toString();
            QString filenameString = res.property(Nepomuk::Vocabulary::NFO::fileName()).toString();

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

QVariantList DocumentQuery::createDecorationData(const Nepomuk::Resource & res) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Publication: {
            Nepomuk::Resource file = res.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();
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
