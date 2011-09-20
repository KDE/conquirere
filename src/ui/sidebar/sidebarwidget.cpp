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

#include "sidebarwidget.h"

#include "publicationwidget.h"
#include "sidebarcomponent.h"
#include "referencewidget.h"

#include <QVBoxLayout>

SidebarWidget::SidebarWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentWidget(0)
{
    QVBoxLayout *vbl = new QVBoxLayout();
    setLayout(vbl);

}

void SidebarWidget::setResource(Nepomuk::Resource & resource)
{
    if(m_currentWidget)
        m_currentWidget->setResource(resource);

}

void SidebarWidget::clear()
{

}

void SidebarWidget::newSelection(LibraryType library, ResourceSelection selection)
{
    layout()->removeWidget(m_currentWidget);

    delete m_currentWidget;

    switch(selection) {
    case Resource_Library:
        //m_currentWidget = new QWidget();
        break;
    case Resource_Document:
        m_currentWidget = new PublicationWidget();
        break;
    case Resource_Mail:
        break;
    case Resource_Media:
        break;
    case Resource_Reference:
        m_currentWidget = new ReferenceWidget();
        break;
    case Resource_Website:
        break;
    case Resource_Note:
        break;
    }

    layout()->addWidget(m_currentWidget);
}
