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

#ifndef SYNCSTORAGE_H
#define SYNCSTORAGE_H

#include <QObject>

#include <kbibtex/file.h>

/**
  * @brief Convenient class to do the dirty work when syncing with an online storage
  *
  * This is just the abstract base, each supported storage must inplement its version of it.
  * The idea is that when @c syncWithStorage() is called the provider which retrieves all items from the server
  * merge them with the available items in the @p bibfile pointer and sends all changed/new items back to the server
  *
  * The @c bibfile will have the synced state as soon as @c syncInProgress(false) is emitted
  *
  * @author Jörg Ehrichs <joerg.ehrichs@gmx.de>
  */
class SyncStorage : public QObject
{
    Q_OBJECT
public:
    explicit SyncStorage(QObject *parent = 0);
    virtual ~SyncStorage();

    void setUserName(const QString & name);
    QString userName() const;
    void setPassword(const QString & pwd);
    QString pasword() const;
    void setUrl(const QString & url);
    QString url() const;

    void setAdoptBibtexTypes(bool adopt);
    bool adoptBibtexTypes() const;
    void setAskBeforeDeletion(bool ask);
    bool askBeforeDeletion() const;
    void setDownloadOnly(bool downloadOnly);
    bool downloadOnly() const;
    void setUploadOnly(bool uploadOnly);
    bool uploadOnly() const;

signals:
    void syncInProgress(bool active);
    void progress(int percent);
    void currentStatus(const QString &status);

public slots:
    /**
      * Must be implemented by the subclasses.
      *
      * emit @c syncInProgress(true) on start and @c syncInProgress(false) when it is finished
      * @p bibfile pointer to the bibtex entries which will be synced with the storage
      */
    virtual void syncWithStorage(File *bibfile, const QString &collection) = 0;

private:
    QString m_name;
    QString m_password;
    QString m_url;

    bool m_adoptBibtexTypes;
    bool m_askBeforeDeletion;
    bool m_downloadOnly;
    bool m_uploadOnly;
};

#endif // SYNCSTORAGE_H
