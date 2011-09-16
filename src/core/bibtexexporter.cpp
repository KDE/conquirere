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

#include "bibtexexporter.h"

#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NCO>

#include "nbib.h"
#include <QFile>
#include <QTextStream>

#include <QDebug>


const QLatin1String br(",\n");

BibTexExporter::BibTexExporter(QObject *parent)
    : QObject(parent)
    , m_style(BibStyle_Strict)
{
}

void BibTexExporter::setResource(Nepomuk::Tag usedTag)
{
    m_resources = usedTag.tagOf();
}

void BibTexExporter::setResource(Nepomuk::Resource usedResource)
{
    m_resources.append(usedResource);
}

void BibTexExporter::setResource(QList <Nepomuk::Resource> usedResources)
{
    m_resources = usedResources;
}

void BibTexExporter::setStyle(BibTexStyle style)
{
    m_style = style;
}

bool BibTexExporter::exportReferences(const QString &filename)
{
    QFile bibFile(filename);
    if (!bibFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&bibFile);

    int citeKeyNumer = 1;
    QString citeRef = QLatin1String("BibTexExport");

    foreach(Nepomuk::Resource document, m_resources) {
        Nepomuk::Resource entryType = document.property(Nepomuk::Vocabulary::NBIB::BibResourceType()).toResource();

        QString entryString;
        if(entryType.isValid()) {
            entryString = entryType.genericLabel();
        }
        // if we don't know what type to cite, ignore entry
        else {
            continue;
        }

        QString citeKey = document.property(Nepomuk::Vocabulary::NBIB::CiteKey()).toString();
        if(citeKey.isEmpty()) {
            citeKey = citeRef + QString::number(citeKeyNumer);
        }

        // now generate the content
        QString fullstring;
        QString returnString;

        returnString = getAddress(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getAnnote(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getAuthor(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getBooktitle(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getChapter(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getCrossref(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getEdition(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getEditor(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getEprint(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getHowpublished(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getInstitution(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getJournal(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getMonth(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getNote(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getNumber(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getOrganization(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getPages(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getPublisher(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getSchool(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getSeries(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getType(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getTitle(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getUrl(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getVolume(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getYear(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getISBN(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getISSN(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getDOI(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        returnString = getMrNumber(document);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;

        fullstring.remove(QRegExp(QLatin1String("^,\n")));


        // if we have strict style, check if the necessary fields are available
        if(m_style != BibStyle_Strict) {
            if(entryType.genericLabel() == QLatin1String("Article")) {
                if(!checkArticleCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Bachelorthesis")) {
                if(!checkBachelorthesisCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Book")) {
                if(!checkBookCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Booklet")) {
                if(!checkBookletCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Conference")) {
                if(!checkConferenceCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Inbook")) {
                if(!checkInbookCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Inproceedings")) {
                if(!checkInproceedingsCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Incollection")) {
                if(!checkIncollectionCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Manual")) {
                if(!checkManualCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Mastersthesis")) {
                if(!checkMastersthesisCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Misc")) {
                if(!checkMiscCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Phdthesis")) {
                if(!checkPhdthesisCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Proceedings")) {
                if(!checkProceedingsCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Techreport")) {
                if(!checkTechreportCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Unpublished")) {
                if(!checkUnpublishedCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Patent")) {
                if(!checkPatentCiteContent(fullstring)) {
                    continue;
                }
            }
            if(entryType.genericLabel() == QLatin1String("Website")) {
                if(!checkWebsiteCiteContent(fullstring)) {
                    continue;
                }
            }
        }

        if(!fullstring.isEmpty()) {
                out << "@" << entryString << "{ " << citeKey << ",\n";
                out << fullstring << "\n";
                out << "}\n\n";

                citeKeyNumer++;
            }
    }

    bibFile.close();
    return true;
}

bool BibTexExporter::checkArticleCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }
    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("journal ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkBachelorthesisCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("school ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkBookCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }
    if(!citecontent.contains(QLatin1String("editor ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("publisher ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkBookletCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkConferenceCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("booktitle ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkInbookCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("editor ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("chapter ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("pages ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("publisher ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkIncollectionCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("editor ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("booktitle ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkManualCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkInproceedingsCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("booktitle ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkMastersthesisCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkMiscCiteContent(const QString &citecontent)
{
    return true;
}

bool BibTexExporter::checkPatentCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkPhdthesisCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("school ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkProceedingsCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkTechreportCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("institution ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("year ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkUnpublishedCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("author ="))) {
        return false;
    }

    if(!citecontent.contains(QLatin1String("title ="))) {
        return false;
    }

    return true;
}

bool BibTexExporter::checkWebsiteCiteContent(const QString &citecontent)
{
    if(!citecontent.contains(QLatin1String("url ="))) {
        return false;
    }

    return true;
}

//########################################################################

QString BibTexExporter::getAddress(const Nepomuk::Resource &document)
{
    qDebug() << "Address in BibTex TODO";
    return QString();
}

QString BibTexExporter::getAnnote(const Nepomuk::Resource &document)
{
    QString annoteString = document.property(Nepomuk::Vocabulary::NBIB::Annote()).toString();

    if(!annoteString.isEmpty()) {
        annoteString.prepend(QLatin1String("\tannote = \""));
        annoteString.append(QLatin1String("\""));
    }

    return annoteString;
}

QString BibTexExporter::getAuthor(const Nepomuk::Resource &document)
{
    QString authorString;
    QList<Nepomuk::Resource> authors = document.property(Nepomuk::Vocabulary::NBIB::Author()).toResourceList();
    if(!authors.isEmpty()) {
        foreach(Nepomuk::Resource a, authors) {
            //TODO don't rely only on fullname of NC::Contact
            authorString.append(a.property(Nepomuk::Vocabulary::NCO::fullname()).toString());
            authorString.append(QLatin1String(" and "));
        }
        authorString.chop(5);
        if(!authorString.isEmpty()) {
            authorString.prepend(QLatin1String("\tauthor = \""));
            authorString.append(QLatin1String("\""));
        }
    }

    return authorString;
}

QString BibTexExporter::getBooktitle(const Nepomuk::Resource &document)
{
    QString booktitleString = document.property(Nepomuk::Vocabulary::NBIB::Booktitle()).toString();

    if(!booktitleString.isEmpty()) {
        booktitleString.prepend(QLatin1String("\tbooktitle = \""));
        booktitleString.append(QLatin1String("\""));
    }

    return booktitleString;
}

QString BibTexExporter::getChapter(const Nepomuk::Resource &document)
{
    QString chapterString = document.property(Nepomuk::Vocabulary::NBIB::Chapter()).toString();

    if(!chapterString.isEmpty()) {
        chapterString.prepend(QLatin1String("\tchapter = \""));
        chapterString.append(QLatin1String("\""));
    }

    return chapterString;
}

QString BibTexExporter::getCrossref(const Nepomuk::Resource &document)
{
    QString crossRefString = document.property(Nepomuk::Vocabulary::NBIB::Crossref()).toString();

    if(!crossRefString.isEmpty()) {
        crossRefString.prepend(QLatin1String("\tcrossref = \""));
        crossRefString.append(QLatin1String("\""));
    }

    return crossRefString;
}

QString BibTexExporter::getEdition(const Nepomuk::Resource &document)
{
    QString editionString = document.property(Nepomuk::Vocabulary::NBIB::Edition()).toString();

    if(!editionString.isEmpty()) {
        editionString.prepend(QLatin1String("\tedition = \""));
        editionString.append(QLatin1String("\""));
    }

    return editionString;
}

QString BibTexExporter::getEditor(const Nepomuk::Resource &document)
{
    QString authorString;
    QList<Nepomuk::Resource> authors = document.property(Nepomuk::Vocabulary::NBIB::Editor()).toResourceList();
    if(!authors.isEmpty()) {
        foreach(Nepomuk::Resource a, authors) {
            //TODO don't rely only on fullname of NC::Contact
            authorString.append(a.property(Nepomuk::Vocabulary::NCO::fullname()).toString());
            authorString.append(QLatin1String(" and "));
        }
        authorString.chop(5);
        if(!authorString.isEmpty()) {
            authorString.prepend(QLatin1String("\tauthor = \""));
            authorString.append(QLatin1String("\""));
        }
    }

    return authorString;
}

QString BibTexExporter::getEprint(const Nepomuk::Resource &document)
{
    QString epString = document.property(Nepomuk::Vocabulary::NBIB::Eprint()).toString();

    if(!epString.isEmpty()) {
        epString.prepend(QLatin1String("\teprint = \""));
        epString.append(QLatin1String("\""));
    }

    return epString;
}

QString BibTexExporter::getHowpublished(const Nepomuk::Resource &document)
{
    QString hpString = document.property(Nepomuk::Vocabulary::NBIB::HowPublished()).toString();

    if(!hpString.isEmpty()) {
        hpString.prepend(QLatin1String("\thowpublished = \""));
        hpString.append(QLatin1String("\""));
    }

    return hpString;
}

QString BibTexExporter::getInstitution(const Nepomuk::Resource &document)
{
    QString institutionString = document.property(Nepomuk::Vocabulary::NBIB::Institution()).toString();

    if(!institutionString.isEmpty()) {
        institutionString.prepend(QLatin1String("\tinstitution = \""));
        institutionString.append(QLatin1String("\""));
    }

    return institutionString;
}

QString BibTexExporter::getJournal(const Nepomuk::Resource &document)
{
    QString journalString = document.property(Nepomuk::Vocabulary::NBIB::Journal()).toString();

    if(!journalString.isEmpty()) {
        journalString.prepend(QLatin1String("\tjournal = \""));
        journalString.append(QLatin1String("\""));
    }

    return journalString;
}

QString BibTexExporter::getMonth(const Nepomuk::Resource &document)
{
    QString monthString = document.property(Nepomuk::Vocabulary::NBIB::Month()).toString();

    if(!monthString.isEmpty()) {
        int mInt = monthString.toInt();

        monthString.prepend(QLatin1String("\tmonth = \""));
        switch(mInt) {
        case 1:
            monthString.append(QLatin1String("jan"));
            break;
        case 2:
            monthString.append(QLatin1String("feb"));
            break;
        case 3:
            monthString.append(QLatin1String("mar"));
            break;
        case 4:
            monthString.append(QLatin1String("apr"));
            break;
        case 5:
            monthString.append(QLatin1String("may"));
            break;
        case 6:
            monthString.append(QLatin1String("jun"));
            break;
        case 7:
            monthString.append(QLatin1String("jul"));
            break;
        case 8:
            monthString.append(QLatin1String("aug"));
            break;
        case 9:
            monthString.append(QLatin1String("sep"));
            break;
        case 10:
            monthString.append(QLatin1String("oct"));
            break;
        case 11:
            monthString.append(QLatin1String("nov"));
            break;
        case 12:
            monthString.append(QLatin1String("dec"));
            break;
        default:
            return QString();
        }

        monthString.append(QLatin1String("\""));
    }

    return monthString;
}

QString BibTexExporter::getNote(const Nepomuk::Resource &document)
{
    QString noteString = document.property(Nepomuk::Vocabulary::NBIB::Note()).toString();

    if(!noteString.isEmpty()) {
        noteString.prepend(QLatin1String("\tnote = \""));
        noteString.append(QLatin1String("\""));
    }

    return noteString;
}

QString BibTexExporter::getNumber(const Nepomuk::Resource &document)
{
    QString numberString = document.property(Nepomuk::Vocabulary::NBIB::Number()).toString();

    if(!numberString.isEmpty()) {
        numberString.prepend(QLatin1String("\tnumber = \""));
        numberString.append(QLatin1String("\""));
    }

    return numberString;
}

QString BibTexExporter::getOrganization(const Nepomuk::Resource &document)
{
    QString orgString = document.property(Nepomuk::Vocabulary::NBIB::Organization()).toString();

    if(!orgString.isEmpty()) {
        orgString.prepend(QLatin1String("\torganization = \""));
        orgString.append(QLatin1String("\""));
    }

    return orgString;
}

QString BibTexExporter::getPages(const Nepomuk::Resource &document)
{
    QString pagesString = document.property(Nepomuk::Vocabulary::NBIB::Pages()).toString();

    if(!pagesString.isEmpty()) {
        pagesString.prepend(QLatin1String("\tpages = \""));
        pagesString.append(QLatin1String("\""));
    }

    return pagesString;
}

QString BibTexExporter::getPublisher(const Nepomuk::Resource &document)
{
    QString publisherString = document.property(Nepomuk::Vocabulary::NBIB::Publisher()).toString();

    if(!publisherString.isEmpty()) {
        publisherString.prepend(QLatin1String("\tpublisher = \""));
        publisherString.append(QLatin1String("\""));
    }

    return publisherString;
}

QString BibTexExporter::getSchool(const Nepomuk::Resource &document)
{
    QString schoolString = document.property(Nepomuk::Vocabulary::NBIB::School()).toString();

    if(!schoolString.isEmpty()) {
        schoolString.prepend(QLatin1String("\tschool = \""));
        schoolString.append(QLatin1String("\""));
    }

    return schoolString;
}

QString BibTexExporter::getSeries(const Nepomuk::Resource &document)
{
    QString seriesString = document.property(Nepomuk::Vocabulary::NBIB::Series()).toString();

    if(!seriesString.isEmpty()) {
        seriesString.prepend(QLatin1String("\tseries = \""));
        seriesString.append(QLatin1String("\""));
    }

    return seriesString;
}

QString BibTexExporter::getTitle(const Nepomuk::Resource &document)
{
    QString titleString = document.property(Nepomuk::Vocabulary::NBIB::Title()).toString();

    if(!titleString.isEmpty()) {
        titleString.prepend(QLatin1String("\ttitle = \""));
        titleString.append(QLatin1String("\""));
    }

    return titleString;
}

QString BibTexExporter::getType(const Nepomuk::Resource &document)
{
    QString typeString = document.property(Nepomuk::Vocabulary::NBIB::Type()).toString();

    if(!typeString.isEmpty()) {
        typeString.prepend(QLatin1String("\ttype = \""));
        typeString.append(QLatin1String("\""));
    }

    return typeString;
}

QString BibTexExporter::getUrl(const Nepomuk::Resource &document)
{
    QString urlString = document.property(Nepomuk::Vocabulary::NBIB::Url()).toString();

    if(!urlString.isEmpty()) {
        urlString.prepend(QLatin1String("\turl = \""));
        urlString.append(QLatin1String("\""));
    }

    return urlString;
}

QString BibTexExporter::getVolume(const Nepomuk::Resource &document)
{
    QString volumeString = document.property(Nepomuk::Vocabulary::NBIB::Volume()).toString();

    if(!volumeString.isEmpty()) {
        volumeString.prepend(QLatin1String("\tvolume = \""));
        volumeString.append(QLatin1String("\""));
    }

    return volumeString;
}

QString BibTexExporter::getYear(const Nepomuk::Resource &document)
{
    QString yearString = document.property(Nepomuk::Vocabulary::NBIB::Year()).toString();

    if(!yearString.isEmpty()) {
        yearString.prepend(QLatin1String("\tyear = \""));
        yearString.append(QLatin1String("\""));
    }

    return yearString;
}

QString BibTexExporter::getISBN(const Nepomuk::Resource &document)
{
    QString isbnString = document.property(Nepomuk::Vocabulary::NBIB::ISBN()).toString();

    if(!isbnString.isEmpty()) {
        isbnString.prepend(QLatin1String("\tisbn = \""));
        isbnString.append(QLatin1String("\""));
    }

    return isbnString;
}

QString BibTexExporter::getISSN(const Nepomuk::Resource &document)
{
    QString issnString = document.property(Nepomuk::Vocabulary::NBIB::ISSN()).toString();

    if(!issnString.isEmpty()) {
        issnString.prepend(QLatin1String("\tissn = \""));
        issnString.append(QLatin1String("\""));
    }

    return issnString;
}

QString BibTexExporter::getDOI(const Nepomuk::Resource &document)
{
    QString doiString = document.property(Nepomuk::Vocabulary::NBIB::DOI()).toString();

    if(!doiString.isEmpty()) {
        doiString.prepend(QLatin1String("\tdoi = \""));
        doiString.append(QLatin1String("\""));
    }

    return doiString;
}

QString BibTexExporter::getMrNumber(const Nepomuk::Resource &document)
{
    QString mrnumberString = document.property(Nepomuk::Vocabulary::NBIB::MRNumber()).toString();

    if(!mrnumberString.isEmpty()) {
        mrnumberString.prepend(QLatin1String("\tmrnumber = \""));
        mrnumberString.append(QLatin1String("\""));
    }

    return mrnumberString;
}
