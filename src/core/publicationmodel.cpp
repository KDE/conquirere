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

#include "publicationmodel.h"
#include "core/library.h"

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

#include <QDebug>

enum ColumnList {
    Column_Reviewed,
    Column_FileAvailable,
    Column_CiteKey,
    Column_Author,
    Column_Title,
    Column_Date,
    Column_Publisher,
    Column_Editor
};

PublicationModel::PublicationModel(QObject *parent)
    : NepomukModel(parent)
    , m_library(0)
{
    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(addData(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(entriesRemoved(QList<QUrl>)), this, SLOT(removeData(QList<QUrl>)));
    connect(m_queryClient, SIGNAL(resultCount(int)), this, SLOT(resultCount(int)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(listingsFinished()));
}

PublicationModel::~PublicationModel()
{
    m_queryClient->close();
    delete m_queryClient;

    m_fileList.clear();
}

void PublicationModel::setLibrary(Library *library)
{
    m_library = library;
}

void PublicationModel::setResourceType(ResourceSelection selection)
{
    m_selection = selection;
}

int PublicationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_fileList.size();
}

int PublicationModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 8;
}

QVariant PublicationModel::data(const QModelIndex &index, int role) const
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
            QString authorSting;
            QList<Nepomuk::Resource> authorList;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();
                authorList = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();
            }
            else {
                authorList = document.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();
            }
            foreach(Nepomuk::Resource a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            return authorSting;
        }
        else if(index.column() == Column_Title) {
            QString titleSting;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();
                titleSting = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
            }
            else {
                titleSting = document.property(Nepomuk::Vocabulary::NIE::title()).toString();
            }

            return titleSting;
        }
        else if(index.column() == Column_Date) {
            QString dateSting;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();
                dateSting = publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();
            }
            else {
                dateSting = document.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();
            }

            return dateSting;
        }
        else if(index.column() == Column_Publisher) {
            QString authorSting;
            QList<Nepomuk::Resource> authorList;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();
                authorList = publication.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();
            }
            else {
                authorList = document.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();
            }
            foreach(Nepomuk::Resource a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            return authorSting;
        }
        else if(index.column() == Column_Editor) {
            QString authorSting;
            QList<Nepomuk::Resource> authorList;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();
                authorList = publication.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
            }
            else {
                authorList = document.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
            }
            foreach(Nepomuk::Resource a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            return authorSting;
        }
        else if(index.column() == Column_CiteKey) {
            QString titleSting;

            if(m_selection != Resource_Reference) {
                //TODO find citykey of a Publication
                return titleSting;
                //                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();
                //                titleSting = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
            }
            else {
                titleSting = document.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
            }

            return titleSting;

        }
        // the next two are availabel as display role only
        //        else if(index.column() == Column_FileAvailable) {

        //        }
        //        else if(index.column() == Column_Reviewed) {

        //        }
    }

    if (role == Qt::DecorationRole) {
        if(index.column() == Column_FileAvailable) {
            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();
                Nepomuk::Resource file = publication.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();
                if(file.isValid()) {
                    return  KIcon(QLatin1String("bookmarks-organize"));
                }
            }
            else {
                Nepomuk::Resource file = document.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();
                if(file.isValid()) {
                    return  KIcon(QLatin1String("bookmarks-organize"));
                }
            }
        }
        else if(index.column() == Column_Reviewed) {
            return KIcon(QLatin1String("dialog-ok-apply"));
        }
    }

    return QVariant();
}

QVariant PublicationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DecorationRole) {
        switch (section) {
        case Column_Reviewed:
            return KIcon(QLatin1String("document-edit-verify"));
        case Column_FileAvailable:
            return KIcon(QLatin1String("bookmarks-organize"));
        default:
            return QVariant();
        }
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case Column_Reviewed:
            return QVariant();
        case Column_FileAvailable:
            return QVariant();
        case Column_Author:
            return tr("Author");
        case Column_Title:
            return tr("Title");
        case Column_Date:
            return tr("Date");
        case Column_Publisher:
            return tr("Publisher");
        case Column_Editor:
            return tr("Editor");
        case Column_CiteKey:
            return tr("Citekey");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case Column_Reviewed:
            return i18n("Reviewed");
        case Column_FileAvailable:
            return i18n("File available");
        case Column_Author:
            return  tr("The author of the document");
        case Column_Title:
            return i18n("The document title");
        case Column_Date:
            return tr("The date of publishing");
        case Column_Publisher:
            return  i18n("The publisher of the document");
        case Column_Editor:
            return  i18n("The editor of the document");
        case Column_CiteKey:
            return i18n("Citekey");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

void PublicationModel::startFetchData()
{
    emit updatefetchDataFor(m_selection,true, m_library);

    Nepomuk::Query::AndTerm andTerm;

    switch(m_selection)
    {
    case Resource_Document: {
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Document() ) );

        // exclude source code
        // is not interresting here and slows down way to much
        andTerm.addSubTerm(  !Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::SourceCode() ) );
    }
        break;
    case Resource_Reference:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Reference() ) );
        break;
    case Resource_Publication:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Publication() ) );
        break;
    }

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

void PublicationModel::stopFetchData()
{
    m_queryClient->close();
}

void PublicationModel::addData(const QList< Nepomuk::Query::Result > &entries)
{
    //qDebug() << "addData(...)" << entries.size();
    // two loops are necessary because addData is not only called on new entries, but with all changes
    // must be a bug in nepomuk
    QList< Nepomuk::Resource > newEntries;
    foreach(Nepomuk::Query::Result r, entries) {
        if( !m_fileList.contains(r.resource()) ) {
            newEntries.append(r.resource());
        }
    }

    if(newEntries.size() > 0) {
        beginInsertRows(QModelIndex(), m_fileList.size(), m_fileList.size() + newEntries.size()-1);
        m_fileList.append(newEntries);
        endInsertRows();
    }

    emit dataSizeChaged(m_fileList.size());

}

void PublicationModel::removeData( const QList< QUrl > &entries )
{
    // remove data is a bug by default
    // must change inplementation in the nepomuk query service

    // we just search through all data and remove the ones that are not valid anymore
    // this function gets called when new entries are created, some are removed or modified
    // sooner or later all deleted entries will be deleted

    QList<int> noValidEntries;
    for(int i = 0; i < m_fileList.size(); i++) {
        if(!m_fileList.at(i).isValid()) {
            noValidEntries.append(i);
        }
    }

    if(noValidEntries.isEmpty()) {
        return;
    }

    qDebug() << "remove values" << noValidEntries << "list size" << m_fileList.size();
    qDebug() << noValidEntries.first() << noValidEntries.last();
    beginRemoveRows(QModelIndex(), noValidEntries.first(), noValidEntries.last() );
    for(int j = 0; j < noValidEntries.size(); j++) {
        m_fileList.removeAt(noValidEntries.at(j) - j);
    }
    endRemoveRows();

    emit dataSizeChaged(m_fileList.size());
}

Nepomuk::Resource PublicationModel::documentResource(const QModelIndex &selection)
{
    return m_fileList.at(selection.row());
}

void PublicationModel::removeSelected(const QModelIndexList & indexes)
{
    if(m_library->libraryType() == Library_System) {
        qWarning() << "try to remove data from the nepomuk system library @ PublicationModel::removeSelected";
    }
    foreach(QModelIndex index, indexes) {
        // get the nepomuk data at the row
        Nepomuk::Resource nr = m_fileList.at(index.row());

        // remove project relation
        nr.removeProperty(Nepomuk::Vocabulary::PIMO::isRelated(), m_library->pimoLibrary());

        //Nepomuk query client will call the slot to remove the file from the index
    }
}

void PublicationModel::resultCount(int number)
{
    if(number == 0) {
        emit updatefetchDataFor(m_selection,false, m_library);
    }
}

void PublicationModel::listingsFinished()
{
    qDebug() << "listingsFinished" << "added something? oO" << m_fileList.size();

    emit updatefetchDataFor(m_selection,false, m_library);
}

void PublicationModel::listingsError(const QString & errorMessage)
{
    qDebug() << "query in rescourcemodel failed" << errorMessage;
}
