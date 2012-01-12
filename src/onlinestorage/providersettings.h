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

#include "storageglobals.h"

#include <KDE/KIcon>
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
    struct AkonadiDetails {
        QString collectionName;
        qint64 collectionID;
    };

    /**
      * Creates a new ProviderSettings widget to specify sync settings
      *
      * @p showAkonadiStuff used to display 2 additional boxes to specify if and where events/contacts
      *                     should be added to akonadi. Not used for KBibTeX
      */
    explicit ProviderSettings(QWidget *parent = 0, bool showAkonadiStuff = false);
    virtual ~ProviderSettings();

    void setProviderSettingsDetails(const ProviderSyncDetails &psd);
    ProviderSyncDetails providerSettingsDetails() const;
    StorageInfo *storageInfoByString(const QString &providerId);

    void savePasswordInKWallet();

public slots:
    // to remove akonadi dependency in this part of the lib, we just provide the necessray
    // informations here, collection fetching is then done in conquirere then.
    // as all we need is the possibility to save if we put data to akonadi and the collection id
    // this makes no difference anyway
    void setAkonadiContactDetails(QList<ProviderSettings::AkonadiDetails> contactCollections);
    void setAkonadiEventDetails(QList<ProviderSettings::AkonadiDetails> contactCollections);

private slots:
    void findPasswordInKWallet();

    void switchProvider(int curIndex);
    void showHelp();
    void fetchCollection();
    void fillCollectionList(const QList<CollectionInfo> &collectionList);

private:
    Ui::ProviderSettings *ui;

    QList<StorageInfo*> m_availableProvider;
    KWallet::Wallet* m_wallet;
};

#endif // PROVIDERSETTINGS_H
