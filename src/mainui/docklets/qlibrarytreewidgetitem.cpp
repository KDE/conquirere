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

#include "qlibrarytreewidgetitem.h"

#include <QtCore/QTimer>

const int TREEFETCH = Qt::UserRole + 1000;
const int TREEANGLE = Qt::UserRole + 1001;

QLibraryTreeWidgetItem::QLibraryTreeWidgetItem()
    : QTreeWidgetItem()
    , m_fetchingInProgress(false)
{
}

void QLibraryTreeWidgetItem::startQueryFetch()
{
    m_fetchingInProgress = true;
    setData(0,TREEFETCH,m_fetchingInProgress);
    QTimer::singleShot(250,this,SLOT(updateFetchAnimation()));
}

void QLibraryTreeWidgetItem::stopQueryFetch()
{
    m_fetchingInProgress = false;
    setData(0,TREEFETCH,m_fetchingInProgress);
}

void QLibraryTreeWidgetItem::updateItemCount(int items)
{
    setText(1, QString::number(items));
}

void QLibraryTreeWidgetItem::updateFetchAnimation()
{
    int angle = data(0,TREEANGLE).toInt();
    angle = (angle+30)%360;
    setData(0,TREEANGLE,angle);

    if(m_fetchingInProgress) {
        QTimer::singleShot(250,this,SLOT(updateFetchAnimation()));
    }

}
