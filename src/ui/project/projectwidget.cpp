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

    // fetch information about system wide files
    m_systemDocumentModel = new ResourceModel;
    m_systemDocumentModel->setResourceType(Resource_Document);
    connect(m_systemDocumentModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    m_systemDocumentModel->startFetchData();

    m_systemMailModel = new ResourceModel;
    m_systemMailModel->setResourceType(Resource_Mail);
    connect(m_systemMailModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    m_systemMailModel->startFetchData();

    m_systemMediaModel = new ResourceModel;
    m_systemMediaModel->setResourceType(Resource_Media);
    connect(m_systemMediaModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    m_systemMediaModel->startFetchData();

    m_systemWebsiteModel = new ResourceModel;
    m_systemWebsiteModel->setResourceType(Resource_Website);
    connect(m_systemWebsiteModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    m_systemWebsiteModel->startFetchData();

}

ProjectWidget::~ProjectWidget()
{
    delete m_project;
    delete m_projectTree;
    delete m_projectDocumentModel;
    delete m_projectMailModel;
    delete m_projectMediaModel;
    delete m_projectWebsiteModel;
    delete m_documentView;
    delete m_documentInfo;
}

void ProjectWidget::setProject(Project *p)
{
    m_project = p;

    m_projectTree->setProject(m_project);

    m_projectDocumentModel = new ResourceModel;
    m_projectDocumentModel->setProject(m_project);
    m_projectDocumentModel->setResourceType(Resource_Document);
    connect(m_projectDocumentModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    m_projectDocumentModel->startFetchData();

    m_projectMailModel = new ResourceModel;
    m_projectMailModel->setProject(m_project);
    m_projectMailModel->setResourceType(Resource_Mail);
    connect(m_projectMailModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    m_projectMailModel->startFetchData();

    m_projectMediaModel = new ResourceModel;
    m_projectMediaModel->setProject(m_project);
    m_projectMediaModel->setResourceType(Resource_Media);
    connect(m_projectMediaModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    m_projectMediaModel->startFetchData();

    m_projectWebsiteModel = new ResourceModel;
    m_projectWebsiteModel->setProject(m_project);
    m_projectWebsiteModel->setResourceType(Resource_Website);
    connect(m_projectWebsiteModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    m_projectWebsiteModel->startFetchData();
}

Project *ProjectWidget::project() const
{
    return m_project;
}

void ProjectWidget::switchView(LibraryType library, ResourceSelection selection)
{
    disconnect(m_documentView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(selectedResource(QModelIndex,QModelIndex)));

    if(library == Library_Project) {
        switch(selection) {
        case Resource_Document:
        {
            m_documentView->setModel(m_projectDocumentModel);
            m_documentInfo->clear();
            break;
        }
        case Resource_Mail:
            m_documentView->setModel(m_projectMailModel);
            m_documentInfo->clear();
            break;
        case Resource_Media:
            m_documentView->setModel(m_projectMediaModel);
            m_documentInfo->clear();
            break;
        case Resource_Website:
            m_documentView->setModel(m_projectWebsiteModel);
            m_documentInfo->clear();
            break;
            //    case References:
            //    case Notes:
        }
    }
    else {
        switch(selection) {
        case Resource_Document:
        {
            m_documentView->setModel(m_systemDocumentModel);
            m_documentInfo->clear();
            break;
        }
        case Resource_Mail:
            m_documentView->setModel(m_systemMailModel);
            m_documentInfo->clear();
            break;
        case Resource_Media:
            m_documentView->setModel(m_systemMediaModel);
            m_documentInfo->clear();
            break;
        case Resource_Website:
            m_documentView->setModel(m_systemWebsiteModel);
            m_documentInfo->clear();
            break;
            //    case References:
            //    case Notes:
        }

    }
    //shrink first 2 colums (review icon / document in project path icon)
    QHeaderView *qhv = m_documentView->horizontalHeader();
    qhv->resizeSection(0,25);
    qhv->resizeSection(1,25);

    connect(m_documentView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(selectedResource(QModelIndex,QModelIndex)));

    m_documentView->selectRow(0);
}

void ProjectWidget::selectedResource( const QModelIndex & current, const QModelIndex & previous )
{
    Q_UNUSED(previous);

    ResourceModel *rm = qobject_cast<ResourceModel *>(m_documentView->model());
    Nepomuk::Resource nr = rm->documentResource(current);

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
    connect(m_projectTree, SIGNAL(newSelection(LibraryType,ResourceSelection)), this, SLOT(switchView(LibraryType,ResourceSelection)));

    // view that holds the table models for selection
    m_documentView = new QTableView;
    m_documentView->setSortingEnabled(true);
    m_documentView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_documentView->horizontalHeader()->setStretchLastSection(true);
    m_documentView->verticalHeader()->hide();
    m_documentView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_documentView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_documentView->setContextMenuPolicy(Qt::CustomContextMenu);

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
