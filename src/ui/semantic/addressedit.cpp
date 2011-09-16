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

#include "addressedit.h"

#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NCO>

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDebug>

AddressEdit::AddressEdit(QWidget *parent) :
    QWidget(parent)
{
    m_label = new QLabel();
    m_label->setWordWrap(true);
    m_label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    m_label->setToolTip(i18n("Split several authors by semicolon ';'"));

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

AddressEdit::~AddressEdit()
{
    delete m_label;
    delete m_lineEdit;
}

void AddressEdit::setResource(Nepomuk::Resource & resource)
{
    m_resource = resource;

    Nepomuk::Resource address = m_resource.property( m_propertyUrl ).toResource();

    QString labelText = address.property( Nepomuk::Vocabulary::NCO::locality() ).toString();

    m_label->setText( labelText );
}

Nepomuk::Resource AddressEdit::resource()
{
    return m_resource;
}

void AddressEdit::setPropertyUrl(const QUrl & propertyUrl)
{
    m_propertyUrl = propertyUrl;
}

QUrl AddressEdit::propertyUrl()
{
    return m_propertyUrl;
}

void AddressEdit::mousePressEvent ( QMouseEvent * e )
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

void AddressEdit::enterEvent ( QEvent * event )
{
    m_label->setAutoFillBackground(true);
    m_label->setBackgroundRole(QPalette::Dark);
}

void AddressEdit::leaveEvent ( QEvent * event ){
    m_label->setAutoFillBackground(false);
    m_label->setBackgroundRole(QPalette::NoRole);
}

void AddressEdit::editingFinished()
{
    if(m_label->text() != m_lineEdit->text()) {
        m_label->setText(m_lineEdit->text());

        // remove all author informations
        m_resource.removeProperty( m_propertyUrl );

        // create new contact resources for each fullname seperated by a ;
        QStringList contacts = m_lineEdit->text().split(QLatin1String(";"));

        foreach(QString s, contacts) {
            Nepomuk::Resource c(s, Nepomuk::Vocabulary::NCO::PostalAddress());
            c.setProperty( Nepomuk::Vocabulary::NCO::locality() , s);
            m_resource.addProperty( m_propertyUrl, c);
        }
    }
    m_lineEdit->hide();
    m_label->show();
}
