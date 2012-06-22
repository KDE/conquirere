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

#include "notemodel.h"

#include "../queryclients/notequery.h"
#include "nbibio/conquirere.h"

#include <QtCore/QModelIndex>

NoteModel::NoteModel(QObject *parent)
    : NepomukModel(parent)
{
    m_queryClient = new NoteQuery();
    m_queryClient->setModel(this);

    connect(m_queryClient, SIGNAL(newCacheEntries(QList<CachedRowEntry>)), this, SLOT(addCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(updateCacheEntries(QList<CachedRowEntry>)), this, SLOT(updateCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(removeCacheEntries(QList<QUrl>)), this, SLOT(removeCacheData(QList<QUrl>)));

    connect(m_queryClient, SIGNAL(queryFinished()), this, SIGNAL(queryFinished()));
}

NoteModel::~NoteModel()
{
    if(ConqSettings::cacheOnStartUp()) {
        saveCache();
    }
}

int NoteModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return NoteQuery::Max_columns;
}

QVariant NoteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case NoteQuery::Column_Tags:
            return i18nc("Header for the tags column","Tags");
        case NoteQuery::Column_Title:
            return i18nc("Header for the title column","Title");
        case NoteQuery::Column_Date:
            return i18nc("Header for the last modified column","Last Modified");
        case NoteQuery::Column_StarRate:
            return i18nc("Header for the rating column","Rating");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case NoteQuery::Column_Tags:
            return i18nc("Tooltip for the tags column","Tags");
        case NoteQuery::Column_Title:
            return i18nc("Tooltip for the title column","Title");
        case NoteQuery::Column_Date:
            return i18nc("Tooltip for the last modified column","Last Modified");
        case NoteQuery::Column_StarRate:
            return i18nc("Tooltip for the rating column","Rating");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

int NoteModel::defaultSectionSize(int i) const
{
    switch (i) {
    case NoteQuery::Column_Tags:
        return 200;
    case NoteQuery::Column_Title:
        return 300;
    case NoteQuery::Column_Date:
        return 100;
    case NoteQuery::Column_StarRate:
        return 75;
    }

    return 100;
}

QList<int> NoteModel::fixedWithSections() const
{
    QList<int> fixedWith;
    fixedWith << NoteQuery::Column_StarRate;

    return fixedWith;
}

QString NoteModel::id()
{
    return QLatin1String("notes");
}
