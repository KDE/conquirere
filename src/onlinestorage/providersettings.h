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

#ifndef PROVIDERSETTINGS_H
#define PROVIDERSETTINGS_H

#include <QWidget>

#include "storageglobals.h"

#include <Akonadi/Collection>
#include <KWallet/Wallet>

namespace Ui {
    class ProviderSettings;
}

class StorageInfo;
class SyncStorage;

class ProviderSettings : public QWidget
{
    Q_OBJECT

public:
    explicit ProviderSettings(QWidget *parent = 0, bool showAkonadiStuff = false);
    virtual ~ProviderSettings();

    void setProviderSettingsDetails(const ProviderSyncDetails &psd);
    ProviderSyncDetails providerSettingsDetails() const;
    StorageInfo *storageInfoByString(const QString &providerId);

    void savePasswordInKWallet();

private slots:
    void collectionsReceived( const Akonadi::Collection::List& );
    void findPasswordInKWallet();

    void switchProvider(int curIndex);
    void showHelp();
    void fetchCollection();
    void fillCollectionList(QList<CollectionInfo> collectionList);

private:
    Ui::ProviderSettings *ui;

    QList<StorageInfo*> m_availableProvider;
    KWallet::Wallet* m_wallet;
};

#endif // PROVIDERSETTINGS_H
