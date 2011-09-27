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
#include "core/library.h"
#include "core/resourcemodel.h"
#include "sidebar/sidebarwidget.h"

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
{
    setupWidget();
}

ProjectWidget::~ProjectWidget()
{
    delete m_documentView;
}

void ProjectWidget::switchView(ResourceSelection selection, Library *p)
{
    if(m_documentView->selectionModel()) {
        disconnect(m_documentView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(selectedResource(QModelIndex,QModelIndex)));
    }

    m_documentView->setModel(p->viewModel(selection));

    m_documentView->horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
    m_documentView->horizontalHeader()->setResizeMode(1, QHeaderView::Fixed);
    m_documentView->horizontalHeader()->setResizeMode(2, QHeaderView::Interactive);
    m_documentView->horizontalHeader()->setResizeMode(3, QHeaderView::Stretch);
    m_documentView->horizontalHeader()->setResizeMode(4, QHeaderView::Fixed);
    m_documentView->horizontalHeader()->resizeSection(0,25);
    m_documentView->horizontalHeader()->resizeSection(1,25);
    m_documentView->horizontalHeader()->resizeSection(4,100);

    connect(m_documentView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(selectedResource(QModelIndex,QModelIndex)));

    m_documentView->selectRow(0);
}

void ProjectWidget::selectedResource( const QModelIndex & current, const QModelIndex & previous )
{
    Q_UNUSED(previous);

    ResourceModel *rm = qobject_cast<ResourceModel *>(m_documentView->model());
    Nepomuk::Resource nr = rm->documentResource(current);

    emit selectedResource(nr);
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
    // view that holds the table models for selection
    m_documentView = new QTableView;
    m_documentView->setSortingEnabled(true);
    m_documentView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_documentView->horizontalHeader()->setStretchLastSection(false);
    m_documentView->verticalHeader()->hide();
    m_documentView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_documentView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_documentView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_documentView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(tableContextMenu(const QPoint &)));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_documentView);

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
