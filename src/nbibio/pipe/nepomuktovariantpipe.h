/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef NEPOMUKTOVARIANTPIPE_H
#define NEPOMUKTOVARIANTPIPE_H

#include <QtCore/QObject>
#include <QtCore/QVariantList>

#include <Nepomuk2/Resource>

#include "nbibio/storageglobals.h"

class NepomukToVariantPipe : public QObject
{
    Q_OBJECT
public:
    explicit NepomukToVariantPipe(QObject *parent = 0);

    /**
      * Does the piping action
      *
      * @p resources list of publication or reference resources
      */
    void pipeExport(QList<Nepomuk2::Resource> resources);

    /**
     * @brief returns the created QVariantList
     */
    QVariantList variantList() const;

    /**
      * If @p strict is true the nepomuk resource types will be transformed into standard bibtex types
      *
      * For example @BachelorThesis becomes Masterthesis
      */
    void useStrictTypes(bool strict);
    void setSyncProviderDetails(const ProviderSyncDetails &psd);

    /**
      * Includes the nepomuk resource uri for the publication and reference to the bibtex output
      */
    void addNepomukUries(bool addThem);

signals:
    void progress(int value);

private:
    /**
      * @todo implement strict bibtex entry type export
      */
    QString retrieveEntryType(Nepomuk2::Resource reference, Nepomuk2::Resource publication);
    void collectContent(Nepomuk2::Resource reference, Nepomuk2::Resource publication);
    void collectNoteContent(Nepomuk2::Resource note);
    void collectAttachmentContent(Nepomuk2::Resource attachment);

    // Helper function to get the nepomuk data
    // title + booktitle
    void setTitle(Nepomuk2::Resource publication, Nepomuk2::Resource reference);
    void setSeriesEditors(Nepomuk2::Resource publication);
    void setPublicationDate(Nepomuk2::Resource publication);
    void setPublisher(Nepomuk2::Resource publication);
    void setOrganization(Nepomuk2::Resource publication);
    void setCrossref( Nepomuk2::Resource publication);
    void setUrl(Nepomuk2::Resource publication);
    void setSeries(Nepomuk2::Resource publication);
    void setJournal(Nepomuk2::Resource publication);
    void setISSN(Nepomuk2::Resource publication);
    void setChapter(Nepomuk2::Resource reference);
    void setEvent(Nepomuk2::Resource publication);
    void setCode(Nepomuk2::Resource publication);
    void setCodeNumber(Nepomuk2::Resource publication);
    void setCodeVolume(Nepomuk2::Resource publication);
    void setReporter(Nepomuk2::Resource publication);
    void setReporterVolume(Nepomuk2::Resource publication);
    void setAssignee(Nepomuk2::Resource publication);
    void setNote(Nepomuk2::Resource publication);
    void setKewords(Nepomuk2::Resource publication);
    void setArticleType(Nepomuk2::Resource publication);

    /**
      * Reuseable function that simply transforms a nepomuk value as QString to bibtex without further processing
      */
    void setValue(Nepomuk2::Resource publication, QUrl property, const QString &bibkey);
    void setContact(Nepomuk2::Resource publication, QUrl property, const QString &bibkey);

    void setSyncDetails(Nepomuk2::Resource publication);

    QVariantList m_bibtexList;
    QVariantMap m_curEntryMap;
    bool m_strict;
    bool m_addNepomukUris;
    ProviderSyncDetails m_psd;

    qreal m_percentPerResource;
};

#endif // NEPOMUKTOVARIANTPIPE_H
