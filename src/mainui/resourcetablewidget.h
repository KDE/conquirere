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

#include "../globals.h"

#include <Nepomuk/Resource>

#include <QtGui/QWidget>
#include <QtGui/QItemSelection>

class Library;
class QTableView;
class PublicationModel;
class SidebarWidget;
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
    ~ResourceTableWidget();

signals:
    void selectedResource(Nepomuk::Resource & nr);
    void selectedMultipleResources(QList<Nepomuk::Resource> resourceList);

public slots:
    void switchView(ResourceSelection selection, ResourceFilter filter, Library *p);
    void selectedResource( const QItemSelection & selected, const QItemSelection & deselected );

    void applyFilter();
    void removeSelectedFromProject();
    void removeSelectedFromSystem();
    void openSelected();
    void exportSelectedToBibTeX();

private slots:
    void tableContextMenu(const QPoint & pos);
    void headerContextMenu(const QPoint &pos);
    void changeHeaderSectionVisibility();

private:
    void setupWidget();

    KLineEdit *m_searchBox;
    KComboBox *m_searchSelection;
    QTableView    *m_documentView;
    Library *m_curLibrary;
    ResourceSelection m_selection;

    KAction* m_removeFromSystem;
    KAction* m_removeFromProject;
    KAction* m_exportToBibTeX;
    KAction* m_openExternal;
};

#endif // RESOURCETABLEWIDGET_H
