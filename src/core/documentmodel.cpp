/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "documentmodel.h"
#include "library.h"

#include "nbib.h"

#include <KDE/KIcon>

#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/NegationTerm>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NMO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Variant>

#include <QModelIndex>
#include <QRegExp>

#include <QDebug>

enum PublicationColumnList {
    Column_StarRate,
    Column_Reviewed,
    Column_Publication,
    Column_Author,
    Column_Title,
    Column_FileName,
    Column_Folder,
    Column_Date,

    Max_columns
};

DocumentModel::DocumentModel(QObject *parent)
    : NepomukModel(parent)
{
}

DocumentModel::~DocumentModel()
{
}

int DocumentModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Max_columns;
}

QVariant DocumentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DecorationRole) {
        switch (section) {
        case Column_Reviewed:
            return KIcon(QLatin1String("document-edit-verify"));
        case Column_Publication:
            return KIcon(QLatin1String("bookmarks-organize"));
        default:
            return QVariant();
        }
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case Column_Reviewed:
            return QVariant();
        case Column_Publication:
            return QVariant();
        case Column_Author:
            return  i18n("Author");
        case Column_Title:
            return i18n("Title");
        case Column_Date:
            return i18n("Date");
        case Column_FileName:
            return  i18n("Filename");
        case Column_Folder:
            return  i18n("Folder");
        case Column_StarRate:
            return i18n("Rating");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case Column_Reviewed:
            return i18n("Reviewed");
        case Column_Publication:
            return i18n("Publication available");
        case Column_Author:
            return  i18n("The author of the document");
        case Column_Title:
            return i18n("The document title");
        case Column_Date:
            return i18n("The date of publishing or file creation");
        case Column_FileName:
            return  i18n("The name of the file");
        case Column_Folder:
            return  i18n("The foldername of the file");
        case Column_StarRate:
            return i18n("Rating");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

void DocumentModel::startFetchData()
{
    emit updateFetchDataFor(m_selection,true, m_library);

    Nepomuk::Query::AndTerm andTerm;

    andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::PaginatedTextDocument() ) );

    if(m_library->libraryType() != Library_System) {
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
                                                            Nepomuk::Query::ResourceTerm(m_library->pimoLibrary()) ) );
    }

    // build the query
    Nepomuk::Query::Query query( andTerm );
    m_queryClient->query(query);
}

QVariantList DocumentModel::createDisplayData(const Nepomuk::Resource & res)
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
                QString titleSting = res.property(Nepomuk::Vocabulary::NIE::title()).toString();

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

QVariantList DocumentModel::createDecorationData(const Nepomuk::Resource & res)
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
