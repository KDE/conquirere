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

#include "config/bibglobals.h"

#include <Nepomuk2/Resource>

#include <QtGui/QDialog>

namespace Ui {
    class ListPublicationsDialog;
}

class Library;
class LibraryManager;
class HtmlDelegate;

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

    void setListMode(BibGlobals::ResourceSelection selection, BibGlobals::BibEntryType filter);

    void setLibraryManager(LibraryManager *lm);

    /**
      * returns the current selected @c nbib:publication
      */
    Nepomuk2::Resource selectedPublication();

    /**
      * activated the filter if enter/return is pressed
      */
    void keyPressEvent(QKeyEvent * e);

private slots:
    void applyFilter();
    void changeLibrary();
    void headerContextMenu(const QPoint &pos);
    void changeHeaderSectionVisibility();
    void sectionResized( int logicalIndex, int oldSize, int newSize );
    void createNew();

private:
    void showLibraryModel(Library *p);
    Ui::ListPublicationsDialog *ui;

    LibraryManager *m_libraryManager;
    BibGlobals::ResourceSelection m_selection;
    BibGlobals::BibEntryType m_filter;
    HtmlDelegate *m_htmlDelegate;
};

#endif // LISTPUBLICATIONSDIALOG_H
