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

#include "dateedit.h"

#include "nbib.h"
#include <Nepomuk/Variant>

#include <KDatePicker>
#include <KGlobalSettings>

#include <QMenu>
#include <QWidgetAction>
#include <QDateTime>
#include <QLineEdit>

#include <QDebug>

DateEdit::DateEdit(QWidget *parent)
    :PropertyEdit(parent)
{
    setDirectEdit(false);
    setUseDetailDialog(true);

    m_dateMenu = new QMenu(this);
    m_datePickerAction = new QWidgetAction(this);
    m_datePicker = new KDatePicker(this);
    m_datePickerAction->setDefaultWidget(m_datePicker);
    m_dateMenu->addAction(m_datePickerAction);

    connect(m_datePicker, SIGNAL(dateChanged(QDate)), this, SLOT(dateChanged(QDate)));
}

void DateEdit::setupLabel()
{
    QString dateString = resource().property(propertyUrl()).toString();

    //[-]CCYY-MM-DDThh:mm:ss[Z|(+|-)hh:mm]
    QDateTime date = QDateTime::fromString(dateString, "yyyy-MM-ddTHH:mm:ss");
    setLabelText(date.toString("dd.MMM.yyyy"));
}

void DateEdit::updateResource(const QString & text)
{
    QDateTime date = QDateTime::fromString(text, "dd.MMM.yyyy");

    resource().setProperty(propertyUrl(),date.toString("yyyy-MM-ddTHH:mm:ss"));
}

void DateEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
}

void DateEdit::detailEditRequested()
{
    const QRect desk = KGlobalSettings::desktopGeometry(this);
    QPoint popupPoint = mapToGlobal(QPoint(0, 0));

    const int dateFrameHeight = m_lineEdit->sizeHint().height();
    if (popupPoint.y() + height() + dateFrameHeight > desk.bottom()) {
        popupPoint.setY(popupPoint.y() - dateFrameHeight);
    } else {
        popupPoint.setY(popupPoint.y() + height());
    }

    const int dateFrameWidth = m_lineEdit->sizeHint().width();
    if (popupPoint.x() + dateFrameWidth > desk.right()) {
        popupPoint.setX(desk.right() - dateFrameWidth);
    }

    if (popupPoint.x() < desk.left()) {
        popupPoint.setX(desk.left());
    }

    if (popupPoint.y() < desk.top()) {
        popupPoint.setY(desk.top());
    }

    m_dateMenu->popup(popupPoint);
}

void DateEdit::dateChanged(QDate date)
{
    setLabelText(date.toString("dd.MMM.yyyy"));
    updateResource(date.toString("dd.MMM.yyyy"));
}