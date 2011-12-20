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

#ifndef SERIESFILTERMODEL_H
#define SERIESFILTERMODEL_H

#include "../../globals.h"

#include <QtGui/QSortFilterProxyModel>

/**
  * @brief Extended filter model to filter all series by it's type
  *
  * Used only to limit the set of series shown from the main project tree widget
  */
class SeriesFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SeriesFilterModel(QObject *parent = 0);

    /**
      * The @p SeriesType to filter for
      */
    void setResourceFilter(SeriesType filter);

protected:
     bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    SeriesType m_curFilter;
};

#endif // SERIESFILTERMODEL_H
