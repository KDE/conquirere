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

#include <Nepomuk/Resource>

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
    enum MergeStrategy {
        Manual,
        UseServer,
        UseLocal
    };
    explicit NBibSync(QObject *parent = 0);

    void setUserName(const QString &name);
    void setPassword(const QString &pwd);
    void setUrl(const QString &url);
    void setCollection(const QString &collection);
    void askBeforeDeletion(bool ask);
    void mergeStrategy( MergeStrategy strategy);

signals:
    void progress(int value);
    void progressStatus(const QString &status);
    /**
      * emited if items from the server are deletet
      * emits a signal so we can show a dialog box not possible here as we run this class in a different thread than the gui thread
      */
    void askForDeletion(QList<SyncDetails>);

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
    QString m_name;
    QString m_pwd;
    QString m_url;
    QString m_collection;
    bool m_askBeforeDeletion;
    MergeStrategy m_mergeStrategy;
    int m_syncSteps;
    int m_curStep;
};

#endif // NBIBSYNC_H
