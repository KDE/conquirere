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

#ifndef FILEOBJECTEDITDIALOG_H
#define FILEOBJECTEDITDIALOG_H

#include <QtGui/QDialog>

#include <Nepomuk2/Resource>

class LibraryManager;

namespace Ui {
    class FileObjectEditDialog;
}

/**
  * @brief Dialog to change the details of the source for a publication
  *
  * Source can be a @c nfo:Website or @c nfo:FileObjectData, @c nfo:RemoteObjectData
  *
  * If the user selects/entered an existing file into the url box, it will be merged manually with the
  * existing one based on the url.
  *
  * @bug BUG: Needs to be changed, when somone changing nie:url/rdf:types cann corrupt existing data when somone changes a nfo:FiledataObject into a website
  *      Better ask the user beforehand what resource he wants to add and create it instead of changing/merging all this stuff here
  */
class FileObjectEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileObjectEditDialog(QWidget *parent = 0);
    ~FileObjectEditDialog();

    void setLibraryManager(LibraryManager *lm);
    void setPublication(const Nepomuk2::Resource &r);
    void setResource(Nepomuk2::Resource r);
    void createNewResource();

    Nepomuk2::Resource resource();

private slots:
    void typeChanged(int newType);
    void openUrlSelectionDialog();

    void saveAndMergeUrlChange();

private:
    Ui::FileObjectEditDialog *ui;

    Nepomuk2::Resource m_publication;
    Nepomuk2::Resource m_fileObject;
};

#endif // FILEOBJECTEDITDIALOG_H
