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

#include "seriesfiltermodel.h"

#include "../queryclients/seriesquery.h"

SeriesFilterModel::SeriesFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_curFilter(BibGlobals::Max_SeriesTypes)
{
    setDynamicSortFilter(false); // setting this to true slows down the view a lot
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void SeriesFilterModel::setResourceFilter(BibGlobals::SeriesType filter)
{
    m_curFilter = filter;
    invalidate();
}

bool SeriesFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool regexpCheck = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);

    QModelIndex typeIndex = sourceModel()->index(sourceRow, SeriesQuery::Column_ResourceType, sourceParent);

    uint type = sourceModel()->data(typeIndex, Qt::UserRole + 1).toUInt();
    BibGlobals::SeriesType typeEnum = (BibGlobals::SeriesType)type;

    if(m_curFilter != BibGlobals::Max_SeriesTypes) {
        return regexpCheck && (typeEnum == m_curFilter);
    }

    return regexpCheck;
}
