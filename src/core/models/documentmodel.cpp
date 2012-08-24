/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "documentmodel.h"

#include "../queryclients/documentquery.h"
#include "nbibio/conquirere.h"

#include <KDE/KIcon>

#include <QtCore/QModelIndex>

DocumentModel::DocumentModel(QObject *parent)
    : NepomukModel(parent)
{
    m_queryClient = new DocumentQuery();

    connect(m_queryClient, SIGNAL(newCacheEntries(QList<CachedRowEntry>)), this, SLOT(addCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(updateCacheEntries(QList<CachedRowEntry>)), this, SLOT(updateCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(removeCacheEntries(QList<QUrl>)), this, SLOT(removeCacheData(QList<QUrl>)));

    connect(m_queryClient, SIGNAL(queryFinished()), this, SIGNAL(queryFinished()));
}

DocumentModel::~DocumentModel()
{
    if(ConqSettings::cacheOnStartUp()) {
        saveCache();
    }
}

int DocumentModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return DocumentQuery::Max_columns;
}

QVariant DocumentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DecorationRole) {
        switch (section) {
        case DocumentQuery::Column_Reviewed:
            return KIcon(QLatin1String("document-edit-verify"));
        case DocumentQuery::Column_Publication:
            return KIcon(QLatin1String("bookmarks-organize"));
        default:
            return QVariant();
        }
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case DocumentQuery::Column_Reviewed:
            return QVariant();
        case DocumentQuery::Column_Publication:
            return QVariant();
        case DocumentQuery::Column_Title:
            return i18nc("Header for the title column","Title");
        case DocumentQuery::Column_Date:
            return i18nc("Header for the creation date column","Date");
        case DocumentQuery::Column_FileName:
            return i18nc("Header for the filename column","Filename");
        case DocumentQuery::Column_Folder:
            return i18nc("Header for the folder column","Folder");
        case DocumentQuery::Column_StarRate:
            return i18nc("Header for the rating column","Rating");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case DocumentQuery::Column_Reviewed:
            return i18nc("ToolTip for the is reviewed column", "Reviewed");
        case DocumentQuery::Column_Publication:
            return i18nc("ToolTip for the publication is available for this file column", "Publication available");
        case DocumentQuery::Column_Title:
            return i18nc("ToolTip for the document title column", "The document title");
        case DocumentQuery::Column_Date:
            return i18nc("ToolTip for the file creation column", "The date of publishing or file creation");
        case DocumentQuery::Column_FileName:
            return i18nc("ToolTip for the file name column", "The name of the file");
        case DocumentQuery::Column_Folder:
            return i18nc("ToolTip for the file path column", "The foldername of the file");
        case DocumentQuery::Column_StarRate:
            return i18nc("ToolTip for the rating column", "Rating");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

int DocumentModel::defaultSectionSize(int i) const
{
    switch (i) {
    case DocumentQuery::Column_Reviewed:
        return 25;
    case DocumentQuery::Column_Publication:
        return 25;
    case DocumentQuery::Column_Title:
        return 300;
    case DocumentQuery::Column_Date:
        return 100;
    case DocumentQuery::Column_FileName:
        return 200;
    case DocumentQuery::Column_Folder:
        return 200;
    case DocumentQuery::Column_StarRate:
        return 75;
    }

    return 100;
}

QList<int> DocumentModel::fixedWidthSections() const
{
    QList<int> fixedWith;
    fixedWith << DocumentQuery::Column_Reviewed << DocumentQuery::Column_StarRate << DocumentQuery::Column_Publication;

    return fixedWith;
}

QString DocumentModel::id()
{
    return QLatin1String("documents");
}
