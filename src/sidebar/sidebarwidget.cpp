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

#include <KGlobalSettings>
#include <QVBoxLayout>
#include <QDebug>

SidebarWidget::SidebarWidget(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::DockWidget)
    , m_currentWidget(0)
{
    ui->setupUi(this);

    QVBoxLayout *vbl = new QVBoxLayout();
    ui->contentWidget->setLayout(vbl);

    setFont(KGlobalSettings::smallestReadableFont());

    ui->newButton->setIcon(KIcon(QLatin1String("document-new")));
    ui->deleteButton->setIcon(KIcon(QLatin1String("document-close")));

    ui->newButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
}

void SidebarWidget::setResource(Nepomuk::Resource & resource)
{
    if(m_currentWidget) {
        m_currentWidget->setResource(resource);

        if(resource.isValid()) {
            ui->deleteButton->setEnabled(true);
        }
        else {
            ui->deleteButton->setEnabled(false);
        }
    }
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

void SidebarWidget::newSelection(ResourceSelection selection, Library *library)
{
    ui->contentWidget->layout()->removeWidget(m_currentWidget);

    delete m_currentWidget;
    m_currentWidget = 0;

    switch(selection) {
    case Resource_Library:
        //TODO do something when the library header is clicked (show welcome page? statistic page?)
        //m_currentWidget = new PublicationWidget();
        break;
    case Resource_Document:
        m_currentWidget = new DocumentWidget();
        ui->titleLabel->setText(i18n("Document"));
        break;
    case Resource_Mail:
        m_currentWidget = new PublicationWidget();
        ui->titleLabel->setText(i18n("Mail"));
        break;
    case Resource_Media:
        m_currentWidget = new PublicationWidget();
        ui->titleLabel->setText(i18n("Media"));
        break;
    case Resource_Reference:
        m_currentWidget = new ReferenceWidget();
        ui->titleLabel->setText(i18n("Reference"));
        break;
    case Resource_Website:
        m_currentWidget = new PublicationWidget();
        ui->titleLabel->setText(i18n("Website"));
        break;
    case Resource_Note:
        m_currentWidget = new NoteWidget();
        ui->titleLabel->setText(i18n("Note"));
        break;
    case Resource_Publication:
        m_currentWidget = new PublicationWidget();
        ui->titleLabel->setText(i18n("Publication"));
        break;
    }

    if(m_currentWidget) {
        m_currentWidget->setLibrary(library);
        ui->contentWidget->layout()->addWidget(m_currentWidget);
        ui->newButton->setEnabled(true);
        ui->deleteButton->setEnabled(false);
    }
    else {
        ui->newButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        //ui->contentWidget->layout()->addWidget(new QWidget());
    }
}
