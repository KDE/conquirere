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

#include "kmultiitemedit.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>

#include "ontology/nbib.h"
#include <Nepomuk2/Variant>

#include <KDE/KDatePicker>
#include <KDE/KLineEdit>
#include <KDE/KGlobalSettings>
#include <KDE/KSqueezedTextLabel>

#include <KDE/KDebug>

#include <QtGui/QMenu>
#include <QtGui/QWidgetAction>
#include <QtCore/QDateTime>
#include <QtGui/QCompleter>
#include <QtGui/QAbstractItemView>

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

    if(dateString.isEmpty()) {
        setLabelText("");
        return;
    }

    //[-]CCYY-MM-DDThh:mm:ss[Z|(+|-)hh:mm]Z
    QDateTime date = QDateTime::fromString(dateString, Qt::ISODate);
    if(date.isValid()) {
        //TODO: get date format from system locale settings
        setLabelText(date.toString("dd.MMM.yyyy"));
    }
    else {
        kWarning() << "no valid date format in the nepomuk storage!" << propertyUrl() << dateString;
    }
}

void DateEdit::updateResource(const QString & newDateText)
{
    if(newDateText.isEmpty()) {
        connect(Nepomuk2::removeProperties(QList<QUrl>() << resource().uri(), QList<QUrl>() << propertyUrl()),
                SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)));
    }
    else {
        //TODO: get date format from system locale settings
        QDateTime date = QDateTime::fromString(newDateText, "dd.MMM.yyyy");

        connect(Nepomuk2::setProperty(QList<QUrl>() << resource().uri(), propertyUrl(), QVariantList() << date.toString(Qt::ISODate)),
                SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );
    }
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

void DateEdit::mousePressEvent ( QMouseEvent * e )
{
    QDateTime date = QDateTime::fromString(m_label->fullText(), "dd.MMM.yyyy");
    QString dateEditString =date.toString(Qt::ISODate);

    if(m_label->isVisible()) {
        m_lineEdit->setText(dateEditString);
        m_label->hide();
        m_lineEdit->show();
        m_lineEdit->setFocus();
    }
    else {
        if(dateEditString != m_lineEdit->text()) {
            QDateTime date = QDateTime::fromString(m_lineEdit->text(), Qt::ISODate);
            if(date.isValid()) {
                setLabelText(date.toString("dd.MMM.yyyy"));
            }
        }
        m_lineEdit->hide();
        m_label->show();
    }

    QWidget::mousePressEvent(e);
}

void DateEdit::editingFinished()
{
    //don't switch to label view when enter is pressed for the completion
    QDateTime date = QDateTime::fromString(m_label->fullText(), "dd.MMM.yyyy");
    QString dateEditString =date.toString(Qt::ISODate);

    if(dateEditString != m_lineEdit->text()) {
        QDateTime date = QDateTime::fromString(m_lineEdit->text(), Qt::ISODate);
        QString inputString = date.toString("dd.MMM.yyyy");
        updateResource(inputString);
        setLabelText(inputString);
    }

    m_lineEdit->hide();
    m_label->show();
}

void DateEdit::editingAborted()
{
    m_lineEdit->hide();
    m_label->show();
}
