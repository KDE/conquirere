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

#ifndef PUBLICATIONWIDGET_H
#define PUBLICATIONWIDGET_H

#include "sidebarcomponent.h"
#include "../globals.h"

#include <Nepomuk/Resource>

#include <QtGui/QWidget>

namespace Ui {
    class PublicationWidget;
}

class PublicationWidget : public SidebarComponent
{
    Q_OBJECT
public:
    explicit PublicationWidget(QWidget *parent = 0);
    virtual ~PublicationWidget();

    /** @todo remove and use ResourceWatcher later on */
    void setLibrary(Library *p);

public slots:
    /* called when something is selected in the project view */
    void setResource(Nepomuk::Resource & resource);

    /**
      * This slot gets called from the propertywidgets, replace the subresource with the main nepomuk resource
      * and calls resourceUpdated()
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void subResourceUpdated(Nepomuk::Resource resource);

signals:
    /* notify connected editwidgets to update their info */
    void resourceChanged(Nepomuk::Resource & resource);

    void hasReference(bool reference);

private slots:
    /**
      * @todo check how the chain of all types are assigned to a resource
      *       currently i create a new resource of the type i want, check .types() and
      *       assign all returned types to the new item
      *
      * @todo clean publication data when nbib:Publication type is changed. For example move nbib:volume data
      */
    void newBibEntryTypeSelected(int index);
    void selectPublication();
    void newButtonClicked();
    void deleteButtonClicked();
    void addReference();
    void removeReference();
    void removeFromSelectedReference();

    void acceptContentChanges();
    void discardContentChanges();
    void acceptNoteChanges();
    void discardNoteChanges();

    void changeRating(int newRating);
    void editContactDialog(Nepomuk::Resource & resource, const QUrl & propertyUrl);

private:
    void setupWidget();

    void selectLayout(BibEntryType entryType);
    void layoutArticle();
    void layoutBook();
    void layoutThesis();
    void layoutReport();
    void layoutCollection();
    void layoutMisc();
    void layoutElectronic();
    void layoutScript();
    void layoutUnpublished();
    void layoutManual();
    void layoutPatent();
    void layoutBill();
    void layoutStatute();
    void layoutCase();

    Nepomuk::Resource m_publication;

    Ui::PublicationWidget *ui;
};

#endif // PUBLICATIONWIDGET_H
