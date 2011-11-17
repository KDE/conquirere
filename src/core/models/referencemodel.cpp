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

#include "referencemodel.h"

#include "../queryclients/referencequery.h"

#include <KDE/KIcon>

#include <QModelIndex>

ReferenceModel::ReferenceModel(QObject *parent)
    : NepomukModel(parent)
{
    m_queryClient = new ReferenceQuery();

    connect(m_queryClient, SIGNAL(newCacheEntries(QList<CachedRowEntry>)), this, SLOT(addCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(updateCacheEntries(QList<CachedRowEntry>)), this, SLOT(updateCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(removeCacheEntries(QList<QUrl>)), this, SLOT(removeCacheData(QList<QUrl>)));

    connect(m_queryClient, SIGNAL(queryFinished()), this, SIGNAL(queryFinished()));
}

ReferenceModel::~ReferenceModel()
{
    // ignored for now, new threaded loading seems to be fast enough
    //saveCache();
}

int ReferenceModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ReferenceQuery::Max_columns;
}

QVariant ReferenceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DecorationRole) {
        switch (section) {
        case ReferenceQuery::Column_Reviewed:
            return KIcon(QLatin1String("document-edit-verify"));
        case ReferenceQuery::Column_FileAvailable:
            return KIcon(QLatin1String("bookmarks-organize"));
        default:
            return QVariant();
        }
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case ReferenceQuery::Column_Reviewed:
            return QVariant();
        case ReferenceQuery::Column_FileAvailable:
            return QVariant();
        case ReferenceQuery::Column_Author:
            return i18n("Author");
        case ReferenceQuery::Column_Title:
            return i18n("Title");
        case ReferenceQuery::Column_Date:
            return i18n("Date");
        case ReferenceQuery::Column_Publisher:
            return i18n("Publisher");
        case ReferenceQuery::Column_Editor:
            return i18n("Editor");
        case ReferenceQuery::Column_CiteKey:
            return i18n("Citekey");
        case ReferenceQuery::Column_ResourceType:
            return i18n("Type");
        case ReferenceQuery::Column_StarRate:
            return i18n("Rating");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case ReferenceQuery::Column_Reviewed:
            return i18n("Reviewed");
        case ReferenceQuery::Column_FileAvailable:
            return i18n("File available");
        case ReferenceQuery::Column_Author:
            return  i18n("The author of the document");
        case ReferenceQuery::Column_Title:
            return i18n("The document title");
        case ReferenceQuery::Column_Date:
            return i18n("The date of publishing");
        case ReferenceQuery::Column_Publisher:
            return  i18n("The publisher of the document");
        case ReferenceQuery::Column_Editor:
            return  i18n("The editor of the document");
        case ReferenceQuery::Column_CiteKey:
            return i18n("Citekey");
        case ReferenceQuery::Column_ResourceType:
            return i18n("How was this published");
        case ReferenceQuery::Column_StarRate:
            return i18n("Rating");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

QString ReferenceModel::id()
{
    return QLatin1String("reference");
}
