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

#ifndef REFERENCEMODEL_H
#define REFERENCEMODEL_H

#include "nepomukmodel.h"

/**
  * @brief Model to list all @c nbib:Reference objects
  *
  * @see ReferenceQuery
  */
class ReferenceModel : public NepomukModel
{
public:
    ReferenceModel(QObject *parent = 0);
    virtual ~ReferenceModel();

    // implemented from QAbstractTableModel
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    int defaultSectionSize(int i) const;
    QList<int> fixedWidthSections() const;

    QString id();
};

#endif // REFERENCEMODEL_H
