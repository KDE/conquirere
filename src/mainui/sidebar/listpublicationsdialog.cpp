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

#include "listpublicationsdialog.h"
#include "ui_listpublicationsdialog.h"

#include "core/library.h"
#include "core/projectsettings.h"
#include "core/models/nepomukmodel.h"
#include "core/models/publicationfiltermodel.h"
#include "core/models/seriesfiltermodel.h"
#include "core/delegates/ratingdelegate.h"
#include "core/delegates/htmldelegate.h"

#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KLineEdit>
#include <KDE/KComboBox>

#include <QtGui/QTableView>
#include <QtGui/QHeaderView>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QKeyEvent>

ListPublicationsDialog::ListPublicationsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ListPublicationsDialog)
    , m_selection(Resource_Publication)
    , m_filter(Max_BibTypes)
{
    ui->setupUi(this);

    // view that holds the table models for selection
    m_htmlDelegate = new HtmlDelegate();
    ui->tableView->setSortingEnabled(true);
    ui->tableView->setItemDelegateForColumn(0, new RatingDelegate());
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->setStretchLastSection(false);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    QHeaderView *hvHorizontal = ui->tableView->horizontalHeader();
    hvHorizontal->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(hvHorizontal, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(headerContextMenu(QPoint)));
    connect(hvHorizontal, SIGNAL(sectionResized(int,int,int)),
            this, SLOT(sectionResized(int,int,int)));

    QHeaderView *hvVertical = ui->tableView->verticalHeader();
    hvVertical->setResizeMode(QHeaderView::ResizeToContents);
}

ListPublicationsDialog::~ListPublicationsDialog()
{
    //rest filter for the mainview again
    PublicationFilterModel * pfm = qobject_cast<PublicationFilterModel *>(m_systemLibrary->viewModel(m_selection));
    if(pfm) {
        pfm->setResourceFilter(Max_BibTypes);
        pfm->setFilterKeyColumn(0);
        pfm->setFilterRegExp(QLatin1String(""));
    }

    delete ui;
}

void ListPublicationsDialog::setListMode(ResourceSelection selection, BibEntryType filter)
{
    m_selection = selection;
    m_filter = filter;
}

void ListPublicationsDialog::setSystemLibrary(Library *p)
{
    m_systemLibrary = p;
    showLibraryModel(m_systemLibrary);

    ui->libraryComboBox->addItem(m_systemLibrary->settings()->name());
}

void ListPublicationsDialog::setOpenLibraries(QList<Library *> openLibList)
{
    m_openLibList = openLibList;
    foreach(Library *l, openLibList) {
        ui->libraryComboBox->addItem(l->settings()->name());
    }
}

Nepomuk::Resource ListPublicationsDialog::selectedPublication()
{
    QItemSelectionModel *sm = ui->tableView->selectionModel();
    QModelIndexList indexes = sm->selectedRows();

    QSortFilterProxyModel *sfpm = qobject_cast<QSortFilterProxyModel *>(ui->tableView->model());
    NepomukModel *rm = qobject_cast<NepomukModel *>(sfpm->sourceModel());

    // the resource for this entry
    Nepomuk::Resource nr = rm->documentResource(sfpm->mapToSource(indexes.first()));

    return nr;
}

void ListPublicationsDialog::keyPressEvent(QKeyEvent * e)
{
    switch(e->key()){
    case Qt::Key_Escape:
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        applyFilter();
        break;
    default:
        break;
    }
}

void ListPublicationsDialog::applyFilter()
{
    QString searchKey = ui->filterLineEdit->text();
    int curIndex = ui->filterComboBox->currentIndex();
    int searchColumn = ui->filterComboBox->itemData(curIndex).toInt();

    QSortFilterProxyModel *sfpm = qobject_cast<QSortFilterProxyModel *>(ui->tableView->model());

    sfpm->setFilterKeyColumn(searchColumn);
    sfpm->setFilterRegExp(searchKey);
}

void ListPublicationsDialog::changeLibrary()
{
    if(ui->libraryComboBox->currentIndex() < 1) {
        showLibraryModel(m_systemLibrary);
    }
    else {
        int index = ui->libraryComboBox->currentIndex() - 1;
        showLibraryModel(m_openLibList.at(index));
    }
}

void ListPublicationsDialog::headerContextMenu(const QPoint &pos)
{
    QMenu menu(this);

    int columnCount = ui->tableView->model()->columnCount();

    //iterate through all available header entries in the current model
    for(int i=0; i < columnCount; i++) {
        QString headerName = ui->tableView->model()->headerData(i,Qt::Horizontal).toString();
        if(headerName.isEmpty()) {
            headerName = ui->tableView->model()->headerData(i,Qt::Horizontal, Qt::ToolTipRole).toString();
        }

        // for each header create a checkable menu entry to change visibility
        QAction *a = menu.addAction(headerName);
        connect(a, SIGNAL(triggered()), this, SLOT(changeHeaderSectionVisibility()));
        a->setData(i);
        a->setCheckable(true);
        a->setChecked(!ui->tableView->horizontalHeader()->isSectionHidden(i));
    }

    menu.exec(mapToGlobal(pos));
}

void ListPublicationsDialog::changeHeaderSectionVisibility()
{
    QAction *a = qobject_cast<QAction *>(sender());

    QHeaderView *hv = ui->tableView->horizontalHeader();
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

void ListPublicationsDialog::sectionResized( int logicalIndex, int oldSize, int newSize )
{
    Q_UNUSED(oldSize);
    if(newSize < 5)
        return;

    QHeaderView *hv = ui->tableView->horizontalHeader();

    QString group;
    group = QLatin1String("TableView");
    group.append((int)m_selection);

    // save selection in the application settings
    KConfig config;
    KConfigGroup tableViewGroup( &config, group );
    QString keySize = QLatin1String("size_") + QString::number(logicalIndex);
    tableViewGroup.writeEntry( keySize, hv->sectionSize(logicalIndex) );
    tableViewGroup.config()->sync();
}

void ListPublicationsDialog::showLibraryModel(Library *p)
{

    PublicationFilterModel * pfm = qobject_cast<PublicationFilterModel *>(p->viewModel(m_selection));
    if(pfm) {
        pfm->setResourceFilter(m_filter);
    }

    switch(m_selection) {
    case Resource_Event:
    case Resource_Series:
        ui->tableView->setItemDelegateForColumn(3, m_htmlDelegate);
        break;
    default:
        ui->tableView->setItemDelegateForColumn(3, NULL);
    }

    ui->tableView->setWordWrap(true);
    ui->tableView->setModel(p->viewModel(m_selection));

    //load settings for visible/hidden columns
    KConfig config;
    QString group = QLatin1String("TableView");
    group.append((int)m_selection);
    KConfigGroup tableViewGroup( &config, group );


    // go through all header elements and apply last known visibility / size status
    // also add each header name to the search combobox for selection
    ui->filterComboBox->clear();
    ui->filterComboBox->addItem(i18n("all entries"), -1); //additem NAME, TABLEHEADERINDEX
    QHeaderView *hv = ui->tableView->horizontalHeader();
    int columnCount = ui->tableView->model()->columnCount();

    NepomukModel *nm = qobject_cast<NepomukModel *>(p->viewModel(m_selection));
    if(!nm) {
        QSortFilterProxyModel *qsf = qobject_cast<QSortFilterProxyModel *>(p->viewModel(m_selection));
        if(qsf) {
            nm = qobject_cast<NepomukModel *>(qsf->sourceModel());
        }
    }

    for(int i=0; i < columnCount; i++) {
        // hidden status
        QString keyHidden = QLatin1String("hidden_") + QString::number(i);
        bool hidden = tableViewGroup.readEntry( keyHidden, false );

        hv->setSectionHidden(i, hidden);

        QString headerName = ui->tableView->model()->headerData(i,Qt::Horizontal).toString();
        if(!headerName.isEmpty()) {
            ui->filterComboBox->addItem(headerName, i);
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

    hv->setResizeMode(QHeaderView::Interactive);

    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->tableView->selectRow(0);

    QList<int> fixedWithList = nm->fixedWithSections();
    foreach(int section, fixedWithList)
        hv->setResizeMode(section, QHeaderView::Fixed);
}
