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

#include "projectwidget.h"
#include "projecttreewidget.h"
#include "mainwindow.h"
#include "../../core/project.h"
#include "../documents/documentinfowidget.h"
#include "../project/resourcemodel.h"

#include <Nepomuk/Resource>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>
#include <KDE/KAction>

#include <QHBoxLayout>
#include <QSplitter>
#include <QTableView>
#include <QHeaderView>
#include <QMenu>
#include <QProcess>
#include <QDesktopServices>

#include <QDebug>

ProjectWidget::ProjectWidget(QWidget *parent)
    : QWidget(parent)
    , m_documentView(0)
    , m_documentInfo(0)
{
    setupWidget();
}

ProjectWidget::~ProjectWidget()
{
    delete m_project;
    delete m_projectTree;
    delete m_documentModel;
    delete m_mailModel;
    delete m_mediaModel;
    delete m_websiteModel;
    delete m_documentView;
    delete m_documentInfo;
}

void ProjectWidget::setProject(Project *p)
{
    m_project = p;
    m_documentModel = new ResourceModel;
    m_documentModel->setProjectTag(m_project->projectTag());
    m_documentModel->setProject(m_project);
    m_documentModel->setResourceType(Resource_Document);
    m_mailModel = new ResourceModel;
    m_mailModel->setProjectTag(m_project->projectTag());
    m_mailModel->setProject(m_project);
    m_mailModel->setResourceType(Resource_Mail);
    m_mediaModel = new ResourceModel;
    m_mediaModel->setProjectTag(m_project->projectTag());
    m_mediaModel->setProject(m_project);
    m_mediaModel->setResourceType(Resource_Media);
    m_websiteModel = new ResourceModel;
    m_websiteModel->setProjectTag(m_project->projectTag());
    m_websiteModel->setProject(m_project);
    m_websiteModel->setResourceType(Resource_Website);
}

Project *ProjectWidget::project() const
{
    return m_project;
}

void ProjectWidget::switchView(ResourceSelection selection)
{
    switch(selection) {
    case Resource_Document:
    {
        m_documentView->setModel(m_documentModel);
        QHeaderView *qhv = m_documentView->horizontalHeader();
        qhv->resizeSection(0,25);
        qhv->resizeSection(1,25);
        m_documentInfo->clear();
        break;
    }
    case Resource_Mail:
        m_documentView->setModel(m_mailModel);
        m_documentInfo->clear();
        break;
    case Resource_Media:
        m_documentView->setModel(m_mediaModel);
        m_documentInfo->clear();
        break;
    case Resource_Website:
        m_documentView->setModel(m_websiteModel);
        m_documentInfo->clear();
        break;
        //    case References:
        //    case Notes:
    }
}

void ProjectWidget::selectedResource( const QModelIndex & index )
{
    ResourceModel *rm = qobject_cast<ResourceModel *>(m_documentView->model());
    Nepomuk::Resource nr = rm->documentResource(index);

    m_documentInfo->setResource(nr);
}

void ProjectWidget::removeSelected()
{
    QItemSelectionModel *sm = m_documentView->selectionModel();
    QModelIndexList indexes = sm->selectedRows();

    ResourceModel *rm = qobject_cast<ResourceModel *>(m_documentView->model());
    rm->removeSelected(indexes);
}

void ProjectWidget::openSelected()
{
    QItemSelectionModel *sm = m_documentView->selectionModel();
    QModelIndexList indexes = sm->selectedRows();

    ResourceModel *rm = qobject_cast<ResourceModel *>(m_documentView->model());
    Nepomuk::Resource nr = rm->documentResource(indexes.first());

    QUrl file = nr.property(Nepomuk::Vocabulary::NIE::url()).toUrl();

    QDesktopServices::openUrl(file);
}

void ProjectWidget::exportSelectedToBibTeX()
{
    QItemSelectionModel *sm = m_documentView->selectionModel();
    QModelIndexList indexes = sm->selectedRows();
}

void ProjectWidget::tableContextMenu(const QPoint & pos)
{
    QMenu menu(this);
    menu.addAction(m_openExternal);
    menu.addSeparator();
    //menu.addAction(m_exportToBibTeX);
    menu.addAction(m_removeFromProject);
    menu.exec(mapToGlobal(pos));
}

void ProjectWidget::setupWidget()
{
    QSplitter *splitter = new QSplitter(this);
    // the left project bar
    m_projectTree = new ProjectTreeWidget;
    connect(m_projectTree, SIGNAL(newSelection(ResourceSelection)), this, SLOT(switchView(ResourceSelection)));

    // view that holds the table models for selection
    m_documentView = new QTableView;
    m_documentView->setSortingEnabled(true);
    m_documentView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_documentView->horizontalHeader()->setStretchLastSection(true);
    m_documentView->verticalHeader()->hide();
    m_documentView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_documentView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_documentView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_documentView, SIGNAL(activated(QModelIndex)), this, SLOT(selectedResource(QModelIndex)));
    connect(m_documentView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(tableContextMenu(const QPoint &)));

    //add panel for the document info
    m_documentInfo = new DocumentInfoWidget;

    splitter->addWidget(m_projectTree);
    splitter->addWidget(m_documentView);
    splitter->addWidget(m_documentInfo);

    QList<int> sizes;
    sizes << 150 << 500 << 200;
    splitter->setSizes(sizes);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(splitter);

    setLayout(layout);

    m_removeFromProject = new KAction(this);
    m_removeFromProject->setText(i18n("Remove from project"));
    m_removeFromProject->setIcon(KIcon(QLatin1String("document-new")));
    connect(m_removeFromProject, SIGNAL(triggered()), this, SLOT(removeSelected()));

    m_exportToBibTeX = new KAction(this);
    m_exportToBibTeX->setText(i18n("Export to BibTex"));
    m_exportToBibTeX->setIcon(KIcon(QLatin1String("document-export")));
    connect(m_exportToBibTeX, SIGNAL(triggered()), this, SLOT(exportSelectedToBibTeX()));

    m_openExternal = new KAction(this);
    m_openExternal->setText(i18n("Open external"));
    m_openExternal->setIcon(KIcon(QLatin1String("document-new")));
    connect(m_openExternal, SIGNAL(triggered()), this, SLOT(openSelected()));
}
