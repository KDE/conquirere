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

#include "writetozotero.h"

#include <kbibtex/entry.h>

#include <qjson/serializer.h>
#include <QDebug>

WriteToZotero::WriteToZotero(QObject *parent)
    : WriteToStorage(parent)
{
}

WriteToZotero::~WriteToZotero()
{

}

void WriteToZotero::pushNewItems(File items)
{
    //POST /users/1/items
    //Content-Type: application/json
    //X-Zotero-Write-Token: 19a4f01ad623aa7214f82347e3711f56
    QString pushString = QString("https://api.zotero.org/users/%1/items").arg(userName());

    if(!pasword().isEmpty()) {
        pushString.append(QString("?key=%1").arg(pasword()));
    }
    QUrl pushUrl(pushString);

    QNetworkRequest request(pushUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    startRequest(request, writeJsonContent(items), QNetworkAccessManager::PostOperation);
}

void WriteToZotero::updateItem(Entry *item)
{
    //PUT /users/1/items/ABCD2345
    //If-Match: "8e984e9b2a8fb560b0085b40f6c2c2b7"
    QString zoteroKey = PlainTextValue::text(e->value("zoteroKey"));
    QString pushString = QString("https://api.zotero.org/users/%1/items/%2").arg(userName()).arg(zoteroKey);

    if(!pasword().isEmpty()) {
        pushString.append(QString("?key=%1").arg(pasword()));
    }
    QUrl pushUrl(pushString);

    QNetworkRequest request(pushUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QString etag = PlainTextValue::text(e->value("zoteroEtag"));
    request.setRawHeader("If-Match", etag);

    File itemFile;
    itemFile.append(item);
    startRequest(request, writeJsonContent(itemFile), QNetworkAccessManager::PutOperation);
}

void WriteToZotero::addItemsToCollection(QList<QString> ids, const QString &collection )
{
    //POST /users/1/collections/QRST9876/items
    QString pushString = QString("https://api.zotero.org/users/%1/collections/%2/items").arg(userName().arg(collection));

    if(!pasword().isEmpty()) {
        pushString.append(QString("?key=%1").arg(pasword()));
    }
    QUrl pushUrl(pushString);


    QNetworkRequest request(pushUrl);

    QString payload;
    foreach(const QString &id, ids) {
        payload.append(id);
        payload.append(QString(" "));
    }
    payload.chop(1);

    startRequest(request, payload.toLatin1(), QNetworkAccessManager::PostOperation);
}

void WriteToZotero::removeItemsFromCollection(QList<QString> ids, const QString &collection )
{
    foreach(const QString &id, ids) {
        //DELETE /users/1/collections/QRST9876/items/ABCD2345
        QString pushString = QString("https://api.zotero.org/users/%1/collections/%2/items/%3").arg(userName().arg(collection).arg(id));

        if(!pasword().isEmpty()) {
            pushString.append(QString("?key=%1").arg(pasword()));
        }
        QUrl pushUrl(pushString);

        QNetworkRequest request(pushUrl);
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

        startRequest(request, 0, QNetworkAccessManager::DeleteOperation);
    }
}

void WriteToZotero::deleteItems(QList<QString> ids)
{
    //DELETE /users/1/items/ABCD2345
    //If-Match: "8e984e9b2a8fb560b0085b40f6c2c2b7"
}

void WriteToZotero::createCollection(CollectionInfo ci, const QString &parent)
{

}

void WriteToZotero::editCollection(CollectionInfo ci)
{

}

void WriteToZotero::deleteCollection(const QString &id)
{

}

void WriteToZotero::requestFinished()
{

}

QByteArray WriteToZotero::writeJsonContent(File items)
{

    QVariantMap jsonMap;
    QVariantList itemList;

    foreach(Element*element, items) {
        Entry *entry = dynamic_cast<Entry *>(element);
        if(!entry) {
            continue;
        }
        if(entry->type().toLower() == QString("article")) {

        }
        else if(entry->type().toLower() == QString("inbook")) {

        }
        else if(entry->type().toLower() == QString("inproceedings")) {

        }
        else if(entry->type().toLower() == QString("booklet")) {

        }
        else if(entry->type().toLower() == QString("conference")) {

        }
        else if(entry->type().toLower() == QString("inproceedings")) {

        }
        else if(entry->type().toLower() == QString("incollection")) {

        }
        else if(entry->type().toLower() == QString("manual")) {

        }
        else if(entry->type().toLower() == QString("mastersthesis")) {

        }
        else if(entry->type().toLower() == QString("misc")) {

        }
        else if(entry->type().toLower() == QString("phdthesis")) {

        }
        else if(entry->type().toLower() == QString("proceedings")) {

        }
        else if(entry->type().toLower() == QString("techreport")) {

        }
        else if(entry->type().toLower() == QString("unpublished")) {

        }
        else if(entry->type().toLower() == QString("artwork")) {

        }
        else if(entry->type().toLower() == QString("audioRecording")) {

        }
        else if(entry->type().toLower() == QString("bill")) {

        }
        else if(entry->type().toLower() == QString("blogPost")) {

        }
        else if(entry->type().toLower() == QString("book")) {
            itemList.append( createBookJson(entry) );
        }
        else if(entry->type().toLower() == QString("bookSection")) {

        }
        else if(entry->type().toLower() == QString("case")) {

        }
        else if(entry->type().toLower() == QString("computerProgram")) {

        }
        else if(entry->type().toLower() == QString("conferencePaper")) {

        }
        else if(entry->type().toLower() == QString("dictionaryEntry")) {

        }
        else if(entry->type().toLower() == QString("document")) {

        }
        else if(entry->type().toLower() == QString("email")) {

        }
        else if(entry->type().toLower() == QString("encyclopediaArticle")) {

        }
        else if(entry->type().toLower() == QString("film")) {

        }
        else if(entry->type().toLower() == QString("forumPost")) {

        }
        else if(entry->type().toLower() == QString("hearing")) {

        }
        else if(entry->type().toLower() == QString("instantMessage")) {

        }
        else if(entry->type().toLower() == QString("interview")) {

        }
        else if(entry->type().toLower() == QString("journalArticle")) {

        }
        else if(entry->type().toLower() == QString("letter")) {

        }
        else if(entry->type().toLower() == QString("magazineArticle")) {

        }
        else if(entry->type().toLower() == QString("manuscript")) {

        }
        else if(entry->type().toLower() == QString("map")) {

        }
        else if(entry->type().toLower() == QString("newspaperArticle")) {

        }
        else if(entry->type().toLower() == QString("note")) {

        }
        else if(entry->type().toLower() == QString("patent")) {

        }
        else if(entry->type().toLower() == QString("podcast")) {

        }
        else if(entry->type().toLower() == QString("presentation")) {

        }
        else if(entry->type().toLower() == QString("radioBroadcast")) {

        }
        else if(entry->type().toLower() == QString("report")) {

        }
        else if(entry->type().toLower() == QString("statute")) {

        }
        else if(entry->type().toLower() == QString("tvBroadcast")) {

        }
        else if(entry->type().toLower() == QString("thesis")) {

        }
        else if(entry->type().toLower() == QString("videoRecording")) {

        }
        else if(entry->type().toLower() == QString("webpage")) {

        }
        else {
            qWarning() << "unknwon bibtex entry type" << entry->type() << "can't create zotero json from it";
        }
    }

    jsonMap.insert("items", itemList);

    QJson::Serializer serializer;
    QByteArray json = serializer.serialize(jsonMap);

    qDebug() << json;

    return json;
}

QVariantList WriteToZotero::createCreatorsJson(Entry *e)
{
    QVariantList jsonMap;

    // start by authors
    foreach(ValueItem* vi, e->value("author")) {
        Person *p = dynamic_cast<Person *>(vi);

        if(p) {
            QVariantMap personMap;
            personMap.insert("creatorType","author");
            personMap.insert("firstName",p->firstName());
            personMap.insert("lastName",p->lastName());
            jsonMap.append(personMap);
        }
    }

    // go through editors
    foreach(ValueItem* vi, e->value("editor")) {
        Person *p = dynamic_cast<Person *>(vi);

        if(p) {
            QVariantMap personMap;
            personMap.insert("creatorType","editor");
            personMap.insert("firstName",p->firstName());
            personMap.insert("lastName",p->lastName());
            jsonMap.append(personMap);
        }
    }

    return jsonMap;
}

QVariantList WriteToZotero::createTagsJson(Entry *e)
{
    QVariantList jsonMap;

    foreach(ValueItem* vi, e->value("keywords")) {
        Keyword *k = dynamic_cast<Keyword *>(vi);

        if(k) {
            QVariantMap tagMap;
            tagMap.insert("tag",k->text());
            jsonMap.append(tagMap);
        }
    }

    return jsonMap;
}

QVariantMap WriteToZotero::createArtworkJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","artwork");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("artworkMedium",PlainTextValue::text(e->value("artworkMedium")));
    jsonMap.insert("artworkSize",PlainTextValue::text(e->value("artworkSize")));
    //jsonMap.insert("date","");
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createAudioRecordingJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","audioRecording");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO "creatorType":"performer"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("audioRecordingFormat",PlainTextValue::text(e->value("audioRecordingFormat")));
    jsonMap.insert("seriesTitle",PlainTextValue::text(e->value("series")));
    jsonMap.insert("volume",PlainTextValue::text(e->value("volume")));
    jsonMap.insert("numberOfVolumes",PlainTextValue::text(e->value("numberOfVolumes")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("label",PlainTextValue::text(e->value("label")));
    //jsonMap.insert("date","");
    jsonMap.insert("runningTime",PlainTextValue::text(e->value("runningTime")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("ISBN",PlainTextValue::text(e->value("isbn")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createBillJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","bill");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO "creatorType":"sponsor"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("billNumber",PlainTextValue::text(e->value("billNumber")));
    jsonMap.insert("code",PlainTextValue::text(e->value("code")));
    jsonMap.insert("codeVolume",PlainTextValue::text(e->value("codeVolume")));
    jsonMap.insert("section",PlainTextValue::text(e->value("section")));
    jsonMap.insert("codePages",PlainTextValue::text(e->value("codePages")));
    jsonMap.insert("legislativeBody",PlainTextValue::text(e->value("legislativeBody")));
    jsonMap.insert("session",PlainTextValue::text(e->value("session")));
    jsonMap.insert("history",PlainTextValue::text(e->value("history")));
    //jsonMap.insert("date","");
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createBlogPostJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","blogPost");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("blogTitle",PlainTextValue::text(e->value("blogTitle")));
    jsonMap.insert("websiteType",PlainTextValue::text(e->value("websiteType")));
    //jsonMap.insert("date","");
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createBookJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","book");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("series",PlainTextValue::text(e->value("series")));
    jsonMap.insert("seriesNumber",PlainTextValue::text(e->value("issue")));
    jsonMap.insert("volume",PlainTextValue::text(e->value("volume")));
    jsonMap.insert("numberOfVolumes",PlainTextValue::text(e->value("numberOfVolumes")));
    jsonMap.insert("edition",PlainTextValue::text(e->value("edition")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("publisher",PlainTextValue::text(e->value("publisher")));
    //jsonMap.insert("date","");
    jsonMap.insert("numPages",PlainTextValue::text(e->value("numPages")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("ISBN",PlainTextValue::text(e->value("isbn")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shorttitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createBookSectionJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","bookSection");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("bookTitle",PlainTextValue::text(e->value("bookTitle")));
    jsonMap.insert("series",PlainTextValue::text(e->value("series")));
    jsonMap.insert("seriesNumber",PlainTextValue::text(e->value("issue")));
    jsonMap.insert("volume",PlainTextValue::text(e->value("volume")));
    jsonMap.insert("numberOfVolumes",PlainTextValue::text(e->value("numberOfVolumes")));
    jsonMap.insert("edition",PlainTextValue::text(e->value("edition")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("publisher",PlainTextValue::text(e->value("publisher")));
    //jsonMap.insert("date","");
    jsonMap.insert("pages",PlainTextValue::text(e->value("pages")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("ISBN",PlainTextValue::text(e->value("isbn")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createCaseJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","case");
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));

    jsonMap.insert("caseName",PlainTextValue::text(e->value("caseName")));
    jsonMap.insert("reporter",PlainTextValue::text(e->value("reporter")));
    jsonMap.insert("reporterVolume",PlainTextValue::text(e->value("reporterVolume")));
    jsonMap.insert("court",PlainTextValue::text(e->value("court")));
    jsonMap.insert("docketNumber",PlainTextValue::text(e->value("docketNumber")));
    jsonMap.insert("firstPage",PlainTextValue::text(e->value("firstPage")));
    jsonMap.insert("history",PlainTextValue::text(e->value("history")));
    jsonMap.insert("dateDecided",PlainTextValue::text(e->value("dateDecided")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createComputerProgramJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","computerProgram");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO "creatorType":"programmer"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("seriesTitle",PlainTextValue::text(e->value("series")));
    jsonMap.insert("version",PlainTextValue::text(e->value("version")));
    //jsonMap.insert("date","");
    jsonMap.insert("system",PlainTextValue::text(e->value("system")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("company",PlainTextValue::text(e->value("company")));
    jsonMap.insert("programmingLanguage",PlainTextValue::text(e->value("programmingLanguage")));
    jsonMap.insert("ISBN",PlainTextValue::text(e->value("isbn")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createConferencePaperJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","conferencePaper");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    //jsonMap.insert("date","");
    jsonMap.insert("proceedingsTitle",PlainTextValue::text(e->value("booktitle")));
    jsonMap.insert("conferenceName",PlainTextValue::text(e->value("booktitle")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("publisher",PlainTextValue::text(e->value("publisher")));
    jsonMap.insert("volume",PlainTextValue::text(e->value("volume")));
    jsonMap.insert("pages",PlainTextValue::text(e->value("pages")));
    jsonMap.insert("series",PlainTextValue::text(e->value("series")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("DOI",PlainTextValue::text(e->value("doi")));
    jsonMap.insert("ISBN",PlainTextValue::text(e->value("isbn")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createDictionaryEntryJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","dictionaryEntry");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("dictionaryTitle",PlainTextValue::text(e->value("dictionaryTitle")));
    jsonMap.insert("series",PlainTextValue::text(e->value("series")));
    jsonMap.insert("seriesNumber",PlainTextValue::text(e->value("issue")));
    jsonMap.insert("volume",PlainTextValue::text(e->value("volume")));
    jsonMap.insert("numberOfVolumes",PlainTextValue::text(e->value("numberOfVolumes")));
    jsonMap.insert("edition",PlainTextValue::text(e->value("edition")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("publisher",PlainTextValue::text(e->value("publisher")));
    //jsonMap.insert("date","");
    jsonMap.insert("pages",PlainTextValue::text(e->value("pages")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("ISBN",PlainTextValue::text(e->value("isbn")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createDocumentJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","document");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("publisher",PlainTextValue::text(e->value("publisher")));
    //jsonMap.insert("date","");
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createEmailJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","email");
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));

    jsonMap.insert("subject",PlainTextValue::text(e->value("subject")));
    //jsonMap.insert("date","");
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createenCyclopediaArticleJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","encyclopediaArticle");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("encyclopediaTitle",PlainTextValue::text(e->value("encyclopediaTitle")));
    jsonMap.insert("series",PlainTextValue::text(e->value("series")));
    jsonMap.insert("seriesNumber",PlainTextValue::text(e->value("issue")));
    jsonMap.insert("volume",PlainTextValue::text(e->value("volume")));
    jsonMap.insert("numberOfVolumes",PlainTextValue::text(e->value("numberOfVolumes")));
    jsonMap.insert("edition",PlainTextValue::text(e->value("edition")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("publisher",PlainTextValue::text(e->value("publisher")));
    //jsonMap.insert("date","");
    jsonMap.insert("pages",PlainTextValue::text(e->value("pages")));
    jsonMap.insert("ISBN",PlainTextValue::text(e->value("isbn")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createFilmJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","film");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO "creatorType":"director"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("distributor",PlainTextValue::text(e->value("distributor")));
    //jsonMap.insert("date","");
    jsonMap.insert("genre",PlainTextValue::text(e->value("genre")));
    jsonMap.insert("videoRecordingFormat",PlainTextValue::text(e->value("videoRecordingFormat")));
    jsonMap.insert("runningTime",PlainTextValue::text(e->value("runningTime")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createForumPostJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","forumPost");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("forumTitle",PlainTextValue::text(e->value("forumTitle")));
    jsonMap.insert("postType",PlainTextValue::text(e->value("postType")));
    //jsonMap.insert("date","");
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createHearingJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","hearing");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO "creatorType":"contributor"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("committee",PlainTextValue::text(e->value("committee")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("publisher",PlainTextValue::text(e->value("publisher")));
    jsonMap.insert("numberOfVolumes",PlainTextValue::text(e->value("numberOfVolumes")));
    jsonMap.insert("documentNumber",PlainTextValue::text(e->value("documentNumber")));
    jsonMap.insert("pages",PlainTextValue::text(e->value("pages")));
    jsonMap.insert("legislativeBody",PlainTextValue::text(e->value("legislativeBody")));
    jsonMap.insert("session",PlainTextValue::text(e->value("session")));
    jsonMap.insert("history",PlainTextValue::text(e->value("history")));
    //jsonMap.insert("date","");
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createInstantMessageJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","instantMessage");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    //jsonMap.insert("date","");
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createInterviewJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","interview");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO "creatorType":"interviewee"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    //jsonMap.insert("date","");
    jsonMap.insert("interviewMedium",PlainTextValue::text(e->value("interviewMedium")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createJournalArticleJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","journalArticle");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));

    jsonMap.insert("publicationTitle",PlainTextValue::text(e->value("publicationTitle")));
    jsonMap.insert("volume",PlainTextValue::text(e->value("volume")));
    jsonMap.insert("issue",PlainTextValue::text(e->value("issue")));
    jsonMap.insert("pages",PlainTextValue::text(e->value("pages")));
    //jsonMap.insert("date","");
    jsonMap.insert("series",PlainTextValue::text(e->value("series")));
    jsonMap.insert("seriesTitle",PlainTextValue::text(e->value("journal")));
    jsonMap.insert("seriesText",PlainTextValue::text(e->value("seriesText")));
    jsonMap.insert("journalAbbreviation",PlainTextValue::text(e->value("journalAbbreviation")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("DOI",PlainTextValue::text(e->value("doi")));
    jsonMap.insert("ISSN",PlainTextValue::text(e->value("issn")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createLetterJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","letter");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("letterType",PlainTextValue::text(e->value("letterType")));
    //jsonMap.insert("date","");
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createMagazineArticleJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","magazineArticle");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));

    jsonMap.insert("publicationTitle",PlainTextValue::text(e->value("publicationTitle")));
    jsonMap.insert("volume",PlainTextValue::text(e->value("volume")));
    jsonMap.insert("issue",PlainTextValue::text(e->value("issue")));
    jsonMap.insert("pages",PlainTextValue::text(e->value("pages")));
    //jsonMap.insert("date","");
    jsonMap.insert("series",PlainTextValue::text(e->value("series")));
    jsonMap.insert("seriesTitle",PlainTextValue::text(e->value("journal")));
    jsonMap.insert("seriesText",PlainTextValue::text(e->value("seriesText")));
    jsonMap.insert("journalAbbreviation",PlainTextValue::text(e->value("journalAbbreviation")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("DOI",PlainTextValue::text(e->value("doi")));
    jsonMap.insert("ISSN",PlainTextValue::text(e->value("issn")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createManuscriptJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","manuscript");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("manuscriptType",PlainTextValue::text(e->value("manuscriptType")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    //jsonMap.insert("date","");
    jsonMap.insert("numPages",PlainTextValue::text(e->value("numPages")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createMapJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","map");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO "creatorType":"cartographer"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("mapType",PlainTextValue::text(e->value("mapType")));
    jsonMap.insert("scale",PlainTextValue::text(e->value("scale")));
    jsonMap.insert("seriesTitle",PlainTextValue::text(e->value("series")));
    jsonMap.insert("edition",PlainTextValue::text(e->value("edition")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("publisher",PlainTextValue::text(e->value("publisher")));
    //jsonMap.insert("date","");
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("ISBN",PlainTextValue::text(e->value("isbn")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createNewspaperArticleJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","newspaperArticle");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));

    jsonMap.insert("publicationTitle",PlainTextValue::text(e->value("publicationTitle")));
    jsonMap.insert("volume",PlainTextValue::text(e->value("volume")));
    jsonMap.insert("issue",PlainTextValue::text(e->value("issue")));
    jsonMap.insert("pages",PlainTextValue::text(e->value("pages")));
    //jsonMap.insert("date","");
    jsonMap.insert("series",PlainTextValue::text(e->value("series")));
    jsonMap.insert("seriesTitle",PlainTextValue::text(e->value("journal")));
    jsonMap.insert("seriesText",PlainTextValue::text(e->value("seriesText")));
    jsonMap.insert("journalAbbreviation",PlainTextValue::text(e->value("journalAbbreviation")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("DOI",PlainTextValue::text(e->value("doi")));
    jsonMap.insert("ISSN",PlainTextValue::text(e->value("issn")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createNoteJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","note");
    jsonMap.insert("note","text");
    jsonMap.insert("tags",createTagsJson(e));

    return jsonMap;
}

QVariantMap WriteToZotero::createPatentJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","patent");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO "creatorType":"inventor"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));

    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("country",PlainTextValue::text(e->value("country")));
    jsonMap.insert("assignee",PlainTextValue::text(e->value("assignee")));
    jsonMap.insert("issuingAuthority",PlainTextValue::text(e->value("issuingAuthority")));
    jsonMap.insert("patentNumber",PlainTextValue::text(e->value("patentNumber")));
    jsonMap.insert("filingDate",PlainTextValue::text(e->value("filingDate")));
    jsonMap.insert("pages",PlainTextValue::text(e->value("pages")));
    jsonMap.insert("applicationNumber",PlainTextValue::text(e->value("applicationNumber")));
    jsonMap.insert("priorityNumbers",PlainTextValue::text(e->value("priorityNumbers")));
    jsonMap.insert("issueDate",PlainTextValue::text(e->value("issueDate")));
    jsonMap.insert("references",PlainTextValue::text(e->value("references")));
    jsonMap.insert("legalStatus",PlainTextValue::text(e->value("legalStatus")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createPodcastJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","podcast");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO"creatorType":"podcaster"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("seriesTitle",PlainTextValue::text(e->value("series")));
    jsonMap.insert("episodeNumber",PlainTextValue::text(e->value("episodeNumber")));
    jsonMap.insert("audioFileType",PlainTextValue::text(e->value("audioFileType")));
    jsonMap.insert("runningTime",PlainTextValue::text(e->value("runningTime")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createPresentationJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","presentation");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO "creatorType":"presenter"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("presentationType",PlainTextValue::text(e->value("presentationType")));
    //jsonMap.insert("date","");
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("meetingName",PlainTextValue::text(e->value("meetingName")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createRadioBroadcastJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","radioBroadcast");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO "creatorType":"director"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("programTitle",PlainTextValue::text(e->value("programTitle")));
    jsonMap.insert("episodeNumber",PlainTextValue::text(e->value("episodeNumber")));
    jsonMap.insert("audioRecordingFormat",PlainTextValue::text(e->value("audioRecordingFormat")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("network",PlainTextValue::text(e->value("network")));
    //jsonMap.insert("date","");
    jsonMap.insert("runningTime",PlainTextValue::text(e->value("runningTime")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createReportJson(Entry *e)
{
    //"url":"","accessDate":"","archive":"","archiveLocation":"","libraryCatalog":"","callNumber":"","rights":"","extra":"","tags":[],"notes":[]}
    QVariantMap jsonMap;

    jsonMap.insert("itemType","report");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));

    jsonMap.insert("reportNumber",PlainTextValue::text(e->value("reportNumber")));
    jsonMap.insert("reportType",PlainTextValue::text(e->value("type")));
    jsonMap.insert("seriesTitle",PlainTextValue::text(e->value("series")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("institution",PlainTextValue::text(e->value("institution")));
    //jsonMap.insert("date","");
    jsonMap.insert("pages",PlainTextValue::text(e->value("pages")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createStatuteJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","statute");
    jsonMap.insert("nameOfAct","nameOfAct");
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("code",PlainTextValue::text(e->value("code")));
    jsonMap.insert("codeNumber",PlainTextValue::text(e->value("codeNumber")));
    jsonMap.insert("publicLawNumber",PlainTextValue::text(e->value("publicLawNumber")));
    jsonMap.insert("dateEnacted",PlainTextValue::text(e->value("dateEnacted")));
    jsonMap.insert("pages",PlainTextValue::text(e->value("pages")));
    jsonMap.insert("section",PlainTextValue::text(e->value("section")));
    jsonMap.insert("session",PlainTextValue::text(e->value("session")));
    jsonMap.insert("history",PlainTextValue::text(e->value("history")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createTvBroadcastJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","tvBroadcast");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO "creatorType":"director"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("programTitle",PlainTextValue::text(e->value("programTitle")));
    jsonMap.insert("episodeNumber",PlainTextValue::text(e->value("episodeNumber")));
    jsonMap.insert("videoRecordingFormat",PlainTextValue::text(e->value("videoRecordingFormat")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("network",PlainTextValue::text(e->value("network")));
    //jsonMap.insert("date","");
    jsonMap.insert("runningTime",PlainTextValue::text(e->value("runningTime")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createThesisJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","thesis");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("thesisType",PlainTextValue::text(e->value("thesisType")));
    jsonMap.insert("university",PlainTextValue::text(e->value("school")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    //jsonMap.insert("date","");
    jsonMap.insert("numPages",PlainTextValue::text(e->value("numPages")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createVideoRecordingJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","videoRecording");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e)); //TODO "creatorType":"director"
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("videoRecordingFormat",PlainTextValue::text(e->value("videoRecordingFormat")));
    jsonMap.insert("seriesTitle",PlainTextValue::text(e->value("series")));
    jsonMap.insert("volume",PlainTextValue::text(e->value("volume")));
    jsonMap.insert("numberOfVolumes",PlainTextValue::text(e->value("numberOfVolumes")));
    jsonMap.insert("place",PlainTextValue::text(e->value("place")));
    jsonMap.insert("studio",PlainTextValue::text(e->value("studio")));
    //jsonMap.insert("date","");
    jsonMap.insert("runningTime",PlainTextValue::text(e->value("runningTime")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("ISBN",PlainTextValue::text(e->value("isbn")));
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("archive",PlainTextValue::text(e->value("archive")));
    jsonMap.insert("archiveLocation",PlainTextValue::text(e->value("archiveLocation")));
    jsonMap.insert("libraryCatalog",PlainTextValue::text(e->value("libraryCatalog")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

QVariantMap WriteToZotero::createWebpageJson(Entry *e)
{
    QVariantMap jsonMap;

    jsonMap.insert("itemType","webpage");
    jsonMap.insert("title", PlainTextValue::text(e->value("title")));
    jsonMap.insert("creators",createCreatorsJson(e));
    jsonMap.insert("abstractNote",PlainTextValue::text(e->value("abstract")));
    jsonMap.insert("websiteTitle",PlainTextValue::text(e->value("websiteTitle")));
    jsonMap.insert("websiteType",PlainTextValue::text(e->value("websiteType")));
    //jsonMap.insert("date","");
    jsonMap.insert("shortTitle",PlainTextValue::text(e->value("shortTitle")));
    jsonMap.insert("url",PlainTextValue::text(e->value("url")));
    jsonMap.insert("accessDate",PlainTextValue::text(e->value("accessDate")));
    jsonMap.insert("language",PlainTextValue::text(e->value("language")));
    jsonMap.insert("callNumber",PlainTextValue::text(e->value("lccn")));
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("extra",PlainTextValue::text(e->value("extra")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}

