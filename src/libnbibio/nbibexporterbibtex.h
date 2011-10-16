/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef NBIBEXPORTERBIBTEX_H
#define NBIBEXPORTERBIBTEX_H

#include "nbibexporter.h"

class NBibExporterBibTex : public NBibExporter
{
    Q_OBJECT
public:
    explicit NBibExporterBibTex();

    bool save(QIODevice *iodevice, const QList<Nepomuk::Resource> referenceList, QStringList *errorLog = NULL);

private:
    QString retrieveEntryType(Nepomuk::Resource reference, Nepomuk::Resource publication);
    QString collectContent(Nepomuk::Resource reference, Nepomuk::Resource publication);

    // Helper function to get the nepomuk data
    QString getTitle(Nepomuk::Resource publication, Nepomuk::Resource reference);
    QString getAuthors(Nepomuk::Resource publication);
    QString getEditors(Nepomuk::Resource publication);
    QString getPublicationDate(Nepomuk::Resource publication);
    QString getPublisher(Nepomuk::Resource publication);
    QString getOrganization(Nepomuk::Resource publication);
    QString getCrossref(Nepomuk::Resource publication);
    QString getUrl(Nepomuk::Resource publication);
    QString getSeries(Nepomuk::Resource publication);
    QString getEdition(Nepomuk::Resource publication);
    QString getJournal(Nepomuk::Resource publication);
    QString getVolume(Nepomuk::Resource publication);
    QString getNumber(Nepomuk::Resource publication);
    QString getPublicationMethod(Nepomuk::Resource publication);
    QString getType(Nepomuk::Resource publication);
    QString getCopyrigth(Nepomuk::Resource publication);
    QString getLastAccessed(Nepomuk::Resource publication);
    QString getEPrint(Nepomuk::Resource publication);
    QString getISBN(Nepomuk::Resource publication);
    QString getISSN(Nepomuk::Resource publication);
    QString getLCCN(Nepomuk::Resource publication);
    QString getMRNumber(Nepomuk::Resource publication);
    QString getDOI(Nepomuk::Resource publication);
    QString getAbstract(Nepomuk::Resource publication);
    QString getTOC(Nepomuk::Resource publication);
    QString getNote(Nepomuk::Resource publication);
    QString getAnnote(Nepomuk::Resource publication);
    QString getChapter(Nepomuk::Resource reference);
    QString getPages(Nepomuk::Resource reference);

    QString addEntry(QString &fullstring, QString entry);
};

#endif // NBIBEXPORTERBIBTEX_H
