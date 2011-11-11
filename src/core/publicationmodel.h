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

#ifndef PUBLICATIONMODEL_H
#define PUBLICATIONMODEL_H

#include "nepomukmodel.h"

/**
  * @brief Model for any kind of publication data
  *
  * Used to display @c nbib:Publication and @c nbib:Reference
  *
  */
class PublicationModel : public NepomukModel
{
    Q_OBJECT
public:
    enum ColumnList {
        Column_StarRate,
        Column_Reviewed,
        Column_FileAvailable,
        Column_CiteKey,
        Column_ResourceType,
        Column_Author,
        Column_Title,
        Column_Date,
        Column_Publisher,
        Column_Editor,

        Max_columns
    };

    explicit PublicationModel(QObject *parent = 0);
    ~PublicationModel();

    // implemented from QAbstractTableModel
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public slots:
    void startFetchData();

protected:
    virtual QVariantList createDisplayData(const Nepomuk::Resource & res);
    virtual QVariantList createDecorationData(const Nepomuk::Resource & res);
};

#endif // PUBLICATIONMODEL_H
