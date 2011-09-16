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
#include "../../core/project.h"
#include "../documents/documentinfowidget.h"
#include "../project/resourcemodel.h"

#include <Nepomuk/Resource>

#include <QHBoxLayout>
#include <QSplitter>
#include <QTableView>
#include <QHeaderView>

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

    connect(m_documentView, SIGNAL(activated(QModelIndex)), this, SLOT(selectedResource(QModelIndex)));

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
}
