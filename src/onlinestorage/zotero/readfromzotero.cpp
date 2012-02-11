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

#include "zoteroinfo.h"

#include <kbibtex/entry.h>

#include <QtNetwork/QNetworkReply>
#include <QtCore/QXmlStreamReader>
#include <qjson/parser.h>

#include <KDE/KDebug>

ReadFromZotero::ReadFromZotero(QObject *parent)
    : ReadFromStorage(parent)
    , m_bibFile(new File)
    , m_fetchIncomplete(false)
{
    // ignore note / attachment downloads with normal requests
        m_searchFilter = ("&itemType=-attachment");

    // build the mappinglist
    // @see https://api.zotero.org/itemTypeFields?itemType=presentation&pprint=1
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
    m_zoteroToBibTeX["legislativeBody"] = QLatin1String("organization");
    m_zoteroToBibTeX["session"] = QLatin1String("event");
    m_zoteroToBibTeX["dateDecided"] = QLatin1String("date");
    m_zoteroToBibTeX["dateEnacted"] = QLatin1String("date");
    m_zoteroToBibTeX["firstPage"] = QLatin1String("pages");
    m_zoteroToBibTeX["issueDate"] = QLatin1String("date");
    m_zoteroToBibTeX["extra"] = QLatin1String("note");
    m_zoteroToBibTeX["meetingName"] = QLatin1String("event");
    m_zoteroToBibTeX["conferenceName"] = QLatin1String("event");
    m_zoteroToBibTeX["mapType"] = QLatin1String("type");

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

    m_zoteroToBibTeX["composer"] = QLatin1String("editor");
}

void ReadFromZotero::fetchItems(const QString &collection)
{
    fetchItems(50,0,collection);
}

void ReadFromZotero::fetchItems(int limit, int start, const QString &collection)
{
    m_collectionToFetchFrom = collection;

    QString apiCommand;
    if(collection.isEmpty()) {
        apiCommand = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/items?format=atom&content=json");
    }
    else {
        apiCommand = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/collections/") + collection + QLatin1String("/items?format=atom&content=json");
    }

    apiCommand.append( QLatin1String("&limit=") + QString::number(limit) + QLatin1String("&start=") + QString::number(start) );

    if(!m_psd.pwd.isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + m_psd.pwd);
    }

    apiCommand.append( m_searchFilter );

    setRequestType(Items);
    startRequest(apiCommand);
}

void ReadFromZotero::fetchItem(const QString &id, const QString &collection )
{
    Q_UNUSED(collection)
    QString apiCommand = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/items/") + id + QLatin1String("?format=atom&content=json&limit=10");
    if(!m_psd.pwd.isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + m_psd.pwd);
    }

    apiCommand.append( m_searchFilter );

    setRequestType(Items);
    startRequest(apiCommand);
}

void ReadFromZotero::fetchCollections(const QString &parent )
{
    fetchCollections(50,0,parent);
}

void ReadFromZotero::fetchCollections(int limit, int start, const QString &parent)
{
    m_collectionToFetchFrom = parent;

    QString subCollection;
    if(!parent.isEmpty()) {
        subCollection = QLatin1String("/") + parent + QLatin1String("/collections");
    }

    QString apiCommand = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/collections") + subCollection + QLatin1String("?format=atom");
    if(!m_psd.pwd.isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + m_psd.pwd);
    }

    apiCommand.append( m_searchFilter );

    apiCommand.append( QLatin1String("&limit=") + QString::number(limit) + QLatin1String("&start=") + QString::number(start) );

    setRequestType(Collections);
    startRequest(apiCommand);
}

void ReadFromZotero::fetchCollection(const QString &collection )
{
    QString apiCommand = QLatin1String("https://api.zotero.org/") + m_psd.url + QLatin1String("/") + m_psd.userName + QLatin1String("/collections/") + collection + QLatin1String("?format=atom");
    if(!m_psd.pwd.isEmpty()) {
        apiCommand.append( QLatin1String("&key=") + m_psd.pwd);
    }

    apiCommand.append( m_searchFilter );

    setRequestType(Collections);
    startRequest(apiCommand);
}

void ReadFromZotero::requestFinished()
{
    int nextFetchLimit = 50;
    int nextFetchStart = -1; // if it stays -1 we know we caught all follow up request and can stop now

    // clear local cache unless this is a follow up from a bigger request
    if(!m_fetchIncomplete) {
        m_cachedCollectionResult.clear();
        m_bibFile->clear();
    }

    QXmlStreamReader xmlReader;
    xmlReader.setDevice(reply());

    while(!xmlReader.atEnd()) {
        if(!xmlReader.readNextStartElement()) {
            continue;
        }

        // parse link suggestions for the case that we have to fetch more items
        if(xmlReader.name() == QLatin1String("link")) {
            QXmlStreamAttributes linkAttributes = xmlReader.attributes();

            // if there do exist more items we need to fetch get the right values for the next start
            if(QLatin1String("next") == linkAttributes.value(QLatin1String("rel")) ) {
                m_fetchIncomplete = true;

                QString href = linkAttributes.value(QLatin1String("href")).toString();

                // fetch the next start suggesstion
                QRegExp rxStart(QLatin1String("start=(\\d+)"));
                int pos = rxStart.indexIn(href);
                if (pos > -1) {
                    nextFetchStart = rxStart.cap(1).toInt();
                }
                else {
                    qWarning() << "could not retrieve next start number for another zotero fetch.";
                }

                // returns a suggested limit value. Always the last used or nothing if no limit was specified
                // as zotero does not support more than 50 items in one go, we default to this
                QRegExp rxLimit(QLatin1String("limit=(\\d+)"));
                pos = rxLimit.indexIn(href);
                if (pos > -1) {
                    nextFetchLimit = rxLimit.cap(1).toInt();
                }
                else {
                    nextFetchLimit = 50;
                }
            }
        }

        // parse the entry content (all item information we actually wanted)
        else if(xmlReader.name() == QLatin1String("entry")) {
            switch(requestType()) {
            case Items:
            {
                Entry *e = readItemEntry(xmlReader);
                if(e != 0)
                    m_bibFile->append( QSharedPointer<Element>( e ) );
                break;
            }
            case Collections:
                m_cachedCollectionResult.append( readCollectionEntry(xmlReader) );
                break;
            }
        }
    }

    if(nextFetchStart == -1)
        m_fetchIncomplete = false;

    if(m_fetchIncomplete) {
        // fetch the next batch
        switch(requestType()) {
        case Items:
            fetchItems(nextFetchLimit, nextFetchStart, m_collectionToFetchFrom);
            break;
        case Collections:
            fetchCollections(nextFetchLimit, nextFetchStart, m_collectionToFetchFrom);
            break;
        }

        return;
    }
    else {
        emit finished();
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

        if(xmlReader.name() == QLatin1String("link")) {
            QXmlStreamAttributes linkAttributes = xmlReader.attributes();

            // only parse the link entry that gives us the parent item
            if(QLatin1String("up") == linkAttributes.value(QLatin1String("rel")) ) {
                QString href = linkAttributes.value(QLatin1String("href")).toString();

                // fetch the next start suggesstion
                QRegExp rxStart(QLatin1String("items/(\\w+)"));
                int pos = rxStart.indexIn(href);
                if (pos > -1) {
                    QString itemParent = rxStart.cap(1);
                    PlainText *zParent = new PlainText(itemParent);
                    Value zpValue;
                    zpValue.append(QSharedPointer<ValueItem>(zParent));
                    e->insert(QLatin1String("zoteroParent"), zpValue);
                }
                else {
                    qWarning() << "could not parse parent item for the note/attachment!";
                }
            }
            else if(QLatin1String("enclosure") == linkAttributes.value(QLatin1String("rel")) ) {
                QString itemFile = linkAttributes.value(QLatin1String("href")).toString();
                PlainText *zFile = new PlainText(itemFile);
                Value zfValue;
                zfValue.append(QSharedPointer<ValueItem>(zFile));
                e->insert(QLatin1String("zoteroAttachmentFile"), zfValue);

                QString itemFileSize = linkAttributes.value(QLatin1String("length")).toString();
                PlainText *zFileSize = new PlainText(itemFileSize);
                Value zfsValue;
                zfsValue.append(QSharedPointer<ValueItem>(zFileSize));
                e->insert(QLatin1String("zoteroAttachmentFileSize"), zfsValue);
            }
        }
        else if(startelement && xmlReader.name() == QLatin1String("zapi:itemType")) {
            QString itemType = xmlReader.readElementText();
            PlainText *zType = new PlainText(itemType);
            Value ztValue;
            ztValue.append(QSharedPointer<ValueItem>(zType));
            e->insert(QLatin1String("zoteroType"), ztValue);
        }
        else if(startelement && xmlReader.name() == QLatin1String("title")) {
            QString zoteroTitle = xmlReader.readElementText();
            PlainText *zTitle = new PlainText(zoteroTitle);
            Value ztValue;
            ztValue.append(QSharedPointer<ValueItem>(zTitle));
            e->insert(QLatin1String("zoteroTitle"), ztValue);
        }
        else if(startelement && xmlReader.name() == QLatin1String("key")) {
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

File *ReadFromZotero::getFile()
{
    return m_bibFile;
}

QList<CollectionInfo> ReadFromZotero::getCollectionInfo()
{
    return m_cachedCollectionResult;
}

void ReadFromZotero::setSearchFilter(const QString &filter)
{
    m_searchFilter = filter;
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

    QString zoteroType;
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
            zoteroType = text;

            if(text == QLatin1String("booksection")) {
                e->setType(QLatin1String("incollection")); // as inbook/incollection is part of a book with its own name
            }
            else if(text == QLatin1String("conferencepaper")) {
                e->setType(QLatin1String("inproceedings"));
            }
            else if(text == QLatin1String("document")) {
                e->setType(QLatin1String("misc"));
            }
            else if(text == QLatin1String("manuscript")) {
                e->setType(QLatin1String("script"));
            }
            else if(text == QLatin1String("journalarticle")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("journal"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                e->insert(QLatin1String("articletype"), valueList);
            }
            else if(text == QLatin1String("magazinearticle")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("magazine"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                e->insert(QLatin1String("articletype"), valueList);
            }
            else if(text == QLatin1String("newspaperarticle")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("newspaper"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                e->insert(QLatin1String("articletype"), valueList);
            }
            else if(text == QLatin1String("encyclopediaarticle")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("encyclopedia"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                e->insert(QLatin1String("articletype"), valueList);
            }
            else if(text == QLatin1String("blogpost")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("blog"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                e->insert(QLatin1String("articletype"), valueList);
            }
            else if(text == QLatin1String("forumpost")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("forum"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                e->insert(QLatin1String("articletype"), valueList);
            }
            else if(text == QLatin1String("webpage")) {
                e->setType(QLatin1String("article"));

                PlainText *ptValue = new PlainText(QLatin1String("webpage"));
                Value valueList;
                valueList.append(QSharedPointer<ValueItem>(ptValue));
                e->insert(QLatin1String("articletype"), valueList);
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

            if(text.isEmpty()) {
                continue;
            }
            PlainText *ptValue = new PlainText(text);
            Value valueList;
            valueList.append(QSharedPointer<ValueItem>(ptValue));
            // here either the transformed key name from the lookup table is used
            // or if nothing is found the key from zotero is used
            e->insert(m_zoteroToBibTeX.value(i.key(), i.key()).toLower(), valueList);
        }
    }

    // now fill the entry with empty values for any key that was not downloaded but is supported by the server
    // these entries must be removed from a local storage then if we merge it with an existing entry
    ZoteroInfo *zi = new ZoteroInfo;
    QStringList defaultkeys = zi->defaultKeysFor(zoteroType);

    foreach(const QString &key, defaultkeys) {
        if(!e->contains(key)) {
            Value value;
            e->insert(key, value);
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
