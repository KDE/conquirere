/*
 * Copyright (C) 2010 Stefan Burnicki (stefan.burnicki@gmx.de)
 * <http://gitorious.org/bangarang> BANGARANG MEDIA PLAYER
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

#include "ratingdelegate.h"
#include "starrating.h"

#include <QtGui/QApplication>
#include <QtGui/QPainter>

RatingDelegate::RatingDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

void RatingDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    QStyleOptionViewItemV4 opt(option);
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

    const int left = option.rect.left();
    const int top = option.rect.top();
    const int width = option.rect.width();
    const int height = option.rect.height();

    //Create base pixmap
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.translate(-option.rect.topLeft());

    //Paint rating
    int rating = index.data(Qt::DisplayRole).toInt();
    StarRating starRating = StarRating(rating, StarRating::Medium);
    starRating.setRating(rating);
    QSize ratingSize = starRating.sizeHint();
    int ratingLeft = left + 2;
    int ratingTop = top + (height - ratingSize.height())/2;
    QRect ratingRect = QRect(QPoint(ratingLeft, ratingTop), ratingSize);
    starRating.setPoint(ratingRect.topLeft());
    starRating.paint(&p);

    p.end();

    //Draw finished pixmap
    painter->drawPixmap(option.rect.topLeft(), pixmap);
}

QSize RatingDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    return StarRating::SizeHint(StarRating::Medium);
}
