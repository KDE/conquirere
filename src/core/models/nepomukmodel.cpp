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
#include "nbibio/conquirere.h"

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
}

NepomukModel::~NepomukModel()
{
    thread.quit();
    while(!thread.isFinished()){}

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
    connect(&thread, SIGNAL(started()), m_queryClient, SLOT(startFetchData()) );
    m_queryClient->moveToThread(&thread);
    thread.start();
//    m_queryClient->startFetchData();
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

QString NepomukModel::id()
{
    return QString("abstract");
}

void NepomukModel::saveCache()
{
    QString cacheName = QString("%1_%2").arg(m_library->settings()->name()).arg(id());
    QString cachePath = KStandardDirs::locateLocal("appdata", cacheName);

    QFile file(cachePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "can't open model cache file" << cachePath;
        return;
    }

    QTextStream out(&file);

    foreach(const CachedRowEntry &cre, m_modelCacheData) {
        foreach(const QVariant &v, cre.displayColums) {
            out << v.toString() << "|#|";
        }
        out << "\n";
        foreach(const QVariant &v, cre.decorationColums) {
            out << v.toString() << "|#|";
        }
        out << "\n";
        //DEBUG save the timestamp when the entry was actually inserted into the program?
        out << cre.resource.uri().toString() << "|#|" << QDateTime::currentDateTime().toString() << "\n";
    }
    file.close();
}

void NepomukModel::loadCache()
{
    qDebug() << "loadCache :: " << id();
    QString cacheName = QString("%1_%2").arg(m_library->settings()->name()).arg(id());
    QString cachePath = KStandardDirs::locateLocal("appdata", cacheName);

    QFile file(cachePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "can't open model cache file" << cachePath;
        return;
    }

    QTextStream in(&file);
    QList<CachedRowEntry> cachedEntries;
    while (!in.atEnd()) {
        CachedRowEntry cre;
        QString displayLine = in.readLine();
        foreach(const QString &col, displayLine.split(QLatin1String("|#|"))) {
            cre.displayColums.append(col);
        }
        QString decorationLine = in.readLine();
        foreach(const QString &col, decorationLine.split(QLatin1String("|#|"))) {
            if(col.isEmpty()) {
                cre.decorationColums.append(QVariant());
            }
            else {
                cre.decorationColums.append(col);
            }
        }
        QStringList resInfo = in.readLine().split(QLatin1String("|#|"));
        cre.resource = Nepomuk2::Resource::fromResourceUri(resInfo.first());
        cre.timestamp = QDateTime::fromString( resInfo.last() );

        // don't add entries which are removed already
        if(cre.resource.isValid()) {
            cachedEntries.append(cre);
        }
    }

    addCacheData(cachedEntries);
    qDebug() << "loadCache finished :: " << id();
}

void NepomukModel::addCacheData(const QList<CachedRowEntry> &entries)
{
    kDebug() << "add new entries" << entries.size() << m_modelCacheData.size() << "model" << id();
    if(entries.size() > 0) {
        beginInsertRows(QModelIndex(), m_modelCacheData.size(), m_modelCacheData.size() + entries.size()-1);
        foreach(const CachedRowEntry &cre, entries) {
            m_modelCacheData.append(cre);
            m_lookupCache.insert(cre.resource.uri().toString(), m_modelCacheData.size()-1);
        }
        endInsertRows();
    }

    kDebug() << "new size " << m_modelCacheData.size();

    emit dataSizeChaged(m_modelCacheData.size());
}

void NepomukModel::removeCacheData( QList<QUrl> urls )
{
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

