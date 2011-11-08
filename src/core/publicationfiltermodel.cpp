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

#include "publicationmodel.h"

PublicationFilterModel::PublicationFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_curFilter(Filter_None)
{
    setDynamicSortFilter(false); // setting this to true slows down the view a lot
}

void PublicationFilterModel::setResourceFilter(ResourceFilter filter)
{
    m_curFilter = filter;
    invalidateFilter(); // thats another slow down like hell
}

bool PublicationFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool regexpCheck = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);

    QModelIndex typeIndex = sourceModel()->index(sourceRow, PublicationModel::Column_ResourceType, sourceParent);

    QString type = sourceModel()->data(typeIndex).toString();
    BibEntryType typeEnum = (BibEntryType)BibEntryTypeTranslation.indexOf(type);

    switch(m_curFilter) {
    case Filter_Articles:
        return regexpCheck && (typeEnum == BibType_Article);
    case Filter_Journals:
        return regexpCheck && ((typeEnum == BibType_Journal) || (typeEnum == BibType_JournalIssue));
    case Filter_Books:
        return regexpCheck && ((typeEnum == BibType_Book) || (typeEnum == BibType_Booklet));
    case Filter_Proceedings:
        return regexpCheck && ((typeEnum == BibType_Article) || (typeEnum == BibType_Proceedings));
    case Filter_Thesisis:
        return regexpCheck && ((typeEnum == BibType_Bachelorthesis) || (typeEnum == BibType_Phdthesis) || (typeEnum == BibType_Mastersthesis));
    case Filter_Presentation:
        return false;
    case Filter_Script:
        return false;
    case Filter_Techreport:
        return regexpCheck && (typeEnum == BibType_Techreport);
    }

    return regexpCheck;
}

