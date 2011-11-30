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

#include "readfromzotero.h"

#include <kbibtex/entry.h>

#include <QtNetwork/QNetworkReply>
#include <QXmlStreamReader>
#include <qjson/parser.h>

#include <QDebug>

ReadFromZotero::ReadFromZotero(QObject *parent)
    : ReadFromStorage(parent)
{
}

void ReadFromZotero::fetchItems(const QString &collection)
{
    QString apiCommand;
    if(collection.isEmpty()) {
        apiCommand = QString("https://api.zotero.org/users/%1/items/top?format=atom&content=json").arg(userName());
    }
    else {
        apiCommand = QString("https://api.zotero.org/users/%1/collections/%2/items?format=atom&content=json").arg(userName()).arg(collection);
    }

    if(!pasword().isEmpty()) {
        apiCommand.append( QString("&key=%2").arg(pasword()));
    }

    setRequestType(Items);
    startRequest(apiCommand);
}

void ReadFromZotero::fetchItem(const QString &id, const QString &collection )
{
    QString apiCommand = QString("https://api.zotero.org/users/%1/items/top?format=atom&content=json&limit=10").arg(userName());
    if(!pasword().isEmpty()) {
        apiCommand.append( QString("&key=%2").arg(pasword()));
    }

    setRequestType(Items);
    startRequest(apiCommand);
}

void ReadFromZotero::fetchCollections(const QString &parent )
{
    QString subCollection;
    if(!parent.isEmpty()) {
        subCollection = QString("/%1/collections").arg(parent);
    }

    QString apiCommand = QString("https://api.zotero.org/users/%1/collections%2?format=atom").arg(userName()).arg(subCollection);
    if(!pasword().isEmpty()) {
        apiCommand.append( QString("&key=%2").arg(pasword()));
    }

    setRequestType(Collections);
    startRequest(apiCommand);
}

void ReadFromZotero::fetchCollection(const QString &collection )
{
    QString apiCommand = QString("https://api.zotero.org/users/%1/collections/%2?format=atom").arg(userName()).arg(collection);
    if(!pasword().isEmpty()) {
        apiCommand.append( QString("&key=%2").arg(pasword()));
    }

    setRequestType(Collections);
    startRequest(apiCommand);
}

void ReadFromZotero::requestFinished()
{
    QXmlStreamReader xmlReader;
    xmlReader.setDevice(reply());

    int totalResults = 0;

    while(!xmlReader.atEnd()) {
        if(!xmlReader.readNextStartElement()) {
            continue;
        }
        if(xmlReader.name() == "totalResults") {
            totalResults = xmlReader.readElementText().toInt();
        }

        if(xmlReader.name() == "entry") {
            switch(requestType()) {
            case Items:
                m_bibFile.append( readItemEntry(xmlReader) );
                break;
            case Collections:
                m_cachedCollectionResult.append( readCollectionEntry(xmlReader) );
                break;
            }
        }
    }

    switch(requestType()) {
    case Items:
        emit itemsInfo(m_bibFile);
        break;
    case Collections:
        emit collectionsInfo(m_cachedCollectionResult);
        break;
    }
}

CollectionInfo ReadFromZotero::readCollectionEntry(QXmlStreamReader &xmlReader)
{
    CollectionInfo ci;
    bool finishEntry = false;
    while(!finishEntry) {
        bool startelement = xmlReader.readNextStartElement();

        if(xmlReader.name() == "title") {
            ci.name = xmlReader.readElementText();
        }
        else if(startelement && xmlReader.name() == "key") {
            ci.id = xmlReader.readElementText();
        }
        else if(startelement && xmlReader.name() == "numCollections") {
            ci.subCollections = xmlReader.readElementText().toInt();
        }
        else if(startelement && xmlReader.name() == "numItems") {
            ci.items = xmlReader.readElementText().toInt();
        }
        else if(xmlReader.name() == "entry") {
            finishEntry = true;
        }
    }

    return ci;
}

Element *ReadFromZotero::readItemEntry(QXmlStreamReader &xmlReader)
{
    Entry *e = new Entry;
    bool finishEntry = false;
    while(!finishEntry) {
        bool startelement = xmlReader.readNextStartElement();

        if(startelement && xmlReader.name() == "key") {
            PlainText *zKey = new PlainText(xmlReader.readElementText());
            Value zkValue;
            zkValue.append(zKey);
            e->insert(QString("zoteroKey"), zkValue);
        }
        else if(startelement && xmlReader.name() == "numChildren") {
            QString zoteroChildren = xmlReader.readElementText();

            PlainText *ptValue = new PlainText(zoteroChildren);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QString("zoteroChildren"), valueList);
        }
        else if(startelement && xmlReader.name() == "updated") {
            QString zoteroUpdated = xmlReader.readElementText();

            PlainText *ptValue = new PlainText(zoteroUpdated);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QString("zoteroUpdated"), valueList);
        }
        else if(xmlReader.name() == "content") {
            QString etag = xmlReader.attributes().value("zapi:etag").toString();

            PlainText *ptValue = new PlainText(etag);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QString("zoteroEtag"), valueList);

            readJsonContent(e, xmlReader.readElementText());
        }
        else if(xmlReader.name() == "entry") {
            finishEntry = true;
        }
    }

    return e;
}

void ReadFromZotero::readJsonContent(Entry *e, const QString &content)
{
    if(adoptBibtexTypes()) {
        readJsonContentBibTeX(e, content);
    }
    else {
        readJsonContentOriginal(e, content);
    }
}

void ReadFromZotero::readJsonContentBibTeX(Entry *e, const QString &content)
{
    QJson::Parser parser;
    bool ok;

    QVariantMap result = parser.parse (content.toLatin1(), &ok).toMap();
    if (!ok) {
        qFatal("ReadFromZotero::readJsonContent :: An error occurred during json parsing");
        return;
    }

    QMapIterator<QString, QVariant> i(result);
    while (i.hasNext()) {
        i.next();

        if(i.key() == "tags") {
            Value tagList;
            foreach (QVariant tag, i.value().toList()) {
                QVariantMap tagMap = tag.toMap();
                Keyword *k = new Keyword(tagMap.value(QString("tag")).toString());
                if(k->text().isEmpty()) {
                    delete k;
                }
                else {
                    tagList.append(k);
                }
            }
            e->insert(QString("keywords"), tagList);
        }
        else if(i.key() == "itemType") {
            QString text = i.value().toString().toLower();

            if(text == "booksection") {
                e->setType(QString("incollection")); // as incollection is part of a book with its own name
            }
            else if(text == "conferencepaper") {
                e->setType(QString("inproceedings"));
            }
            else if(text == "journalarticle") {
                e->setType(QString("article"));

                PlainText *ptValue = new PlainText(QString("journal"));
                Value valueList;
                valueList.append(ptValue);
                e->insert(QString("journalType"), valueList);
            }
            else if(text == "magazinearticle") {
                e->setType(QString("article"));

                PlainText *ptValue = new PlainText(QString("magazine"));
                Value valueList;
                valueList.append(ptValue);
                e->insert(QString("journalType"), valueList);
            }
            else if(text == "newspaperarticle") {
                e->setType(QString("article"));

                PlainText *ptValue = new PlainText(QString("newspaper"));
                Value valueList;
                valueList.append(ptValue);
                e->insert(QString("journalType"), valueList);
            }
            else {
                e->setType(text);
            }
        }
        else if(i.key() == "creators") {
            Value authorList;
            Value editorList;
            foreach (QVariant author, i.value().toList()) {
                QVariantMap authorMap = author.toMap();
                Person *p = new Person(authorMap.value(QString("firstName")).toString(), authorMap.value(QString("lastName")).toString());
                if(authorMap.value(QString("creatorType")).toString() == QString("author")) {
                    authorList.append(p);
                }
                else {
                    editorList.append(p);
                }
            }
            e->insert(QString("author"), authorList);
            e->insert(QString("editor"), editorList);
        }
        else if(i.key() == "proceedingstitle") {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QString("booktitle"), valueList);
        }
        else if(i.key() == "seriesNumber") {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QString("number"), valueList);
        }
        else if(i.key() == "publicationtitle") {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QString("journal"), valueList);
        }
        else if(i.key() == "abstractNote") {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QString("abstract"), valueList);
        }
        else if(i.key() == "seriesTitle") {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QString("series"), valueList);
        }
        else {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(ptValue);
            e->insert(i.key(), valueList);
        }
    }
}

void ReadFromZotero::readJsonContentOriginal(Entry *e, const QString &content)
{
    QJson::Parser parser;
    bool ok;

    QVariantMap result = parser.parse (content.toLatin1(), &ok).toMap();
    if (!ok) {
        qFatal("ReadFromZotero::readJsonContent :: An error occurred during json parsing");
        return;
    }

    QMapIterator<QString, QVariant> i(result);
    while (i.hasNext()) {
        i.next();

        if(i.key() == "tags") {
            Value tagList;
            foreach (QVariant tag, i.value().toList()) {
                QVariantMap tagMap = tag.toMap();
                Keyword *k = new Keyword(tagMap.value(QString("tag")).toString());
                if(k->text().isEmpty()) {
                    delete k;
                }
                else {
                    tagList.append(k);
                }
            }
            e->insert(QString("tags"), tagList);
        }
        else if(i.key() == "itemType") {
            QString text = i.value().toString().toLower();

            e->setType(text);
        }
        else if(i.key() == "creators") {
            foreach (QVariant author, i.value().toList()) {
                QVariantMap authorMap = author.toMap();
                Person *p = new Person(authorMap.value(QString("firstName")).toString(), authorMap.value(QString("lastName")).toString());

                QString type = authorMap.value(QString("creatorType")).toString();
                Value list = e->value(type);
                list.append(p);
                e->remove(type);
                e->insert(type, list);
            }
        }
        else {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(ptValue);
            e->insert(i.key(), valueList);
        }
    }
}
