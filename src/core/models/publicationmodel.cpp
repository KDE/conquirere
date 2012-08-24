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

#include "publicationmodel.h"

#include "../queryclients/publicationquery.h"
#include "nbibio/conquirere.h"

#include <KDE/KIcon>

#include <QModelIndex>

PublicationModel::PublicationModel(QObject *parent)
    : NepomukModel(parent)
{
    m_queryClient = new PublicationQuery();

    connect(m_queryClient, SIGNAL(newCacheEntries(QList<CachedRowEntry>)), this, SLOT(addCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(updateCacheEntries(QList<CachedRowEntry>)), this, SLOT(updateCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(removeCacheEntries(QList<QUrl>)), this, SLOT(removeCacheData(QList<QUrl>)));

    connect(m_queryClient, SIGNAL(queryFinished()), this, SIGNAL(queryFinished()));
}

PublicationModel::~PublicationModel()
{
    if(ConqSettings::cacheOnStartUp()) {
        saveCache();
    }
}

int PublicationModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return PublicationQuery::Max_columns;
}

QVariant PublicationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DecorationRole) {
        switch (section) {
        case PublicationQuery::Column_Reviewed:
            return KIcon(QLatin1String("document-edit-verify"));
        case PublicationQuery::Column_FileAvailable:
            return KIcon(QLatin1String("bookmarks-organize"));
        default:
            return QVariant();
        }
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case PublicationQuery::Column_Reviewed:
            return QVariant();
        case PublicationQuery::Column_FileAvailable:
            return QVariant();
        case PublicationQuery::Column_Author:
            return i18nc("Header for the author column","Author");
        case PublicationQuery::Column_Title:
            return i18nc("Header for the title column","Title");
        case PublicationQuery::Column_Date:
            return i18nc("Header for the publication date column","Date");
        case PublicationQuery::Column_Publisher:
            return i18nc("Header for the publisher column","Publisher");
        case PublicationQuery::Column_Editor:
            return i18nc("Header for the editor column","Editor");
        case PublicationQuery::Column_CiteKey:
            return i18nc("Header for the citekeys column","Citekey");
        case PublicationQuery::Column_ResourceType:
            return i18nc("Header for the type column","Type");
        case PublicationQuery::Column_StarRate:
            return i18nc("Header for the rating column","Rating");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case PublicationQuery::Column_Reviewed:
            return i18n("Reviewed");
        case PublicationQuery::Column_FileAvailable:
            return i18n("File available");
        case PublicationQuery::Column_Author:
            return  i18n("The author of the document");
        case PublicationQuery::Column_Title:
            return i18n("The document title");
        case PublicationQuery::Column_Date:
            return i18n("The date of publishing");
        case PublicationQuery::Column_Publisher:
            return  i18n("The publisher of the document");
        case PublicationQuery::Column_Editor:
            return  i18n("The editor of the document");
        case PublicationQuery::Column_CiteKey:
            return i18n("Citekey");
        case PublicationQuery::Column_ResourceType:
            return i18n("How was this published");
        case PublicationQuery::Column_StarRate:
            return i18n("Rating");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

int PublicationModel::defaultSectionSize(int i) const
{
    switch (i) {
    case PublicationQuery::Column_Reviewed:
        return 25;
    case PublicationQuery::Column_FileAvailable:
        return 25;
    case PublicationQuery::Column_Author:
        return 100;
    case PublicationQuery::Column_Title:
        return 300;
    case PublicationQuery::Column_Date:
        return 100;
    case PublicationQuery::Column_Publisher:
        return 100;
    case PublicationQuery::Column_Editor:
        return 100;
    case PublicationQuery::Column_CiteKey:
        return 100;
    case PublicationQuery::Column_ResourceType:
        return 100;
    case PublicationQuery::Column_StarRate:
        return 75;
    }

    return 100;
}

QList<int> PublicationModel::fixedWidthSections() const
{
    QList<int> fixedWith;
    fixedWith << PublicationQuery::Column_StarRate << PublicationQuery::Column_Reviewed << PublicationQuery::Column_FileAvailable;

    return fixedWith;
}

QString PublicationModel::id()
{
    return QLatin1String("publications");
}
