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

#ifndef PROJECTTREEWIDGET_H
#define PROJECTTREEWIDGET_H

#include <QWidget>

#include "../globals.h"

#include <QList>

class Library;
class QTreeWidgetItem;

namespace Ui {
    class ProjectTreeWidget;
}

class ProjectTreeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectTreeWidget(QWidget *parent = 0);
    ~ProjectTreeWidget();

    void addLibrary(Library *p);
    void closeLibrary(Library *p);

signals:
    void newSelection(ResourceSelection selection, Library *p);

public slots:
    void fetchDataFor(ResourceSelection selection, bool start, Library *p=0);

private slots:
    void selectionchanged();
    void updateFetchAnimation();

private:
    void setupLibraryTree(QTreeWidgetItem *root);

    QList<Library *> m_openLibraries;
    QList<QTreeWidgetItem *> m_items;
    Ui::ProjectTreeWidget *ui;
    bool m_fetchingInProgress;
};

#endif // PROJECTTREEWIDGET_H
