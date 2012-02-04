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

#ifndef BIBTEXTONEPOMUKPIPE_H
#define BIBTEXTONEPOMUKPIPE_H

#include "bibtexpipe.h"
#include "nbib.h"

#include <kbibtex/value.h>
#include <Nepomuk/Resource>
#include <Nepomuk/Thing>

#include <Akonadi/Collection>

#include <QtCore/QUrl>
#include <QtCore/QList>
#include <QtCore/QMap>

class Entry;

/**
  * @brief Pipes the content of a KBibTeX File to the Nepomuk storage
  *
  * For each BibTeX entry new resources for the publication, reference, the contacts and others will be created.
  *
  * @see NBIB ontology
  */
class BibTexToNepomukPipe : public BibTexPipe
{
    Q_OBJECT
public:
    BibTexToNepomukPipe();
    ~BibTexToNepomukPipe();

    /**
      * Does the piping action
      *
      * @p bibEntries File is a list of all Entry elements which form a bibtex entry
      */
    void pipeExport(File & bibEntries);
    void merge(Nepomuk::Resource syncResource, Entry *external, bool keepLocal);
    void mergeManual(Nepomuk::Resource syncResource, Entry *selectedDiff);

    /* updating entry */
    static Entry * getDiff(Nepomuk::Resource local, Entry *externalEntry, bool keepLocal, QSharedPointer<Entry> &localEntry);

    /**
      * Sets the Akonadi addressbook where all contacts (authors, editors etc) are imported to beside the Nepomuk storage.
      *
      * @p addressbook a valid Akonadi::Collection representing a addressbook
      */
    void setAkonadiAddressbook(Akonadi::Collection & addressbook);

    /**
      * If we import something from an online storage like Zotero we alos add the @c sync:SyncDetails
      */
    void setSyncDetails(const QString &url, const QString &userid);

    /**
      * if the @p projectThing is valid all imported data will be related via @c pimo:isRelated to the project
      */
    void setProjectPimoThing(Nepomuk::Thing projectThing);

private:
    /**
      * Used to transform the KBibTeX Person ValueItem.
      *
      * This is necessary because the Person does not contain a full name and this way around it makes
      * it easier to handle cases where one of the entries are missing
      */
    struct Name {
        QString first;
        QString last;
        QString suffix;
        QString full;
    };

    /**
      * This imports a single Bibtex Entry
      *
      * @p e One BibTeX entry
      */
    void import(Entry *e);

    /**
      * handle @note type to cretae pimo:Notes instead of the usual publication/references
      */
    void createNoteContent(Entry *e);
    void createAttachmentContent(Entry *e);

    QUrl typeToUrl(const QString & entryType);
    void addContent(const QString &key, const Value &value, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType);

    /* Helping functions */
    void addPublisher(const Value &publisherString, const Value &address, Nepomuk::Resource publication);
    void addJournal(const Value &journal, const Value &volume, const Value &number, Nepomuk::Resource publication,
                    QUrl seriesUrl = Nepomuk::Vocabulary::NBIB::Journal(),
                    QUrl issueUrl = Nepomuk::Vocabulary::NBIB::JournalIssue());
    void addSpecialArticle(const Value &titleValue, Nepomuk::Resource article, QUrl collectionUrl = Nepomuk::Vocabulary::NBIB::Encyclopedia());



    /**
      * @bug Akonadifeeder bug. item->url() can't be used to create a Resource anymore. It will result in a new resource with random URI and url to the akonadiitem
      *      this results in duplication of the contacts which won't be updated correctly
      */
    void addAuthor(const Value &content, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType);
    void addBooktitle(const QString &content, Nepomuk::Resource publication, const QString & originalEntryType);
    void addBookAuthor(const Value &contentValue, Nepomuk::Resource publication);
    void addSeriesEditor(const Value &contentValue, Nepomuk::Resource publication);
    void addChapter(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference);
    void addCrossref(const QString &content, Nepomuk::Resource publication);

    /**
      * @bug Akonadifeeder bug. item->url() can't be used to create a Resource anymore. It will result in a new resource with random URI and url to the akonadiitem
      *      this results in duplication of the contacts which won't be updated correctly
      */
    void addEditor(const Value &content, Nepomuk::Resource publication);
    void addIssn(const QString &content, Nepomuk::Resource publication);
    void addMonth(const QString &content, Nepomuk::Resource publication);
    void addOrganization(const QString &content, Nepomuk::Resource publication);
    void addCode(const QString &content, Nepomuk::Resource publication);
    void addCodeNumber(const QString &content, Nepomuk::Resource publication);
    void addCodeVolume(const QString &content, Nepomuk::Resource publication);
    void addReporter(const QString &content, Nepomuk::Resource publication);
    void addReporterVolume(const QString &content, Nepomuk::Resource publication);
    void addEvent(const QString &content, Nepomuk::Resource publication);
    void addSeries(const QString &content, Nepomuk::Resource publication);
    void addTitle(const QString &content, Nepomuk::Resource publication, Nepomuk::Resource reference, const QString & originalEntryType);
    void addAssignee(const Value &contentValue, Nepomuk::Resource publication);
    void addContributor(const Value &contentValue, Nepomuk::Resource publication);
    void addTranslator(const Value &contentValue, Nepomuk::Resource publication);
    void addReviewedAuthor(const Value &contentValue, Nepomuk::Resource publication);
    /**
      * @bug replace NFO::Website() with NFO::WebDataObject() when available
      */
    void addUrl(const QString &content, Nepomuk::Resource publication);
    void addYear(const QString &content, Nepomuk::Resource publication);
    void addKewords(const Value &content, Nepomuk::Resource publication);

    /**
      * writes teh zotero Snc details to the resource
      *
      * Also adds it as a nao:isRelated to the zoteroParent if the resource is a child note
      *
      */
    void addZoteroSyncDetails(Nepomuk::Resource publication, Nepomuk::Resource reference, Entry *e);

    /**
      * creates the contact resource and push it to nepomuk if necessary
      */
    void addContact(const Value &contentValue, Nepomuk::Resource res, QUrl property, QUrl contactType );

    /**
      * simply sets the value
      */
    void addValue(const QString &content, Nepomuk::Resource publication, QUrl property);

    /**
      * Sets the value with bibtex macro lookup to replace certain abbreviations
      */
    void addValueWithLookup(const QString &content, Nepomuk::Resource publication, QUrl property);

    QMap<QString, Nepomuk::Resource> m_allContacts;
    QMap<QString, Nepomuk::Resource> m_allProceedings;
    QMap<QString, Nepomuk::Resource> m_allSeries;
    QMap<QString, Nepomuk::Resource> m_allCollection;
    QMap<QString, Nepomuk::Resource> m_allPimoEvents;
    QMap<QString, QString> m_macroLookup;

    Akonadi::Collection m_addressbook;
    Nepomuk::Thing m_projectThing;

    QString m_syncUrl;
    QString m_syncUserId;

    bool m_replaceMode; /**< replace the content of the current publication with the content in the bibfile */
};

#endif // BIBTEXTONEPOMUKPIPE_H

