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
#include "../../globals.h"

#include <QWidget>
#include <QModelIndex>

class Project;
class QTableView;
class ResourceModel;
class DocumentInfoWidget;
class KAction;

class ProjectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectWidget(QWidget *parent = 0);
    ~ProjectWidget();

    void setProject(Project *p);
    Project *project() const;

public slots:
    void switchView(ResourceSelection selection);
    void selectedResource( const QModelIndex & index );

    void removeSelected();
    void openSelected();
    void exportSelectedToBibTeX();

private slots:
    void tableContextMenu(const QPoint & pos);

private:
    void setupWidget();

    Project* m_project;
    ProjectTreeWidget *m_projectTree;

    ResourceModel *m_documentModel;
    ResourceModel *m_mailModel;
    ResourceModel *m_mediaModel;
    ResourceModel *m_websiteModel;
    QTableView    *m_documentView;

    DocumentInfoWidget *m_documentInfo;

    KAction* m_removeFromProject;
    KAction* m_exportToBibTeX;
    KAction* m_openExternal;
};

#endif // PROJECTWIDGET_H
