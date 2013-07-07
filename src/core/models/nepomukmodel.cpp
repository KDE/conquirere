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

#include "../library.h"
#include "../projectsettings.h"
#include "config/conquirere.h"

#include <KDE/KStandardDirs>

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QThread>

#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>

NepomukModel::NepomukModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_library(0)
    , m_queryClient(0)
{
    setObjectName("abstract");
}

NepomukModel::~NepomukModel()
{
    m_modelCacheData.clear();
    m_lookupCache.clear();

    m_queryClient->deleteLater();
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

    else if(role == Qt::UserRole + 1) { //TODO: define fixed unum for the resourceType role
        CachedRowEntry entryCache = m_modelCacheData.at(index.row());
        return entryCache.resourceType;
    }

    return QVariant();
}

QList<int> NepomukModel::fixedWidthSections() const
{
    QList<int> emptylist;
    return emptylist;
}

void NepomukModel::setLibrary(Library *library)
{
    m_library = library;
    m_queryClient->setLibrary(m_library);
}

void NepomukModel::startFetchData()
{
    emit queryStarted();
    m_queryClient->startFetchData();
}

Nepomuk2::Resource NepomukModel::documentResource(const QModelIndex &selection)
{
    Nepomuk2::Resource ret;

    if(!m_modelCacheData.isEmpty() && selection.row() >= 0) {
        CachedRowEntry entryCache = m_modelCacheData.at(selection.row());
        ret = entryCache.resource;
    }

    return ret;
}

void NepomukModel::addCacheData(const QList<CachedRowEntry> &entries)
{
    kDebug() << "add" << entries.size() << "new entries to model" << objectName() << ". Current size" << m_modelCacheData.size();

    if(entries.size() > 0) {
        beginInsertRows(QModelIndex(), m_modelCacheData.size(), m_modelCacheData.size() + entries.size()-1);
        foreach(const CachedRowEntry &cre, entries) {
            m_modelCacheData.append(cre);
            m_lookupCache.insert(cre.resource.uri().toString(), m_modelCacheData.size()-1);
        }
        endInsertRows();
    }

    emit dataSizeChaged(m_modelCacheData.size());
}

void NepomukModel::removeCacheData( QList<QUrl> urls )
{
    kDebug() << "removeCacheData" << urls;
    foreach(const QUrl &url, urls) {
        //iterate through the full list of entries and find the one we are going to remove
        int i = 0;
        foreach(const CachedRowEntry & cre, m_modelCacheData) {
            if(!cre.resource.isValid() || cre.resource.uri() == url) {

                beginRemoveRows(QModelIndex(), i, i );
                m_modelCacheData.removeAt(i);
                endRemoveRows();

                // in order to keep the cache lookup right, replace all indexes
                QMapIterator<QString, int> j(m_lookupCache);
                 while (j.hasNext()) {
                     j.next();
                     int index = j.value();
                     if(index >= i) {
                         index--;
                         m_lookupCache.insert(j.key(), index);
                     }
                 }

                break;
            }
            i++;
        }
    }

    emit dataSizeChaged(m_modelCacheData.size());
}

void NepomukModel::updateCacheData(const QList<CachedRowEntry> &entries)
{
    kDebug() << "updateCacheData";
    foreach(const CachedRowEntry &entry, entries) {

        int pos = m_lookupCache.value(entry.resource.uri().toString(), -1);
        if(pos < 0) {
            QList<CachedRowEntry> e;
            e << entry;
            addCacheData(e);
        }
        else {
            m_modelCacheData.replace(pos, entry);
            emit dataChanged(index(pos,0), index(pos,columnCount()));
        }
    }
}

