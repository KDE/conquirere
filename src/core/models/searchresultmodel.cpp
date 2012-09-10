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

#include <KDE/KGlobalSettings>
#include <KDE/KStandardDirs>
#include <KDE/KMimeType>

#include <QtGui/QFont>

#include <KDE/KDebug>

SearchResultModel::SearchResultModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

SearchResultModel::~SearchResultModel()
{
    //qDeleteAll(m_modelCacheData);
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

SearchResultModel::SRCachedRowEntry SearchResultModel::webResultAt(const QModelIndex &selection)
{
    if(!m_modelCacheData.isEmpty() && selection.row() >= 0) {
        return m_modelCacheData.at(selection.row());
    }

    return SRCachedRowEntry();
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

void SearchResultModel::clearData()
{
    if (m_modelCacheData.isEmpty()) {
        return;
    }

    beginRemoveRows(QModelIndex(), 0, m_modelCacheData.size()-1);
    m_modelCacheData.clear();
    endRemoveRows();
}

void SearchResultModel::addSearchResult(const QVariantList &searchResult)
{
    QList<SRCachedRowEntry> entryList;

    foreach(const QVariant &entry, searchResult) {
        QVariantMap entryMap = entry.toMap();

        SRCachedRowEntry srcre;
        srcre.displayColums = createDisplayData(entryMap);
        srcre.decorationColums = createDecorationData(entryMap);
        srcre.toolTipColums = createToolTipData(entryMap);

        if(entryMap.value(QLatin1String("engine-type")).toString() == QLatin1String("web")) {
            srcre.detailsurl = entryMap.value(QLatin1String("url")).toString();
            srcre.engineId = entryMap.value(QLatin1String("engine-id")).toString();
            srcre.engineScript = entryMap.value(QLatin1String("engine-script")).toString();
        }
        else {
            srcre.resource = Nepomuk2::Resource( entryMap.value(QLatin1String("nepomuk-uri")).toString() );
        }

        entryList.append(srcre);
    }

    kDebug() << "add entries" << entryList.size();
    beginInsertRows(QModelIndex(), m_modelCacheData.size(), m_modelCacheData.size() + entryList.size() - 1);
    m_modelCacheData.append(entryList);
    endInsertRows();

    kDebug() << "m_modelCacheData" << m_modelCacheData.size();
}


QVariantList SearchResultModel::createDisplayData(const QVariantMap &entryMap) const
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_EntryType: {
            newEntry = translateEntryType(entryMap.value(QLatin1String("publicationtype")).toString());
            break;
        }
        case Column_Author: {
            newEntry = entryMap.value(QLatin1String("authors"));
            break;
        }
        case Column_Name: {
            newEntry = entryMap.value(QLatin1String("title"));
            break;
        }
        case Column_Date: {
            newEntry = entryMap.value(QLatin1String("date"));
            break;
        }
        case Column_Details: {
            QString text = entryMap.value(QLatin1String("plaintext")).toString();

            text.prepend(QLatin1String("<font size=\"90%\">"));
            text.append(QLatin1String("</font>"));
            text.remove(QLatin1String("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
            text.remove(QLatin1String("\n"));

            newEntry = text;
            break;
        }
        case Column_StarRate: {
            newEntry = entryMap.value(QLatin1String("star"));
            break;
        }
        default:
            newEntry = QVariant();
        }

        displayList.append(newEntry);
    }

    return displayList;
}

QVariantList SearchResultModel::createDecorationData(const QVariantMap &entryMap) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_EngineIcon:
        {
            newEntry = KIcon(entryMap.value(QLatin1String("engine-icon")).toString() );
            break;
        }
        case Column_EntryType:
        {
            newEntry = iconizeEntryType(entryMap.value(QLatin1String("publicationtype")).toString(),
                                        entryMap.value(QLatin1String("fileurl")).toString());
            break;
        }
        default:
            newEntry = QVariant();
        }

        decorationList.append(newEntry);
    }

    return decorationList;
}

QVariantList SearchResultModel::createToolTipData(const QVariantMap &entryMap) const
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_EngineIcon:
        {
            newEntry = entryMap.value(QLatin1String("engine-name"));
            break;
        }
        default:
            newEntry = QVariant();
        }

        decorationList.append(newEntry);
    }

    return decorationList;
}

QString SearchResultModel::translateEntryType(const QString & typeList) const
{
    //REFACTOR: nepomuk types to usefull string
    if(typeList.toLower().contains("publication")) {
        return i18nc("General publication type","Publication");
        //BibEntryType type = BibEntryTypeFromUrl(resource);
        //return BibEntryTypeTranslation.at(type);
    }
    if(typeList.toLower().contains("article")) {
        return i18n("Article");
    }
    if(typeList.toLower().contains("inproceedings")) {
        return i18n("InProceedings");
    }
    if(typeList.toLower().contains("book")) {
        return i18n("Book");
    }
    if(typeList.toLower().contains("document")) {
        return i18nc("General document type","Document");
    }
    if(typeList.toLower().contains("audio")) {
        return i18nc("Audio resource type","Audio");
    }
    if(typeList.toLower().contains("video")) {
        return i18nc("Video resource type","Video");
    }
    if(typeList.toLower().contains("image")) {
        return i18nc("Image resource type","Image");
    }
    if(typeList.toLower().contains("message")) {
        return i18nc("Email resource type","EMail");
    }
    if(typeList.toLower().contains("note")) {
        return i18nc("Note resource type","Note");
    }
    if(typeList.toLower().contains("website")) {
        return i18nc("Website or bookmark","Website");
    }

    return i18nc("other unknown resource type","other");
}

KIcon SearchResultModel::iconizeEntryType(const QString & typeList, const QString & url) const
{
    if(typeList.toLower().contains("publication")) {
        //BibEntryType type = BibEntryTypeFromUrl(resource);
        return KIcon(BibEntryTypeIcon.at(BibType_Book));
    }
    if(typeList.toLower().contains("article")) {
        //BibEntryType type = BibEntryTypeFromUrl(resource);
        return KIcon(BibEntryTypeIcon.at(BibType_Article));
    }
    if(typeList.toLower().contains("inproceedings")) {
        //BibEntryType type = BibEntryTypeFromUrl(resource);
        return KIcon(BibEntryTypeIcon.at(BibType_Proceedings));
    }
    if(typeList.toLower().contains("book")) {
        return KIcon(BibEntryTypeIcon.at(BibType_Book));
    }
    if(typeList.toLower().contains("document")) {
        KUrl path( url );
        return KIcon(KMimeType::iconNameForUrl(path));
    }
    if(typeList.toLower().contains("audio")) {
        KUrl path(url );
        return KIcon(KMimeType::iconNameForUrl(path));
    }
    if(typeList.toLower().contains("video")) {
        KUrl path( url );
        return KIcon(KMimeType::iconNameForUrl(path));
    }
    if(typeList.toLower().contains("image")) {
        KUrl path( url );
        return KIcon(KMimeType::iconNameForUrl(path));
    }
    if(typeList.toLower().contains("message")) {
        return KIcon(QLatin1String("internet-mail"));
    }
    if(typeList.toLower().contains("note")) {
        return KIcon(QLatin1String("knotes"));
    }
    if(typeList.toLower().contains("website")) {
        KUrl path( url );
        QString iconName = KMimeType::favIconForUrl(path);
        if(iconName.isEmpty()) {
            iconName = QLatin1String("text-html");
        }
        return KIcon(iconName);
    }

    return KIcon(QLatin1String("unknown"));
}
