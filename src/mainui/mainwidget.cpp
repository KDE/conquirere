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

#include "mainwidget.h"
#include "projecttreewidget.h"
#include "../core/library.h"
#include "../core/nepomukmodel.h"

#include <Nepomuk/Resource>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>
#include <KDE/KAction>
#include <KConfig>
#include <KConfigGroup>

#include <QHBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QMenu>
#include <QDesktopServices>

#include <QDebug>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , m_documentView(0)
{
    setupWidget();
}

MainWidget::~MainWidget()
{
    delete m_documentView;
}

void MainWidget::switchView(ResourceSelection selection, Library *p)
{
    m_selection = selection;

    if(m_documentView->selectionModel()) {
        disconnect(m_documentView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(selectedResource(QModelIndex,QModelIndex)));
    }

    m_documentView->setModel(p->viewModel(selection));

    //load settings for visible/hidden columns
    KConfig config;
    QString group = QLatin1String("TableView");
    group.append((int)m_selection);
    KConfigGroup tableViewGroup( &config, group );

    QHeaderView *hv = m_documentView->horizontalHeader();
    int columnCount =m_documentView->model()->columnCount();
    for(int i=0; i < columnCount; i++) {
        bool hidden = tableViewGroup.readEntry( QString::number(i), false );
        hv->setSectionHidden(i, hidden);
    }

    hv->setResizeMode(QHeaderView::Interactive);
    switch(m_selection) {
    case Resource_Library:
        break;
    case Resource_Document:
        hv->setResizeMode(4, QHeaderView::Stretch);
        m_documentView->horizontalHeader()->resizeSection(0,25);
        m_documentView->horizontalHeader()->resizeSection(1,25);
        break;
    case Resource_Mail:
        break;
    case Resource_Media:
        break;
    case Resource_Publication:
    case Resource_Reference:
        hv->setResizeMode(4, QHeaderView::Stretch);
        m_documentView->horizontalHeader()->resizeSection(0,25);
        m_documentView->horizontalHeader()->resizeSection(1,25);
        break;
    case Resource_Website:
        hv->setResizeMode(0, QHeaderView::Stretch);
        break;
    case Resource_Note:
        hv->setResizeMode(0, QHeaderView::Stretch);
        break;
    }

    connect(m_documentView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(selectedResource(QModelIndex,QModelIndex)));

    m_documentView->selectRow(0);
}

void MainWidget::selectedResource( const QModelIndex & current, const QModelIndex & previous )
{
    Q_UNUSED(previous);

    NepomukModel *rm = qobject_cast<NepomukModel *>(m_documentView->model());
    Nepomuk::Resource nr = rm->documentResource(current);

    emit selectedResource(nr);
}

void MainWidget::removeSelected()
{
    QItemSelectionModel *sm = m_documentView->selectionModel();
    QModelIndexList indexes = sm->selectedRows();

    NepomukModel *rm = qobject_cast<NepomukModel *>(m_documentView->model());
    rm->removeSelected(indexes);
}

void MainWidget::openSelected()
{
    QItemSelectionModel *sm = m_documentView->selectionModel();
    QModelIndexList indexes = sm->selectedRows();

    NepomukModel *rm = qobject_cast<NepomukModel *>(m_documentView->model());
    Nepomuk::Resource nr = rm->documentResource(indexes.first());

    QUrl file = nr.property(Nepomuk::Vocabulary::NIE::url()).toUrl();

    QDesktopServices::openUrl(file);
}

void MainWidget::exportSelectedToBibTeX()
{
    QItemSelectionModel *sm = m_documentView->selectionModel();
    QModelIndexList indexes = sm->selectedRows();
}

void MainWidget::tableContextMenu(const QPoint & pos)
{

    QMenu menu(this);
    menu.addAction(m_openExternal);
    menu.addSeparator();
    //menu.addAction(m_exportToBibTeX);
    menu.addAction(m_removeFromProject);
    menu.exec(mapToGlobal(pos));
}

void MainWidget::headerContextMenu(const QPoint &pos)
{
    QMenu menu(this);

    int columnCount =m_documentView->model()->columnCount();

    //iterate through all available header entries in the current model
    for(int i=0; i < columnCount; i++) {
        QString headerName = m_documentView->model()->headerData(i,Qt::Horizontal).toString();
        if(headerName.isEmpty()) {
            headerName = m_documentView->model()->headerData(i,Qt::Horizontal, Qt::ToolTipRole).toString();
        }

        // for each header create a checkable menu entry to change visibility
        QAction *a = menu.addAction(headerName);
        connect(a, SIGNAL(triggered()), this, SLOT(changeHeaderSectionVisibility()));
        a->setData(i);
        a->setCheckable(true);
        a->setChecked(!m_documentView->horizontalHeader()->isSectionHidden(i));
    }

    menu.exec(mapToGlobal(pos));
}

void MainWidget::changeHeaderSectionVisibility()
{
    QAction *a = qobject_cast<QAction *>(sender());

    QHeaderView *hv = m_documentView->horizontalHeader();
    hv->setSectionHidden(a->data().toInt(),
                         !hv->isSectionHidden(a->data().toInt()));

    // save selection in the application settings
    KConfig config;
    QString group = QLatin1String("TableView");
    group.append((int)m_selection);
    KConfigGroup tableViewGroup( &config, group );
    tableViewGroup.writeEntry( a->data().toString(), hv->isSectionHidden(a->data().toInt()) );
    tableViewGroup.config()->sync();
}

void MainWidget::setupWidget()
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

    QHeaderView *hv = m_documentView->horizontalHeader();
    hv->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(hv, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(headerContextMenu(const QPoint &)));

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
