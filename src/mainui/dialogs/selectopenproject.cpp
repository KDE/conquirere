/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "selectopenproject.h"
#include "ui_selectopenproject.h"

#include "core/library.h"
#include "core/projectsettings.h"

#include <QtCore/QModelIndex>

SelectOpenProject::SelectOpenProject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectOpenProject)
{
    ui->setupUi(this);
}

SelectOpenProject::~SelectOpenProject()
{
    delete ui;
}

void SelectOpenProject::setActionText(const QString &text)
{
    ui->actionLabel->setText(text);
}

void SelectOpenProject::setProjectList(QList<Library *> libList)
{
    m_libList = libList;

    foreach(Library *l, m_libList) {
        ui->listWidget->addItem(l->settings()->name());
    }
}

Library *SelectOpenProject::getSelected()
{
    int selection = ui->listWidget->currentIndex().row();

    if(selection >= 0 && selection < m_libList.size())
        return m_libList.at(selection);
    else
        return 0;
}
