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

#ifndef HTMLDELEGATE_H
#define HTMLDELEGATE_H

#include <QtGui/QStyledItemDelegate>

class QLabel;

/**
  * @brief This delegate takes a HTML String from any QModelIndex and display it as richtext
  *
  * @see SearchResultModel::createDisplayData -> Column_Details
  * @see SeriesQuery::createDisplayData -> Column_Details
  * @see EventQuery::createDisplayData -> Column_Publication
  */
class HtmlDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit HtmlDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const;

    QSize sizeHint( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif // HTMLDELEGATE_H
