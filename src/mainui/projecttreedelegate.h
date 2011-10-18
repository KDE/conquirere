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

#ifndef PROJECTTREEDELEGATE_H
#define PROJECTTREEDELEGATE_H

#include <QStyledItemDelegate>

/**
  * Delegate for an Project Tree Widget Item
  *
  * Used to show an animation during data fetching.
  * Indicates that not all data is currently available
  */
class ProjectTreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ProjectTreeDelegate(QWidget *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractTableModel *model, const QModelIndex &index) const;

protected:
    void timerEvent(QTimerEvent * event);

private:
    int m_angle;
};

#endif // PROJECTTREEDELEGATE_H
