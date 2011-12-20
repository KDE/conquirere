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

#include "../../globals.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QList>
#include <QtCore/QString>

class Entry;
class FileExporterXSLT;

/**
  * @brief Model to list all the serach results from the KBibTeX websearch
  *
  * Unlike most other TableModels this one does not get its data from nepomuk.
  * Instead this ListModel will be populated by the Websearch widget and each
  * KBibTeX file parsed with a simple citation style xsl file.
  *
  * @see FileExporterXSLT
  * @see HtmlDelegate
  * @see WebSearchWidget
  */
class SearchResultModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SearchResultModel(QObject *parent = 0);
    ~SearchResultModel();

    // implemented from QAbstractTableModel
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void addEntry(Entry *e);
    Entry *entryAt(int row);
    void clear();

private:
    QString translateEntryType(const QString & type) const;
    QList<Entry *> m_searchResults;
    FileExporterXSLT *exporterXSLT;
};

#endif // SEARCHRESULTMODEL_H
