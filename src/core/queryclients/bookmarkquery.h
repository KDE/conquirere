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

    void startFetchData();

private:
    QVariantList createDisplayData(const Nepomuk::Resource & res) const;
    QVariantList createDecorationData(const Nepomuk::Resource & res) const;

};

#endif // BOOKMARKQUERY_H
