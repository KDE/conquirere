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
#include "serieswidget.h"
#include "mergeresourceswidget.h"

#include "../mainui/mainwindow.h"
#include "../core/library.h"
#include "../nbibio/pipe/nepomuktobibtexpipe.h"

/*
#include <kbibtex/findpdfui.h>
*/
#include <Nepomuk/Resource>
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/PIMO>

#include <KDE/KGlobalSettings>

#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedLayout>
#include <QtGui/QMenu>

#include <QtCore/QDebug>

SidebarWidget::SidebarWidget(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::DockWidget)
    , m_currentWidget(0)
    , m_curSelection(Resource_Library)
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

    ui->linkAddButton->setIcon(KIcon(QLatin1String("insert-link")));
    ui->linkRemoveButton->setIcon(KIcon(QLatin1String("remove-link")));
    ui->linkAddButton->setEnabled(false);
    ui->linkRemoveButton->setEnabled(false);

    ui->addPublication->setIcon(KIcon(QLatin1String("news-subscribe")));
    ui->removePublication->setIcon(KIcon(QLatin1String("news-unsubscribe")));
    ui->addPublication->setVisible(false);
    ui->removePublication->setVisible(false);
    ui->addPublication->setEnabled(false);
    ui->removePublication->setEnabled(false);

    ui->addReference->setIcon(KIcon(QLatin1String("format-indent-more")));
    ui->removeReference->setIcon(KIcon(QLatin1String("format-indent-less")));
    ui->addReference->setEnabled(true);
    ui->removeReference->setEnabled(false);
    ui->addReference->setVisible(false);
    ui->removeReference->setVisible(false);


    ui->findPdf->setIcon(KIcon(QLatin1String("application-pdf")));
    ui->findPdf->setEnabled(false);
    ui->findPdf->setVisible(false);
    ui->lineFindPdf->setVisible(false);
}

SidebarWidget::~SidebarWidget()
{
    delete ui;
    delete m_mergeWidget;
    delete m_blankPage;
    delete m_currentWidget;
    delete m_stackedLayout;
}

void SidebarWidget::setResource(Nepomuk::Resource & resource)
{
    m_curResource = resource;

    if(m_currentWidget) {
        m_currentWidget->setResource(resource);

        ui->newButton->setEnabled(true);

        if(resource.isValid()) {
            ui->deleteButton->setEnabled(true);
            m_stackedLayout->setCurrentWidget(m_currentWidget);
            ui->linkAddButton->setEnabled(true);
            ui->linkRemoveButton->setEnabled(true);
            ui->findPdf->setEnabled(true);
        }
        else {
            ui->deleteButton->setEnabled(false);
            m_stackedLayout->setCurrentWidget(m_blankPage);
            ui->linkAddButton->setEnabled(false);
            ui->linkRemoveButton->setEnabled(false);
        }
    }
}


void SidebarWidget::setMultipleResources(QList<Nepomuk::Resource> resourcelist)
{
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

void SidebarWidget::addToProject()
{
    QList<QAction*> actionCollection;
    QMenu addToProjects;

    if(m_parent->openLibraries().isEmpty()) {
        addToProjects.addAction(i18n("no open collections"));
    }
    else {
        foreach(Library *l, m_parent->openLibraries()) {
            QAction *a = new QAction(l->name(), this);
            a->setData(l->pimoLibrary().resourceUri());
            connect(a, SIGNAL(triggered(bool)),this, SLOT(addToSelectedProject()));
            addToProjects.addAction(a);
            actionCollection.append(a);
        }
    }

    addToProjects.exec(QCursor::pos());

    qDeleteAll(actionCollection);
}

void SidebarWidget::addToSelectedProject()
{
    QAction *a = qobject_cast<QAction *>(sender());

    if(!a)
        return;

    Nepomuk::Resource pimoProject = Nepomuk::Resource(a->data().toString());

    if(m_curResource.isValid()) {
        m_curResource.addProperty(Nepomuk::Vocabulary::PIMO::isRelated(), pimoProject);
        pimoProject.addProperty(Nepomuk::Vocabulary::PIMO::isRelated(), m_curResource);
    }
}

void SidebarWidget::removeFromProject()
{
    QList<QAction*> actionCollection;
    QMenu addToProjects;

    QList<Nepomuk::Resource> relatedList = m_curResource.property(Nepomuk::Vocabulary::PIMO::isRelated()).toResourceList();

    if(relatedList.isEmpty()) {
        addToProjects.addAction(i18n("not related to any project"));
    }
    else {
        foreach(const Nepomuk::Resource &r, relatedList) {
            QAction *a = new QAction(r.genericLabel(), this);
            a->setData(r.resourceUri());
            connect(a, SIGNAL(triggered(bool)),this, SLOT(removeFromSelectedProject()));
            addToProjects.addAction(a);
            actionCollection.append(a);
        }
    }

    addToProjects.exec(QCursor::pos());

    qDeleteAll(actionCollection);
}

void SidebarWidget::removeFromSelectedProject()
{
    QAction *a = qobject_cast<QAction *>(sender());

    if(!a)
        return;

    Nepomuk::Resource pimoProject = Nepomuk::Resource(a->data().toString());

    if(m_curResource.isValid()) {
        m_curResource.removeProperty(Nepomuk::Vocabulary::PIMO::isRelated(), pimoProject);
        pimoProject.removeProperty(Nepomuk::Vocabulary::PIMO::isRelated(), m_curResource);
    }
}

void SidebarWidget::findPdf()
{
    qDebug() << "find pdf";

/*
    NepomukToBibTexPipe bibtexPipe;
    QList<Nepomuk::Resource> exportList;
    exportList.append(m_curResource);
    bibtexPipe.pipeExport(exportList);
    File bibFile = bibtexPipe.bibtexFile();

    Entry *e = dynamic_cast<Entry *>(bibFile.first());
    FindPDFUI::interactiveFindPDF(*e, bibFile, this);
    */

}

void SidebarWidget::setMainWindow(MainWindow *mw)
{
    m_parent = mw;
}

void SidebarWidget::hasPublication(bool publication)
{
    ui->addPublication->setEnabled(!publication);
    ui->removePublication->setEnabled(publication);
}

void SidebarWidget::hasReference(bool reference)
{
    ui->removeReference->setEnabled(reference);
}

void SidebarWidget::newSelection(ResourceSelection selection, BibEntryType filter, Library *library)
{
    if(m_curSelection == selection)
        return;

    m_curSelection = selection;

    SidebarComponent *newWidget = 0;
    ui->addPublication->setVisible(false);
    ui->removePublication->setVisible(false);
    ui->addReference->setVisible(false);
    ui->removeReference->setVisible(false);
    ui->findPdf->setVisible(false);
    ui->lineFindPdf->setVisible(false);
    ui->newButton->setToolTip(QString());
    ui->deleteButton->setToolTip(QString());

    switch(selection) {
    case Resource_Library:
        ui->titleLabel->setText(QLatin1String(""));
        break;
    case Resource_Document:
        newWidget = new DocumentWidget(this);
        newWidget->setMainWindow(m_parent);
        ui->titleLabel->setText(i18n("Document"));
        ui->addPublication->setVisible(true);
        ui->removePublication->setVisible(true);
        connect(ui->addPublication, SIGNAL(clicked()), newWidget, SLOT(setPublication()));
        connect(ui->removePublication, SIGNAL(clicked()), newWidget, SLOT(removePublication()));
        ui->newButton->setToolTip(i18n("New document details"));
        ui->deleteButton->setToolTip(i18n("Delete document"));
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
        ui->newButton->setToolTip(i18n("New reference"));
        ui->deleteButton->setToolTip(i18n("Delete reference"));
        ui->findPdf->setVisible(true);
        ui->lineFindPdf->setVisible(true);
        break;
    case Resource_Website:
        newWidget = new PublicationWidget();
        ui->titleLabel->setText(i18n("Website"));
        break;
    case Resource_Note:
        newWidget = new NoteWidget();
        ui->titleLabel->setText(i18n("Note"));
        ui->newButton->setToolTip(i18n("New note"));
        ui->deleteButton->setToolTip(i18n("Delete note"));
        break;
    case Resource_Publication:
        newWidget = new PublicationWidget(this);
        ui->titleLabel->setText(i18n("Publication"));
        ui->addReference->setVisible(true);
        ui->removeReference->setVisible(true);
        connect(ui->addReference, SIGNAL(clicked()), newWidget, SLOT(addReference()));
        connect(ui->removeReference, SIGNAL(clicked()), newWidget, SLOT(removeReference()));
        ui->newButton->setToolTip(i18n("New publication"));
        ui->deleteButton->setToolTip(i18n("Delete publication"));
        ui->findPdf->setVisible(true);
        ui->lineFindPdf->setVisible(true);
        break;
    case Resource_Series:
        newWidget = new SeriesWidget();
        ui->titleLabel->setText(i18n("Series"));
        ui->newButton->setToolTip(i18n("New series"));
        ui->deleteButton->setToolTip(i18n("Delete series"));
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
