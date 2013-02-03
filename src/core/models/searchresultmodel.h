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

#ifndef SEARCHRESULTMODEL_H
#define SEARCHRESULTMODEL_H

#include <KDE/KIcon>

#include <Nepomuk2/Resource>

#include <QtCore/QAbstractTableModel>
#include <QtCore/QList>
#include <QtCore/QString>

class Entry;

/**
  * @brief Model to list all the search results from Nepomuk and the WebExtractor
  *
  * Unlike most other TableModels this one does not get its data from nepomuk.
  * Instead this ListModel will be populated by the SearchWidget
  *
  * @see HtmlDelegate
  * @see SearchWidget
  *
  */
class SearchResultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
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

    /**
      * @brief Holds the cached processed table data for one search row
      *
      * Cache is used to speed up sorting/display of large datasets
      */
    struct SRCachedRowEntry {
        QVariantList displayColums;
        QVariantList decorationColums;
        QVariantList toolTipColums;
         // for nepomuk results
        Nepomuk2::Resource resource;
         // for web results
        QUrl detailsurl;
        QString engineId;
        QString engineScript;
    };

    explicit SearchResultModel(QObject *parent = 0);
    ~SearchResultModel();

    // implemented from QAbstractTableModel
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    /**
      * @return the default width for a table section if the user didn't changed it.
      */
    int defaultSectionSize(int i) const;

    /**
      * @return The numbers of all sections with a fixed with the user can't change (with will be default width).
      *
      * @see defaultSectionSize
      */
    QList<int> fixedWithSections() const;

    /**
      * @return the resource for the current search entry or an invalid resource if it contains a Webresult entry
      */
    Nepomuk2::Resource nepomukResourceAt(const QModelIndex &selection);

    /**
      * @return return the entry for the web search result for the current row
      */
    SearchResultModel::SRCachedRowEntry webResultAt(const QModelIndex &selection);

public slots:
    void clearData();
    void addSearchResult(const QVariantList &searchResult);

private:
    QVariantList createDisplayData(const QVariantMap &entryMap) const;
    QVariantList createDecorationData(const QVariantMap &entryMap) const;
    QVariantList createToolTipData(const QVariantMap &entryMap) const;

    /**
      * Generates a Translated string for the type of the nepomuk resource document from the entry
      */
    QString translateEntryType(const QString &typeList) const;

    /**
      * Generates an Icon for the mimetype of the entry that is being displayed
      *
      * @todo find better icons based on mimetype from nepomuk or the url extension rather than using a fixed generic icon
      */
    KIcon iconizeEntryType(const QString &typeList, const QString &url) const;

    QList<SRCachedRowEntry> m_modelCacheData;
};

#endif // SEARCHRESULTMODEL_H
