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

#include "tableviewmenu.h"
#include "core/librarymanager.h"

#include "core/library.h"
#include "core/projectsettings.h"

#include "core/models/nepomukmodel.h"
#include "core/models/publicationfiltermodel.h"
#include "core/models/seriesfiltermodel.h"
#include "core/models/searchresultmodel.h"

#include "core/delegates/ratingdelegate.h"
#include "core/delegates/htmldelegate.h"

#include <Nepomuk2/Resource>

#include <KDE/KWidgetItemDelegate>
#include <KDE/KRatingWidget>
#include <KDE/KAction>
#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KLineEdit>
#include <KDE/KComboBox>
#include <KDE/KMimeType>
#include <KDE/KDebug>

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QTableView>
#include <QtGui/QHeaderView>
#include <QtGui/QMenu>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QItemDelegate>

ResourceTableWidget::ResourceTableWidget(QWidget *parent)
    : QWidget(parent)
    , m_documentView(0)
    , m_searchResultModel(0)
{
    setupWidget();

    setMouseTracking(true);
}

ResourceTableWidget::~ResourceTableWidget()
{
    delete m_searchBox;
    delete m_searchSelection;
    delete m_documentView;
}

void ResourceTableWidget::setLibraryManager(LibraryManager *lm)
{
    m_libraryManager = lm;
}

void ResourceTableWidget::setSearchResultModel(SearchResultModel* srm)
{
    delete m_searchResultModel;
    m_searchResultModel = new QSortFilterProxyModel();
    m_searchResultModel->setSourceModel(srm);
}

void ResourceTableWidget::switchView(ResourceSelection selection, BibEntryType filter, Library *p)
{
    m_selection = selection;

    PublicationFilterModel * pfm = qobject_cast<PublicationFilterModel *>(p->viewModel(selection));
    if(pfm) {
        pfm->setResourceFilter(filter);
    }
    else {
        SeriesFilterModel * pfm = qobject_cast<SeriesFilterModel *>(p->viewModel(selection));
        if(pfm) {
            pfm->setResourceFilter(SeriesType(filter));
        }
    }

    // if we only need to change the filter, forget the rest after this check
    QAbstractItemModel *oldModel = m_documentView->model();
    QAbstractItemModel *newModel = p->viewModel(selection);
    if(!newModel)
        return;
    if(oldModel == newModel)
        return;

    if(m_documentView->selectionModel()) {
        disconnect(m_documentView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                this, SLOT(selectedResource(QItemSelection,QItemSelection)));
    }

    switch(m_selection) {
    case Resource_Event:
    case Resource_Series:
        m_documentView->setItemDelegateForColumn(3, m_htmlDelegate);
        break;
    default:
        m_documentView->setItemDelegateForColumn(3, NULL);
    }

    m_documentView->setWordWrap(true);
    m_documentView->setModel(p->viewModel(selection));

    //load settings for visible/hidden columns
    KConfig config;
    QString group = QLatin1String("TableView");
    group.append((int)m_selection);
    KConfigGroup tableViewGroup( &config, group );

    // go through all header elements and apply last known visibility / size status
    // also add each header name to the search combobox for selection
    QString curSearchSelection =  m_searchSelection->currentText();
    m_searchSelection->clear();
    m_searchSelection->addItem(i18n("all entries"), -1); //additem NAME, TABLEHEADERINDEX
    QHeaderView *hv = m_documentView->horizontalHeader();
    int columnCount = m_documentView->model()->columnCount();

    NepomukModel *nm = qobject_cast<NepomukModel *>(newModel);
    if(!nm) {
        QSortFilterProxyModel *qsf = qobject_cast<QSortFilterProxyModel *>(newModel);
        if(qsf) {
            nm = qobject_cast<NepomukModel *>(qsf->sourceModel());
        }
    }

    for(int i=0; i < columnCount; i++) {
        // hidden status
        QString keyHidden = QLatin1String("hidden_") + QString::number(i);
        bool hidden = tableViewGroup.readEntry( keyHidden, false );

        hv->setSectionHidden(i, hidden);

        QString headerName = m_documentView->model()->headerData(i,Qt::Horizontal).toString();
        if(!headerName.isEmpty()) {
            m_searchSelection->addItem(headerName, i);
        }

        // size status
        int sectionSize = 50;
        QString keySize = QLatin1String("size_") + QString::number(i);
        int size = tableViewGroup.readEntry( keySize, -1 );
        if(size > 4) {
            sectionSize = size;
        }
        else {
            if(nm) {
                sectionSize = nm->defaultSectionSize(i);
            }
        }

        hv->resizeSection(i,sectionSize);
    }

    //try to be clever and set the same searchElement as before, if it exist
    int lastSelection = m_searchSelection->findText(curSearchSelection);
    if(lastSelection != -1)
        m_searchSelection->setCurrentIndex(lastSelection);

    hv->setResizeMode(QHeaderView::Interactive);

    m_documentView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(m_documentView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(selectedResource(QItemSelection,QItemSelection)));

    m_documentView->selectRow(0);

    QList<int> fixedWithList = nm->fixedWidthSections();
    foreach(int section, fixedWithList) {
        hv->setResizeMode(section, QHeaderView::Fixed);
    }
}

void ResourceTableWidget::showSearchResult()
{
    QAbstractItemModel *oldModel = m_documentView->model();
    QAbstractItemModel *newModel = m_searchResultModel;
    if(oldModel == newModel)
        return;

    if(m_documentView->selectionModel()) {
        disconnect(m_documentView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
                this, SLOT(selectedResource(QItemSelection,QItemSelection)));
    }

    m_documentView->setWordWrap(true);
    m_documentView->setItemDelegateForColumn(3, m_htmlDelegate);
    m_documentView->setModel(m_searchResultModel);

    //load settings for visible/hidden columns
    KConfig config;
    QString group = QLatin1String("SearchResultView");
    KConfigGroup tableViewGroup( &config, group );

    // go through all header elements and apply last known visibility / size status
    // also add each header name to the search combobox for selection
    m_searchSelection->clear();
    m_searchSelection->addItem(i18n("all entries"), -1); //additem NAME, TABLEHEADERINDEX
    QHeaderView *hv = m_documentView->horizontalHeader();
    int columnCount = m_documentView->model()->columnCount();

    SearchResultModel *srm = qobject_cast<SearchResultModel *>(m_searchResultModel->sourceModel());

    for(int i=0; i < columnCount; i++) {
        // hidden status
        QString keyHidden = QLatin1String("hidden_") + QString::number(i);
        bool hidden = tableViewGroup.readEntry( keyHidden, false );
        hv->setSectionHidden(i, hidden);

        QString headerName = m_documentView->model()->headerData(i,Qt::Horizontal).toString();
        if(!headerName.isEmpty()) {
            m_searchSelection->addItem(headerName, i);
        }

        // size status
        int sectionSize = 50;
        QString keySize = QLatin1String("size_") + QString::number(i);
        int size = tableViewGroup.readEntry( keySize, -1 );
        if(size > 4) {
            sectionSize = size;
        }
        else {
            if(srm) {
                sectionSize = srm->defaultSectionSize(i);
            }
        }

        hv->resizeSection(i,sectionSize);
    }

    hv->setResizeMode(QHeaderView::Interactive);

    m_documentView->setSelectionMode(QAbstractItemView::ContiguousSelection);

    connect(m_documentView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(selectedResource(QItemSelection,QItemSelection)));

    m_documentView->selectRow(0);

    QList<int> fixedWithList = srm->fixedWithSections();
    foreach(int section, fixedWithList)
        hv->setResizeMode(section, QHeaderView::Fixed);
}

void ResourceTableWidget::selectedResource( const QItemSelection & selected, const QItemSelection & deselected )
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    QModelIndexList selectedIndex = m_documentView->selectionModel()->selectedRows();

    if(selectedIndex.isEmpty()) {
        Nepomuk2::Resource empty;
        emit selectedResource(empty,false);
    }
    else if(selectedIndex.size() > 1) {
        QSortFilterProxyModel *sfpm = qobject_cast<QSortFilterProxyModel *>(m_documentView->model());
        NepomukModel *rm = qobject_cast<NepomukModel *>(sfpm->sourceModel());
        if(!rm) {
            SearchResultModel *srm = qobject_cast<SearchResultModel *>(sfpm->sourceModel());
            if(srm) {
                QList<Nepomuk2::Resource> resourceList;
                foreach(const QModelIndex & mi, selectedIndex) {
                    Nepomuk2::Resource nr = srm->nepomukResourceAt(sfpm->mapToSource(mi));
                    resourceList.append(nr);
                }

                emit selectedMultipleResources(resourceList);
            }
        }
        else {
            QList<Nepomuk2::Resource> resourceList;
            foreach(const QModelIndex & mi, selectedIndex) {
                Nepomuk2::Resource nr = rm->documentResource(sfpm->mapToSource(mi));
                resourceList.append(nr);
            }

            emit selectedMultipleResources(resourceList);
        }
    }
    else {
        QSortFilterProxyModel *sfpm = qobject_cast<QSortFilterProxyModel *>(m_documentView->model());
        NepomukModel *rm = qobject_cast<NepomukModel *>(sfpm->sourceModel());

        if(rm) {
            Nepomuk2::Resource nr = rm->documentResource(sfpm->mapToSource(selectedIndex.first()));
            emit selectedResource(nr,false);
        }
        else {
            SearchResultModel *srm = qobject_cast<SearchResultModel *>(sfpm->sourceModel());
            if(srm) {
                Nepomuk2::Resource nr = srm->nepomukResourceAt(sfpm->mapToSource(selectedIndex.first()));
                if(nr.isValid()) {
                    emit selectedResource(nr,false);
                }
                else {
                    SearchResultModel::SRCachedRowEntry webResult = srm->webResultAt( sfpm->mapToSource(selectedIndex.first()) );
                    emit selectedResource(webResult,false);
                }
            }
            else {
                Nepomuk2::Resource empty;
                emit selectedResource(empty,false);
            }
        }
    }
}

void ResourceTableWidget::applyFilter()
{
    QString searchKey = m_searchBox->text();
    int curIndex = m_searchSelection->currentIndex();
    int searchColumn = m_searchSelection->itemData(curIndex).toInt();

    QSortFilterProxyModel *sfpm = qobject_cast<QSortFilterProxyModel *>(m_documentView->model());

    if(!sfpm)
        return;

    sfpm->setFilterKeyColumn(searchColumn);
    sfpm->setFilterRegExp(searchKey);
}

void ResourceTableWidget::tableContextMenu(const QPoint & pos)
{
    Q_UNUSED(pos);
    QItemSelectionModel *sm = m_documentView->selectionModel();
    if(!sm) { return; }

    QModelIndexList selectedIndex = sm->selectedRows();
    if(selectedIndex.isEmpty()) { return; }

    Nepomuk2::Resource nepomukRescource;
    SearchResultModel::SRCachedRowEntry webResource;

    QSortFilterProxyModel *sfpm = qobject_cast<QSortFilterProxyModel *>(m_documentView->model());
    NepomukModel *nm = qobject_cast<NepomukModel *>(sfpm->sourceModel());

    if(nm) {
        nepomukRescource = nm->documentResource(sfpm->mapToSource(selectedIndex.first()));
    }
    else {
        SearchResultModel *srm = qobject_cast<SearchResultModel *>(sfpm->sourceModel());
        if(srm) {
            nepomukRescource = srm->nepomukResourceAt(sfpm->mapToSource(selectedIndex.first()));
            webResource = srm->webResultAt(sfpm->mapToSource(selectedIndex.first()));
        }
        else {
            kDebug() << "ResourceTableWidget::tableContextMenu no model found";
            return;
        }
    }

    TableViewMenu tvm;
    connect(&tvm, SIGNAL(openResourceInTab(Nepomuk2::Resource&, bool)), this, SIGNAL(selectedResource(Nepomuk2::Resource&, bool)));
    tvm.setLibraryManager(m_libraryManager);

    if(nepomukRescource.isValid()) {
        tvm.showNepomukEntryMenu(nepomukRescource);
    }
    else {
        tvm.showWebResultEntryMenu(webResource);
    }
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

void ResourceTableWidget::mouseDoubleClickEvent ( QModelIndex index )
{
    QItemSelectionModel *sm = m_documentView->selectionModel();
    if(!sm) { return; }

    QModelIndexList selectedIndex = sm->selectedRows();
    if(selectedIndex.isEmpty()) { return; }

    Nepomuk2::Resource nepomukRescource;

    QSortFilterProxyModel *sfpm = qobject_cast<QSortFilterProxyModel *>(m_documentView->model());
    NepomukModel *nm = qobject_cast<NepomukModel *>(sfpm->sourceModel());

    if(nm) {
        nepomukRescource = nm->documentResource(sfpm->mapToSource(index));
    }
    else {
        SearchResultModel *srm = qobject_cast<SearchResultModel *>(sfpm->sourceModel());
        if(srm) {
            nepomukRescource = srm->nepomukResourceAt(sfpm->mapToSource(selectedIndex.first()));
        }
        else {
            kDebug() << "ResourceTableWidget::tableContextMenu no model found";
            return;
        }
    }

    emit selectedResource(nepomukRescource, true);
}

void ResourceTableWidget::changeHeaderSectionVisibility()
{
    QAction *a = qobject_cast<QAction *>(sender());

    QHeaderView *hv = m_documentView->horizontalHeader();
    hv->setSectionHidden(a->data().toInt(),
                         !hv->isSectionHidden(a->data().toInt()));

    QString group;
    if(m_documentView->model() == m_searchResultModel) {
        group = QLatin1String("SearchResultView");
    }
    else {
        group = QLatin1String("TableView");
        group.append((int)m_selection);
    }

    // save selection in the application settings
    KConfig config;
    KConfigGroup tableViewGroup( &config, group );
    QString keyHidden = QLatin1String("hidden_") + a->data().toString();
    tableViewGroup.writeEntry( keyHidden, hv->isSectionHidden(a->data().toInt()) );
    tableViewGroup.config()->sync();
}

void ResourceTableWidget::sectionResized( int logicalIndex, int oldSize, int newSize )
{
    Q_UNUSED(oldSize);
    if(newSize < 5)
        return;

    QHeaderView *hv = m_documentView->horizontalHeader();

    QString group;
    if(m_documentView->model() == m_searchResultModel) {
        group = QLatin1String("SearchResultView");
    }
    else {
        group = QLatin1String("TableView");
        group.append((int)m_selection);
    }

    // save selection in the application settings
    KConfig config;
    KConfigGroup tableViewGroup( &config, group );
    QString keySize = QLatin1String("size_") + QString::number(logicalIndex);
    tableViewGroup.writeEntry( keySize, hv->sectionSize(logicalIndex) );
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
    m_htmlDelegate = new HtmlDelegate();
    m_documentView = new QTableView;
    m_documentView->setSortingEnabled(true);
    m_documentView->setItemDelegateForColumn(0, new RatingDelegate());
    m_documentView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_documentView->horizontalHeader()->setStretchLastSection(false);
    m_documentView->verticalHeader()->hide();
    m_documentView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_documentView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_documentView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_documentView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(tableContextMenu(QPoint)));

    connect(m_documentView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(mouseDoubleClickEvent(QModelIndex)));

    QHeaderView *hvHorizontal = m_documentView->horizontalHeader();
    hvHorizontal->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(hvHorizontal, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(headerContextMenu(QPoint)));
    connect(hvHorizontal, SIGNAL(sectionResized(int,int,int)),
            this, SLOT(sectionResized(int,int,int)));

    mainLayout->addWidget(m_documentView);
}
