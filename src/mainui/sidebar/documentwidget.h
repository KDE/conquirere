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

#ifndef DOCUMENTWIDGET_H
#define DOCUMENTWIDGET_H

#include "sidebarcomponent.h"

#include <Nepomuk/Resource>

namespace Ui {
    class DocumentWidget;
}

class KFileMetaDataWidget;

/**
  * @brief shows information about a specific document via KFileMetaDataWidget
  *
  * In addition it offers two buttons to set/remove a @c nbib:Publication for the document.
  * The selection is made via the ListPublicationsDialog
  */
class DocumentWidget : public SidebarComponent
{
    Q_OBJECT

public:
    explicit DocumentWidget(QWidget *parent = 0);
    virtual ~DocumentWidget();

    void setLibraryManager(LibraryManager *lm);
    Nepomuk::Resource resource();

public slots:
    /**
      * sets the document resource for the SidebarComponent
      *
      * @pre resource must be a nfo:Document
      */
    virtual void setResource(Nepomuk::Resource & resource);

    /**
      * @todo implement file add (push data into nepomuk which is not there yet)
      */
    virtual void newButtonClicked();

    /**
      * deletes the file from the disk together with its nepomuk metadata
      *
      * @todo implement file deletion
      */
    virtual void deleteButtonClicked();

    /**
      * opens the ListPublicationsDialog and sets the nbib:publishedAs
      */
    void setPublication();

    /**
      * removes the nbib:publishedAs value again
      */
    void removePublication();

signals:
    /**
      * Emits if the shown document has a @c nbib:Publication attached to it
      *
      * Used to enable/disable the icon in the SidebarWidget
      */
    void hasPublication(bool publication);

private:
    Nepomuk::Resource m_document; /**< the @c nfo:Document resource */

    Ui::DocumentWidget *ui;
};

#endif // DOCUMENTWIDGET_H
