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
#include <QtCore/QXmlStreamReader>
#include <qjson/parser.h>

#include <QDebug>

ReadFromZotero::ReadFromZotero(QObject *parent)
    : ReadFromStorage(parent)
{
    // build the mappinglist
    m_zoteroToBibTeX["callNumber"] = QLatin1String("lccn");
    m_zoteroToBibTeX["rights"] = QLatin1String("copyright");
    m_zoteroToBibTeX["tags"] = QLatin1String("keywords");
    m_zoteroToBibTeX["seriesTitle"] = QLatin1String("series");
    m_zoteroToBibTeX["blogTitle"] = QLatin1String("booktitle");
    m_zoteroToBibTeX["abstractNote"] = QLatin1String("abstract");
    m_zoteroToBibTeX["dictionaryTitle"] = QLatin1String("booktitle");
    m_zoteroToBibTeX["encyclopediaTitle"] = QLatin1String("booktitle");
    m_zoteroToBibTeX["publicationTitle"] = QLatin1String("journal");
    m_zoteroToBibTeX["forumTitle"] = QLatin1String("booktitle");
    m_zoteroToBibTeX["proceedingsTitle"] = QLatin1String("booktitle");
    m_zoteroToBibTeX["websiteType"] = QLatin1String("type");
    m_zoteroToBibTeX["seriesNumber"] = QLatin1String("number");
    m_zoteroToBibTeX["distributor"] = QLatin1String("publisher");
    m_zoteroToBibTeX["manuscriptType"] = QLatin1String("type");
    m_zoteroToBibTeX["episodeNumber"] = QLatin1String("number");
    m_zoteroToBibTeX["presentationType"] = QLatin1String("type");
    m_zoteroToBibTeX["reportType"] = QLatin1String("type");
    m_zoteroToBibTeX["reportNumber"] = QLatin1String("number");
    m_zoteroToBibTeX["thesisType"] = QLatin1String("type");
    m_zoteroToBibTeX["university"] = QLatin1String("school");
    m_zoteroToBibTeX["websiteTitle"] = QLatin1String("booktitle");
    m_zoteroToBibTeX["programTitle"] = QLatin1String("booktitle");
    m_zoteroToBibTeX["websiteType"] = QLatin1String("type");
    m_zoteroToBibTeX["place"] = QLatin1String("address");
    m_zoteroToBibTeX["billNumber"] = QLatin1String("number");
    m_zoteroToBibTeX["docketNumber"] = QLatin1String("number");
    m_zoteroToBibTeX["caseName"] = QLatin1String("title");
    m_zoteroToBibTeX["documentNumber"] = QLatin1String("number");
    m_zoteroToBibTeX["issue"] = QLatin1String("number");
    m_zoteroToBibTeX["patentNumber"] = QLatin1String("number");
    m_zoteroToBibTeX["publicLawNumber"] = QLatin1String("number");
    m_zoteroToBibTeX["letterType"] = QLatin1String("type");
    m_zoteroToBibTeX["serie"] = QLatin1String("series");
    m_zoteroToBibTeX["label"] = QLatin1String("publisher");
    m_zoteroToBibTeX["section"] = QLatin1String("chapter");
    m_zoteroToBibTeX["nameOfAct"] = QLatin1String("title");
    m_zoteroToBibTeX["postType"] = QLatin1String("type");
    m_zoteroToBibTeX["codePages"] = QLatin1String("pages");
    m_zoteroToBibTeX["court"] = QLatin1String("organization");
    m_zoteroToBibTeX["issuingAuthority"] = QLatin1String("organization");
    m_zoteroToBibTeX["legislativebody"] = QLatin1String("institution");
    m_zoteroToBibTeX["session"] = QLatin1String("event");
    m_zoteroToBibTeX["dateDecided"] = QLatin1String("date");
    m_zoteroToBibTeX["firstPage"] = QLatin1String("pages");
    m_zoteroToBibTeX["issueDate"] = QLatin1String("date");

    //creator type adoption
    // translates original zotero types to author/editor
    // if you change anything here you must chage WriteToZotero::bibtexCreatorZoteroMapping
    m_zoteroToBibTeX["artist"] = QLatin1String("author");
    m_zoteroToBibTeX["performer"] = QLatin1String("author");
    m_zoteroToBibTeX["sponsor"] = QLatin1String("author");
    m_zoteroToBibTeX["programmer"] = QLatin1String("author");
    m_zoteroToBibTeX["director"] = QLatin1String("author");
    m_zoteroToBibTeX["interviewee"] = QLatin1String("author");
    m_zoteroToBibTeX["cartographer"] = QLatin1String("author");
    m_zoteroToBibTeX["inventor"] = QLatin1String("author");
    m_zoteroToBibTeX["podcaster"] = QLatin1String("author");
    m_zoteroToBibTeX["presenter"] = QLatin1String("author");

    m_zoteroToBibTeX["contributor"] = QLatin1String("editor");
    m_zoteroToBibTeX["composer"] = QLatin1String("editor");
}

void ReadFromZotero::fetchItems(const QString &collection)
{
    QString apiCommand;
    if(collection.isEmpty()) {
        apiCommand = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/items/top?format=atom&content=json");
    }
    else {
        apiCommand = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/collections/") + collection + QLatin1String("/items?format=atom&content=json");
    }

    if(!m_psd.pwd.isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + m_psd.pwd);
    }

    setRequestType(Items);
    startRequest(apiCommand);
}

void ReadFromZotero::fetchItem(const QString &id, const QString &collection )
{
    QString apiCommand = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/items/top?format=atom&content=json&limit=10");
    if(!m_psd.pwd.isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + m_psd.pwd);
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

    QString apiCommand = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/collections") + subCollection + QLatin1String("?format=atom");
    if(!m_psd.pwd.isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + m_psd.pwd);
    }

    setRequestType(Collections);
    startRequest(apiCommand);
}

void ReadFromZotero::fetchCollection(const QString &collection )
{
    QString apiCommand = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/collections/") + collection + QLatin1String("?format=atom");
    if(!m_psd.pwd.isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + m_psd.pwd);
    }

    setRequestType(Collections);
    startRequest(apiCommand);
}

void ReadFromZotero::requestFinished()
{
    m_cachedCollectionResult.clear();
    m_bibFile.clear();

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
                m_bibFile.append( QSharedPointer<Element>(readItemEntry(xmlReader)) );
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

Entry * ReadFromZotero::readItemEntry(QXmlStreamReader &xmlReader)
{
    Entry *e = new Entry;
    bool finishEntry = false;
    while(!finishEntry) {
        bool startelement = xmlReader.readNextStartElement();

        if(startelement && xmlReader.name() == QLatin1String("key")) {
            QString key = xmlReader.readElementText();
            PlainText *zKey = new PlainText(key);
            Value zkValue;
            zkValue.append(QSharedPointer<ValueItem>(zKey));
            e->insert(QLatin1String("zoteroKey"), zkValue);
            e->setId(key);
        }
        else if(startelement && xmlReader.name() == QLatin1String("numChildren")) {
            QString zoteroChildren = xmlReader.readElementText();

            PlainText *ptValue = new PlainText(zoteroChildren);
            Value valueList;
            valueList.append(QSharedPointer<ValueItem>(ptValue));
            e->insert(QLatin1String("zoteroChildren"), valueList);
        }
        else if(startelement && xmlReader.name() == QLatin1String("updated")) {
            QString zoteroUpdated = xmlReader.readElementText();

            PlainText *ptValue = new PlainText(zoteroUpdated);
            Value valueList;
            valueList.append(QSharedPointer<ValueItem>(ptValue));
            e->insert(QLatin1String("zoteroUpdated"), valueList);
        }
        else if(xmlReader.name() == "content") {
            QString etag = xmlReader.attributes().value(QLatin1String("zapi:etag")).toString();

            PlainText *ptValue = new PlainText(etag);
            Value valueList;
            valueList.append(QSharedPointer<ValueItem>(ptValue));
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

const File & ReadFromZotero::getFile()
{
    return m_bibFile;
}

void ReadFromZotero::readJsonContentBibTeX(Entry *e, const QString &content)
{
    QJson::Parser parser;
    bool ok;

    QVariantMap result = parser.parse (content.toLatin1(), &ok).toMap();
    if (!ok) {
        qFatal("ReadFromZotero::readJsonContentBibTeX :: An error occurred during json parsing");
        return;
    }

    QMapIterator<QString, QVariant> i(result);
    while (i.hasNext()) {
        i.next();

        //##########################################################################
        //# special tag handling

        if(i.key() == QLatin1String("tags")) {
            Value tagList;
            foreach (const QVariant &tag, i.value().toList()) {
                QVariantMap tagMap = tag.toMap();
                Keyword *k = new Keyword(tagMap.value(QLatin1String("tag")).toString());
                if(k->text().isEmpty()) {
                    delete k;
                }
                else {
                    tagList.append(QSharedPointer<ValueItem>(k));
                }
            }
            e->insert(m_zoteroToBibTeX.value(QLatin1String("tags"), QLatin1String("keywords")), tagList);
        }

        //##########################################################################
        //# change the entry type to something bibtex offers if possible

        else if(i.key() == QLatin1String("itemType")) {
            QString text = i.value().toString().toLower();

            if(text == QLatin1String("booksection")) {
                e->setType(QLatin1String("inbook")); // as inbook/incollection is part of a book with its own name
            }
            else if(text == QLatin1String("conferencepaper")) {
                e->setType(QLatin1String("inproceedings"));
            }
            else if(text == QLatin1String("journalarticle")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("journal"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                e->insert(QLatin1String("type"), valueList);
            }
            else if(text == QLatin1String("magazinearticle")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("magazine"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                e->insert(QLatin1String("type"), valueList);
            }
            else if(text == QLatin1String("newspaperarticle")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("newspaper"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                e->insert(QLatin1String("type"), valueList);
            }
            else {
                e->setType(text);
            }
        }

        //##########################################################################
        //# parse creators, change some fields to author/editor if possible
        //# all other fields are left alone and inserted with their normal name (like creator type translator)

        else if(i.key() == QLatin1String("creators")) {
            foreach (const QVariant &author, i.value().toList()) {
                QVariantMap authorMap = author.toMap();

                Person *p = new Person(authorMap.value(QLatin1String("firstName")).toString(),
                                       authorMap.value(QLatin1String("lastName")).toString());

                // get the Valuelist for the current creator type
                // either the translated one if availabe or the zotero one, if none exist an empty new Value
                QString creatorType = authorMap.value(QLatin1String("creatorType")).toString();
                QString translatedCreatorType = m_zoteroToBibTeX.value(creatorType, creatorType);
                Value creatorValue = e->value(translatedCreatorType);

                creatorValue.append(QSharedPointer<ValueItem>(p));
                e->remove(m_zoteroToBibTeX.value(creatorType, creatorType));
                e->insert(m_zoteroToBibTeX.value(creatorType, creatorType), creatorValue);
            }
        }
        else {
            QString text = i.value().toString();
            if(text.isEmpty())
                continue;
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(QSharedPointer<ValueItem>(ptValue));
            // here either the transformed key name from the lookup table is used
            // or if nothing is found the key from zotero is used
            e->insert(m_zoteroToBibTeX.value(i.key(), i.key()), valueList);
        }
    }
}

void ReadFromZotero::readJsonContentOriginal(Entry *e, const QString &content)
{
    QJson::Parser parser;
    bool ok;

    QVariantMap result = parser.parse (content.toLatin1(), &ok).toMap();
    if (!ok) {
        qFatal("ReadFromZotero::readJsonContentOriginal :: An error occurred during json parsing");
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
                    tagList.append(QSharedPointer<ValueItem>(k));
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
                list.append(QSharedPointer<ValueItem>(p));
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
            valueList.append(QSharedPointer<ValueItem>(ptValue));
            e->insert(i.key(), valueList);
        }
    }
}
