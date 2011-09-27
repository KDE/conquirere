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
#include "../globals.h"

#include <Nepomuk/Resource>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>

#include <QtCore/QList>

class Library;

/**
  * Model for any kind of publication data
  *
  * Used to display @c nbib:Publication and @c nbib:Reference
  *
  */
class PublicationModel : public NepomukModel
{
    Q_OBJECT
public:
    explicit PublicationModel(QObject *parent = 0);
    ~PublicationModel();

    void setLibrary(Library *library);
    void setResourceType(ResourceSelection selection);

    Nepomuk::Resource documentResource(const QModelIndex &selection);

    void removeSelected(const QModelIndexList & indexes);

    // implemented from QAbstractTableModel
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public slots:
    void startFetchData();
    void stopFetchData();

signals:
    void dataSizeChaged(int size);
    void updatefetchDataFor(ResourceSelection selection, bool start, Library *library);

private slots:
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

private:
    Library *m_library;
    ResourceSelection m_selection;
    Nepomuk::Query::QueryServiceClient *m_queryClient;
    QList<Nepomuk::Resource> m_fileList;

};

#endif // PUBLICATIONMODEL_H
