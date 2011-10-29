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
#include "ui_sidebarwidget.h"

#include "publicationwidget.h"
#include "sidebarcomponent.h"
#include "referencewidget.h"
#include "documentwidget.h"
#include "notewidget.h"
#include "mergeresourceswidget.h"

#include <KGlobalSettings>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QDebug>

SidebarWidget::SidebarWidget(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::DockWidget)
    , m_currentWidget(0)
{
    ui->setupUi(this);

    m_stackedLayout = new QStackedLayout;
    ui->contentWidget->setLayout(m_stackedLayout);

    m_blankPage = new QWidget;
    m_stackedLayout->addWidget(m_blankPage);
    m_mergeWidget = new MergeResourcesWidget;
    m_stackedLayout->addWidget(m_mergeWidget);

    setFont(KGlobalSettings::smallestReadableFont());

    ui->newButton->setIcon(KIcon(QLatin1String("document-new")));
    ui->deleteButton->setIcon(KIcon(QLatin1String("document-close")));

    ui->newButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
}

SidebarWidget::~SidebarWidget()
{
    delete m_mergeWidget;
    delete m_blankPage;
    delete m_currentWidget;
    delete m_stackedLayout;
}

void SidebarWidget::setResource(Nepomuk::Resource & resource)
{
    if(m_currentWidget) {
        m_currentWidget->setResource(resource);

        ui->newButton->setEnabled(true);

        if(resource.isValid()) {
            ui->deleteButton->setEnabled(true);
            m_stackedLayout->setCurrentWidget(m_currentWidget);
        }
        else {
            ui->deleteButton->setEnabled(false);
            m_stackedLayout->setCurrentWidget(m_blankPage);
        }
    }
}


void SidebarWidget::setMultipleResources(QList<Nepomuk::Resource> resourcelist)
{
    qDebug() << "set multiple resources";
    m_stackedLayout->setCurrentWidget(m_mergeWidget);
    m_mergeWidget->setResources(resourcelist);
}

void SidebarWidget::newButtonClicked()
{
    if(m_currentWidget) {
        m_currentWidget->newButtonClicked();
    }
}

void SidebarWidget::deleteButtonClicked()
{
    if(m_currentWidget) {
        m_currentWidget->deleteButtonClicked();
    }
}

void SidebarWidget::newSelection(ResourceSelection selection, ResourceFilter filter, Library *library)
{
    if(m_curSelection == selection)
        return;

    m_curSelection = selection;

    SidebarComponent *newWidget = 0;

    switch(selection) {
    case Resource_Library:
        ui->titleLabel->setText(QLatin1String(""));
        break;
    case Resource_Document:
        newWidget = new DocumentWidget();
        ui->titleLabel->setText(i18n("Document"));
        break;
    case Resource_Mail:
        newWidget = new PublicationWidget();
        ui->titleLabel->setText(i18n("Mail"));
        break;
    case Resource_Media:
        newWidget = new PublicationWidget();
        ui->titleLabel->setText(i18n("Media"));
        break;
    case Resource_Reference:
        newWidget = new ReferenceWidget();
        ui->titleLabel->setText(i18n("Reference"));
        break;
    case Resource_Website:
        newWidget = new PublicationWidget();
        ui->titleLabel->setText(i18n("Website"));
        break;
    case Resource_Note:
        newWidget = new NoteWidget();
        ui->titleLabel->setText(i18n("Note"));
        break;
    case Resource_Publication:
        newWidget = new PublicationWidget();
        ui->titleLabel->setText(i18n("Publication"));
        break;
    }

    if(newWidget) {
        newWidget->setLibrary(library);
        m_stackedLayout->addWidget(newWidget);
        m_stackedLayout->setCurrentWidget(newWidget);
        ui->newButton->setEnabled(true);
        ui->deleteButton->setEnabled(false);
    }
    else {
        m_stackedLayout->setCurrentWidget(m_blankPage);
        ui->newButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
    }

    if(selection == Resource_Document) {
        ui->newButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
    }

    // remove old widget
    m_stackedLayout->removeWidget(m_currentWidget);
    delete m_currentWidget;
    m_currentWidget = newWidget;
}

void SidebarWidget::clear()
{
    Nepomuk::Resource empty;
    setResource(empty);
}
