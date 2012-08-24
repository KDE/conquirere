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

#ifndef PUBLICATIONQUERY_H
#define PUBLICATIONQUERY_H

#include "queryclient.h"

namespace Nepomuk2 {
    class ResourceWatcher;
}

/**
  * @brief @c QueryClient to fetch all @c nbib:Publication objects
  *
  * @see PublicationModel
  */
class PublicationQuery : public QueryClient
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

    explicit PublicationQuery(QObject *parent = 0);
    ~PublicationQuery();

public slots:
    void startFetchData();

private:
    // creates cache entries from Soprano:Model search
    QVariantList createDisplayData(const QStringList & item) const;
    QVariantList createDecorationData(const QStringList & item) const;

    // creates cache entry from resource handed via resourcewatcher
    QVariantList createDisplayData(const Nepomuk2::Resource & res) const;
    QVariantList createDecorationData(const Nepomuk2::Resource & res) const;

    Nepomuk2::ResourceWatcher* m_newWatcher;
};

#endif // PUBLICATIONQUERY_H
