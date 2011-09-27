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
#include "documentwidget.h"

#include <QVBoxLayout>
#include <QDebug>

SidebarWidget::SidebarWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentWidget(0)
{
    QVBoxLayout *vbl = new QVBoxLayout();
    setLayout(vbl);

}

void SidebarWidget::setResource(Nepomuk::Resource & resource)
{
    qDebug() << "set resource";
    if(m_currentWidget)
        m_currentWidget->setResource(resource);

}

void SidebarWidget::clear()
{

}

void SidebarWidget::setLibrary(Library *p)
{
    m_project = p;
}

Library *SidebarWidget::library()
{
    return m_project;
}

void SidebarWidget::newSelection(ResourceSelection selection, Library *p)
{
    layout()->removeWidget(m_currentWidget);

    delete m_currentWidget;

    switch(selection) {
    case Resource_Library:
        //TODO do something when the library header is clicked (show welcome page? statistic page?)
        m_currentWidget = new PublicationWidget();
        break;
    case Resource_Document:
        m_currentWidget = new DocumentWidget();
        break;
    case Resource_Mail:
        m_currentWidget = new PublicationWidget();
        break;
    case Resource_Media:
        m_currentWidget = new PublicationWidget();
        break;
    case Resource_Reference:
        m_currentWidget = new ReferenceWidget();
        break;
    case Resource_Website:
        m_currentWidget = new PublicationWidget();
        break;
    case Resource_Note:
        m_currentWidget = new PublicationWidget();
        break;
    case Resource_Publication:
        m_currentWidget = new PublicationWidget();
        break;
    }

    m_currentWidget->setLibrary(p);
    layout()->addWidget(m_currentWidget);
}
