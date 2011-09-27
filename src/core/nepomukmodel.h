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

#ifndef NEPOMUKMODEL_H
#define NEPOMUKMODEL_H

#include "../globals.h"
#include <Nepomuk/Resource>

#include <QAbstractTableModel>

class Library;
class QModelIndex;

class NepomukModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit NepomukModel(QObject *parent = 0);

    virtual void setLibrary(Library *library) = 0;
    virtual void setResourceType(ResourceSelection selection) = 0;
    virtual Nepomuk::Resource documentResource(const QModelIndex &selection) = 0;

    virtual void startFetchData() = 0;
    virtual void stopFetchData() = 0;
    virtual void removeSelected(const QModelIndexList & indexes) = 0;

//signals:
//    void dataSizeChaged(int size);
//    void updatefetchDataFor(ResourceSelection selection, bool start, Library *library);

};
#endif // NEPOMUKMODEL_H
