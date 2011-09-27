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

#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include <QWidget>

#include "../globals.h"

#include <Nepomuk/Resource>

class SidebarComponent;
class Project;

class SidebarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SidebarWidget(QWidget *parent = 0);


    virtual void setDialogMode(bool dialogMode) { }

    void setProject(Project *p);
    Project *project();

public slots:
    /* called when somethinh is selected in the project view */
    void clear();

    /* called when something is selected in the project tree */
    void newSelection(LibraryType library, ResourceSelection selection, Project *p=0);

    virtual void setResource(Nepomuk::Resource & resource);

private:
    SidebarComponent *m_currentWidget;
    Project *m_project;
};

#endif // SIDEBARWIDGET_H
