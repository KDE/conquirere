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

#include "zoteroinfo.h"

QStringList ZoteroInfo::defaultKeysFor(const QString &key)
{
    // retrns a list of default keys in bibtey style format
    // necessary to know when an entry on the server was deleted. Them no entry is returned back
    // and we need to initialize those as "" so we can remove it locally
    // as the local side supports more keys than zotero does, we can't simple remove all entries not
    // returned by the server :/
    QStringList defaultKeys;

    if(key == QLatin1String("attachment")) {
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("linkMode");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("note");
        defaultKeys << QLatin1String("mimeType");
        defaultKeys << QLatin1String("charset");
        defaultKeys << QLatin1String("keywords");
    }
    else if(key == QLatin1String("artwork")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("artworkMedium");
        defaultKeys << QLatin1String("artworkSize");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("audioRecording")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("editor") <<  QLatin1String("contributor") <<  QLatin1String("wordsBy");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("audioRecording");
        defaultKeys << QLatin1String("audioRecordingFormat");
        defaultKeys << QLatin1String("volume");
        defaultKeys << QLatin1String("numberOfVolumes");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("runningTime");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("isbn");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("publisher");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("bill")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("cosponsor");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("code");
        defaultKeys << QLatin1String("codeVolume");
        defaultKeys << QLatin1String("history");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("chapter");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("organization");
        defaultKeys << QLatin1String("event");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("blogPost")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("commenter") <<  QLatin1String("contributor");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("booktitle");
        defaultKeys << QLatin1String("type");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("book")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("editor") <<  QLatin1String("contributor") <<  QLatin1String("seriesEditor") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("volume");
        defaultKeys << QLatin1String("numberOfVolumes");
        defaultKeys << QLatin1String("edition");
        defaultKeys << QLatin1String("publisher");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("numPages");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("isbn");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("note");
        defaultKeys << QLatin1String("address");
    }
    else if(key == QLatin1String("bookSection")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("bookAuthor") <<  QLatin1String("editor") <<  QLatin1String("contributor") <<  QLatin1String("seriesEditor") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("booktitle");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("volume");
        defaultKeys << QLatin1String("numberOfVolumes");
        defaultKeys << QLatin1String("edition");
        defaultKeys << QLatin1String("publisher");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("isbn");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("case")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("counsel");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("reporter");
        defaultKeys << QLatin1String("history");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("reportervolume");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("organization");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("computerProgram")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("computerProgram");
        defaultKeys << QLatin1String("version");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("system");
        defaultKeys << QLatin1String("company");
        defaultKeys << QLatin1String("programmingLanguage");
        defaultKeys << QLatin1String("isbn");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("conferencePaper")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("editor") <<  QLatin1String("seriesEditor") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("conferencePaper");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("publisher");
        defaultKeys << QLatin1String("volume");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("doi");
        defaultKeys << QLatin1String("isbn");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("booktitle");
        defaultKeys << QLatin1String("note");
        defaultKeys << QLatin1String("event");
    }
    else if(key == QLatin1String("dictionaryEntry")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("editor") <<  QLatin1String("seriesEditor") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("dictionaryEntry");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("volume");
        defaultKeys << QLatin1String("numberOfVolumes");
        defaultKeys << QLatin1String("edition");
        defaultKeys << QLatin1String("publisher");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("isbn");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("booktitle");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("document")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("editor") <<  QLatin1String("reviewedAuthor") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("publisher");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstractNote");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("email")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("recipient");
        defaultKeys << QLatin1String("subject");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("encyclopediaArticle")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("editor") <<  QLatin1String("seriesEditor") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("volume");
        defaultKeys << QLatin1String("numberOfVolumes");
        defaultKeys << QLatin1String("edition");
        defaultKeys << QLatin1String("publisher");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("isbn");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("booktitle");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("film")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("producer") <<  QLatin1String("scriptwriter");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("genre");
        defaultKeys << QLatin1String("videoRecordingFormat");
        defaultKeys << QLatin1String("runningTime");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("publisher");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("forumPost")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("booktitle");
        defaultKeys << QLatin1String("type");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("hearing")) {
        defaultKeys << QLatin1String("contributor");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("committee");
        defaultKeys << QLatin1String("publisher");
        defaultKeys << QLatin1String("numberOfVolumes");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("legislativeBody");
        defaultKeys << QLatin1String("session");
        defaultKeys << QLatin1String("history");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("instantMessage")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("recipient");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("interview")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("interviewer") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("interviewMedium");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
    }
    else if(key == QLatin1String("journalArticle")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("editor") <<  QLatin1String("reviewedAuthor") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("volume");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("seriesText");
        defaultKeys << QLatin1String("journalAbbreviation");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("doi");
        defaultKeys << QLatin1String("issn");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("journal");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("letter")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("recipient");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("type");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("magazineArticle")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("reviewedAuthor") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("volume");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("seriesText");
        defaultKeys << QLatin1String("journalAbbreviation");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("doi");
        defaultKeys << QLatin1String("issn");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("journal");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("manuscript")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("numPages");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("type");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("map")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("seriesEditor");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("scale");
        defaultKeys << QLatin1String("edition");
        defaultKeys << QLatin1String("publisher");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("isbn");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("type");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("newspaperArticle")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("reviewedAuthor") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("volume");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("seriesText");
        defaultKeys << QLatin1String("journalAbbreviation");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("doi");
        defaultKeys << QLatin1String("issn");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("journal");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("chapter");
        defaultKeys << QLatin1String("note");
        defaultKeys << QLatin1String("address");
    }
    else if(key == QLatin1String("note")) {
        defaultKeys << QLatin1String("note");
        defaultKeys << QLatin1String("keywords");
    }
    else if(key == QLatin1String("patent")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("attorneyAgent") <<  QLatin1String("contributor");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("country");
        defaultKeys << QLatin1String("assignee");
        defaultKeys << QLatin1String("filingDate");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("applicationNumber");
        defaultKeys << QLatin1String("priorityNumbers");
        defaultKeys << QLatin1String("references");
        defaultKeys << QLatin1String("legalStatus");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("organization");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("podcast")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("guest");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("audioFileType");
        defaultKeys << QLatin1String("runningTime");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("presentation")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("type");
        defaultKeys << QLatin1String("event");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("radioBroadcast")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("castMember") <<  QLatin1String("contributor") <<  QLatin1String("guest") <<  QLatin1String("producer") <<  QLatin1String("scriptwriter");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("audioRecordingFormat");
        defaultKeys << QLatin1String("network");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("runningTime");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("booktitle");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("report")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("seriesEditor") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("institution");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("type");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("statute")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("code");
        defaultKeys << QLatin1String("codeNumber");
        defaultKeys << QLatin1String("pages");
        defaultKeys << QLatin1String("history");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("chapter");
        defaultKeys << QLatin1String("note");
        defaultKeys << QLatin1String("event");
        defaultKeys << QLatin1String("date");
    }
    else if(key == QLatin1String("tvBroadcast")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("castMember") <<  QLatin1String("contributor") <<  QLatin1String("guest") <<  QLatin1String("producer") <<  QLatin1String("scriptwriter");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("videoRecordingFormat");
        defaultKeys << QLatin1String("network");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("runningTime");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("booktitle");
        defaultKeys << QLatin1String("number");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("thesis")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("numPages");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("type");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("videoRecording")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("castMember") <<  QLatin1String("contributor") <<  QLatin1String("producer") <<  QLatin1String("scriptwriter");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("videoRecordingFormat");
        defaultKeys << QLatin1String("numberOfVolumes");
        defaultKeys << QLatin1String("studio");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("runningTime");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("isbn");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("archive");
        defaultKeys << QLatin1String("archiveLocation");
        defaultKeys << QLatin1String("libraryCatalog");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("address");
        defaultKeys << QLatin1String("series");
        defaultKeys << QLatin1String("note");
    }
    else if(key == QLatin1String("webpage")) {
        defaultKeys << QLatin1String("author") <<  QLatin1String("contributor") <<  QLatin1String("translator");
        defaultKeys << QLatin1String("title");
        defaultKeys << QLatin1String("date");
        defaultKeys << QLatin1String("shortTitle");
        defaultKeys << QLatin1String("url");
        defaultKeys << QLatin1String("accessDate");
        defaultKeys << QLatin1String("language");
        defaultKeys << QLatin1String("keywords");
        defaultKeys << QLatin1String("abstract");
        defaultKeys << QLatin1String("lccn");
        defaultKeys << QLatin1String("copyright");
        defaultKeys << QLatin1String("booktitle");
        defaultKeys << QLatin1String("type");
        defaultKeys << QLatin1String("note");
    }

    return defaultKeys;
}
