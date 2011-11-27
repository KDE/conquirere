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

    startRequest(request, writeJsonContent(items));
}

void WriteToZotero::updateItem(Entry *item)
{

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

    startRequest(request, payload.toLatin1());
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

        startRequest(request, 0);
    }
}

void WriteToZotero::deleteItems(QList<QString> ids)
{

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
        if(entry->type() == QString("article")) {

        }
        else if(entry->type() == QString("inbook")) {

        }
        else if(entry->type() == QString("inproceedings")) {

        }
        else if(entry->type() == QString("booklet")) {

        }
        else if(entry->type() == QString("conference")) {

        }
        else if(entry->type() == QString("inproceedings")) {

        }
        else if(entry->type() == QString("incollection")) {

        }
        else if(entry->type() == QString("manual")) {

        }
        else if(entry->type() == QString("mastersthesis")) {

        }
        else if(entry->type() == QString("misc")) {

        }
        else if(entry->type() == QString("phdthesis")) {

        }
        else if(entry->type() == QString("proceedings")) {

        }
        else if(entry->type() == QString("techreport")) {

        }
        else if(entry->type() == QString("unpublished")) {

        }
        else if(entry->type() == QString("artwork")) {

        }
        else if(entry->type() == QString("audioRecording")) {

        }
        else if(entry->type() == QString("bill")) {

        }
        else if(entry->type() == QString("blogPost")) {

        }
        else if(entry->type() == QString("book")) {
            itemList.append( createBookJson(entry) );
        }
        else if(entry->type() == QString("bookSection")) {

        }
        else if(entry->type() == QString("case")) {

        }
        else if(entry->type() == QString("computerProgram")) {

        }
        else if(entry->type() == QString("conferencePaper")) {

        }
        else if(entry->type() == QString("dictionaryEntry")) {

        }
        else if(entry->type() == QString("document")) {

        }
        else if(entry->type() == QString("email")) {

        }
        else if(entry->type() == QString("encyclopediaArticle")) {

        }
        else if(entry->type() == QString("film")) {

        }
        else if(entry->type() == QString("forumPost")) {

        }
        else if(entry->type() == QString("hearing")) {

        }
        else if(entry->type() == QString("instantMessage")) {

        }
        else if(entry->type() == QString("interview")) {

        }
        else if(entry->type() == QString("journalArticle")) {

        }
        else if(entry->type() == QString("letter")) {

        }
        else if(entry->type() == QString("magazineArticle")) {

        }
        else if(entry->type() == QString("manuscript")) {

        }
        else if(entry->type() == QString("map")) {

        }
        else if(entry->type() == QString("newspaperArticle")) {

        }
        else if(entry->type() == QString("note")) {

        }
        else if(entry->type() == QString("patent")) {

        }
        else if(entry->type() == QString("podcast")) {

        }
        else if(entry->type() == QString("presentation")) {

        }
        else if(entry->type() == QString("radioBroadcast")) {

        }
        else if(entry->type() == QString("report")) {

        }
        else if(entry->type() == QString("statute")) {

        }
        else if(entry->type() == QString("tvBroadcast")) {

        }
        else if(entry->type() == QString("thesis")) {

        }
        else if(entry->type() == QString("videoRecording")) {

        }
        else if(entry->type() == QString("webpage")) {

        }
        else {
            qWarning() << "unknwon bibtex entry type" << entry->type() << "can't create zotero json from it";
        }
    }

    jsonMap.insert("items", itemList);

    QJson::Serializer serializer;
    QByteArray json = serializer.serialize(jsonMap);

    return json;
}

QVariantList WriteToZotero::createCreatorsJson(Entry *e)
{
    QVariantList jsonMap;

    // start by authors
    foreach(ValueItem* vi, e->value("authors")) {
        Person *p = dynamic_cast<Person *>(vi);

        if(p) {
            QVariantMap personMap;
            personMap.insert("creatorType","author");
            personMap.insert("firstName",p->firstName());
            personMap.insert("lastName",p->lastName());
            jsonMap.append(jsonMap);
        }
    }

    // go through editors
    foreach(ValueItem* vi, e->value("editors")) {
        Person *p = dynamic_cast<Person *>(vi);

        if(p) {
            QVariantMap personMap;
            personMap.insert("creatorType","editor");
            personMap.insert("firstName",p->firstName());
            personMap.insert("lastName",p->lastName());
            jsonMap.append(jsonMap);
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
    jsonMap.insert("rights",PlainTextValue::text(e->value("rights")));
    jsonMap.insert("tags",createTagsJson(e));
    //jsonMap.insert("notes","");

    return jsonMap;
}
