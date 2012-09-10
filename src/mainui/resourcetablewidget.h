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

#ifndef RESOURCETABLEWIDGET_H
#define RESOURCETABLEWIDGET_H

#include "globals.h"

#include <Nepomuk2/Resource>

#include "core/models/searchresultmodel.h"

#include <QtGui/QWidget>
#include <QtGui/QItemSelection>

class LibraryManager;
class QSortFilterProxyModel;
class SearchResultModel;
class Library;
class QTableView;
class PublicationModel;
class SidebarWidget;
class HtmlDelegate;
class KAction;
class KLineEdit;
class KComboBox;

/**
  * @brief Widget for the main content
  *
  * Shows a TableView with the content of each Library model
  */
class ResourceTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResourceTableWidget(QWidget *parent = 0);
    virtual ~ResourceTableWidget();

    void setLibraryManager(LibraryManager *lm);
    void setSearchResultModel(SearchResultModel* srm);

signals:
    void selectedResource(Nepomuk2::Resource & nr, bool);
    void selectedResource(const SearchResultModel::SRCachedRowEntry & nr, bool);

    void selectedMultipleResources(QList<Nepomuk2::Resource> resourceList);

public slots:
    void switchView(ResourceSelection selection, BibEntryType filter, Library *p);
    void showSearchResult();
    void selectedResource( const QItemSelection & selected, const QItemSelection & deselected );

    void applyFilter();

private slots:
    void tableContextMenu(const QPoint & pos);
    void headerContextMenu(const QPoint &pos);
    void mouseDoubleClickEvent ( QModelIndex index );
    void changeHeaderSectionVisibility();
    void sectionResized( int logicalIndex, int oldSize, int newSize );

private:
    void setupWidget();

    LibraryManager *m_libraryManager;
    KLineEdit *m_searchBox;
    KComboBox *m_searchSelection;
    QTableView *m_documentView;
    ResourceSelection m_selection;
    QSortFilterProxyModel* m_searchResultModel;

    HtmlDelegate *m_htmlDelegate;
};

#endif // RESOURCETABLEWIDGET_H
