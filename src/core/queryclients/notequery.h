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

#ifndef NOTEQUERY_H
#define NOTEQUERY_H

#include "queryclient.h"

#include <QtCore/QList>

namespace Nepomuk2 {
    class ResourceWatcher;
}


/**
  * @brief @c QueryClient to fetch all @c pimo::Notes objects
  *
  * @see NoteModel
  */
class NoteQuery : public QueryClient
{
    Q_OBJECT
public:
    enum ColumnList {
        Column_StarRate = 0,
        Column_Title,
        Column_Date,
        Column_Tags,

        Max_columns
    };

    explicit NoteQuery(QObject *parent = 0);
    ~NoteQuery();

public slots:
    void startFetchData();
    void stopFetchData();
    void updateCacheEntry(const QUrl &uri, const QueryClient::UpdateType &updateType);

protected:
    // creates cache entries from Soprano:Model search
    QVariantList createDisplayData(const QStringList & item) const;
    QVariantList createDecorationData(const QStringList & item) const;

private:
    QList<CachedRowEntry> initialQueryNepomuk();
    QList<CachedRowEntry> updateQueryNepomuk(const QUrl &uri);
    QList<CachedRowEntry> processQueryResults(const QString &query, const QUrl &uri = QUrl());

    Nepomuk2::ResourceWatcher* m_newWatcher;
};

#endif // NOTEQUERY_H
