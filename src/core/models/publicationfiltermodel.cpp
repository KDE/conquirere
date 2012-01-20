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
    , m_curFilter(Max_BibTypes)
{
    setDynamicSortFilter(false); // setting this to true slows down the view a lot
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void PublicationFilterModel::setResourceFilter(BibEntryType filter)
{
    m_curFilter = filter;
    invalidate();
}

bool PublicationFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool regexpCheck = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);

    QModelIndex typeIndex = sourceModel()->index(sourceRow, PublicationQuery::Column_ResourceType, sourceParent);

    QString type = sourceModel()->data(typeIndex).toString();
    BibEntryType typeEnum = (BibEntryType)BibEntryTypeTranslation.indexOf(type);

    if(m_curFilter == Max_BibTypes) {
        return regexpCheck;
    }
    // alsow show subtypes
    if(m_curFilter == BibType_Collection) {
        return regexpCheck && (typeEnum == BibType_Collection ||
                               typeEnum == BibType_Proceedings ||
                               typeEnum == BibType_JournalIssue ||
                               typeEnum == BibType_MagazinIssue ||
                               typeEnum == BibType_NewspaperIssue ||
                               typeEnum == BibType_Blog ||
                               typeEnum == BibType_Forum ||
                               typeEnum == BibType_WebPage ||
                               typeEnum == BibType_CodeOfLaw ||
                               typeEnum == BibType_CourtReporter );
    }
    // alsow show subtypes
    if(m_curFilter == BibType_Article) {
        return regexpCheck && (typeEnum == BibType_Article ||
                               typeEnum == BibType_ForumPost ||
                               typeEnum == BibType_BlogPost ||
                               typeEnum == BibType_WebSite );
    }

    return regexpCheck && (typeEnum == m_curFilter);
}

