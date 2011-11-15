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

#include <QtCore/QModelIndex>

NoteModel::NoteModel(QObject *parent)
    : NepomukModel(parent)
{
    m_queryClient = new NoteQuery();

    connect(m_queryClient, SIGNAL(newCacheEntries(QList<CachedRowEntry>)), this, SLOT(addCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(updateCacheEntries(QList<CachedRowEntry>)), this, SLOT(updateCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(removeCacheEntries(QList<QUrl>)), this, SLOT(removeCacheData(QList<QUrl>)));

    connect(m_queryClient, SIGNAL(queryFinished()), this, SIGNAL(queryFinished()));
}

NoteModel::~NoteModel()
{
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
            return i18n("Tags");
        case NoteQuery::Column_Title:
            return i18n("Title");
        case NoteQuery::Column_Date:
            return i18n("Last Modified");
        case NoteQuery::Column_StarRate:
            return i18n("Rating");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case NoteQuery::Column_Tags:
            return i18n("Tags");
        case NoteQuery::Column_Title:
            return i18n("Title");
        case NoteQuery::Column_Date:
            return i18n("Last Modified");
        case NoteQuery::Column_StarRate:
            return i18n("Rating");
        default:
            return QVariant();
        }
    }

    return QVariant();
}
