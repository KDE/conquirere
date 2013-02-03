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

#include "publicationfiltermodel.h"

#include "../queryclients/publicationquery.h"

PublicationFilterModel::PublicationFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_curFilter(BibGlobals::Max_BibTypes)
{
    setDynamicSortFilter(false); // setting this to true slows down the view a lot
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void PublicationFilterModel::setResourceFilter(BibGlobals::BibEntryType filter)
{
    m_curFilter = filter;
    invalidate();
}

bool PublicationFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool regexpCheck = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);

    QModelIndex typeIndex = sourceModel()->index(sourceRow, PublicationQuery::Column_ResourceType, sourceParent);

    uint type = sourceModel()->data(typeIndex, Qt::UserRole + 1).toUInt();
    BibGlobals::BibEntryType typeEnum = (BibGlobals::BibEntryType)type;

    if(m_curFilter == BibGlobals::Max_BibTypes) {
        return regexpCheck;
    }
    // alsow show subtypes
    if(m_curFilter == BibGlobals::BibType_Collection) {
        return regexpCheck && (typeEnum == BibGlobals::BibType_Collection ||
                               typeEnum == BibGlobals::BibType_Proceedings ||
                               typeEnum == BibGlobals::BibType_JournalIssue ||
                               typeEnum == BibGlobals::BibType_MagazinIssue ||
                               typeEnum == BibGlobals::BibType_NewspaperIssue ||
                               typeEnum == BibGlobals::BibType_Blog ||
                               typeEnum == BibGlobals::BibType_Forum ||
                               typeEnum == BibGlobals::BibType_WebPage ||
                               typeEnum == BibGlobals::BibType_CodeOfLaw ||
                               typeEnum == BibGlobals::BibType_CourtReporter );
    }
    // alsow show subtypes
    if(m_curFilter == BibGlobals::BibType_Article) {
        return regexpCheck && (typeEnum == BibGlobals::BibType_Article ||
                               typeEnum == BibGlobals::BibType_ForumPost ||
                               typeEnum == BibGlobals::BibType_BlogPost ||
                               typeEnum == BibGlobals::BibType_WebSite );
    }

    return regexpCheck && (typeEnum == m_curFilter);
}

