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
#include <QtCore/QFutureWatcher>
#include <QtCore/QThread>
#include <QtCore/QtConcurrentRun>

NepomukModel::NepomukModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_library(0)
    , m_queryFinished(false)
    , m_activeFutureWatchers(0)
{
    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(addData(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(entriesRemoved(QList<QUrl>)), this, SLOT(removeData(QList<QUrl>)));
    connect(m_queryClient, SIGNAL(resultCount(int)), this, SLOT(resultCount(int)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(listingsFinished()));
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

void NepomukModel::removeSelectedFromProject(const QModelIndex & index, Library *l)
{
    if(m_library->libraryType() == Library_System) {
        qWarning() << "try to remove data from the nepomuk system library @ PublicationModel::removeSelectedFromProject";
    }
    // get the nepomuk data at the row
    Nepomuk::Resource nr = m_modelCacheData.at(index.row()).resource;

    // remove project relation
    nr.removeProperty(Nepomuk::Vocabulary::PIMO::isRelated(), l->pimoLibrary());

    //Nepomuk query client will call the slot to remove the file from the index
}

void NepomukModel::removeSelectedFromSystem(const QModelIndex & index)
{
    // get the nepomuk data at the row
    Nepomuk::Resource nr =  m_modelCacheData.at(index.row()).resource;

    //get all connected references
    QList<Nepomuk::Resource> refList = nr.property(Nepomuk::Vocabulary::NBIB::reference()).toResourceList();

    foreach(Nepomuk::Resource r, refList) {
        r.remove();
    }

    // remove resource
    nr.remove();
    //Nepomuk query client will call the slot to remove the file from the index
}

void NepomukModel::addData(const QList< Nepomuk::Query::Result > &entries)
{
    // start background thread the data
    QFuture<QList<CachedRowEntry> > future = QtConcurrent::run(this, &NepomukModel::addToCache, entries);

    QFutureWatcher<QList<CachedRowEntry> > *futureWatcher = new QFutureWatcher<QList<CachedRowEntry> >();
    m_activeFutureWatchers++;
    futureWatcher->setFuture(future);
    connect(futureWatcher, SIGNAL(finished()),this, SLOT(dataCacheProcessed()));
}

void NepomukModel::dataCacheProcessed()
{
    QFutureWatcher<QList<CachedRowEntry> > *futureWatcher = static_cast<QFutureWatcher<QList<CachedRowEntry> > *>(sender());
    // take all results and prepare them for the entry cache
    QList<CachedRowEntry> cacheEntries = futureWatcher->future().result();

    if(cacheEntries.size() > 0) {
        beginInsertRows(QModelIndex(), m_modelCacheData.size(), m_modelCacheData.size() + cacheEntries.size()-1);
        m_modelCacheData.append(cacheEntries);
        endInsertRows();
    }

    emit dataSizeChaged(m_modelCacheData.size());

    disconnect(futureWatcher, SIGNAL(finished()),this, SLOT(dataCacheProcessed()));
    delete futureWatcher;

    m_activeFutureWatchers--;

    if(m_queryFinished && m_activeFutureWatchers == 0) {
        emit updateFetchDataFor(m_selection,false, m_library);
    }
}

void NepomukModel::removeData( const QList< QUrl > &entries )
{
    foreach(QUrl url, entries) {
        //iterate through the full list of entries and find the one we are giong to remove
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
    m_queryFinished = true;

    if(m_activeFutureWatchers == 0) {
        emit updateFetchDataFor(m_selection,false, m_library);
    }
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

void NepomukModel::updateCacheEntry(Nepomuk::Resource resource)
{
    int i = 0;
    foreach(CachedRowEntry cre, m_modelCacheData) {
        if(cre.resource.uri() == resource.uri()) {
            cre.displayColums = createDisplayData(resource);
            cre.decorationColums = createDecorationData(resource);

            m_modelCacheData.replace(i, cre);
        }
        i++;
    }
}
