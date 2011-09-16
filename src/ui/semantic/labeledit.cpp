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

#include "labeledit.h"

#include <Nepomuk/Variant>

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

LabelEdit::LabelEdit(QWidget *parent) :
    QWidget(parent)
{
    m_label = new QLabel();
    m_label->setWordWrap(true);
    m_label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    m_lineEdit = new QLineEdit();
    m_lineEdit->hide();

    connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_label);
    layout->addWidget(m_lineEdit);
    setLayout(layout);

    setMouseTracking(true);

    setMaximumHeight(m_lineEdit->size().height());
}

LabelEdit::~LabelEdit()
{
    delete m_label;
    delete m_lineEdit;
}

void LabelEdit::setResource(Nepomuk::Resource & resource)
{
   m_resource = resource;

   m_label->setText( m_resource.property( m_propertyUrl ).toString() );
}

Nepomuk::Resource LabelEdit::resource()
{
    return m_resource;
}

void LabelEdit::setPropertyUrl(const QUrl & propertyUrl)
{
    m_propertyUrl = propertyUrl;
}

QUrl LabelEdit::propertyUrl()
{
    return m_propertyUrl;
}

void LabelEdit::mousePressEvent ( QMouseEvent * e )
{
    if(m_label->isVisible()) {
        m_lineEdit->setText(m_label->text());
        m_label->hide();
        m_lineEdit->show();
        m_lineEdit->setFocus();
    }
    else {
        if(m_label->text() != m_lineEdit->text()) {
            m_label->setText(m_lineEdit->text());
        }
        m_lineEdit->hide();
        m_label->show();
    }
}

void LabelEdit::enterEvent ( QEvent * event )
{
    m_label->setAutoFillBackground(true);
    m_label->setBackgroundRole(QPalette::Dark);
}

void LabelEdit::leaveEvent ( QEvent * event )
{
    m_label->setAutoFillBackground(false);
    m_label->setBackgroundRole(QPalette::NoRole);
}

void LabelEdit::editingFinished()
{
    if(m_label->text() != m_lineEdit->text()) {
        m_label->setText(m_lineEdit->text());

        m_resource.setProperty(m_propertyUrl, m_label->text());

    }
    m_lineEdit->hide();
    m_label->show();
}
