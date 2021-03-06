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

#include "eventmodel.h"

#include "../queryclients/eventquery.h"
#include "config/conquirere.h"

#include <QModelIndex>

EventModel::EventModel(QObject *parent)
    : NepomukModel(parent)
{
    m_queryClient = new EventQuery();

    connect(m_queryClient, SIGNAL(newCacheEntries(QList<CachedRowEntry>)), this, SLOT(addCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(updateCacheEntries(QList<CachedRowEntry>)), this, SLOT(updateCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(removeCacheEntries(QList<QUrl>)), this, SLOT(removeCacheData(QList<QUrl>)));

    connect(m_queryClient, SIGNAL(queryFinished()), this, SIGNAL(queryFinished()));

    setObjectName("events");
}

EventModel::~EventModel()
{
}

int EventModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return EventQuery::Max_columns;
}

QVariant EventModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case EventQuery::Column_Title:
            return i18nc("Header for the title column","Title");
        case EventQuery::Column_Date:
            return i18nc("Header for the event date column","Date");
        case EventQuery::Column_Publication:
            return i18nc("Header for the publications of this event column","Publication");
        case EventQuery::Column_StarRate:
            return i18nc("Header for the rating column","Rating");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case EventQuery::Column_Title:
            return i18nc("Tooltip for the title column","The document title");
        case EventQuery::Column_Date:
            return i18nc("Tooltip for the event date column","The date of the event");
        case EventQuery::Column_Publication:
            return i18nc("Tooltip for the list of all connected publications column","The publications connected with this event");
        case EventQuery::Column_StarRate:
            return i18nc("Tooltip for the rating column","Rating");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

int EventModel::defaultSectionSize(int i) const
{
    switch (i) {
    case EventQuery::Column_Title:
        return 300;
    case EventQuery::Column_Akonadi:
        return 25;
    case EventQuery::Column_Date:
        return 100;
    case EventQuery::Column_Publication:
        return 300;
    case EventQuery::Column_StarRate:
        return 75;
    }
    return 100;
}

QList<int> EventModel::fixedWidthSections() const
{
    QList<int> fixedWith;
    fixedWith << EventQuery::Column_StarRate << EventQuery::Column_Akonadi;

    return fixedWith;
}
