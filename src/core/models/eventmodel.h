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

#ifndef EVENTMODEL_H
#define EVENTMODEL_H

#include "nepomukmodel.h"

/**
  * @brief Model that holds all pimo:Event and ncal:Event resources
  *
  * the ncal:Events are creatded by the nepomuk feeder while conquiere created the pimo:Thing for it
  * to attach the publications, star rating and tags.
  *
  * @see EventQuery
  */
class EventModel : public NepomukModel
{
    Q_OBJECT
public:
    explicit EventModel(QObject *parent = 0);
    virtual ~EventModel();

    // implemented from QAbstractTableModel
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    int defaultSectionSize(int i) const;
    QList<int> fixedWithSections() const;

    QString id();
};

#endif // EVENTMODEL_H
