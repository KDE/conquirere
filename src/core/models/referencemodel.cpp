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
#include "nbibio/conquirere.h"

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

    setObjectName("reference");
}

ReferenceModel::~ReferenceModel()
{
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
            return i18nc("Header for the author","Author");
        case ReferenceQuery::Column_Title:
            return i18nc("Header for the title","Title");
        case ReferenceQuery::Column_Date:
            return i18nc("Header for the publication date","Date");
        case ReferenceQuery::Column_Publisher:
            return i18nc("Header for the publisher","Publisher");
        case ReferenceQuery::Column_Editor:
            return i18nc("Header for the editor","Editor");
        case ReferenceQuery::Column_CiteKey:
            return i18nc("Header for the citekey","Citekey");
        case ReferenceQuery::Column_ResourceType:
            return i18nc("Header for the type","Type");
        case ReferenceQuery::Column_StarRate:
            return i18nc("Header for the rating","Rating");
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

int ReferenceModel::defaultSectionSize(int i) const
{
    switch (i) {
    case ReferenceQuery::Column_Reviewed:
        return 25;
    case ReferenceQuery::Column_FileAvailable:
        return 25;
    case ReferenceQuery::Column_Author:
        return 100;
    case ReferenceQuery::Column_Title:
        return 300;
    case ReferenceQuery::Column_Date:
        return 100;
    case ReferenceQuery::Column_Publisher:
        return 100;
    case ReferenceQuery::Column_Editor:
        return 100;
    case ReferenceQuery::Column_CiteKey:
        return 100;
    case ReferenceQuery::Column_ResourceType:
        return 100;
    case ReferenceQuery::Column_StarRate:
        return 75;
    }
    return 100;
}

QList<int> ReferenceModel::fixedWidthSections() const
{
    QList<int> fixedWith;
    fixedWith << ReferenceQuery::Column_Reviewed << ReferenceQuery::Column_StarRate << ReferenceQuery::Column_FileAvailable;

    return fixedWith;
}
