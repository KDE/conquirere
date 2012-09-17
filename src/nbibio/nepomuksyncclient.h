/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef NEPOMUKSYNCCLIENT_H
#define NEPOMUKSYNCCLIENT_H

#include <QtCore/QObject>

#include "storageglobals.h"

#include <Nepomuk2/Resource>

class NepomukSyncClient : public QObject
{
    Q_OBJECT
public:
    explicit NepomukSyncClient(QObject *parent = 0);

    void setProviderSettings(const ProviderSyncDetails &psd);
    ProviderSyncDetails providerSettings() const;

    void setProject(const Nepomuk2::Resource &project);

    /**
     * @brief importData from onlinestorage to nepomuk
     */
    void importData();

    /**
     * @brief exportData from nepomuk to onlinestorage
     */
    void exportData();
    void syncData();

signals:
    void status(const QString &message);
    void progress(int currentProgress);
    void error(const QString &message);
    void finished();

public slots:
    void cancel();

private:
    ProviderSyncDetails m_psd;
    Nepomuk2::Resource m_project;
};

#endif // NEPOMUKSYNCCLIENT_H
