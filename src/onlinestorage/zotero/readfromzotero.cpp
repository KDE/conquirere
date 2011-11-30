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
    setUrl(QLatin1String("users"));
}

void ReadFromZotero::fetchItems(const QString &collection)
{
    QString apiCommand;
    if(collection.isEmpty()) {
        apiCommand = QLatin1String("https://api.zotero.org/") + url() + QLatin1String("/") + userName() + QLatin1String("/items/top?format=atom&content=json");
    }
    else {
        apiCommand = QLatin1String("https://api.zotero.org/") + url() + QLatin1String("/") + userName() + QLatin1String("/collections/") + collection + QLatin1String("/items?format=atom&content=json");
    }

    if(!pasword().isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + pasword());
    }

    setRequestType(Items);
    startRequest(apiCommand);
}

void ReadFromZotero::fetchItem(const QString &id, const QString &collection )
{
    QString apiCommand = QLatin1String("https://api.zotero.org/") + url() + QLatin1String("/") + userName() + QLatin1String("/items/top?format=atom&content=json&limit=10");
    if(!pasword().isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + pasword());
    }

    setRequestType(Items);
    startRequest(apiCommand);
}

void ReadFromZotero::fetchCollections(const QString &parent )
{
    QString subCollection;
    if(!parent.isEmpty()) {
        subCollection = QLatin1String("/") + parent + QLatin1String("/collections");
    }

    QString apiCommand = QLatin1String("https://api.zotero.org/") + url() + QLatin1String("/") + userName() + QLatin1String("/collections") + subCollection + QLatin1String("?format=atom");
    if(!pasword().isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + pasword());
    }

    setRequestType(Collections);
    startRequest(apiCommand);
}

void ReadFromZotero::fetchCollection(const QString &collection )
{
    QString apiCommand = QLatin1String("https://api.zotero.org/") + url() + QLatin1String("/") + userName() + QLatin1String("/collections/") + collection + QLatin1String("?format=atom");
    if(!pasword().isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + pasword());
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
        if(xmlReader.name() == QLatin1String("totalResults")) {
            totalResults = xmlReader.readElementText().toInt();
        }

        if(xmlReader.name() == QLatin1String("entry")) {
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

        if(xmlReader.name() == QLatin1String("title")) {
            ci.name = xmlReader.readElementText();
        }
        else if(startelement && xmlReader.name() == QLatin1String("key")) {
            ci.id = xmlReader.readElementText();
        }
        else if(startelement && xmlReader.name() == QLatin1String("numCollections")) {
            ci.subCollections = xmlReader.readElementText().toInt();
        }
        else if(startelement && xmlReader.name() == QLatin1String("numItems")) {
            ci.items = xmlReader.readElementText().toInt();
        }
        else if(xmlReader.name() == QLatin1String("entry")) {
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

        if(startelement && xmlReader.name() == QLatin1String("key")) {
            PlainText *zKey = new PlainText(xmlReader.readElementText());
            Value zkValue;
            zkValue.append(zKey);
            e->insert(QLatin1String("zoteroKey"), zkValue);
        }
        else if(startelement && xmlReader.name() == QLatin1String("numChildren")) {
            QString zoteroChildren = xmlReader.readElementText();

            PlainText *ptValue = new PlainText(zoteroChildren);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QLatin1String("zoteroChildren"), valueList);
        }
        else if(startelement && xmlReader.name() == QLatin1String("updated")) {
            QString zoteroUpdated = xmlReader.readElementText();

            PlainText *ptValue = new PlainText(zoteroUpdated);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QLatin1String("zoteroUpdated"), valueList);
        }
        else if(xmlReader.name() == "content") {
            QString etag = xmlReader.attributes().value(QLatin1String("zapi:etag")).toString();

            PlainText *ptValue = new PlainText(etag);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QLatin1String("zoteroEtag"), valueList);

            readJsonContent(e, xmlReader.readElementText());
        }
        else if(xmlReader.name() == QLatin1String("entry")) {
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

        if(i.key() == QLatin1String("tags")) {
            Value tagList;
            foreach (const QVariant &tag, i.value().toList()) {
                QVariantMap tagMap = tag.toMap();
                Keyword *k = new Keyword(tagMap.value(QLatin1String("tag")).toString());
                if(k->text().isEmpty()) {
                    delete k;
                }
                else {
                    tagList.append(k);
                }
            }
            e->insert(QLatin1String("keywords"), tagList);
        }
        else if(i.key() == QLatin1String("itemType")) {
            QString text = i.value().toString().toLower();

            if(text == QLatin1String("booksection")) {
                e->setType(QLatin1String("incollection")); // as incollection is part of a book with its own name
            }
            else if(text == QLatin1String("conferencepaper")) {
                e->setType(QLatin1String("inproceedings"));
            }
            else if(text == QLatin1String("journalarticle")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("journal"));
                Value valueList;
                valueList.append(ptValue);
                e->insert(QLatin1String("journalType"), valueList);
            }
            else if(text == QLatin1String("magazinearticle")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("magazine"));
                Value valueList;
                valueList.append(ptValue);
                e->insert(QLatin1String("journalType"), valueList);
            }
            else if(text == QLatin1String("newspaperarticle")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("newspaper"));
                Value valueList;
                valueList.append(ptValue);
                e->insert(QLatin1String("journalType"), valueList);
            }
            else {
                e->setType(text);
            }
        }
        else if(i.key() == QLatin1String("creators")) {
            Value authorList;
            Value editorList;
            foreach (const QVariant &author, i.value().toList()) {
                QVariantMap authorMap = author.toMap();
                Person *p = new Person(authorMap.value(QLatin1String("firstName")).toString(), authorMap.value(QLatin1String("lastName")).toString());
                if(authorMap.value(QLatin1String("creatorType")).toString() == QLatin1String("author")) {
                    authorList.append(p);
                }
                else {
                    editorList.append(p);
                }
            }
            e->insert(QLatin1String("author"), authorList);
            e->insert(QLatin1String("editor"), editorList);
        }
        else if(i.key() == QLatin1String("proceedingstitle")) {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QLatin1String("booktitle"), valueList);
        }
        else if(i.key() == QLatin1String("seriesNumber")) {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QLatin1String("number"), valueList);
        }
        else if(i.key() == QLatin1String("publicationtitle")) {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QLatin1String("journal"), valueList);
        }
        else if(i.key() == QLatin1String("abstractNote")) {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QLatin1String("abstract"), valueList);
        }
        else if(i.key() == QLatin1String("seriesTitle")) {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(ptValue);
            e->insert(QLatin1String("series"), valueList);
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

        if(i.key() == QLatin1String("tags")) {
            Value tagList;
            foreach (const QVariant &tag, i.value().toList()) {
                QVariantMap tagMap = tag.toMap();
                Keyword *k = new Keyword(tagMap.value(QLatin1String("tag")).toString());
                if(k->text().isEmpty()) {
                    delete k;
                }
                else {
                    tagList.append(k);
                }
            }
            e->insert(QLatin1String("tags"), tagList);
        }
        else if(i.key() == QLatin1String("itemType")) {
            QString text = i.value().toString().toLower();

            e->setType(text);
        }
        else if(i.key() == QLatin1String("creators")) {
            foreach (const QVariant &author, i.value().toList()) {
                QVariantMap authorMap = author.toMap();
                Person *p = new Person(authorMap.value(QLatin1String("firstName")).toString(), authorMap.value(QLatin1String("lastName")).toString());

                QString type = authorMap.value(QLatin1String("creatorType")).toString();
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
