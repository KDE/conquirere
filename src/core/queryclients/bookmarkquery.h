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

#ifndef BOOKMARKQUERY_H
#define BOOKMARKQUERY_H

#include "queryclient.h"

/**
  * @brief @c QueryClient to populate the @c BookmarkModel with the @c nfo:Website nepomuk resources
  *
  * Not used at the moment as it is unclear how bookmarks are represented in nepomuk.
  * Check rekonq if it will be implemented there properly someday and use their way of doing it.
  *
  * @see BookmarkModel
  */
class BookmarkQuery : public QueryClient
{
    Q_OBJECT
public:
    enum ColumnList {
        Column_StarRate,
        Column_Title,
        Column_Link,
        Column_Tags,
        Column_Date,

        Max_columns
    };

    explicit BookmarkQuery(QObject *parent = 0);

public slots:
    void startFetchData();

    /**
      * Indicates that the resource has been changed and the cache needs an update
      *
      * @todo remove when starting to use ResourceWatcher later on
      */
    void resourceChanged (const Nepomuk2::Resource &resource);

private:
    QVariantList createDisplayData(const Nepomuk2::Resource & res) const;
    QVariantList createDecorationData(const Nepomuk2::Resource & res) const;
};

#endif // BOOKMARKQUERY_H
