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

#include "nepomukmodel.h"

#include "library.h"

NepomukModel::NepomukModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_library(0)
    , m_queryClient(0)
{
}

NepomukModel::~NepomukModel()
{
    m_queryClient->quit();
    delete m_queryClient;

    m_modelCacheData.clear();
}

int NepomukModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_modelCacheData.size();
}

QVariant NepomukModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_modelCacheData.size() || index.row() < 0) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        CachedRowEntry entryCache = m_modelCacheData.at(index.row());

        QVariantList columnList = entryCache.displayColums;

        return columnList.at(index.column());
    }

    else if (role == Qt::DecorationRole) {
        CachedRowEntry entryCache = m_modelCacheData.at(index.row());
        QVariantList columnList = entryCache.decorationColums;

        return columnList.at(index.column());
    }

    return QVariant();
}

void NepomukModel::setLibrary(Library *library)
{
    m_library = library;

    if(m_library->libraryType() == Library_Project) {
        m_queryClient->setPimoProject(m_library->pimoLibrary());
    }
}

Nepomuk::Resource NepomukModel::documentResource(const QModelIndex &selection)
{
    Nepomuk::Resource ret;

    if(!m_modelCacheData.isEmpty() && selection.row() >= 0) {
        CachedRowEntry entryCache = m_modelCacheData.at(selection.row());
        ret = entryCache.resource;
    }

    return ret;
}

void NepomukModel::startFetchData()
{
    Q_ASSERT(m_queryClient);

    emit queryStarted();
    m_queryClient->startFetchData();
}

void NepomukModel::stopFetchData()
{
    Q_ASSERT(m_queryClient);

    m_queryClient->stopFetchData();
}

void NepomukModel::addCacheData(const QList<CachedRowEntry> &entries)
{
    if(entries.size() > 0) {
        beginInsertRows(QModelIndex(), m_modelCacheData.size(), m_modelCacheData.size() + entries.size()-1);
        m_modelCacheData.append(entries);
        endInsertRows();
    }

    foreach(CachedRowEntry cre, entries) {
        emit resourceAdded(cre.resource);
    }

    emit dataSizeChaged(m_modelCacheData.size());
}

void NepomukModel::removeCacheData( QList<QUrl> urls )
{
    foreach(QUrl url, urls) {
        //iterate through the full list of entries and find the one we are going to remove
        int i = 0;
        foreach(const CachedRowEntry & cre, m_modelCacheData) {
            if(!cre.resource.isValid()) {

                beginRemoveRows(QModelIndex(), i, i );
                m_modelCacheData.removeAt(i);
                endRemoveRows();

                break;
            }
            i++;
        }

        emit resourceRemoved(url);
    }

    emit dataSizeChaged(m_modelCacheData.size());
}

void NepomukModel::updateCacheData(const QList<CachedRowEntry> &entries)
{
    qDebug() << "update cahce data";

    foreach(CachedRowEntry entry, entries) {

        // go through all cached entries and find the right resource
        int i = 0;
        foreach(CachedRowEntry cre, m_modelCacheData) {
            if(cre.resource.uri() == entry.resource.uri()) {
                m_modelCacheData.replace(i, cre);
                emit resourceUpdated(cre.resource);
            }
            i++;
        }

    }
}

