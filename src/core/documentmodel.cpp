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

enum ColumnList {
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

QVariant DocumentModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_fileList.size() || index.row() < 0)
        return QVariant();

    Nepomuk::Resource document = m_fileList.at(index.row());

    if(!document.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        if(index.column() == Column_Author) {
            Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();

            if(!publication.isValid()) {
                return QVariant();
            }

            QString authorSting;
            QList<Nepomuk::Resource> authorList;

            authorList = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();

            foreach(Nepomuk::Resource a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            return authorSting;
        }
        else if(index.column() == Column_Title) {
            Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();

            if(!publication.isValid()) {
                return QVariant();
            }

            QString titleSting = document.property(Nepomuk::Vocabulary::NIE::title()).toString();

            return titleSting;
        }
        else if(index.column() == Column_Date) {
            QString dateSting;
            dateSting = document.property(Nepomuk::Vocabulary::NIE::lastModified()).toString();

            if(dateSting.isEmpty()) {
                dateSting = document.property(Nepomuk::Vocabulary::NIE::created()).toString();
            }

            return dateSting;
        }
        else if(index.column() == Column_FileName) {
            QString filenameString;

            filenameString = document.property(Nepomuk::Vocabulary::NFO::fileName()).toString();

            return filenameString;
        }
        else if(index.column() == Column_Folder) {
            QString folderString;

            folderString = document.property(Nepomuk::Vocabulary::NIE::url()).toString();
            QString filenameString = document.property(Nepomuk::Vocabulary::NFO::fileName()).toString();

            folderString.remove(filenameString);
            folderString.remove(QLatin1String("file://"));
            folderString.replace(QRegExp(QLatin1String("/home/\\w*/")), QLatin1String("~/"));

            return folderString;
        }
    }

    if (role == Qt::DecorationRole) {
        if(index.column() == Column_Publication) {
            Nepomuk::Resource file = document.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();
            if(file.isValid()) {
                return  KIcon(QLatin1String("bookmarks-organize"));
            }
        }
        else if(index.column() == Column_Reviewed) {
            return KIcon(QLatin1String("dialog-ok-apply"));
        }
    }

    return QVariant();
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

    andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Document() ) );

    // exclude source code
    // is not interresting here and slows down way to much
    andTerm.addSubTerm(  !Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::SourceCode() ) );


    if(m_library->libraryType() != Library_System) {
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
                                                            Nepomuk::Query::ResourceTerm(m_library->pimoLibrary()) ) );
    }

    //sort result by edit date to get only the newest if we have to many results
    //Nepomuk::Query::ComparisonTerm term(Soprano::Vocabulary::NAO::lastModified(), Nepomuk::Query::Term());
    //term.setSortWeight(1, Qt::DescendingOrder);
    //andTerm.addSubTerm(term);

    // build the query
    Nepomuk::Query::Query query( andTerm );
    //query.setLimit(100);
    m_queryClient->query(query);
}
