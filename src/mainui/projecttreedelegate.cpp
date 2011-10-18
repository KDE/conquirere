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

#include "projecttreedelegate.h"

#include <QModelIndex>
#include <QIcon>
#include <QPainter>

ProjectTreeDelegate::ProjectTreeDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
{
}

void ProjectTreeDelegate::timerEvent(QTimerEvent * /*event*/)
{
    m_angle = (m_angle+30)%360;
}

void ProjectTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    // first draw background for selected entries
    QStyleOptionViewItemV4 opt = option;
    QStyledItemDelegate::initStyleOption(&opt, index);
    QRect rect = opt.rect;

    // handle selection
    if(option.state & QStyle::State_Selected){
        painter->save();

        //QBrush selectionBrush( QPalette::highlight() );
        painter->setBrush( opt.palette.color( QPalette::Highlight ).light( 150 ) );
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);

        painter->restore();
    }

    // draw fetch in prgress
    if(index.data(80).toBool()) {
        painter->save();
        int width = option.rect.height() - 1;

        int outerRadius = (width-1)*0.5;
        int innerRadius = (width-1)*0.5*0.38;

        int capsuleHeight = outerRadius - innerRadius;
        int capsuleWidth  = (width > 32 ) ? capsuleHeight *.23 : capsuleHeight *.35;
        int capsuleRadius = capsuleWidth/2;

        for (int i=0; i<12; i++)
        {
            QColor color = Qt::black;
            color.setAlphaF(1.0f - (i/12.0f));
            painter->setPen(Qt::NoPen);
            painter->setBrush(color);
            painter->save();
            //painter->translate(option.rect.center());
            painter->translate(option.rect.x() + (option.rect.height() - 1)/2,option.rect.y() + (option.rect.height() - 1)/2);
            painter->rotate(index.data(81).toInt() - i*30.0f);
            painter->drawRoundedRect(-capsuleWidth*0.5, -(innerRadius+capsuleHeight), capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
            painter->restore();
        }
        painter->restore();
    }
    else {
        QIcon icon = qVariantValue<QIcon>(index.data(Qt::DecorationRole));
        icon.paint(painter, QRect(option.rect.x(),option.rect.y(), option.rect.height()-1, option.rect.height()-1));
    }


    //afterwards draw the text above it
    painter->drawText(QRect(option.rect.x() + option.rect.height() + 5,option.rect.y(), option.rect.width()-option.rect.height() - 5, option.rect.height()),
                      index.data(Qt::DisplayRole).toString());

}

QSize ProjectTreeDelegate::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *ProjectTreeDelegate::createEditor(QWidget *parent,const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void ProjectTreeDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
    QStyledItemDelegate::setEditorData(editor, index);
}

void ProjectTreeDelegate::setModelData(QWidget *editor, QAbstractTableModel *model,
                                       const QModelIndex &index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}
