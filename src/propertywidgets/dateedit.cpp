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

#include <KDE/KDatePicker>
#include <KDE/KLineEdit>
#include <KDE/KGlobalSettings>

#include <QtGui/QMenu>
#include <QtGui/QWidgetAction>
#include <QtCore/QDateTime>

DateEdit::DateEdit(QWidget *parent)
    : PropertyEdit(parent)
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

DateEdit::~DateEdit()
{
    delete m_dateMenu;
    delete m_datePickerAction;
    //delete m_datePicker;
}

void DateEdit::setupLabel()
{
    QString dateString = resource().property(propertyUrl()).toString();

    //[-]CCYY-MM-DDThh:mm:ss[Z|(+|-)hh:mm]
    QDateTime date = QDateTime::fromString(dateString, "yyyy-MM-ddTHH:mm:ss");
    if(date.isValid())
        setLabelText(date.toString("dd.MMM.yyyy"));
    else {
        QString year;
        QString month;
        QString day;
        QRegExp rx(QLatin1String("(\\d*)-(\\d*)-(\\d*)*"));
        if (rx.indexIn(dateString) != -1) {
            year = rx.cap(1);
            month = rx.cap(2);
            day = rx.cap(3);
        }
        if(month == QLatin1String("00"))
            setLabelText(year);
        else if(day == QLatin1String("00"))
            setLabelText(year + QLatin1String("-") + month);
        else
            setLabelText(year + QLatin1String("-") + month + QLatin1String("-") + day);
    }

}

void DateEdit::updateResource(const QString & text)
{
    QDateTime date = QDateTime::fromString(text, "dd.MMM.yyyy");

    resource().setProperty(propertyUrl(),date.toString("yyyy-MM-ddTHH:mm:ss"));
}

QList<QStandardItem*> DateEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    Q_UNUSED(entries);

    QList<QStandardItem*> result;
    return result;
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
