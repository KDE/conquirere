/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "seriesmodel.h"

#include "../queryclients/seriesquery.h"
#include "nbibio/conquirere.h"

#include <QModelIndex>

SeriesModel::SeriesModel(QObject *parent)
    : NepomukModel(parent)
{
    m_queryClient = new SeriesQuery();

    connect(m_queryClient, SIGNAL(newCacheEntries(QList<CachedRowEntry>)), this, SLOT(addCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(updateCacheEntries(QList<CachedRowEntry>)), this, SLOT(updateCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(removeCacheEntries(QList<QUrl>)), this, SLOT(removeCacheData(QList<QUrl>)));

    connect(m_queryClient, SIGNAL(queryFinished()), this, SIGNAL(queryFinished()));
}

SeriesModel::~SeriesModel()
{
    if(ConqSettings::cacheOnStartUp()) {
        saveCache();
    }
}

int SeriesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return SeriesQuery::Max_columns;
}

QVariant SeriesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case SeriesQuery::Column_Title:
            return i18nc("Header for the series title column","Title");
        case SeriesQuery::Column_ResourceType:
            return i18nc("Header for the series type column","Type");
        case SeriesQuery::Column_StarRate:
            return i18nc("Header for the series rating column","Rating");
        case SeriesQuery::Column_Content:
            return i18nc("Header for the series content  column(list of all publications in the series)","Content");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case SeriesQuery::Column_Title:
            return i18nc("Tooltip for the series title column","Title");
        case SeriesQuery::Column_ResourceType:
            return i18nc("Tooltip for the series type column","Type");
        case SeriesQuery::Column_StarRate:
            return i18nc("Tooltip for the series rating column","Rating");
        case SeriesQuery::Column_Content:
            return i18nc("Tooltip for the series content column (list of all publications in the series)","Content");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

int SeriesModel::defaultSectionSize(int i) const
{
    switch (i) {
    case SeriesQuery::Column_Title:
        return 300;
    case SeriesQuery::Column_ResourceType:
        return 100;
    case SeriesQuery::Column_StarRate:
        return 75;
    case SeriesQuery::Column_Content:
        return 300;
    }

    return 100;
}

QList<int> SeriesModel::fixedWithSections() const
{
    QList<int> fixedWith;
    fixedWith << SeriesQuery::Column_StarRate;

    return fixedWith;
}

QString SeriesModel::id()
{
    return QLatin1String("series");
}
