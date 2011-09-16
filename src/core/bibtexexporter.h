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

#ifndef BIBTEXEXPORTER_H
#define BIBTEXEXPORTER_H

#include <QObject>

#include <Nepomuk/Resource>
#include <Nepomuk/Tag>

enum BibTexStyle {
    BibStyle_Strict,    /**< use only exact BibTex styles put unknown Styles (patent/website/etc) in the Misc section and ommit entry if not all required fields are available*/
    BibStyle_Extended   /**< add non standard BibTex styles (Patent/Website) */
};

class BibTexExporter : public QObject
{
    Q_OBJECT
public:
    explicit BibTexExporter(QObject *parent = 0);

    void setResource(Nepomuk::Tag usedTag);
    void setResource(Nepomuk::Resource usedResource);
    void setResource(QList <Nepomuk::Resource> usedResources);

    void setStyle(BibTexStyle style);

    bool exportReferences(const QString &filename);

private:
    QString getAddress(const Nepomuk::Resource &document);
    QString getAnnote(const Nepomuk::Resource &document);
    QString getAuthor(const Nepomuk::Resource &document);
    QString getBooktitle(const Nepomuk::Resource &document);
    QString getChapter(const Nepomuk::Resource &document);
    QString getCrossref(const Nepomuk::Resource &document);
    QString getEdition(const Nepomuk::Resource &document);
    QString getEditor(const Nepomuk::Resource &document);
    QString getEprint(const Nepomuk::Resource &document);
    QString getHowpublished(const Nepomuk::Resource &document);
    QString getInstitution(const Nepomuk::Resource &document);
    QString getJournal(const Nepomuk::Resource &document);
    QString getMonth(const Nepomuk::Resource &document);
    QString getNote(const Nepomuk::Resource &document);
    QString getNumber(const Nepomuk::Resource &document);
    QString getOrganization(const Nepomuk::Resource &document);
    QString getPages(const Nepomuk::Resource &document);
    QString getPublisher(const Nepomuk::Resource &document);
    QString getSchool(const Nepomuk::Resource &document);
    QString getSeries(const Nepomuk::Resource &document);
    QString getTitle(const Nepomuk::Resource &document);
    QString getType(const Nepomuk::Resource &document);
    QString getUrl(const Nepomuk::Resource &document);
    QString getVolume(const Nepomuk::Resource &document);
    QString getYear(const Nepomuk::Resource &document);
    QString getISBN(const Nepomuk::Resource &document);
    QString getISSN(const Nepomuk::Resource &document);
    QString getDOI(const Nepomuk::Resource &document);
    QString getMrNumber(const Nepomuk::Resource &document);

    bool checkArticleCiteContent(const QString &citecontent);
    bool checkBachelorthesisCiteContent(const QString &citecontent);
    bool checkBookCiteContent(const QString &citecontent);
    bool checkBookletCiteContent(const QString &citecontent);
    bool checkConferenceCiteContent(const QString &citecontent);
    bool checkInbookCiteContent(const QString &citecontent);
    bool checkIncollectionCiteContent(const QString &citecontent);
    bool checkManualCiteContent(const QString &citecontent);
    bool checkInproceedingsCiteContent(const QString &citecontent);
    bool checkMastersthesisCiteContent(const QString &citecontent);
    bool checkMiscCiteContent(const QString &citecontent);
    bool checkPatentCiteContent(const QString &citecontent);
    bool checkPhdthesisCiteContent(const QString &citecontent);
    bool checkProceedingsCiteContent(const QString &citecontent);
    bool checkTechreportCiteContent(const QString &citecontent);
    bool checkUnpublishedCiteContent(const QString &citecontent);
    bool checkWebsiteCiteContent(const QString &citecontent);

    BibTexStyle m_style;
    QList<Nepomuk::Resource> m_resources;
};

#endif // BIBTEXEXPORTER_H
