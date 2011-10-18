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

#include <QWidget>

enum BibDataType {
    BibData_Annote,
    BibData_Author,
    BibData_Booktitle,
    BibData_Crossref,
    BibData_Edition,
    BibData_Editor,
    BibData_Eprint,
    BibData_EntryType,
    BibData_Howpublished,
    BibData_Institution,
    BibData_Journal,
    BibData_Note,
    BibData_Pages,
    BibData_Publisher,
    BibData_School,
    BibData_Series,
    BibData_Title,
    BibData_Type,
    BibData_Url
};

namespace Ui {
    class PublicationWidget;
}

class KComboBox;
class PublicationWidget : public SidebarComponent
{
    Q_OBJECT
public:
    explicit PublicationWidget(QWidget *parent = 0);
    virtual ~PublicationWidget();

    void setDialogMode(bool dialogMode);

public slots:
    /* called when something is selected in the project view */
    void setResource(Nepomuk::Resource & resource);

signals:
    /* notify connected editwidgets to update their info */
    void resourceChanged(Nepomuk::Resource & resource);

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

    void acceptContentChanges();
    void discardContentChanges();
    void acceptNoteChanges();
    void discardNoteChanges();

private:
    void setupWidget();

    void selectLayout(BibEntryType entryType);
    void layoutArticle();
    void layoutBook();
    void layoutBachelorthesis();
    void layoutBooklet();
    void layoutCollection();
    void layoutProceedings();
    void layoutInproceedings();
    void layoutManual();
    void layoutMastersthesis();
    void layoutMisc();
    void layoutPhdthesis();
    void layoutTechreport();
    void layoutUnpublished();
    void layoutPatent();
    void layoutElectronic();
    void layoutJournalIssue();

    Nepomuk::Resource m_publication;

    Ui::PublicationWidget *ui;
};

#endif // PUBLICATIONWIDGET_H
