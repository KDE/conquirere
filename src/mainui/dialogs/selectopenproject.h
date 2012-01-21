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

#ifndef SELECTOPENPROJECT_H
#define SELECTOPENPROJECT_H

#include <QtGui/QDialog>

namespace Ui {
    class SelectOpenProject;
}

class Library;

/**
  * @brief Small helper dialog to select one of the opened project @c Librarys from the LibraryManager
  *
  * Used to select which Library shoul be closed or deleted
  */
class SelectOpenProject : public QDialog
{
    Q_OBJECT

public:
    explicit SelectOpenProject(QWidget *parent = 0);
    ~SelectOpenProject();

    void setActionText(const QString &text);
    void setProjectList(QList<Library *> libList);

    Library *getSelected();

private:
    Ui::SelectOpenProject *ui;
    QList<Library *> m_libList;
};

#endif // SELECTOPENPROJECT_H
