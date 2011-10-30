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

#include <KDE/KGlobalSettings>
#include <KDE/KStandardDirs>

#include <QtGui/QFont>
#include <QtCore/QBuffer>

enum BibTeXColumnList {
    Column_Reference,

    Max_columns
};

SearchResultModel::SearchResultModel(QObject *parent) :
    QAbstractListModel(parent)
{
    exporterXSLT = new FileExporterXSLT();
    exporterXSLT->setXSLTFilename(KStandardDirs::locate("data", QLatin1String("conquiere/simple.xsl")));
}

SearchResultModel::~SearchResultModel()
{
    qDeleteAll(m_searchResults);
}

int SearchResultModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_searchResults.size();
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

    if (index.row() >= m_searchResults.size() || index.row() < 0)
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    Entry *entry = m_searchResults.at(index.row());

    QStringList errorLog;
    QBuffer buffer;

    buffer.open(QBuffer::WriteOnly);
    exporterXSLT->save(&buffer, entry, &errorLog);
    buffer.close();

    buffer.open(QBuffer::ReadOnly);
    QTextStream ts(&buffer);
    QString text = ts.readAll();
    buffer.close();

    text.prepend(QLatin1String("</b></font><br/>"));
    text.prepend(translateEntryType(entry->type()));
    text.prepend(QLatin1String("<font color=\"#000099\"><b>"));
    text.remove(QLatin1String("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
    text.remove(QLatin1String("\n"));
    text.append(QLatin1String("<hr width=\"100%\">"));

    return text;
}

QVariant SearchResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
            return i18n("Reference");
    }

    return QVariant();
}

void SearchResultModel::addEntry(Entry *e)
{
    beginInsertRows(QModelIndex(), m_searchResults.size(), m_searchResults.size());
    m_searchResults.append(e);
    endInsertRows();
}

Entry *SearchResultModel::entryAt(int row)
{
    return m_searchResults.at(row);
}

void SearchResultModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_searchResults.size()-1);
    qDeleteAll(m_searchResults);
    m_searchResults.clear();
    endRemoveRows();
}

QString SearchResultModel::translateEntryType(const QString & type) const
{
    if(type == QLatin1String("article")) {
        return BibEntryTypeTranslation.at(BibType_Article);
    }
    else if(type == QLatin1String("book")) {
        return BibEntryTypeTranslation.at(BibType_Book);
    }
    else if(type == QLatin1String("booklet")) {
        return BibEntryTypeTranslation.at(BibType_Booklet);
    }
    else if(type == QLatin1String("conference")) {
        return BibEntryTypeTranslation.at(BibType_Proceedings);
    }
    else if(type == QLatin1String("inbook")) {
        return BibEntryTypeTranslation.at(BibType_Book);
    }
    else if(type == QLatin1String("incollection")) {
        return BibEntryTypeTranslation.at(BibType_Collection);
    }
    else if(type == QLatin1String("inproceedings")) {
        return BibEntryTypeTranslation.at(BibType_Proceedings);
    }
    else if(type == QLatin1String("mastersthesis")) {
        return BibEntryTypeTranslation.at(BibType_Mastersthesis);
    }
    else if(type == QLatin1String("misc")) {
        return BibEntryTypeTranslation.at(BibType_Misc);
    }
    else if(type == QLatin1String("phdthesis")) {
        return BibEntryTypeTranslation.at(BibType_Phdthesis);
    }
    else if(type == QLatin1String("proceedings")) {
        return BibEntryTypeTranslation.at(BibType_Proceedings);
    }
    else if(type == QLatin1String("techreport")) {
        return BibEntryTypeTranslation.at(BibType_Techreport);
    }
    else if(type == QLatin1String("unpublished")) {
        return BibEntryTypeTranslation.at(BibType_Unpublished);
    }
    else if(type == QLatin1String("electronic")) {
        return BibEntryTypeTranslation.at(BibType_Electronic);
    }

    return type;
}
