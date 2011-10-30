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
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>

#include <QtCore/QAbstractTableModel>

class Library;
class QModelIndex;

class NepomukModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit NepomukModel(QObject *parent = 0);
    virtual ~NepomukModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual void setLibrary(Library *library);
    virtual void setResourceType(ResourceSelection selection);
    virtual Nepomuk::Resource documentResource(const QModelIndex &selection);

    virtual void removeSelectedFromProject(const QModelIndexList & indexes, Library *l);
    virtual void removeSelectedFromSystem(const QModelIndexList & indexes);

public slots:
    virtual void startFetchData() = 0;
    virtual void stopFetchData();

protected slots:
    /**
      * @bug does not work efficient because addData(QList<Nepomuk::Query::Result>) from Nepomuk::Query::QueryServiceClient returns wrong values
      */
    void addData(const QList< Nepomuk::Query::Result > &entries);
    /**
      * @bug does not work because removeData(QList<QUrl>) from Nepomuk::Query::QueryServiceClient returns wrong values
      */
    void removeData( const QList< QUrl > &entries );

    void resultCount(int number);
    void listingsFinished();
    void listingsError(const QString & 	errorMessage);

signals:
    void dataSizeChaged(int size);
    void updateFetchDataFor(ResourceSelection selection, bool start, Library *library);

protected:
    Library *m_library;
    ResourceSelection m_selection;
    Nepomuk::Query::QueryServiceClient *m_queryClient;
    QList<Nepomuk::Resource> m_fileList;
};
#endif // NEPOMUKMODEL_H
