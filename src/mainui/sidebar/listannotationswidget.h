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

#ifndef LISTANNOTATIONSWIDGET_H
#define LISTANNOTATIONSWIDGET_H

#include <QtGui/QWidget>
#include <Nepomuk/Resource>

namespace Ui {
    class ListAnnotationsWidget;
}

class LibraryManager;

/**
  * Shows a list of all @c nao:isRelated pimo:Notes objects
  *
  * Also allows to edit/add/remove them
  */
class ListAnnotationsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ListAnnotationsWidget(QWidget *parent = 0);
    ~ListAnnotationsWidget();

    void setLibraryManager(LibraryManager *lm);
    void setResource(Nepomuk::Resource resource);

signals:
    /**
      * This signal gets thrown when the resource was changed and must be updated in the table model cache
      * redirects the signal from all propertywidgets
      *
      * Must be connected to the resourceUpdated signal from the widget it is contained in
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void resourceCacheNeedsUpdate(Nepomuk::Resource resource);

    /**
      * Emits the resource of the note the user has selected
      *
      * Used to update the KRichText field that displays the content of the note
      * @see PublicationWidget
      */
    void selectedAnnotation(Nepomuk::Resource &resource);

private slots:
    /**
      * called when the user clicks the edit button
      */
    void editAnnotation();

    /**
      * called when the user clicks the add button
      */
    void addAnnotation();

    /**
      * called when the user clicks the remove button
      */
    void removeAnnotation();

    /**
      * called when a new note was selected in the list view
      *
      * emits the selectedAnnotation() signal with the current selected nepomuk resource
      */
    void selectionChanged();

private:
    Ui::ListAnnotationsWidget *ui;
    LibraryManager *m_libraryManager;

    Nepomuk::Resource m_resource;
};

#endif // LISTANNOTATIONSWIDGET_H
