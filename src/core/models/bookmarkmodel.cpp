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

#include "bookmarkmodel.h"

#include "../queryclients/bookmarkquery.h"
#include "nbibio/conquirere.h"

#include <QtCore/QModelIndex>

BookmarkModel::BookmarkModel(QObject *parent)
    : NepomukModel(parent)
{
    m_queryClient = new BookmarkQuery();

    connect(m_queryClient, SIGNAL(newCacheEntries(QList<CachedRowEntry>)), this, SLOT(addCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(updateCacheEntries(QList<CachedRowEntry>)), this, SLOT(updateCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(removeCacheEntries(QList<QUrl>)), this, SLOT(removeCacheData(QList<QUrl>)));

    connect(m_queryClient, SIGNAL(queryFinished()), this, SIGNAL(queryFinished()));
}

BookmarkModel::~BookmarkModel()
{
    if(ConqSettings::cacheOnStartUp()) {
        saveCache();
    }
}

int BookmarkModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return BookmarkQuery::Max_columns;
}

QVariant BookmarkModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case BookmarkQuery::Column_Tags:
            return i18nc("Header for the tag column","Tags");
        case BookmarkQuery::Column_Title:
            return i18nc("Header for the title column","Title");
        case BookmarkQuery::Column_Date:
            return i18nc("Header for the last modified section","Last Modified");
        case BookmarkQuery::Column_Link:
            return i18nc("Header for the www url column","Link");
        case BookmarkQuery::Column_StarRate:
            return i18nc("Header for the rating column","Rating");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case BookmarkQuery::Column_Tags:
            return i18nc("Tooltip for the tags column","Tags");
        case BookmarkQuery::Column_Title:
            return i18nc("Tooltip for the title column","Title");
        case BookmarkQuery::Column_Date:
            return i18nc("Tooltip for the last modified section","Last Modified");
        case BookmarkQuery::Column_Link:
            return i18nc("Tooltip for the www url column","Link");
        case BookmarkQuery::Column_StarRate:
            return i18nc("Tooltip for the rating column","Rating");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

int BookmarkModel::defaultSectionSize(int i) const
{
    switch (i) {
    case BookmarkQuery::Column_Tags:
        return 200;
    case BookmarkQuery::Column_Title:
        return 300;
    case BookmarkQuery::Column_Date:
        return 100;
    case BookmarkQuery::Column_Link:
        return 100;
    case BookmarkQuery::Column_StarRate:
        return 75;
    }

    return 100;
}

QList<int> BookmarkModel::fixedWithSections() const
{
    QList<int> fixedWith;
    fixedWith << BookmarkQuery::Column_StarRate;

    return fixedWith;
}

QString BookmarkModel::id()
{
    return QLatin1String("bookmarks");
}
