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

#include "globals.h"

#include <Nepomuk/Query/Result>
#include <KDE/KIcon>

#include <QtCore/QAbstractTableModel>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QMetaType>

class Entry;
class FileExporterXSLT;
class OnlineSearchAbstract;

/**
  * @brief Holds the date for one search entry as retrieved from the @c SearchWidget
  */
struct SearchResultEntry {
    Nepomuk::Query::Result nepomukResult; /**< The valid resource if the result was found in the nepomuk storage */
    OnlineSearchAbstract *webEngine;      /**< The OnlineSearch Engine details if the result comes from a websearch */
    QSharedPointer<Entry> webResult;      /**< The BibTeX entry if the result comes from a websearch */
};

Q_DECLARE_METATYPE(SearchResultEntry)

/**
  * @brief Model to list all the serach results from the KBibTeX websearch
  *
  * Unlike most other TableModels this one does not get its data from nepomuk.
  * Instead this ListModel will be populated by the Websearch widget and each
  * KBibTeX file parsed with a simple citation style xsl file.
  *
  * @see FileExporterXSLT
  * @see HtmlDelegate
  * @see SearchWidget
  *
  * @todo make the citation.xsl file configurable
  */
class SearchResultModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /**
      * @brief Holds the cached processed table data for one search row
      *
      * Cache is used to speed up sorting/display of large datasets
      */
    struct SRCachedRowEntry {
        QVariantList displayColums;
        QVariantList decorationColums;
        QVariantList toolTipColums;
        Nepomuk::Resource resource;
        QSharedPointer<Entry> entry;
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
      * @return the resource for the current search entry or an invalid resource if it contains a BibTeX entry
      */
    Nepomuk::Resource nepomukResourceAt(const QModelIndex &selection);

    /**
      * @return the BibTeX entry for the current search entry or an invalid sharedpointer if it contains a Nepomuk Resource
      */
    QSharedPointer<Entry> bibTeXResourceAt(const QModelIndex &selection);

public slots:
    void clearData();
    void addSearchResult(SearchResultEntry newEntry);

private:
    QVariantList createDisplayData(const Nepomuk::Query::Result & nepomukResult) const;
    QVariantList createDecorationData(const Nepomuk::Query::Result & nepomukResult) const;
    QVariantList createToolTipData(const Nepomuk::Query::Result & nepomukResult) const;
    QVariantList createDisplayData(QSharedPointer<Entry> entry, OnlineSearchAbstract *engine) const;
    QVariantList createDecorationData(QSharedPointer<Entry> entry, OnlineSearchAbstract *engine) const;
    QVariantList createToolTipData(QSharedPointer<Entry> entry, OnlineSearchAbstract *engine) const;

    /**
      * Generates a Translated string for the type of the nepomuk resource document from the entry
      */
    QString translateEntryType(const Nepomuk::Resource & resource) const;

    /**
      * Generates an Icon for the mimetype of the entry that is beeing displayed
      *
      * @todo find better icons based on mimetype from nepomuk or the url extension rather than using a fixed generic icon
      */
    KIcon iconizeEntryType(const Nepomuk::Resource & resource) const;

    QList<SRCachedRowEntry> m_modelCacheData;
    FileExporterXSLT *m_exporterXSLT;
};

#endif // SEARCHRESULTMODEL_H
