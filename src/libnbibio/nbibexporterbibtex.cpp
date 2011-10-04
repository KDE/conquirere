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

#include "nbibexporterbibtex.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NUAO>
#include <Nepomuk/Variant>

#include <QDebug>

const QLatin1String br(",\n");

NBibExporterBibTex::NBibExporterBibTex() :
    NBibExporter()
{
}

bool NBibExporterBibTex::save(QIODevice *iodevice, const QList<Nepomuk::Resource> referenceList, QStringList *errorLog)
{
    if(referenceList.isEmpty()) {
        return false;
    }

    int citeKeyNumer = 1;
    QString citeRef = QLatin1String("BibTexExport");

    foreach(Nepomuk::Resource document, referenceList) {

        Nepomuk::Resource reference;
        Nepomuk::Resource publication;

        // first check if we operate on a Reference or a Publication
        if(document.hasType( Nepomuk::Vocabulary::NBIB::Reference() )) {
            // we have a Reference
            reference = document;
            publication = reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
        }
        else {
            //we have a publication and no idea what reference to use with it
            // we will extract as many information sas possible anyway
            publication = document;
        }

        //collect nepomuk content
        QString contentString;

        contentString = collectContent(reference, publication);

        if(contentString.isEmpty()) {
            continue;
        }

        QString entryType = retrieveEntryType(reference, publication);

        if(entryType.isEmpty()) {
            qWarning() << "unknown entry type for the boibtex export";
            continue;
        }

        QString citeKey = reference.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();

        if(citeKey.isEmpty()) {
            qWarning() << "unknown citeKey for the boibtex export :: create one";
            citeKey = citeRef + QString::number(citeKeyNumer);
            citeKeyNumer++;
        }

        qDebug() << entryType << citeKey << contentString;

        iodevice->putChar('@');
        iodevice->write(entryType.toAscii().data());
        iodevice->putChar('{');
        iodevice->write(citeKey.toAscii().data());
        iodevice->putChar(',');
        iodevice->putChar('\n');
        iodevice->write(contentString.toAscii().data());
        iodevice->putChar('\n');
        iodevice->putChar('}');
        iodevice->putChar('\n');
        iodevice->putChar('\n');
    }

    return true;
}

QString NBibExporterBibTex::retrieveEntryType(Nepomuk::Resource reference, Nepomuk::Resource publication)
{
    QString type;

    Nepomuk::Resource typeResource(publication.type());

    type = typeResource.genericLabel();

    // now handle some special cases
    QString pages = reference.property(Nepomuk::Vocabulary::NBIB::pages()).toString();
    Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedChapter()).toResource();
    Nepomuk::Resource chapterAuthor = chapter.property(Nepomuk::Vocabulary::NCO::creator()).toResource();

    if(!pages.isEmpty() || chapter.isValid()) {
        if(chapterAuthor.isValid()) {
            type = QLatin1String("Incollection");
        }
        else {
            type = QLatin1String("Inbook");
        }
    }

    if(publication.type() == Nepomuk::Vocabulary::NBIB::Publication().toString()) {
        type = QLatin1String("Misc");
    }

    // if we have strict export, transforn into standard types
    //    if(false) {
    //        if(type.contains(QRegExp("")))
    //    }

    return type;
}

QString NBibExporterBibTex::collectContent(Nepomuk::Resource reference, Nepomuk::Resource publication)
{
    QString fullstring;
    QString returnString;

    returnString = getTitle(publication);
    if(!returnString.isEmpty())
        fullstring = returnString;

    returnString = getChapter(reference);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    // if the chapter has an author attached to it , don't search author of the publication
    if(!returnString.contains(QLatin1String("author ="))) {
        returnString = getAuthors(publication);
        if(!returnString.isEmpty())
            fullstring = fullstring + br + returnString;
    }

    returnString = getEditors(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getPublicationDate(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getPublisher(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getOrganization(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getCrossref(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getUrl(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getSeries(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getEdition(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getJournal(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getVolume(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getNumber(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getPublicationMethod(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getType(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getCopyrigth(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getLastAccessed(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getEPrint(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getISBN(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getISSN(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getLCCN(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getMRNumber(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getDOI(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getAbstract(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getTOC(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getNote(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getAnnote(publication);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    returnString = getPages(reference);
    if(!returnString.isEmpty())
        fullstring = fullstring + br + returnString;

    return fullstring;
}

QString NBibExporterBibTex::getTitle(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\ttitle = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getAuthors(Nepomuk::Resource publication)
{
    QString string;
    QList<Nepomuk::Resource> authors = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();
    if(!authors.isEmpty()) {
        foreach(Nepomuk::Resource a, authors) {
            //TODO don't rely only on fullname of NC::Contact
            string.append(a.property(Nepomuk::Vocabulary::NCO::fullname()).toString());
            string.append(QLatin1String(" and "));
        }

        string.chop(5);

        if(!string.isEmpty()) {
            string.prepend(QLatin1String("\tauthor = \""));
            string.append(QLatin1String("\""));
        }
    }

    return string;
}

QString NBibExporterBibTex::getEditors(Nepomuk::Resource publication)
{
    QString string;
    QList<Nepomuk::Resource> authors = publication.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();
    if(!authors.isEmpty()) {
        foreach(Nepomuk::Resource a, authors) {
            //TODO don't rely only on fullname of NC::Contact
            string.append(a.property(Nepomuk::Vocabulary::NCO::fullname()).toString());
            string.append(QLatin1String(" and "));
        }

        string.chop(5);

        if(!string.isEmpty()) {
            string.prepend(QLatin1String("\teditor = \""));
            string.append(QLatin1String("\""));
        }
    }

    return string;
}

QString NBibExporterBibTex::getPublicationDate(Nepomuk::Resource publication)
{
    QString pdString = publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();

    QRegExp rx(QLatin1String("(\\d{4})-(\\d{2})-(\\d{2})*"));
    QString year;
    QString month;
    QString day;
    if (rx.indexIn(pdString) != -1) {
        year = rx.cap(1);
        month = rx.cap(2);
        day = rx.cap(3);
    }

    QString string;
    if(!month.isEmpty()) {
        int mInt = month.toInt();

        string.prepend(QLatin1String("\tmonth = \""));
        switch(mInt) {
        case 1:
            string.append(QLatin1String("jan"));
            break;
        case 2:
            string.append(QLatin1String("feb"));
            break;
        case 3:
            string.append(QLatin1String("mar"));
            break;
        case 4:
            string.append(QLatin1String("apr"));
            break;
        case 5:
            string.append(QLatin1String("may"));
            break;
        case 6:
            string.append(QLatin1String("jun"));
            break;
        case 7:
            string.append(QLatin1String("jul"));
            break;
        case 8:
            string.append(QLatin1String("aug"));
            break;
        case 9:
            string.append(QLatin1String("sep"));
            break;
        case 10:
            string.append(QLatin1String("oct"));
            break;
        case 11:
            string.append(QLatin1String("nov"));
            break;
        case 12:
            string.append(QLatin1String("dec"));
            break;
        default:
            return month; //DEBUG
        }

        string.append(QLatin1String("\""));
    }

    if(!year.isEmpty()) {
        string.append(br);
        string.append(QLatin1String("\tyear = \""));
        string.append(year);
        string.append(QLatin1String("\""));

    }

    return string;
}

QString NBibExporterBibTex::getPublisher(Nepomuk::Resource publication)
{
    QString string;
    QList<Nepomuk::Resource> authors = publication.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
    if(!authors.isEmpty()) {
        foreach(Nepomuk::Resource a, authors) {
            //TODO don't rely only on fullname of NCO::Contact
            //TODO find publisher address
            string.append(a.property(Nepomuk::Vocabulary::NCO::fullname()).toString());
            string.append(QLatin1String(" and "));
        }

        string.chop(5);

        if(!string.isEmpty()) {
            string.prepend(QLatin1String("\tpublisher = \""));
            string.append(QLatin1String("\""));
        }
    }

    return string;
}

QString NBibExporterBibTex::getOrganization(Nepomuk::Resource publication)
{
    Nepomuk::Resource org = publication.property(Nepomuk::Vocabulary::NBIB::organization()).toResource();

    QString string = org.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\torganization = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getCrossref(Nepomuk::Resource publication)
{
    QString string;
    QList<Nepomuk::Resource> crossrefs = publication.property(Nepomuk::Vocabulary::NIE::links()).toResourceList();
    if(!crossrefs.isEmpty()) {
        foreach(Nepomuk::Resource a, crossrefs) {
            string.append(a.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString());
            string.append(QLatin1String(", "));
        }

        string.chop(2);

        if(!string.isEmpty()) {
            string.prepend(QLatin1String("\tcrossref = \""));
            string.append(QLatin1String("\""));
        }
    }

    return string;
}

QString NBibExporterBibTex::getUrl(Nepomuk::Resource publication)
{
    qDebug() << "NBibExporterBibTex::getUrl /!\ needs proper implementation /!\ ";

    return QString();
}

QString NBibExporterBibTex::getSeries(Nepomuk::Resource publication)
{
    Nepomuk::Resource series = publication.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    QString string = series.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tseries = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getEdition(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::edition()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tedition = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getJournal(Nepomuk::Resource publication)
{
    Nepomuk::Resource journalIssue = publication.property(Nepomuk::Vocabulary::NBIB::journalIssue()).toResource();

    if(!journalIssue.isValid()) {
        return QString(); // no journal available for his resource
    }

    // if we have a JournalIssue, get the journal contact and number/volume
    QString journalNumber = journalIssue.property(Nepomuk::Vocabulary::NBIB::issueNumber()).toString();
    QString journalVolume = journalIssue.property(Nepomuk::Vocabulary::NBIB::volume()).toString();

    Nepomuk::Resource journal = journalIssue.property(Nepomuk::Vocabulary::NBIB::journal()).toResource();
    QString journalName = journal.property(Nepomuk::Vocabulary::NIE::title()).toString();;

    if(!journalNumber.isEmpty()) {
        journalNumber.prepend(QLatin1String("\tnumber = \""));
        journalNumber.append(QLatin1String("\""));
    }
    if(!journalVolume.isEmpty()) {
        journalVolume.prepend(QLatin1String("\tvolume = \""));
        journalVolume.append(QLatin1String("\""));
    }
    if(!journalName.isEmpty()) {
        journalName.prepend(QLatin1String("\tjournal = \""));
        journalName.append(QLatin1String("\""));
    }

    QString string = journalNumber + br + journalVolume + br + journalName;

    return string;
}

QString NBibExporterBibTex::getVolume(Nepomuk::Resource publication)
{
    // this only returns proper values if the volume is not used to identify a journal
    // journal volumes are captured by the getJournal method
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::volume()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tvolume = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getNumber(Nepomuk::Resource publication)
{
    // this only returns proper values if the number is not used to identify a journal
    // journal numbers are captured by the getJournal method
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::issueNumber()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tnumber = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getPublicationMethod(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::publicationMethod()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\thowpublished = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getType(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::type()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\ttype = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getCopyrigth(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::copyright()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tcopyrigth = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getLastAccessed(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NUAO::lastUsage()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tlastUsage = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getEPrint(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::eprint()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\teprint = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getISBN(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::isbn()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tisbn = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getISSN(Nepomuk::Resource publication)
{
    qDebug() << "NBibExporterBibTex::getISSN /!\ needs proper implementation /!\ ";
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::issn()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tissn = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getLCCN(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::lccn()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tlccn = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getMRNumber(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::mrNumber()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tmrnumber = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getDOI(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::doi()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tdoi = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getAbstract(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::abstract()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tabstract = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getTOC(Nepomuk::Resource publication)
{
    qDebug() << "NBibExporterBibTex::getTOC /!\ needs proper implementation /!\ ";

    return QString();
}

QString NBibExporterBibTex::getNote(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::description()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tnote = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getAnnote(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::comment()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tannote = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}

QString NBibExporterBibTex::getChapter(Nepomuk::Resource reference)
{
    Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedChapter()).toResource();

    QString chapterTitle = chapter.property(Nepomuk::Vocabulary::NIE::title()).toString();

    Nepomuk::Resource author = chapter.property(Nepomuk::Vocabulary::NCO::creator()).toResource();
    QString chapterAuthor = author.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    Nepomuk::Resource book = reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
    QString bookTitle = book.property(Nepomuk::Vocabulary::NIE::title()).toString();

    QString string;
    if(!chapterTitle.isEmpty()) {
        string.prepend(QLatin1String("\tchapter = \""));
        string.append(chapterTitle);
        string.append(QLatin1String("\""));
        string.append(br);
        string.append(QLatin1String("\tbooktitle = \""));
        string.append(bookTitle);
        string.append(QLatin1String("\""));

        if(!chapterAuthor.isEmpty()) {
            string.append(br);
            string.append(QLatin1String("\tauthor = \""));
            string.append(chapterAuthor);
            string.append(QLatin1String("\""));
        }
    }

    return string;
}

QString NBibExporterBibTex::getPages(Nepomuk::Resource reference)
{
    QString string = reference.property(Nepomuk::Vocabulary::NBIB::pages()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tpages = \""));
        string.append(QLatin1String("\""));
    }

    return string;
}
