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

#ifndef DOCUMENTMODEL_H
#define DOCUMENTMODEL_H

#include "nepomukmodel.h"

/**
  * @brief table model to contain all @c nfo:PaginatedTextDocument in the nepomuk storage
  */
class DocumentModel : public NepomukModel
{
    Q_OBJECT
public:
    explicit DocumentModel(QObject *parent = 0);
    ~DocumentModel();

    // implemented from QAbstractTableModel
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public slots:
    void startFetchData();

protected:
    QVariantList createDisplayData(const Nepomuk::Resource & res);
    QVariantList createDecorationData(const Nepomuk::Resource & res);
};

#endif // DOCUMENTMODEL_H
