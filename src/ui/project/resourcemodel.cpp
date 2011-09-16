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
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NMO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Variant>

#include <QModelIndex>

#include <QDebug>

bool nameAscending(const Nepomuk::Resource &s1, const Nepomuk::Resource &s2)
{
    return s1.genericLabel() < s2.genericLabel();
}

bool nameDescending(const Nepomuk::Resource &s1, const Nepomuk::Resource &s2)
{
    return s1.genericLabel() > s2.genericLabel();
}

ResourceModel::ResourceModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(addData(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(entriesRemoved(QList<QUrl>)), this, SLOT(removeData(QList<QUrl>)));
}

ResourceModel::~ResourceModel()
{
    m_queryClient->close();
    delete m_queryClient;

    m_fileList.clear();
}

void ResourceModel::setProjectTag(Nepomuk::Tag tag)
{
    m_projectTag = tag;
}

void ResourceModel::setProject(Project *p)
{
    m_project = p;
}

void ResourceModel::setResourceType(ResourceSelection selection)
{
    m_selection = selection;
    startFetchData();
}

int ResourceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_fileList.size();
}

int ResourceModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

QVariant ResourceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_fileList.size() || index.row() < 0)
        return QVariant();

    Nepomuk::Resource document = m_fileList.at(index.row());

    if (role == Qt::DisplayRole) {
        if(index.column() == 2) {
            switch(m_selection)
            {
            case Resource_Mail:
            {
                Nepomuk::Variant nv = document.property(Nepomuk::Vocabulary::NMO::from());
                return nv.toResource().label();
            }
            default:
                Nepomuk::Variant nv = document.property(Nepomuk::Vocabulary::NBIB::Author());

                return nv.toResource().genericLabel();
            }
        }
        if(index.column() == 3) {
            QString nv = document.property(Nepomuk::Vocabulary::NBIB::Title()).toString();

            if(!nv.isEmpty()) {
                return nv;
            }
            else {
                return document.genericLabel();
            }
        }
    }

    if (role == Qt::DecorationRole) {
        switch(index.column()) {
        case 0:
            return KIcon(QLatin1String("dialog-ok-apply"));
        case 1:
            if(m_project->isInPath(document.property(Nepomuk::Vocabulary::NIE::url()).toString())) {
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
            return tr("Document Data");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case 0:
            return tr("Reviewed");
        case 1:
            return tr("File in Project path available");
        case 2:
            return tr("Document Author");
        case 3:
            return tr("Document Data");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

void ResourceModel::startFetchData()
{
    Nepomuk::Query::AndTerm andTerm;

    switch(m_selection)
    {
    case Resource_Document:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Document() ) );
        break;
    case Resource_Mail:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NMO::Email() ) );
        break;
    case Resource_Media:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Media() ) );
        break;
    case Resource_Website:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::PlainTextDocument() ) );
        break;
    case Resource_Reference:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::BibReference() ) );
        break;
    }

    andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
                                                        Nepomuk::Query::ResourceTerm( m_projectTag ) ) );

    // build the query
    Nepomuk::Query::Query query( andTerm );
    m_queryClient->query(query);
}

void ResourceModel::stopFetchData()
{
    m_queryClient->close();
}

void ResourceModel::addData(const QList< Nepomuk::Query::Result > &entries)
{
    // two loops are necessary because addData is not only called on new entries, but with all changes
    // must be a bug in nepomuk
    int insertItems = 0;
    foreach(Nepomuk::Query::Result r, entries) {
        if( !m_fileList.contains(r.resource()) ) {
            insertItems++;
        }
    }

    if(insertItems > 0) {
        beginInsertRows(QModelIndex(), m_fileList.size(), m_fileList.size());
        foreach(Nepomuk::Query::Result r, entries) {
            if( !m_fileList.contains(r.resource()) ) {
                m_fileList.append(r.resource());
            }
        }
        endInsertRows();
    }
}

void ResourceModel::removeData( const QList< QUrl > &entries )
{
    // two loops are necessary because removeData is not only called on removed entries, but with all changes
    // must be a bug in nepomuk

    Nepomuk::Resource muh(entries.first());
    if(muh.tags().contains(m_projectTag)) {
        return;
    }

    int removedRow  = m_fileList.indexOf(muh);

    beginRemoveRows(QModelIndex(), removedRow, removedRow);
    m_fileList.removeOne(muh);
    endRemoveRows();
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
