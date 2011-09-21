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

#include "resourcemodel.h"

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

bool nameAscending(const Nepomuk::Resource &s1, const Nepomuk::Resource &s2)
{
    QString title1 = s1.property(Nepomuk::Vocabulary::NIE::title()).toString();
    QString title2 = s2.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(title1.isEmpty()) {
        title1 = s1.genericLabel();
    }
    if(title2.isEmpty()) {
        title2 = s2.genericLabel();
    }

    return title1 < title2;
}

bool nameDescending(const Nepomuk::Resource &s1, const Nepomuk::Resource &s2)
{
    QString title1 = s1.property(Nepomuk::Vocabulary::NIE::title()).toString();
    QString title2 = s2.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(title1.isEmpty()) {
        title1 = s1.genericLabel();
    }
    if(title2.isEmpty()) {
        title2 = s2.genericLabel();
    }

    return title1 > title2;
}

ResourceModel::ResourceModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_project(0)
{
    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(addData(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(entriesRemoved(QList<QUrl>)), this, SLOT(removeData(QList<QUrl>)));
    connect(m_queryClient, SIGNAL(resultCount(int)), this, SLOT(resultCount(int)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(listingsFinished()));
}

ResourceModel::~ResourceModel()
{
    m_queryClient->close();
    delete m_queryClient;

    m_fileList.clear();
}

void ResourceModel::setProject(Project *p)
{
    m_project = p;
}

void ResourceModel::setResourceType(ResourceSelection selection)
{
    m_selection = selection;
}

int ResourceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_fileList.size();
}

int ResourceModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 5;
}

QVariant ResourceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_fileList.size() || index.row() < 0)
        return QVariant();

    Nepomuk::Resource document = m_fileList.at(index.row());

    if(!document.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {

        // Display Author
        if(index.column() == 2) {
            switch(m_selection) {
            case Resource_Mail:
            {
                Nepomuk::Variant nv = document.property(Nepomuk::Vocabulary::NMO::from());
                return nv.toResource().label();
            }
            case Resource_Reference:
            {
                QString authorSting;
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();
                QList<Nepomuk::Resource> authorList = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();

                foreach(Nepomuk::Resource a, authorList) {
                    authorSting.append(a.genericLabel());
                    authorSting.append(QLatin1String("; "));
                }
                authorSting.chop(2);

                return authorSting;
            }
            case Resource_Document:
            {
                QString authorSting;
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();
                QList<Nepomuk::Resource> authorList = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();

                foreach(Nepomuk::Resource a, authorList) {
                    authorSting.append(a.genericLabel());
                    authorSting.append(QLatin1String("; "));
                }
                authorSting.chop(2);

                return authorSting;
            }
            default:
                QString authorSting;
                QList<Nepomuk::Resource> authorList = document.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();

                foreach(Nepomuk::Resource a, authorList) {
                    authorSting.append(a.genericLabel());
                    authorSting.append(QLatin1String("; "));
                }
                authorSting.chop(2);

                return authorSting;
            }
        }

        // Display Titel
        if(index.column() == 3) {
            switch(m_selection) {
            case Resource_Mail:
            {
                Nepomuk::Resource mail = document.property(Nepomuk::Vocabulary::NMO::messageFrom()).toResource();
                return mail.genericLabel();
            }
            case Resource_Reference:
            {
                QString titleString;
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();
                titleString = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();

                if(titleString.isEmpty()) {
                    Nepomuk::Resource fileDocument = publication.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResource();

                    if(fileDocument.isValid()) {
                        titleString = fileDocument.genericLabel();
                    }
                    else {
                        titleString = i18n("publication not named");
                    }
                }

                return titleString;
            }
            case Resource_Document:
            {
                Nepomuk::Resource bibResource = document.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();
                if(bibResource.isValid()) {
                    return bibResource.genericLabel();
                }
                else {
                    return document.genericLabel();
                }
            }
            default:
                return document.genericLabel();
            }
        }

        // display date (creation or published)
        if(index.column() == 4) {
            switch(m_selection) {
            case Resource_Mail:
            {
                QString date = document.property(Nepomuk::Vocabulary::NMO::receivedDate()).toString();
                return date;
            }
            case Resource_Reference:
            {
                QString dateString;
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();
                dateString = publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();

                if(dateString.isEmpty()) {
                    Nepomuk::Resource fileDocument = publication.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResource();

                    if(fileDocument.isValid()) {
                        dateString = fileDocument.property(Nepomuk::Vocabulary::NIE::contentLastModified()).toString();
                    }
                }

                return dateString;
            }
            case Resource_Document:
            {
                QString dateString;

                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();
                if(publication.isValid()) {
                    dateString = publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();

                    if(!dateString.isEmpty()) {
                        return dateString;
                    }
                }
                return document.property(Nepomuk::Vocabulary::NIE::contentLastModified()).toString();

            }
            default:
                return document.property(Nepomuk::Vocabulary::NIE::contentLastModified()).toString();
            }
        }
    }

    if (role == Qt::DecorationRole) {
        switch(index.column()) {
        case 0: //display "reviewed icon"
            return KIcon(QLatin1String("dialog-ok-apply"));
        case 1: //display "is file availabe either in the system or even in project path"
            if(m_project && m_project->isInPath(document.property(Nepomuk::Vocabulary::NIE::url()).toString())) {
                return  KIcon(QLatin1String("bookmarks-organize"));
            }
        }
    }

    return QVariant();
}

QVariant ResourceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DecorationRole) {
        switch (section) {
        case 0:
            return KIcon(QLatin1String("document-edit-verify"));
        case 1:
            return KIcon(QLatin1String("bookmarks-organize"));
        default:
            return QVariant();
        }
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return QVariant();
        case 1:
            return QVariant();
        case 2:
            return tr("Author");
        case 3:
            return tr("Title");
        case 4:
            return tr("Date");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case 0:
            return tr("Reviewed");
        case 1:
            return tr("File available (Either in the project folder or on the system)");
        case 2:
            return tr("The Author of the document");
        case 3:
            return tr("The document title");
        case 4:
            return tr("The date of creation or publishing");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

void ResourceModel::startFetchData()
{
    if(m_project) {
        emit updatefetchDataFor(Library_Project,m_selection,true);
    }
    else {
        emit updatefetchDataFor(Library_System,m_selection,true);
    }

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
    case Resource_Mail:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NMO::Email() ) );
        break;
    case Resource_Media:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Media() ) );
        break;
    case Resource_Website:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Website() ) );
        break;
    case Resource_Reference:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::BibReference() ) );
        break;
    case Resource_Publication:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Publication() ) );
        break;
    }

    if(m_project) {
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
                                                            Nepomuk::Query::ResourceTerm(m_project->pimoProject()) ) );
    }

    //sort result by edit date to get only the newest if we have to many results
    //Nepomuk::Query::ComparisonTerm term(Soprano::Vocabulary::NAO::lastModified(), Nepomuk::Query::Term());
    //term.setSortWeight(1, Qt::DescendingOrder);
    //andTerm.addSubTerm(term);

    // build the query
    Nepomuk::Query::Query query( andTerm );
    query.setLimit(100);
    m_queryClient->query(query);

}

void ResourceModel::stopFetchData()
{
    m_queryClient->close();
}

void ResourceModel::addData(const QList< Nepomuk::Query::Result > &entries)
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

void ResourceModel::removeData( const QList< QUrl > &entries )
{
    //qDebug() << "remove data :: " << entries;
    // two loops are necessary because removeData is not only called on removed entries, but with all changes
    // must be a bug in nepomuk
    //    Nepomuk::Resource muh(entries.first());
    //    Nepomuk::Resource relatesTo = muh.property( Nepomuk::Vocabulary::PIMO::isRelated()).toResource();

    //    if ( relatesTo == m_project->pimoProject()) {
    //        return;
    //    }

    //    int removedRow  = m_fileList.indexOf(muh);

    //    beginRemoveRows(QModelIndex(), removedRow, removedRow);
    //    m_fileList.removeOne(muh);
    //    endRemoveRows();

    //    emit dataSizeChaged(m_fileList.size());
}

void ResourceModel::sort ( int column, Qt::SortOrder order )
{
    Q_UNUSED(column);

    if(order == Qt::AscendingOrder) {
        qSort(m_fileList.begin(), m_fileList.end(), nameAscending);
    }
    else {
        qSort(m_fileList.begin(), m_fileList.end(), nameDescending);
    }

    dataChanged(index(0,0), index(m_fileList.size(),3));
}

Nepomuk::Resource ResourceModel::documentResource(const QModelIndex &selection)
{
    return m_fileList.at(selection.row());
}

void ResourceModel::removeSelected(const QModelIndexList & indexes)
{
    // ignore removal of system wide documents
    if(!m_project)
        return;

    foreach(QModelIndex index, indexes) {
        // get the nepomuk data at the row
        Nepomuk::Resource nr = m_fileList.at(index.row());

        // remove project tag
        nr.removeProperty(Nepomuk::Vocabulary::PIMO::isRelated(), m_project->pimoProject());

        //Nepomuk query client will call the slot to remove the file from the index
    }
}

void ResourceModel::resultCount(int number)
{
    if(number == 0) {
        if(m_project) {
            emit updatefetchDataFor(Library_Project,m_selection,false);
        }
        else {
            emit updatefetchDataFor(Library_System,m_selection,false);
        }
    }
}

void ResourceModel::listingsFinished()
{
    //BUG this is never called from the query client.
    qDebug() << "listingsFinished" << "added something? oO" << m_fileList.size();
    if(m_project) {
        emit updatefetchDataFor(Library_Project,m_selection,false);
    }
    else {
        emit updatefetchDataFor(Library_System,m_selection,false);
    }
}

void ResourceModel::listingsError(const QString & errorMessage)
{
    qDebug() << errorMessage;

    if(m_project) {
        emit updatefetchDataFor(Library_Project,m_selection,false);
    }
    else {
        emit updatefetchDataFor(Library_System,m_selection,false);
    }
}
