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

    File *getFile();
    File *getFailedPushRequestItems();

signals:
    /**
      * After we changed the data on the server the etag value will change
      *
      * This emits the changed entry so we can update it in other places if we do not work solely with
      * the bibtex format. If we do, this signal is not needed.
      */
    void entryItemUpdated(const QString &id, const QString &etag, const QString &updated);

public slots:
    void pushItems(File *items, const QString &collection = QString());
    void pushNewItems(File *items, const QString &collection = QString());
    void updateItem(QSharedPointer<Element> item);
    void addItemsToCollection(const QList<QString> &ids, const QString &collection );
    void removeItemsFromCollection(const QList<QString> &ids, const QString &collection );
    void deleteItems(File *items);
    // pair.first = zoteroKey
    // pair.second = etag
    void deleteItems(QList<QPair<QString, QString> > items);

    void createCollection(const CollectionInfo &ci);
    void editCollection(const CollectionInfo &ci);
    void deleteCollection(const CollectionInfo &ci);

protected slots:
    void requestFinished();

private:
    bool m_allRequestsSend;

    /**
      * holds items if we need to upload more than 50 (MAX_ITEM_TO_PUSH)
      */
    File m_itemsToPushCache;

    /**
      * when the server returns error "Internal Server Error" as responce of a new item push request
      * we couldn't add the sync data to it. Thus on a next sync we need to keep in mind
      * all intems in this list will be downloaded as duplicates!
      * because the items are available on the server but we don't know what ID they have
      */
    QList<File*> m_failedItemPush;

    /**
      * contains the list of all entries we created newly on the server and where we didn't get the "Internal Server Error" responce
      * So we need to find the duplicates for it and add the zoteroKey to them
      */
    File *m_entriesAfterSync;


    qreal m_progressPerFile;
    qreal m_progress;
    QString m_addToCollection;

    QByteArray writeJsonContent(File *items, bool onlyUpdate = false);
    QByteArray writeJsonContent(const CollectionInfo &collection);

    /**
      * Creates the right creator VariantList for the zotero upload
      *
      * Each bibtex field that holds a "person" value is transformed to the right
      * entry for zotero. This works for the real perosn (author/editor) and other fields
      * like translator and such
      */
    QVariantList createCreatorsJson(Entry *e, const QString &type);

    /**
      * Creates the list of valid contributor entries from zotero
      *
      * This way we know all the bibtex keys @c createCreatorsJson() has to look for
      *
      * @see https://api.zotero.org/itemTypeCreatorTypes?itemType=XXX where XX can be webpage, book, etc
      */
    QStringList creatorTypeForZoteroMapping(const QString &type);

    /**
      * Handles what creator type from Zotero was renamed to fit into the author / editor scheme of KBibTeX
      *
      * Because not all creators for a zotero entry have author/editor but sometime inventor and such things
      * we map them to author/editor to get a more "standard compliant" result.
      *
      * Here we map them back and tell which original zotero entry is the bibtexentry author (first entry)
      * and bibtex entry editor (2nd entry)
      */
    QStringList bibtexCreatorZoteroMapping(const QString &type);

    /**
      * Creates zotero "keywords" from the bibtex entry keyword and tag
      */
    QVariantList createTagsJson(Entry *e);

    // everything below is responsible for mapping the bibtex keys back to what zotero expects
    // this takes care of the correct remapping if adoptToBibTeX is true
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
    QVariantMap createAttachmentJson(Entry *e);
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

    // copied from kbibtex/fileimporterbibtex
    // necessary to detect/parse propper persons from entries like translator= etc.
    // kbibtex supports this only for author/editor fields
    /**
      * splits a lis tof names into single names (normally names have XXX and XXX)
      */
    void splitPersonList(const QString& name, QStringList &resultList);
    /**
      * simplified version of the nonfunctional splitname from kbibtex
      */
    void splitName(const QString& name, QString &first, QString &last );
};

#endif // WRITETOZOTERO_H
