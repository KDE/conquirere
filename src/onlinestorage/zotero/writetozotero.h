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

#ifndef WRITETOZOTERO_H
#define WRITETOZOTERO_H

#include "../writetostorage.h"

/**
  * @brief sync bibtex items with the zotero online storage
  *
  * Before an item can be updated in the storage it must be fetched to retrieve the latest
  * etag value. This ensures the item wasn't changed on the server in the meantime.
  *
  * When a new item was created or updated a new etag value and zotero updated date is retrieved from
  * the server, these values are write back to the bibte file.
  *
  * Only the allowed zotero item and their allowed values are synced to the server.
  * The @c writeJsonContent() tries to fit bibtex entries that do not relate directly to corresponding
  * zotero values.
  *
  * @see https://www.zotero.org/support/dev/server_api/write_api
  *
  * @author Jörg Ehrichs <joerg.ehrichs@gmx.de>
  */
class WriteToZotero : public WriteToStorage
{
    Q_OBJECT
public:
    explicit WriteToZotero(QObject *parent = 0);
    virtual ~WriteToZotero();

public slots:
    void pushItems(File items);
    void pushNewItems(File items);
    void updateItem(Entry *item);
    void addItemsToCollection(QList<QString> ids, const QString &collection );
    void removeItemsFromCollection(QList<QString> ids, const QString &collection );
    void deleteItems(File items);

    void createCollection(const CollectionInfo &ci);
    void editCollection(const CollectionInfo &ci);
    void deleteCollection(const CollectionInfo &ci);

protected slots:
    void requestFinished();

private:
    bool m_allRequestsSend;
    File m_entriesAfterSync;
    qreal m_progressPerFile;
    qreal m_progress;

    QByteArray writeJsonContent(File items, bool onlyUpdate = false);
    QByteArray writeJsonContent(const CollectionInfo &collection);

    QVariantList createCreatorsJson(Entry *e);
    QVariantList createTagsJson(Entry *e);

    QVariantMap createArtworkJson(Entry *e);
    QVariantMap createAudioRecordingJson(Entry *e);
    QVariantMap createBillJson(Entry *e);
    QVariantMap createBlogPostJson(Entry *e);
    QVariantMap createBookJson(Entry *e);
    QVariantMap createBookSectionJson(Entry *e);
    QVariantMap createCaseJson(Entry *e);
    QVariantMap createComputerProgramJson(Entry *e);
    QVariantMap createConferencePaperJson(Entry *e);
    QVariantMap createDictionaryEntryJson(Entry *e);
    QVariantMap createDocumentJson(Entry *e);
    QVariantMap createEmailJson(Entry *e);
    QVariantMap createEncyclopediaArticleJson(Entry *e);
    QVariantMap createFilmJson(Entry *e);
    QVariantMap createForumPostJson(Entry *e);
    QVariantMap createHearingJson(Entry *e);
    QVariantMap createInstantMessageJson(Entry *e);
    QVariantMap createInterviewJson(Entry *e);
    QVariantMap createJournalArticleJson(Entry *e);
    QVariantMap createLetterJson(Entry *e);
    QVariantMap createMagazineArticleJson(Entry *e);
    QVariantMap createManuscriptJson(Entry *e);
    QVariantMap createMapJson(Entry *e);
    QVariantMap createNewspaperArticleJson(Entry *e);
    QVariantMap createNoteJson(Entry *e);
    QVariantMap createPatentJson(Entry *e);
    QVariantMap createPodcastJson(Entry *e);
    QVariantMap createPresentationJson(Entry *e);
    QVariantMap createRadioBroadcastJson(Entry *e);
    QVariantMap createReportJson(Entry *e);
    QVariantMap createStatuteJson(Entry *e);
    QVariantMap createTvBroadcastJson(Entry *e);
    QVariantMap createThesisJson(Entry *e);
    QVariantMap createVideoRecordingJson(Entry *e);
    QVariantMap createWebpageJson(Entry *e);
};

#endif // WRITETOZOTERO_H
