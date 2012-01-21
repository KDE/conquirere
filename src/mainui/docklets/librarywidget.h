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

#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include "globals.h"

#include <QtGui/QDockWidget>
#include <QtCore/QList>

class LibraryManager;
class Library;
class QLibraryTreeWidgetItem;

namespace Ui {
    class LibraryWidget;
}

/**
  * @brief Dockwidget to show a treewidget with all libraries and its content
  */
class LibraryWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit LibraryWidget(QWidget *parent = 0);
    ~LibraryWidget();

    void setLibraryManager(LibraryManager *lm);

signals:
    void newSelection(ResourceSelection selection, BibEntryType filter, Library *p);
    void showSearchResults();

private slots:
    void addLibrary(Library *p);
    void closeLibrary(const QUrl &projectThingUrl);

    void selectionchanged();
    void listContextMenu(const QPoint & pos);
    void renameLibrary(Library *p);

    void exportToFile();
    void importFromFile();
    void openSettings();
    void closeProject();
    void deleteProject();

private:
    Library *libForAction();
    void setupUi();
    void setupLibraryTree(QLibraryTreeWidgetItem *root, Library *p);
    void connectModelSignals(QLibraryTreeWidgetItem *root, Library *p, ResourceSelection resourceType);

    Ui::LibraryWidget *ui;
    QLibraryTreeWidgetItem *m_systemRoot;
    LibraryManager *m_libraryManager;
};

#endif // LIBRARYWIDGET_H
