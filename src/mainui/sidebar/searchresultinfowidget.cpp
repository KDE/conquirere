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

#include "searchresultinfowidget.h"
#include "ui_searchresultinfowidget.h"

SearchResultInfoWidget::SearchResultInfoWidget(QWidget *parent) :
    SidebarComponent(parent),
    ui(new Ui::SearchResultInfoWidget)
{
    ui->setupUi(this);
}

SearchResultInfoWidget::~SearchResultInfoWidget()
{
    delete ui;
}



Nepomuk::Resource SearchResultInfoWidget::resource()
{
    qWarning() << "this shouldn't be used .... SearchResultInfoWidget can't return a resource";
    Nepomuk::Resource empty;
    return empty;
}

void SearchResultInfoWidget::setResource(Nepomuk::Resource & resource)
{
    Q_UNUSED(resource);
}

void SearchResultInfoWidget::newButtonClicked()
{

}

void SearchResultInfoWidget::deleteButtonClicked()
{

}
