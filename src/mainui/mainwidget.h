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

#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include "projecttreewidget.h"
#include "../globals.h"

#include <QWidget>
#include <QModelIndex>

#include <Nepomuk/Resource>

class Library;
class QTableView;
class PublicationModel;
class SidebarWidget;
class KAction;

/**
  * Widget for the main content
  *
  * Shows a TableView with the content of each Library model
  */
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

signals:
    void selectedResource(Nepomuk::Resource & nr);

public slots:
    void switchView(ResourceSelection selection, Library *p);
    void selectedResource( const QModelIndex & current, const QModelIndex & previous );

    void removeSelected();
    void openSelected();
    void exportSelectedToBibTeX();

private slots:
    void tableContextMenu(const QPoint & pos);

private:
    void setupWidget();

    QTableView    *m_documentView;

    KAction* m_removeFromProject;
    KAction* m_exportToBibTeX;
    KAction* m_openExternal;
};

#endif // PROJECTWIDGET_H
