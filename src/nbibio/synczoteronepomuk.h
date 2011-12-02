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

#ifndef SYNCZOTERONEPOMUK_H
#define SYNCZOTERONEPOMUK_H

#include <QObject>
#include <QtCore/QMetaType>

#include <kbibtex/file.h>

class ReadFromZotero;
class WriteToZotero;
class BibTexToNepomukPipe;
class NepomukToBibTexPipe;
class Element;
class EntryClique;


Q_DECLARE_METATYPE(EntryClique*)
Q_DECLARE_METATYPE(QList<EntryClique*>)

class SyncZoteroNepomuk : public QObject
{
    Q_OBJECT
public:
    explicit SyncZoteroNepomuk(QObject *parent = 0);
    virtual ~SyncZoteroNepomuk();

    void setUserName(const QString &name);
    void setPassword(const QString &pwd);
    void setUrl(const QString &url);
    void setCollection(const QString &collection);
    void askBeforeDeletion(bool ask);

public slots:
    void startUpload();
    void startDownload();
    void startSync();

    /**
      * connectes to the response of askForDeletion() and proceeds with the zoteroSync
      */
    void deleteLocalFiles(bool deleteThem);
    void resultsMerged(QList<EntryClique*> cliques);

signals:
    void progress(int value);
    void progressStatus(const QString &status);

    /**
      * emited if items from the server are deletet
      * emits a signal so we can show a dialog box not possible here as we run this class in a different thread than the gui thread
      */
    void askForDeletion(int items);
    void mergeResults(QList<EntryClique*> cliques, File *bibCache);

private slots:
    void calculateProgress(int value);

    /**
      * called from startDownload
      */
    void readDownloadSync(File zoteroData);

    /**
      * clalled from startSync
      */
    void readSyncronizeSync(File zoteroData);
    void writeSync(File zoteroData);

private:
    ReadFromZotero *m_rfz;
    WriteToZotero *m_wtz;
    File m_bibCache;
    BibTexToNepomukPipe *m_btnp;
    NepomukToBibTexPipe *m_ntnp;

    QString m_name;
    QString m_pwd;
    QString m_url;
    QString m_collection;
    bool m_askBeforeDeletion;
    int m_syncSteps;
    int m_curStep;

    QList<Element*> temp_toBeDeleted;
};

#endif // SYNCZOTERONEPOMUK_H
