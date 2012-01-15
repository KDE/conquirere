/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef LOADPROJECT_H
#define LOADPROJECT_H

#include <QDialog>

namespace Ui {
    class LoadProject;
}

class Library;

class LoadProject : public QDialog
{
    Q_OBJECT

public:
    explicit LoadProject(QWidget *parent = 0);
    ~LoadProject();

    Library *loadedLibrary() const;

private slots:
    void showCollection(int currentRow);

    void accept();

private:
    void loadCollections();

    Ui::LoadProject *ui;
    Library *m_loadLibrary;
};

#endif // LOADPROJECT_H
