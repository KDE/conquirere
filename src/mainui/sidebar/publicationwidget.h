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
#include "config/bibglobals.h"

#include <Nepomuk2/Resource>

#include <QtGui/QWidget>

namespace Ui {
    class PublicationWidget;
}

class LibraryManager;

/**
  * @brief SidbarWidget to show and manipulate all @c nbib:Publication resources
  */
class PublicationWidget : public SidebarComponent
{
    Q_OBJECT
public:
    explicit PublicationWidget(QWidget *parent = 0);
    virtual ~PublicationWidget();

    void setLibraryManager(LibraryManager *lm);
    Nepomuk2::Resource resource();

public slots:
    /**
      * called when something is selected in the project view
      */
    void setResource(Nepomuk2::Resource & resource);

signals:
    void hasReference(bool reference);

    void openDocument(Nepomuk2::Resource & resource, bool inTab);

private slots:
    /**
      * @todo check how the chain of all types are assigned to a resource
      *       currently i create a new resource of the type i want, check .types() and
      *       assign all returned types to the new item
      *
      * @todo clean publication data when nbib:Publication type is changed. For example move nbib:volume data
      */
    void newBibEntryTypeSelected(int index);
    void newButtonClicked();
    void deleteButtonClicked();
    void addReference();
    void removeReference();
    void removeFromSelectedReference();

    void acceptContentChanges();
    void discardContentChanges();

    /**
      * called when a new annotation was selected in the listwidget
      *
      * shows the content of the note in the edit box
      */
    void newAnnotationSelected(Nepomuk2::Resource & noteResource);
    void saveAnnotationContent();


    void changeRating(uint newRating);
    void editContactDialog(Nepomuk2::Resource & resource, const QUrl & propertyUrl);
    void showDetailDialog(Nepomuk2::Resource & resource, const QUrl & propertyUrl);

private:
    void setupWidget();

    void selectLayout(BibGlobals::BibEntryType entryType);
    void layoutArticle();
    void layoutArticleExtra();
    void layoutBook();
    void layoutCollection();
    void layoutThesis();
    void layoutReport();
    void layoutElectronic();
    void layoutScript();
    void layoutUnpublished();
    void layoutMisc();
    void layoutManual();
    void layoutStandard();
    void layoutPatent();
    void layoutCodeOfLaw();
    void layoutCourtReporter();
    void layoutLegislation();
    void layoutMap();
    void layoutCase();

    Nepomuk2::Resource m_publication;
    Nepomuk2::Resource m_currentAnnotation; /**< the current opened annotation. Necessary so we can save them automatically*/

    Ui::PublicationWidget *ui;
};

#endif // PUBLICATIONWIDGET_H
