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

#include "mailmodel.h"

#include "../queryclients/mailquery.h"
#include "nbibio/conquirere.h"

#include <QtCore/QModelIndex>

MailModel::MailModel(QObject *parent)
    : NepomukModel(parent)
{
    m_queryClient = new MailQuery();

    connect(m_queryClient, SIGNAL(newCacheEntries(QList<CachedRowEntry>)), this, SLOT(addCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(updateCacheEntries(QList<CachedRowEntry>)), this, SLOT(updateCacheData(QList<CachedRowEntry>)));
    connect(m_queryClient, SIGNAL(removeCacheEntries(QList<QUrl>)), this, SLOT(removeCacheData(QList<QUrl>)));

    connect(m_queryClient, SIGNAL(queryFinished()), this, SIGNAL(queryFinished()));
}

MailModel::~MailModel()
{
    if(ConqSettings::cacheOnStartUp()) {
        saveCache();
    }
}

int MailModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return MailQuery::Max_columns;
}

QVariant MailModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case MailQuery::Column_Tags:
            return i18nc("Header for the tags column","Tags");
        case MailQuery::Column_Title:
            return i18nc("Header for the title column","Title");
        case MailQuery::Column_Date:
            return i18nc("Header for the mail received from section column","Received");
        case MailQuery::Column_From:
            return i18nc("Header for the mail from section column","From");
        case MailQuery::Column_StarRate:
            return i18nc("Header for the rating column","Rating");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case MailQuery::Column_Tags:
            return i18nc("Tooltip for the tags column","Tags");
        case MailQuery::Column_Title:
            return i18nc("Tooltip for the title column","Title");
        case MailQuery::Column_Date:
            return i18nc("Tooltip for the mail received from section column","Received");
        case MailQuery::Column_From:
            return i18nc("Tooltip for the mail from section column","From");
        case MailQuery::Column_StarRate:
            return i18nc("Tooltip for the rating column","Rating");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

int MailModel::defaultSectionSize(int i) const
{
    switch (i) {
    case MailQuery::Column_Tags:
        return 100;
    case MailQuery::Column_Title:
        return 300;
    case MailQuery::Column_Date:
        return 50;
    case MailQuery::Column_From:
        return 50;
    case MailQuery::Column_StarRate:
        return 75;
    }

    return 100;
}

QList<int> MailModel::fixedWithSections() const
{
    QList<int> fixedWith;
    fixedWith << MailQuery::Column_StarRate;

    return fixedWith;
}

QString MailModel::id()
{
    return QLatin1String("mails");
}
