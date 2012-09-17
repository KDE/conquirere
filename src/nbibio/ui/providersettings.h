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

#include <QtGui/QWidget>

#include "nbibio/storageglobals.h"

#include <KDE/KIcon>
#include <KWallet/Wallet>

namespace Ui {
    class ProviderSettings;
}

class OnlineStorage;

class ProviderSettings : public QWidget
{
    Q_OBJECT

public:
    explicit ProviderSettings(QWidget *parent = 0);
    virtual ~ProviderSettings();

    void setProviderSettingsDetails(const ProviderSyncDetails &psd);
    ProviderSyncDetails providerSettingsDetails() const;
    OnlineStorage *storageInfoByString(const QString &providerId);

    void savePasswordInKWallet();

private slots:
    void findPasswordInKWallet();

    void switchProvider(int curIndex);
    void showHelp();
    void fetchCollection();
    void fillCollectionList();

    void showLocalStoragePathCompletion();

private:
    Ui::ProviderSettings *ui;
    ProviderSyncDetails m_oldPsd;

    QList<OnlineStorage*> m_availableProvider;
    KWallet::Wallet* m_wallet;
};

#endif // PROVIDERSETTINGS_H
