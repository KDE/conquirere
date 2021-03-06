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

#ifndef PUBLICATIONFILTERMODEL_H
#define PUBLICATIONFILTERMODEL_H

#include "config/bibglobals.h"

#include <QtGui/QSortFilterProxyModel>

/**
  * @brief Extended filter model to allow filtering by publication type
  *
  * Used to restrict the table in the project by its type only.
  * @see BibEntryType
  */
class PublicationFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit PublicationFilterModel(QObject *parent = 0);

    /**
      * The @c BibEntryType type to @p filter
      */
    void setResourceFilter(BibGlobals::BibEntryType filter);

protected:
     bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    BibGlobals::BibEntryType m_curFilter;
};

#endif // PUBLICATIONFILTERMODEL_H
