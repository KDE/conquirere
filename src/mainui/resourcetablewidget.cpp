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

#include "resourcetablewidget.h"
#include "projecttreewidget.h"
#include "../core/library.h"
#include "../core/nepomukmodel.h"
#include "../core/publicationfiltermodel.h"
#include "../core/ratingdelegate.h"

#include <Nepomuk/Resource>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <KDE/KWidgetItemDelegate>
#include <KDE/KRatingWidget>
#include <KDE/KAction>
#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KLineEdit>
#include <KDE/KComboBox>

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QTableView>
#include <QtGui/QHeaderView>
#include <QtGui/QMenu>
#include <QtGui/QDesktopServices>
#include <QtGui/QSortFilterProxyModel>

#include <QtCore/QDebug>

ResourceTableWidget::ResourceTableWidget(QWidget *parent)
    : QWidget(parent)
    , m_documentView(0)
{
    setupWidget();
}

ResourceTableWidget::~ResourceTableWidget()
{
    delete m_documentView;
}

void ResourceTableWidget::switchView(ResourceSelection selection, ResourceFilter filter, Library *p)
{
    m_selection = selection;
    m_curLibrary = p;

    PublicationFilterModel * pfm = qobject_cast<PublicationFilterModel *>(p->viewModel(selection));
    if(pfm) {
        pfm->setResourceFilter(filter);
    }

    // if we only need to change the filter, forget the rest after this check
    QAbstractItemModel *oldModel = m_documentView->model();
    QAbstractItemModel *newModel = p->viewModel(selection);
    if(oldModel == newModel)
        return;

    if(m_documentView->selectionModel()) {
        disconnect(m_documentView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                this, SLOT(selectedResource(QItemSelection,QItemSelection)));
    }

    m_documentView->setModel(p->viewModel(selection));

    //load settings for visible/hidden columns
    KConfig config;
    QString group = QLatin1String("TableView");
    group.append((int)m_selection);
    KConfigGroup tableViewGroup( &config, group );

    // go through all header elements and apply last known visibility status
    // also add each header name to the search combobox for selection
    QString curSearchSelection =  m_searchSelection->currentText();
    m_searchSelection->clear();
    m_searchSelection->addItem(i18n("all entries"), -1); //additem NAME, TABLEHEADERINDEX
    QHeaderView *hv = m_documentView->horizontalHeader();
    int columnCount = m_documentView->model()->columnCount();
    for(int i=0; i < columnCount; i++) {
        bool hidden = tableViewGroup.readEntry( QString::number(i), false );
        hv->setSectionHidden(i, hidden);

        QString headerName = m_documentView->model()->headerData(i,Qt::Horizontal).toString();
        if(!headerName.isEmpty()) {
            m_searchSelection->addItem(headerName, i);
        }
    }

    //try to be clever and set the same searchElement as before, if it exist
    int lastSelection = m_searchSelection->findText(curSearchSelection);
    if(lastSelection != -1)
        m_searchSelection->setCurrentIndex(lastSelection);

    hv->setResizeMode(QHeaderView::Interactive);
    switch(m_selection) {
    case Resource_Library:
        break;
    case Resource_Document:
        hv->setResizeMode(5, QHeaderView::Stretch);
        m_documentView->horizontalHeader()->resizeSection(1,25);
        m_documentView->horizontalHeader()->resizeSection(2,25);
        m_documentView->setSelectionMode(QAbstractItemView::SingleSelection);
        break;
    case Resource_Mail:
        m_documentView->setSelectionMode(QAbstractItemView::SingleSelection);
        break;
    case Resource_Media:
        m_documentView->setSelectionMode(QAbstractItemView::SingleSelection);
        break;
    case Resource_Publication:
    case Resource_Reference:
        hv->setResizeMode(6, QHeaderView::Stretch);
        m_documentView->horizontalHeader()->resizeSection(1,25);
        m_documentView->horizontalHeader()->resizeSection(2,25);
        m_documentView->setSelectionMode(QAbstractItemView::ExtendedSelection);
        break;
    case Resource_Website:
        hv->setResizeMode(1, QHeaderView::Stretch);
        m_documentView->setSelectionMode(QAbstractItemView::SingleSelection);
        break;
    case Resource_Note:
        hv->setResizeMode(1, QHeaderView::Stretch);
        m_documentView->setSelectionMode(QAbstractItemView::SingleSelection);
        break;
    }

    connect(m_documentView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(selectedResource(QItemSelection,QItemSelection)));

    m_documentView->selectRow(0);
}

void ResourceTableWidget::selectedResource( const QItemSelection & selected, const QItemSelection & deselected )
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    QModelIndexList selectedIndex =m_documentView->selectionModel()->selectedRows();

    if(selectedIndex.isEmpty()) {
        Nepomuk::Resource empty;
        emit selectedResource(empty);
    }
    else if(selectedIndex.size() > 1) {
        QSortFilterProxyModel *sfpm = qobject_cast<QSortFilterProxyModel *>(m_documentView->model());
        NepomukModel *rm = qobject_cast<NepomukModel *>(sfpm->sourceModel());

        QList<Nepomuk::Resource> resourceList;
        foreach(const QModelIndex & mi, selectedIndex) {
            Nepomuk::Resource nr = rm->documentResource(sfpm->mapToSource(mi));
            resourceList.append(nr);
        }

        emit selectedMultipleResources(resourceList);
    }
    else {
        QSortFilterProxyModel *sfpm = qobject_cast<QSortFilterProxyModel *>(m_documentView->model());
        NepomukModel *rm = qobject_cast<NepomukModel *>(sfpm->sourceModel());

        if(rm) {
            Nepomuk::Resource nr = rm->documentResource(sfpm->mapToSource(selectedIndex.first()));
            emit selectedResource(nr);
        }
    }
}

void ResourceTableWidget::applyFilter()
{
    QString searchKey = m_searchBox->text();
    int curIndex = m_searchSelection->currentIndex();
    int searchColumn = m_searchSelection->itemData(curIndex).toInt();

    QSortFilterProxyModel *sfpm = qobject_cast<QSortFilterProxyModel *>(m_documentView->model());

    sfpm->setFilterKeyColumn(searchColumn);
    sfpm->setFilterRegExp(searchKey);
}

void ResourceTableWidget::removeSelectedFromProject()
{
    QItemSelectionModel *sm = m_documentView->selectionModel();
    QModelIndexList indexes = sm->selectedRows();

    NepomukModel *rm = qobject_cast<NepomukModel *>(m_documentView->model());
    rm->removeSelectedFromProject(indexes, m_curLibrary);
}

void ResourceTableWidget::removeSelectedFromSystem()
{
    QItemSelectionModel *sm = m_documentView->selectionModel();
    QModelIndexList indexes = sm->selectedRows();

    QSortFilterProxyModel *sfpm = qobject_cast<QSortFilterProxyModel *>(m_documentView->model());
    NepomukModel *rm = qobject_cast<NepomukModel *>(sfpm->sourceModel());
    rm->removeSelectedFromSystem(indexes);
}

void ResourceTableWidget::openSelected()
{
    QItemSelectionModel *sm = m_documentView->selectionModel();
    QModelIndexList indexes = sm->selectedRows();

    QSortFilterProxyModel *sfpm = qobject_cast<QSortFilterProxyModel *>(m_documentView->model());
    NepomukModel *rm = qobject_cast<NepomukModel *>(sfpm->sourceModel());

    if(rm && !indexes.isEmpty()) {
        Nepomuk::Resource nr = rm->documentResource(indexes.first());
        QUrl file = nr.property(Nepomuk::Vocabulary::NIE::url()).toUrl();
        QDesktopServices::openUrl(file);
    }
}

void ResourceTableWidget::exportSelectedToBibTeX()
{
    QItemSelectionModel *sm = m_documentView->selectionModel();
    QModelIndexList indexes = sm->selectedRows();
}

void ResourceTableWidget::tableContextMenu(const QPoint & pos)
{
    // no context menu for the welcome pages
    if(m_selection == Resource_Library) {
        return;
    }

    QMenu menu(this);
    menu.addAction(m_openExternal);
    menu.addSeparator();

    if(m_selection == Resource_Publication ||
       m_selection == Resource_Reference ) {
        menu.addAction(m_removeFromSystem);
    }

    if(m_curLibrary->libraryType() == Library_System) {
    }
    else {
        menu.addAction(m_removeFromProject);
    }

    menu.exec(QCursor::pos());
}

void ResourceTableWidget::headerContextMenu(const QPoint &pos)
{
    QMenu menu(this);

    int columnCount = m_documentView->model()->columnCount();

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

void ResourceTableWidget::changeHeaderSectionVisibility()
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

void ResourceTableWidget::setupWidget()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    //add searchbox
    m_searchBox = new KLineEdit( this );
    m_searchBox->setClearButtonShown(true);
    connect(m_searchBox, SIGNAL(returnPressed()), this, SLOT(applyFilter()));

    m_searchSelection = new KComboBox(this);
    connect(m_searchSelection, SIGNAL(currentIndexChanged(int)), this, SLOT(applyFilter()));

    QHBoxLayout *searchLayout = new QHBoxLayout;
    searchLayout->addWidget(m_searchBox);
    searchLayout->addWidget(m_searchSelection);
    mainLayout->addLayout(searchLayout);

    // view that holds the table models for selection
    m_documentView = new QTableView;
    m_documentView->setSortingEnabled(true);
    m_documentView->setItemDelegateForColumn(0, new RatingDelegate());
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

    mainLayout->addWidget(m_documentView);

    m_removeFromProject = new KAction(this);
    m_removeFromProject->setText(i18n("Remove from project"));
    m_removeFromProject->setIcon(KIcon(QLatin1String("document-new")));
    connect(m_removeFromProject, SIGNAL(triggered()), this, SLOT(removeSelectedFromProject()));

    m_removeFromSystem = new KAction(this);
    m_removeFromSystem->setText(i18n("Remove from system"));
    m_removeFromSystem->setIcon(KIcon(QLatin1String("document-new")));
    connect(m_removeFromSystem, SIGNAL(triggered()), this, SLOT(removeSelectedFromSystem()));

    m_exportToBibTeX = new KAction(this);
    m_exportToBibTeX->setText(i18n("Export to BibTex"));
    m_exportToBibTeX->setIcon(KIcon(QLatin1String("document-export")));
    connect(m_exportToBibTeX, SIGNAL(triggered()), this, SLOT(exportSelectedToBibTeX()));

    m_openExternal = new KAction(this);
    m_openExternal->setText(i18n("Open external"));
    m_openExternal->setIcon(KIcon(QLatin1String("document-new")));
    connect(m_openExternal, SIGNAL(triggered()), this, SLOT(openSelected()));
}
