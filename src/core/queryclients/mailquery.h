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

#ifndef MAILQUERY_H
#define MAILQUERY_H

#include "queryclient.h"

/**
  * @brief QUeryClient to fetch all @c nmo::Email objects
  *
  * @see MailModel
  */
class MailQuery : public QueryClient
{
    Q_OBJECT
public:
    enum ColumnList {
        Column_StarRate,
        Column_From,
        Column_Title,
        Column_Tags,
        Column_Date,

        Max_columns
    };

    explicit MailQuery(QObject *parent = 0);

public slots:
    void startFetchData();

    /**
      * @todo remove when starting to use ResourceWatcher later on
      */
    void resourceChanged (const Nepomuk::Resource &resource);

private:
    QVariantList createDisplayData(const Nepomuk::Resource & res) const;
    QVariantList createDecorationData(const Nepomuk::Resource & res) const;

};

#endif // MAILQUERY_H
