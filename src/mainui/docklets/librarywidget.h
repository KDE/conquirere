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

#include "../../globals.h"

#include <QtGui/QDockWidget>
#include <QtCore/QList>

class Library;
class QLibraryTreeWidgetItem;

namespace Ui {
    class LibraryWidget;
}

class LibraryWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit LibraryWidget(QWidget *parent = 0);
    ~LibraryWidget();

    void addLibrary(Library *p);
    void closeLibrary(Library *p);

signals:
    void newSelection(ResourceSelection selection, BibEntryType filter, Library *p);

private slots:
    void selectionchanged();

private:
    void setupLibraryTree(QLibraryTreeWidgetItem *root, Library *p);
    void connectModelSignals(QLibraryTreeWidgetItem *root, Library *p, ResourceSelection resourceType);

    Ui::LibraryWidget *ui;

    QList<Library *> m_openLibraries;
    QList<QLibraryTreeWidgetItem *> m_items;
};

#endif // LIBRARYWIDGET_H
