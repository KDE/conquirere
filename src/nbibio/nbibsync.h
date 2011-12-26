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

#ifndef NBIBSYNC_H
#define NBIBSYNC_H

#include <QObject>
#include <QtCore/QMetaType>

#include "onlinestorage/storageglobals.h"

#include <Nepomuk/Resource>
#include <Akonadi/Collection>

class StorageInfo;
class Entry;

struct SyncDetails {
    Nepomuk::Resource syncResource;
    Entry* externalResource;
};

Q_DECLARE_METATYPE(SyncDetails)
Q_DECLARE_METATYPE(QList<SyncDetails>)

class NBibSync : public QObject
{
    Q_OBJECT
public:
    explicit NBibSync(QObject *parent = 0);

    virtual StorageInfo *providerInfo() const = 0;

    void setId(const QString &providerId);
    QString providerId() const;
    void setUserName(const QString &name);
    QString userName() const;
    void setPassword(const QString &pwd);
    void setUrl(const QString &url);
    QString url() const;
    void setCollection(const QString &collection);
    QString collection() const;
    void setAskBeforeDeletion(bool ask);
    bool askBeforeDeletion() const;
    void setMergeStrategy( MergeStrategy strategy);
    MergeStrategy mergeStrategy() const;
    /**
      * Sets the Akonadi addressbook where all contacts (authors, editors etc) are imported to beside the Nepomuk storage.
      *
      * @p addressbook a valid Akonadi::Collection representing a addressbook
      */
    void setAkonadiAddressbook(Akonadi::Collection & addressbook);

signals:
    void progress(int value);
    void progressStatus(const QString &status);
    /**
      * emitted if items from the server are deleted
      * emits a signal so we can show a dialog box not possible here as we run this class in a different thread than the gui thread
      *
      * call deleteLocalFiles() when the user made his choice to proceed sync
      */
    void askForDeletion(QList<SyncDetails>);

    /**
      * emitted when the user needs to decide how to merge certain items
      *
      * call mergeFinished() when the user finished merging (merging should be done directly in nepomuk)
      */
    void userMerge(QList<SyncDetails>);

public slots:
    virtual void startUpload() = 0;
    virtual void startDownload() = 0;
    virtual void startSync() = 0;

    /**
      * connects to the response of askForDeletion() and proceeds with the sync
      */
    virtual void deleteLocalFiles(bool deleteThem) = 0;
    virtual void mergeFinished() = 0;

protected slots:
    void calculateProgress(int value);

protected:
    QString m_providerId;
    QString m_name;
    QString m_pwd;
    QString m_url;
    QString m_collection;
    bool m_askBeforeDeletion;
    MergeStrategy m_mergeStrategy;
    Akonadi::Collection m_addressbook;
    int m_syncSteps;
    int m_curStep;
};

#endif // NBIBSYNC_H
