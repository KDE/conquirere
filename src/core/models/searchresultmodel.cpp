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

#include "searchresultmodel.h"

#include <kbibtex/entry.h>
#include <kbibtex/fileexporterxslt.h>
#include <kbibtex/onlinesearchabstract.h>

#include <Nepomuk2/Variant>
#include <Nepomuk2/Vocabulary/NFO>
#include <Nepomuk2/Vocabulary/NMO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NCO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/PIMO>

#include <KDE/KGlobalSettings>
#include <KDE/KStandardDirs>
#include <KDE/KMimeType>

#include <QtGui/QFont>
#include <QtCore/QBuffer>

enum BibTeXColumnList {
    Column_StarRate,
    Column_EngineIcon,
    Column_EntryType,
    Column_Details,
    Column_Name,
    Column_Author,
    Column_Date,

    Max_columns
};

SearchResultModel::SearchResultModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    qRegisterMetaType<SearchResultEntry>("SearchResultEntry");

    m_exporterXSLT = new FileExporterXSLT();
    m_exporterXSLT->setXSLTFilename(KStandardDirs::locate("data", QLatin1String("conquirere/simple.xsl")));
}

SearchResultModel::~SearchResultModel()
{
    //qDeleteAll(m_modelCacheData);
    delete m_exporterXSLT;
}

int SearchResultModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_modelCacheData.size();
}

int SearchResultModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Max_columns;
}

QVariant SearchResultModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_modelCacheData.size() || index.row() < 0)
        return QVariant();

    SRCachedRowEntry srcre = m_modelCacheData.at(index.row());

    if (role == Qt::DisplayRole) {
        return srcre.displayColums.at(index.column());
    }
    if (role == Qt::DecorationRole) {
        return srcre.decorationColums.at(index.column());
    }
    if (role == Qt::ToolTipRole) {
        return srcre.toolTipColums.at(index.column());
    }

    return QVariant();
}

QVariant SearchResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch(section) {
        case Column_EntryType:
            return i18nc("Header name for the search result column type (document, article, email etc)","Type");
            break;
        case Column_Details:
            return i18nc("Header name for the search result column with the general details","Details");
            break;
        case Column_Name:
            return i18nc("HeaderName of the document/publication name search result column etc","Name");
            break;
        case Column_Date:
            return i18nc("Header name for the search result date","Date");
            break;
        case Column_Author:
            return i18nc("Header name for the search result column author","Author");
            break;
        case Column_StarRate:
            return i18nc("Header name for the rating column","Rating");
        }
    }

    if (role == Qt::ToolTipRole) {
        switch(section) {
        case Column_EngineIcon:
            return i18nc("ToolTip for the search result column Engine (nepomuk or one of the search engines)","The Engine where the entry was found in");
            break;
        case Column_EntryType:
            return i18nc("ToolTip for the search result column entry type (Document/article etc.)", "The type of the entry");
            break;
        case Column_Details:
            return i18nc("ToolTip for the search result column details", "Some details on the entry");
            break;
        case Column_Name:
            return i18nc("ToolTip for the search result column name", "The name of the entry");
            break;
        case Column_Date:
            return i18nc("ToolTip for the search result column date", "The date of the entry");
            break;
        case Column_Author:
            return i18nc("ToolTip for the search result column author", "The Author of the entry");
            break;
        case Column_StarRate:
            return i18nc("ToolTip for the search result column rating", "Rating");
        }
    }

    return QVariant();
}

Nepomuk2::Resource SearchResultModel::nepomukResourceAt(const QModelIndex &selection)
{
    Nepomuk2::Resource ret;

    if(!m_modelCacheData.isEmpty() && selection.row() >= 0) {
        SRCachedRowEntry entryCache = m_modelCacheData.at(selection.row());
        ret = entryCache.resource;
    }

    return ret;
}

int SearchResultModel::defaultSectionSize(int i) const
{
    switch (i) {
    case Column_EngineIcon:
        return 25;
    case Column_EntryType:
        return 100;
    case Column_Details:
        return 300;
    case Column_Name:
        return 200;
    case Column_Author:
        return 100;
    case Column_Date:
        return 100;
    case Column_StarRate:
        return 75;
    }

    return 100;
}

QList<int> SearchResultModel::fixedWithSections() const
{
    QList<int> fixedWith;
    fixedWith << Column_EngineIcon << Column_StarRate;

    return fixedWith;
}

QSharedPointer<Entry> SearchResultModel::bibTeXResourceAt(const QModelIndex &selection)
{
    QSharedPointer<Entry> entry;

    if(!m_modelCacheData.isEmpty() && selection.row() >= 0) {
        SRCachedRowEntry entryCache = m_modelCacheData.at(selection.row());
        entry = entryCache.entry;
    }

    return entry;
}

void SearchResultModel::clearData()
{
    if (m_modelCacheData.isEmpty()) {
        return;
    }

    beginRemoveRows(QModelIndex(), 0, m_modelCacheData.size()-1);
    m_modelCacheData.clear();
    endRemoveRows();
}

void SearchResultModel::addSearchResult(SearchResultEntry newEntry)
{
    SRCachedRowEntry srcre;

    if(newEntry.webResult) {
        srcre.displayColums = createDisplayData(newEntry.webResult, newEntry.webEngine);
        srcre.decorationColums = createDecorationData(newEntry.webResult, newEntry.webEngine);
        srcre.toolTipColums = createToolTipData(newEntry.webResult, newEntry.webEngine);
        srcre.entry = newEntry.webResult;
    }
    else {
        srcre.displayColums = createDisplayData(newEntry.nepomukResult);
        srcre.decorationColums = createDecorationData(newEntry.nepomukResult);
        srcre.toolTipColums = createToolTipData(newEntry.nepomukResult);
        srcre.resource = newEntry.nepomukResult.resource();
        //srcre.entry = 0;
    }

    beginInsertRows(QModelIndex(), m_modelCacheData.size(), m_modelCacheData.size());
    m_modelCacheData.append(srcre);
    endInsertRows();
}

QVariantList SearchResultModel::createDisplayData(const Nepomuk2::Query::Result & nepomukResult) const
{
    Nepomuk2::Resource res = nepomukResult.resource();
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_EntryType: {
            newEntry = translateEntryType(res);
            break;
        }
        case Column_Author: {
            QString authorSting;
            QList<Nepomuk2::Resource> authorList = res.property(Nepomuk2::Vocabulary::NCO::creator()).toResourceList();

            foreach(const Nepomuk2::Resource & a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            newEntry = authorSting;
            break;
        }
        case Column_Name: {
            QString titleSting = res.property(Nepomuk2::Vocabulary::NIE::title()).toString();

            newEntry = titleSting;
            break;
        }
        case Column_Date: {
            QString dateString;
            if(res.hasType(Nepomuk2::Vocabulary::NBIB::Publication()))
                dateString = res.property(Nepomuk2::Vocabulary::NBIB::publicationDate()).toString();
            else {
                dateString = res.property(Nepomuk2::Vocabulary::NMO::sentDate()).toString();
                if(dateString.isEmpty())
                    dateString = res.property(Nepomuk2::Vocabulary::NIE::contentCreated()).toString();
            }

            QDateTime date = QDateTime::fromString(dateString, Qt::ISODate);
            if(date.isValid()) {
                newEntry = date.toString("dd.MM.yyyy");
            }
            else {
                newEntry = dateString;
            }

            newEntry = dateString;
            break;
        }
        case Column_Details: {
            QString titleSting = res.property(Nepomuk2::Vocabulary::NIE::title()).toString();
            if(titleSting.isEmpty())
                titleSting = res.genericLabel();

            QString detailText;
            detailText.append(QLatin1String("<font size=\"100%\"><b>"));
            detailText.append(titleSting);
            detailText.append(QLatin1String("</b></font><br/><font size=\"85%\">"));
            detailText.append(nepomukResult.excerpt());
            detailText.append(QLatin1String("</font>"));

            newEntry = detailText;
            break;
        }
        case Column_StarRate: {
            int rating = res.property(Soprano::Vocabulary::NAO::numericRating()).toInt();

            newEntry = rating;
            break;
        }
        default:
            newEntry = QVariant();
        }

        displayList.append(newEntry);
    }

    return displayList;
}

QVariantList SearchResultModel::createDecorationData(const Nepomuk2::Query::Result & nepomukResult) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_EngineIcon:
        {
            newEntry = KIcon(QLatin1String("nepomuk"));
            break;
        }
        case Column_EntryType:
        {
            newEntry = iconizeEntryType(nepomukResult.resource());
            break;
        }
        default:
            newEntry = QVariant();
        }

        decorationList.append(newEntry);
    }

    return decorationList;
}

QVariantList SearchResultModel::createToolTipData(const Nepomuk2::Query::Result & nepomukResult) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_EngineIcon:
        {
            newEntry = i18n("Nepomuk");
            break;
        }
        default:
            newEntry = QVariant();
        }

        decorationList.append(newEntry);
    }

    return decorationList;
}

QVariantList SearchResultModel::createDisplayData(QSharedPointer<Entry> entry, OnlineSearchAbstract *engine) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_EntryType: {
            newEntry = entry->type();
            break;
        }
        case Column_Author: {
            QString authorSting = PlainTextValue::text(entry->value(QLatin1String("author")));

            if(authorSting.isEmpty()) {
                QString editorSting = PlainTextValue::text(entry->value(QLatin1String("editor")));
                newEntry = editorSting;
            }
            else {
                newEntry = authorSting;
            }
            break;
        }
        case Column_Name: {
            QString titleSting = PlainTextValue::text(entry->value(QLatin1String("title")));

            newEntry = titleSting;
            break;
        }
        case Column_Date: {
            QString dateSting = PlainTextValue::text(entry->value(QLatin1String("year")));

            newEntry = dateSting;
            break;
        }
        case Column_Details: {
            QStringList errorLog;
            QBuffer buffer;

            buffer.open(QBuffer::WriteOnly);
            m_exporterXSLT->save(&buffer, entry, &errorLog);
            buffer.close();

            buffer.open(QBuffer::ReadOnly);
            QTextStream ts(&buffer);
            QString text = ts.readAll();
            buffer.close();

            text.prepend(QLatin1String("<font size=\"90%\">"));
            text.append(QLatin1String("</font>"));
            text.remove(QLatin1String("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
            text.remove(QLatin1String("\n"));

            newEntry = text;
            break;
        }
        default:
            newEntry = QVariant();
        }

        displayList.append(newEntry);
    }

    return displayList;
}

QVariantList SearchResultModel::createDecorationData(QSharedPointer<Entry> entry, OnlineSearchAbstract *engine) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_EngineIcon:
        {
            newEntry = engine->icon();
            break;
        }
        default:
            newEntry = QVariant();
        }

        decorationList.append(newEntry);
    }

    return decorationList;
}

QVariantList SearchResultModel::createToolTipData(QSharedPointer<Entry> entry, OnlineSearchAbstract *engine) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_EngineIcon:
        {
            newEntry = engine->name();
            break;
        }
        default:
            newEntry = QVariant();
        }

        decorationList.append(newEntry);
    }

    return decorationList;
}

QString SearchResultModel::translateEntryType(const Nepomuk2::Resource & resource) const
{
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Publication())) {
        BibEntryType type = BibEntryTypeFromUrl(resource);
        return BibEntryTypeTranslation.at(type);
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NFO::Document())) {
        return i18nc("General document type","Document");
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NFO::Audio())) {
        return i18nc("Audio resource type","Audio");
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NFO::Video())) {
        return i18nc("Video resource type","Video");
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NFO::Image())) {
        return i18nc("Image resource type","Image");
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NMO::Message())) {
        return i18nc("Email resource type","EMail");
    }
    if(resource.hasType(Nepomuk2::Vocabulary::PIMO::Note())) {
        return i18nc("Note resource type","Note");
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NFO::Website())) {
        return i18nc("Website or bookmark","Website");
    }

    return i18nc("other unknown resource type","other");
}

KIcon SearchResultModel::iconizeEntryType(const Nepomuk2::Resource & resource) const
{
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Publication())) {
        BibEntryType type = BibEntryTypeFromUrl(resource);
        return KIcon(BibEntryTypeIcon.at(type));
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NFO::Document())) {
        KUrl path = resource.property(Nepomuk2::Vocabulary::NIE::url()).toUrl();
        return KIcon(KMimeType::iconNameForUrl(path));
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NFO::Audio())) {
        KUrl path = resource.property(Nepomuk2::Vocabulary::NIE::url()).toUrl();
        return KIcon(KMimeType::iconNameForUrl(path));
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NFO::Video())) {
        KUrl path = resource.property(Nepomuk2::Vocabulary::NIE::url()).toUrl();
        return KIcon(KMimeType::iconNameForUrl(path));
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NFO::Image())) {
        KUrl path = resource.property(Nepomuk2::Vocabulary::NIE::url()).toUrl();
        return KIcon(KMimeType::iconNameForUrl(path));
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NMO::Message())) {
        return KIcon(QLatin1String("internet-mail"));
    }
    if(resource.hasType(Nepomuk2::Vocabulary::PIMO::Note())) {
        return KIcon(QLatin1String("knotes"));
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NFO::Website())) {
        KUrl path = resource.property(Nepomuk2::Vocabulary::NIE::url()).toUrl();
        QString iconName = KMimeType::favIconForUrl(path);
        if(iconName.isEmpty()) {
            iconName = QLatin1String("text-html");
        }
        return KIcon(iconName);
    }

    return KIcon(QLatin1String("unknown"));
}
