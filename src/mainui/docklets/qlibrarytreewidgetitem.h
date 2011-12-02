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

#ifndef QLIBRARYTREEWIDGETITEM_H
#define QLIBRARYTREEWIDGETITEM_H

#include <QtGui/QTreeWidgetItem>
#include <QtCore/QObject>

class QLibraryTreeWidgetItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
    explicit QLibraryTreeWidgetItem();

public slots:
    void startQueryFetch();
    void stopQueryFetch();
    void updateItemCount(int items);

private slots:
    void updateFetchAnimation();

private:
    bool m_fetchingInProgress;
};

#endif // QLIBRARYTREEWIDGETITEM_H
