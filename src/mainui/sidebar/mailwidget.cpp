/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "mailwidget.h"
#include "ui_mailwidget.h"

MailWidget::MailWidget(QWidget *parent) :
    SidebarComponent(parent),
    ui(new Ui::MailWidget)
{
    ui->setupUi(this);
}

MailWidget::~MailWidget()
{
    delete ui;
}

Nepomuk2::Resource MailWidget::resource()
{
    return m_mail;
}

void MailWidget::setResource(Nepomuk2::Resource & resource)
{
    m_mail = resource;
}

void MailWidget::newButtonClicked()
{

}

void MailWidget::deleteButtonClicked()
{

}
