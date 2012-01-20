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

#ifndef DOCUMENTQUERY_H
#define DOCUMENTQUERY_H

#include "queryclient.h"

/**
  * @brief QueryClient to fetch all document data. @c nfo:PaginatedTextDocument
  *
  * @see DocumentModel
  */
class DocumentQuery : public QueryClient
{
    Q_OBJECT
public:
    enum ColumnList {
        Column_StarRate,
        Column_Reviewed,
        Column_Publication,
        Column_Author,
        Column_Title,
        Column_FileName,
        Column_Folder,
        Column_Date,

        Max_columns
    };

    explicit DocumentQuery(QObject *parent = 0);

public slots:
    void startFetchData();

    /**
      * Indicates that the resource has been changed and the cache needs an update
      *
      * @todo remove when starting to use ResourceWatcher later on
      */
    void resourceChanged (const Nepomuk::Resource &resource);

private:
    QVariantList createDisplayData(const Nepomuk::Resource & res) const;
    QVariantList createDecorationData(const Nepomuk::Resource & res) const;
};

#endif // DOCUMENTQUERY_H
