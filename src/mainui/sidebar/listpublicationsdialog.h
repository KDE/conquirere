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

#ifndef LISTPUBLICATIONSDIALOG_H
#define LISTPUBLICATIONSDIALOG_H

#include "../globals.h"

#include <Nepomuk/Resource>

#include <QtGui/QDialog>

namespace Ui {
    class ListPublicationsDialog;
}

class Library;

/**
  * @brief shows a simple table with all bibliographic relevant entries
  *
  * What entry is used is specified by @c setListMode
  *
  * The table can be filtered by some keywords or restrict to a list of publications
  * from one of the opened libraries.
  */
class ListPublicationsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ListPublicationsDialog(QWidget *parent = 0);
    virtual ~ListPublicationsDialog();

    void setListMode(ResourceSelection selection, BibEntryType filter);

    /**
      * sets the Library with all systemwide publication data
      */
    void setSystemLibrary(Library *p);

    /**
      * sets the list of all open libraries
      *
      * useful to show only publications in a specific collection
      */
    void setOpenLibraries(QList<Library *> openLibList);

    /**
      * returns the current selected @c nbib:publication
      */
    Nepomuk::Resource selectedPublication();

    /**
      * activated the filter if enter/return is pressed
      */
    void keyPressEvent(QKeyEvent * e);

private slots:
    void applyFilter();
    void changeLibrary();
    void headerContextMenu(const QPoint &pos);
    void changeHeaderSectionVisibility();

private:
    void showLibraryModel(Library *p);
    Ui::ListPublicationsDialog *ui;

    Library *m_systemLibrary;
    QList<Library *> m_openLibList;
    ResourceSelection m_selection;
    BibEntryType m_filter;
};

#endif // LISTPUBLICATIONSDIALOG_H
