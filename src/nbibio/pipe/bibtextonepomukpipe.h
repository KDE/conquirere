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
#include "nbib/nbib.h"

#include <kbibtex/value.h>

#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/SimpleResource>

#include "sro/nbib/publication.h"
#include "sro/nbib/reference.h"

#include <Nepomuk2/Resource>
#include <Nepomuk2/Thing>

#include <Akonadi/Collection>

#include <QtCore/QUrl>
#include <QtCore/QList>
#include <QtCore/QMap>

class Entry;
class KJob;

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

    /**
      * Does the piping action for local bibtex files
      *
      * @p bibEntries File is a list of all Entry elements which form a bibtex entry
      * @p importFile is used to resolve relative paths of local files
      */
    void pipeExport(File & bibEntries, const KUrl& importFile);
    void merge(Nepomuk2::Resource syncResource, Entry *external, bool keepLocal);
    void mergeManual(Nepomuk2::Resource syncResource, Entry *selectedDiff);

    /* updating entry */
    static Entry * getDiff(Nepomuk2::Resource local, Entry *externalEntry, bool keepLocal, QSharedPointer<Entry> &localEntry);

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
    void setProjectPimoThing(Nepomuk2::Thing projectThing);

private slots:
    void slotSaveToNepomukDone(KJob *job);

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

    void importNote(Entry *entry, Nepomuk2::SimpleResourceGraph &graph);
    void importAttachment(Entry *entry, Nepomuk2::SimpleResourceGraph &graph);
    void importBibResource(Entry *entry, Nepomuk2::SimpleResourceGraph &graph);
    void addPublicationSubTypes(Nepomuk2::NBIB::Publication &publication, Entry *entry);
    void handleSpecialCases(Entry *entry, Nepomuk2::SimpleResourceGraph &graph, Nepomuk2::NBIB::Publication &publication, Nepomuk2::NBIB::Reference &reference);


    /* Helping functions */
    void addPublisher(const Value &publisherValue, const Value &addressValue, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);

    void addJournal(const Value &journal, const Value &volume, const Value &number, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph,
                    QUrl seriesUrl = Nepomuk2::Vocabulary::NBIB::Journal(),
                    QUrl issueUrl = Nepomuk2::Vocabulary::NBIB::JournalIssue());
    void addSpecialArticle(const Value &titleValue, Nepomuk2::NBIB::Publication &article, Nepomuk2::SimpleResourceGraph &graph, QUrl collectionUrl = Nepomuk2::Vocabulary::NBIB::Encyclopedia());

    void addContent(const QString &key, const Value &value, Nepomuk2::NBIB::Publication &publication,
                    Nepomuk2::NBIB::Reference &reference, Nepomuk2::SimpleResourceGraph &graph,
                    const QString & originalEntryType, const QString & citeKey);

    void addNote(const QString &content, const QString &noteType, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);


    void addAuthor(const Value &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::NBIB::Reference &reference, Nepomuk2::SimpleResourceGraph &graph, const QString & originalEntryType);
    void addBooktitle(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph, const QString & originalEntryType);
    void addSeriesEditor(const Value &contentValue, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);
    void addChapter(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::NBIB::Reference &reference, Nepomuk2::SimpleResourceGraph &graph);
    void addChapterName(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::NBIB::Reference &reference, Nepomuk2::SimpleResourceGraph &graph);
    void addIssn(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);

    void addOrganization(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);
    void addCode(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);
    void addCodeNumber(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);
    void addCodeVolume(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);
    void addReporter(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);
    void addReporterVolume(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);
    void addEvent(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);
    void addSeries(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);
    void addTitle(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::NBIB::Reference &reference, Nepomuk2::SimpleResourceGraph &graph, const QString & originalEntryType);
    /**
      * @bug split webpages by ',' or ';' if necessary
      */
    void addWebsite(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);
    void addFileUrl(const QString &content, Nepomuk2::NBIB::Publication &publication, Nepomuk2::SimpleResourceGraph &graph);
    void addPublicationDate(const QString &fullDate, Nepomuk2::NBIB::Publication &publication);
    void addPublicationDate(const QString &year, const QString &month, const QString &day, Nepomuk2::NBIB::Publication &publication);

    void addTag(const Value &content, Nepomuk2::SimpleResource &resource, Nepomuk2::SimpleResourceGraph &graph);
    void addTopic(const Value &content, Nepomuk2::SimpleResource &resource, Nepomuk2::SimpleResourceGraph &graph);

    /**
      * writes the zotero Snc details to the resource
      *
      * Also adds it as a nao:isRelated to the zoteroParent if the resource is a child note
      *
      */
    void addZoteroSyncDetails(Nepomuk2::SimpleResource &mainResource, Nepomuk2::SimpleResource &referenceResource,Entry *e, Nepomuk2::SimpleResourceGraph &graph);

    /**
      * creates the contact resource and push it to nepomuk if necessary
      */
    void addContact(const Value &contentValue, Nepomuk2::SimpleResource &resource, Nepomuk2::SimpleResourceGraph &graph, QUrl contactProperty, QUrl contactType );

    /**
      * simply sets the value
      */
    void addValue(const QString &content, Nepomuk2::SimpleResource &resource, QUrl property);

    /**
      * Sets the value with bibtex macro lookup to replace certain abbreviations
      */
    void addValueWithLookup(const QString &content, Nepomuk2::SimpleResource &resource, QUrl property);

    QMap<QString, QString> m_macroLookup;

    Akonadi::Collection m_addressbook;
    Nepomuk2::Thing m_projectThing;

    QString m_syncUrl;
    QString m_syncUserId;

    bool m_replaceMode; /**< replace the content of the current publication with the content in the bibfile */
    Nepomuk2::Resource m_publicationToReplace;
    Nepomuk2::Resource m_referenceToReplace;

    KUrl m_bibtexImportFile;
};

#endif // BIBTEXTONEPOMUKPIPE_H

