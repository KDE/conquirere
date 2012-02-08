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

#ifndef FILEOBJECTEDIT_H
#define FILEOBJECTEDIT_H

#include <QtGui/QWidget>

#include <Nepomuk/Resource>

class LibraryManager;
class QListWidgetItem;

namespace Ui {
    class FileObjectEdit;
}

class FileObjectEdit : public QWidget
{
    Q_OBJECT

public:
    explicit FileObjectEdit(QWidget *parent = 0);
    ~FileObjectEdit();

    void setLibraryManager(LibraryManager *lm);
    void setResource(Nepomuk::Resource & resource);

private slots:
    void fillFileObjectWidget();
    void fileObjectEdit();
    void fileObjectAdd();
    void fileObjectRemove();

private:
    void addItemInfo(QListWidgetItem *i, const Nepomuk::Resource &resource);

    Ui::FileObjectEdit *ui;

    LibraryManager *m_libraryManager;
    Nepomuk::Resource m_publication;
};

#endif // FILEOBJECTEDIT_H
