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

#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

NepomukModel::NepomukModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_library(0)
{
    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(addData(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(entriesRemoved(QList<QUrl>)), this, SLOT(removeData(QList<QUrl>)));
    connect(m_queryClient, SIGNAL(resultCount(int)), this, SLOT(resultCount(int)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(listingsFinished()));

//    m_resourceWatcher =  new Nepomuk::ResourceWatcher(this);

//    connect(m_resourceWatcher, SIGNAL(propertyAdded(Nepomuk::Resource, Nepomuk::Types::Property, QVariant)),
//            this, SLOT(updateCache(Nepomuk::Resource, Nepomuk::Types::Property, QVariant)));
//    connect(m_resourceWatcher, SIGNAL(propertyRemoved(Nepomuk::Resource, Nepomuk::Types::Property, QVariant)),
//            this, SLOT(updateCache(Nepomuk::Resource, Nepomuk::Types::Property, QVariant)));

    connect(this, SIGNAL(updateEntry(int)), this, SLOT(updateCacheEntry(int)));
}

NepomukModel::~NepomukModel()
{
    m_queryClient->close();
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

//        if(entryCache.lastModified != entryCache.resource.property(Soprano::Vocabulary::NAO::lastModified()).toString()) {
//            emit updateCacheEntry(index.row());
//            //entryCache = updateCacheEntry(index.row());
//            qDebug() << "cahce update requested for " << entryCache.resource.genericLabel();
//            qDebug() << entryCache.lastModified << entryCache.resource.property(Soprano::Vocabulary::NAO::lastModified()).toString();
//        }

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
}

void NepomukModel::setResourceType(ResourceSelection selection)
{
    m_selection = selection;
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

void NepomukModel::stopFetchData()
{
    m_queryClient->close();
}

void NepomukModel::removeSelectedFromProject(const QModelIndexList & indexes, Library *l)
{
    //    if(m_library->libraryType() == Library_System) {
    //        qWarning() << "try to remove data from the nepomuk system library @ PublicationModel::removeSelectedFromProject";
    //    }
    //    foreach(const QModelIndex & index, indexes) {
    //        // get the nepomuk data at the row
    //        Nepomuk::Resource nr = m_fileList.at(index.row());

    //        // remove project relation
    //        nr.removeProperty(Nepomuk::Vocabulary::PIMO::isRelated(), l->pimoLibrary());

    //        //Nepomuk query client will call the slot to remove the file from the index
    //    }
}

void NepomukModel::removeSelectedFromSystem(const QModelIndexList & indexes)
{
    //foreach(const QModelIndex & index, indexes) {
    // get the nepomuk data at the row
    //Nepomuk::Resource nr = m_fileList.at(index.row());

    //get all connected references
    //QList<Nepomuk::Resource> refList = nr.property(Nepomuk::Vocabulary::NBIB::)
    qWarning() << "TODO delete all references of the publication we are about to remove from nepomuk";
    // remove resource
    //nr.remove();
    //Nepomuk query client will call the slot to remove the file from the index
    //}
}

void NepomukModel::addData(const QList< Nepomuk::Query::Result > &entries)
{
    // qDebug() << "addData(...)" << entries.size();
    // take all results and prepare them for the entry cache
    QList<CachedRowEntry> cacheEntries = addToCache(entries);

    if(cacheEntries.size() > 0) {
        beginInsertRows(QModelIndex(), m_modelCacheData.size(), m_modelCacheData.size() + cacheEntries.size()-1);
        m_modelCacheData.append(cacheEntries);
        endInsertRows();
    }

    emit dataSizeChaged(m_modelCacheData.size());
}

void NepomukModel::removeData( const QList< QUrl > &entries )
{
    foreach(QUrl url, entries) {
        //iterate through the full list of entriues and find the one we are giong to remove
        int i = 0;
        int j = -1;
        foreach(const CachedRowEntry & cre, m_modelCacheData) {
            if(!cre.resource.isValid()) {
                j = i;
                break;
            }
            i++;
        }

        if(j >= 0) {
            beginRemoveRows(QModelIndex(), j, j );
            m_modelCacheData.removeAt(j);
            endRemoveRows();
        }
    }

    emit dataSizeChaged(m_modelCacheData.size());
}

void NepomukModel::resultCount(int number)
{
    if(number == 0) {
        emit updateFetchDataFor(m_selection,false, m_library);
    }
}

void NepomukModel::listingsFinished()
{
    qDebug() << "listingsFinished" << "added something? oO" << m_modelCacheData.size();

    emit updateFetchDataFor(m_selection,false, m_library);
}

void NepomukModel::listingsError(const QString & errorMessage)
{
    qDebug() << "query in rescourcemodel failed" << errorMessage;
}

QList<CachedRowEntry> NepomukModel::addToCache( const QList< Nepomuk::Query::Result > &entries )
{
    QList<CachedRowEntry> newCache;

    foreach(Nepomuk::Query::Result nqr, entries) {
        Nepomuk::Resource r = nqr.resource();
        CachedRowEntry cre;
        cre.displayColums = createDisplayData(r);
        cre.decorationColums = createDecorationData(r);
        cre.resource = r;
        cre.lastModified = r.property(Soprano::Vocabulary::NAO::lastModified()).toString();

        newCache.append(cre);

        QList<Nepomuk::Tag> tags = r.tags();
        foreach(Nepomuk::Tag t, tags) {
            hasTag(t.label());
        }
    }

    return newCache;
}

CachedRowEntry NepomukModel::updateCacheEntry(int entry)
{
    CachedRowEntry cre;
    Nepomuk::Resource r = m_modelCacheData.at(entry).resource;
    cre.displayColums = createDisplayData(r);
    cre.decorationColums = createDecorationData(r);
    cre.resource = r;
    cre.lastModified = r.property(Soprano::Vocabulary::NAO::lastModified()).toString();

    m_modelCacheData.replace(entry, cre);

    return cre;
}
