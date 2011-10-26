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
#include "encoderlatex.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NUAO>
#include <Nepomuk/Variant>
#include <Nepomuk/Tag>

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

    qreal percentperFile = 100/(referenceList.size());
    int fileNumber = 0;

    int citeKeyNumer = 1;
    QString citeRef = QLatin1String("BibTexExport");

    foreach(Nepomuk::Resource resource, referenceList) {

        Nepomuk::Resource reference;
        Nepomuk::Resource publication;

        // first check if we operate on a Reference or a Publication
        if(resource.hasType( Nepomuk::Vocabulary::NBIB::Reference() )) {
            // we have a Reference
            reference = resource;
            publication = reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
        }
        else {
            //we have a publication and no idea what reference to use with it
            // we will extract as many information sas possible anyway
            publication = resource;
        }

        QString citeKey = reference.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
        if(citeKey.isEmpty()) {
            qDebug() << "unknown citeKey for the bibtex export :: create one";
            citeKey = citeRef + QString::number(citeKeyNumer);
            citeKeyNumer++;
        }

        QString entryType = retrieveEntryType(reference, publication);
        if(entryType.isEmpty()) {
            qWarning() << "unknown entry type for the bibtex export with citekey" << citeKey;
            continue;
        }

        //collect nepomuk content
        QString contentString;
        contentString = collectContent(reference, publication);

        if(contentString.isEmpty()) {
            continue;
        }

        contentString = EncoderLaTeX::currentEncoderLaTeX()->encode(contentString);

        //qDebug() << entryType << citeKey << contentString;

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

        fileNumber++;
        emit progress( percentperFile * fileNumber );

        if(m_cancel) {
            break;
        }
    }

    if(!m_cancel) {
        emit progress( 100 );
    }

    return true;
}

QString NBibExporterBibTex::retrieveEntryType(Nepomuk::Resource reference, Nepomuk::Resource publication)
{
    QString type;

    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Book()) || publication.hasType(Nepomuk::Vocabulary::NBIB::Collection())) {
        QString pages = reference.property(Nepomuk::Vocabulary::NBIB::pages()).toString();
        Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedChapter()).toResource();
        Nepomuk::Resource chapterAuthor = chapter.property(Nepomuk::Vocabulary::NCO::creator()).toResource();

        if(!pages.isEmpty() || chapter.isValid()) {
            if(chapterAuthor.isValid()) {
                type = QLatin1String("Incollection"); //book ref with defined author in the chapter
            }
            else {
                type = QLatin1String("Inbook"); //book ref with chapter/pages
            }
        }
        else {
            Nepomuk::Resource typeResource(publication.type());
            type = typeResource.genericLabel();
        }
    }
    else if(publication.hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        Nepomuk::Resource proceedings = publication.property(Nepomuk::Vocabulary::NBIB::proceedings()).toResource();
        if(proceedings.isValid()) {
            type = QLatin1String("Inproceedings"); //article in some proceedings paper
        }
        else {
            type = QLatin1String("Article"); //normal article in a journal or magazine
        }
    }
    else {
        Nepomuk::Resource typeResource(publication.type());
        type = typeResource.genericLabel();

        Nepomuk::Resource typeResource2(Nepomuk::Vocabulary::NBIB::Publication());
        if(type == typeResource2.genericLabel()) {
            type = QLatin1String("Misc");
        }
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

    returnString = getTitle(publication, reference);
    addEntry(fullstring, returnString);

    returnString = getChapter(reference);
    addEntry(fullstring, returnString);

    // if the chapter has an author attached to it , don't search author of the publication
    // solves the incollection special case where the author of the chapter is not the one from the whole collection is ment
    if(!returnString.contains(QLatin1String("author ="))) {
        returnString = getAuthors(publication);
        addEntry(fullstring, returnString);
    }

    returnString = getEditors(publication);
    addEntry(fullstring, returnString);

    returnString = getPublicationDate(publication);
    addEntry(fullstring, returnString);

    returnString = getPublisher(publication);
    addEntry(fullstring, returnString);

    returnString = getOrganization(publication);
    addEntry(fullstring, returnString);

    returnString = getUrl(publication);
    addEntry(fullstring, returnString);

    returnString = getSeries(publication);
    addEntry(fullstring, returnString);

    returnString = getEdition(publication);
    addEntry(fullstring, returnString);

    returnString = getJournal(publication);
    addEntry(fullstring, returnString);

    returnString = getVolume(publication);
    addEntry(fullstring, returnString);

    returnString = getNumber(publication);
    addEntry(fullstring, returnString);

    returnString = getPublicationMethod(publication);
    addEntry(fullstring, returnString);

    returnString = getType(publication);
    addEntry(fullstring, returnString);

    returnString = getCopyrigth(publication);
    addEntry(fullstring, returnString);

    returnString = getLastAccessed(publication);
    addEntry(fullstring, returnString);

    returnString = getEPrint(publication);
    addEntry(fullstring, returnString);

    returnString = getISBN(publication);
    addEntry(fullstring, returnString);

    returnString = getISSN(publication);
    addEntry(fullstring, returnString);

    returnString = getLCCN(publication);
    addEntry(fullstring, returnString);

    returnString = getMRNumber(publication);
    addEntry(fullstring, returnString);

    returnString = getDOI(publication);
    addEntry(fullstring, returnString);

    returnString = getAbstract(publication);
    addEntry(fullstring, returnString);

    returnString = getTOC(publication);
    addEntry(fullstring, returnString);

    returnString = getNote(publication);
    addEntry(fullstring, returnString);

    returnString = getAnnote(publication);
    addEntry(fullstring, returnString);

    returnString = getPages(reference);
    addEntry(fullstring, returnString);

    returnString = getKewords(publication);
    addEntry(fullstring, returnString);

    return fullstring;
}

QString NBibExporterBibTex::getTitle(Nepomuk::Resource publication, Nepomuk::Resource reference)
{
    QString string;
    QString title;
    QString booktitle;
    Nepomuk::Resource proceedings = publication.property(Nepomuk::Vocabulary::NBIB::proceedings()).toResource();

    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Article()) && proceedings.isValid()) {
        booktitle = proceedings.property(Nepomuk::Vocabulary::NIE::title()).toString();
        title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
    }
    else{
        //ignore the case of a reference with a chapter (inbook/incollection)
        Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedChapter()).toResource();

        if(!chapter.isValid()) {
            title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
        }
    }

    if(!title.isEmpty()) {
        string.append(QLatin1String("\ttitle = {"));
        string.append(title);
        string.append(QLatin1String("}"));
    }

    if(!booktitle.isEmpty()) {
        if(!string.isEmpty())
            string.append(br);

        string.append(QLatin1String("\tbooktitle = {"));
        string.append(booktitle);
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getChapter(Nepomuk::Resource reference)
{
    QString chapterEntry = QLatin1String("chapter");
    Nepomuk::Resource publication = reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Collection())) {
        chapterEntry = QLatin1String("title");
    }

    Nepomuk::Resource chapter = reference.property(Nepomuk::Vocabulary::NBIB::referencedChapter()).toResource();
    QString chapterTitle = chapter.property(Nepomuk::Vocabulary::NIE::title()).toString();

    Nepomuk::Resource author = chapter.property(Nepomuk::Vocabulary::NCO::creator()).toResource();
    QString chapterAuthor = author.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    Nepomuk::Resource book = reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
    QString bookTitle = book.property(Nepomuk::Vocabulary::NIE::title()).toString();

    QString string;
    if(!chapterTitle.isEmpty()) {
        string.append(QLatin1String("\t"));
        string.append(chapterEntry);
        string.append(QLatin1String(" = {"));
        string.append(chapterTitle);
        string.append(QLatin1String("}"));
        string.append(br);
        string.append(QLatin1String("\tbooktitle = {"));
        string.append(bookTitle);
        string.append(QLatin1String("}"));

        if(!chapterAuthor.isEmpty()) {
            string.append(br);
            string.append(QLatin1String("\tauthor = {"));
            string.append(chapterAuthor);
            string.append(QLatin1String("}"));
        }
    }

    return string;
}

QString NBibExporterBibTex::getAuthors(Nepomuk::Resource publication)
{
    QList<Nepomuk::Resource> authors;
    authors = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();

    QString string;

    if(!authors.isEmpty()) {
        foreach(Nepomuk::Resource a, authors) {
            //TODO don't rely only on fullname of NC::Contact
            string.append(a.property(Nepomuk::Vocabulary::NCO::fullname()).toString());
            string.append(QLatin1String(" and "));
        }

        string.chop(5);

        if(!string.isEmpty()) {
            string.prepend(QLatin1String("\tauthor = {"));
            string.append(QLatin1String("}"));
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
            string.prepend(QLatin1String("\teditor = {"));
            string.append(QLatin1String("}"));
        }
    }

    return string;
}

QString NBibExporterBibTex::getPublicationDate(Nepomuk::Resource publication)
{
    QString pdString = publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();

    QRegExp rx(QLatin1String("(\\d*)-(\\d*)-(\\d*)*"));
    QString year;
    QString month;
    QString day;
    if (rx.indexIn(pdString) != -1) {
        year = rx.cap(1);
        month = rx.cap(2);
    }
    else {
        return QString();
    }

    QString string;
    if(!month.isEmpty()) {
        int mInt = month.toInt();
        QString monthName;

        switch(mInt) {
        case 1:
            monthName.append(QLatin1String("jan"));
            break;
        case 2:
            monthName.append(QLatin1String("feb"));
            break;
        case 3:
            monthName.append(QLatin1String("mar"));
            break;
        case 4:
            monthName.append(QLatin1String("apr"));
            break;
        case 5:
            monthName.append(QLatin1String("may"));
            break;
        case 6:
            monthName.append(QLatin1String("jun"));
            break;
        case 7:
            monthName.append(QLatin1String("jul"));
            break;
        case 8:
            monthName.append(QLatin1String("aug"));
            break;
        case 9:
            monthName.append(QLatin1String("sep"));
            break;
        case 10:
            monthName.append(QLatin1String("oct"));
            break;
        case 11:
            monthName.append(QLatin1String("nov"));
            break;
        case 12:
            monthName.append(QLatin1String("dec"));
            break;
        }

        if(!monthName.isEmpty()) {
            string.prepend(QLatin1String("\tmonth = {"));
            string.append(monthName);
            string.append(QLatin1String("}"));
        }
    }

    if(!year.isEmpty()) {
        if(!string.isEmpty())
            string.append(br);

        string.append(QLatin1String("\tyear = {"));
        string.append(year);
        string.append(QLatin1String("}"));

    }

    return string;
}

QString NBibExporterBibTex::getPublisher(Nepomuk::Resource publication)
{
    QString publisherEntry = QLatin1String("publisher");
    QList<Nepomuk::Resource> publisher;

    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Thesis())) {
        publisherEntry = QLatin1String("school");
        publisher = publication.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
    }
    else if(publication.hasType(Nepomuk::Vocabulary::NBIB::Techreport())) {
        publisherEntry = QLatin1String("institution");
        publisher = publication.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
    }
    else {
        publisher = publication.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
    }

    QString string;
    QString names;
    QString address;
    if(!publisher.isEmpty()) {
        foreach(Nepomuk::Resource p, publisher) {
            //TODO don't rely only on fullname of NCO::Contact
            names.append(p.property(Nepomuk::Vocabulary::NCO::fullname()).toString());
            names.append(QLatin1String(" and "));


            Nepomuk::Resource existingAddr = p.property(Nepomuk::Vocabulary::NCO::hasPostalAddress()).toResource();
            if(existingAddr.isValid())
                address.append(existingAddr.property(Nepomuk::Vocabulary::NCO::extendedAddress()).toString());
        }

        names.chop(5);

        if(!names.isEmpty()) {
            string.append(QLatin1String("\t"));
            string.append(publisherEntry);
            string.append(QLatin1String(" = {"));
            string.append(names);
            string.append(QLatin1String("}"));
        }
        if(!address.isEmpty()) {
            string.append(br);
            string.append(QLatin1String("\taddress = {"));
            string.append(address);
            string.append(QLatin1String("}"));
        }
    }

    return string;
}

QString NBibExporterBibTex::getOrganization(Nepomuk::Resource publication)
{
    Nepomuk::Resource org;
    Nepomuk::Resource proceedings = publication.property(Nepomuk::Vocabulary::NBIB::proceedings()).toResource();
    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Article()) && proceedings.isValid()) {
        org = proceedings.property(Nepomuk::Vocabulary::NBIB::organization()).toResource();
    }
    else {
        org = publication.property(Nepomuk::Vocabulary::NBIB::organization()).toResource();
    }

    QString string = org.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\torganization = {"));
        string.append(QLatin1String("}"));
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
        string.prepend(QLatin1String("\tseries = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getEdition(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::edition()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tedition = {"));
        string.append(QLatin1String("}"));
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
        journalNumber.prepend(QLatin1String("\tnumber = {"));
        journalNumber.append(QLatin1String("}"));
    }
    if(!journalVolume.isEmpty()) {
        journalVolume.prepend(QLatin1String("\tvolume = {"));
        journalVolume.append(QLatin1String("}"));
    }
    if(!journalName.isEmpty()) {
        journalName.prepend(QLatin1String("\tjournal = {"));
        journalName.append(QLatin1String("}"));
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
        string.prepend(QLatin1String("\tvolume = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getNumber(Nepomuk::Resource publication)
{
    // this only returns proper values if the number is not used to identify a journal
    // journal numbers are captured by the getJournal method
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::issueNumber()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tnumber = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getPublicationMethod(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::publicationMethod()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\thowpublished = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getType(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::type()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\ttype = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getCopyrigth(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::copyright()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tcopyrigth = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getLastAccessed(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NUAO::lastUsage()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tlastUsage = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getEPrint(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::eprint()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\teprint = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getISBN(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::isbn()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tisbn = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getISSN(Nepomuk::Resource publication)
{
    Nepomuk::Resource series = publication.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    QString string = series.property(Nepomuk::Vocabulary::NBIB::issn()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tissn = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getLCCN(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::lccn()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tlccn = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getMRNumber(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::mrNumber()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tmrnumber = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getDOI(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::doi()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tdoi = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getAbstract(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NBIB::abstract()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tabstract = {"));
        string.append(QLatin1String("}"));
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
        string.prepend(QLatin1String("\tnote = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getAnnote(Nepomuk::Resource publication)
{
    QString string = publication.property(Nepomuk::Vocabulary::NIE::comment()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tannote = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getPages(Nepomuk::Resource reference)
{
    QString string = reference.property(Nepomuk::Vocabulary::NBIB::pages()).toString();

    if(!string.isEmpty()) {
        string.prepend(QLatin1String("\tpages = {"));
        string.append(QLatin1String("}"));
    }

    return string;
}

QString NBibExporterBibTex::getKewords(Nepomuk::Resource publication)
{
    QList<Nepomuk::Tag> tags = publication.tags();

    QString keywords;
    foreach(Nepomuk::Tag tag, tags) {
        keywords.append(tag.genericLabel());
        keywords.append(QLatin1String(", "));
    }

    keywords.chop(2);

    if(!keywords.isEmpty()) {
        keywords.prepend(QLatin1String("\tkeywords = {"));
        keywords.append(QLatin1String("}"));
    }

    return keywords;
}

QString NBibExporterBibTex::addEntry(QString &fullstring, QString entry)
{
    if(!entry.isEmpty()) {
        if(!fullstring.isEmpty())
            fullstring = fullstring + br + entry;
        else
            fullstring = entry;
    }

    return fullstring;
}
