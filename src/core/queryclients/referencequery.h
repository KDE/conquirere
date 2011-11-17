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

#ifndef REFERENCEQUERY_H
#define REFERENCEQUERY_H

#include "queryclient.h"

class ReferenceQuery : public QueryClient
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

    explicit ReferenceQuery(QObject *parent = 0);

public slots:
    void startFetchData();

private:
    QVariantList createDisplayData(const Nepomuk::Resource & res) const;
    QVariantList createDecorationData(const Nepomuk::Resource & res) const;

};

#endif // REFERENCEQUERY_H
