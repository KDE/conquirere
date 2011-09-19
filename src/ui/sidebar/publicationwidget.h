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
#include <Nepomuk/Resource>

#include <QWidget>

enum BibEntryType {
    BibType_Article,           /**< An article from a journal or magazine. */
    BibType_Bachelorthesis,    /**< A Bachelor thesis. */
    BibType_Book,              /**< A book with an explicit publisher. */
    BibType_Booklet,           /**< A work that is printed and bound, but without a named publisher or sponsoring institution. */
    BibType_Collection,        /**< A collection of different publications */
    BibType_Incollection,      /**< A part of a book having its own title.. */
    BibType_Inproceedings,     /**< An article in a conference proceedings. */
    BibType_Manual,            /**< Technical documentation. */
    BibType_Mastersthesis,     /**< A Master's thesis. */
    BibType_Misc,              /**< Use this type when nothing else fits. */
    BibType_Phdthesis,         /**< A PhD thesis. */
    BibType_Proceedings,       /**< The proceedings of a conference. */
    BibType_Techreport,        /**< A report published by a school or other institution, usually numbered within a series. */
    BibType_Unpublished,       /**< A document having an author and title, but not formally published. */
    BibType_Patent,            /**< A patent. */
    BibType_Electronic         /**< A electronic publication */
};

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

class KComboBox;
class PublicationWidget : public SidebarComponent
{
    Q_OBJECT
public:
    explicit PublicationWidget(QWidget *parent = 0);
    virtual ~PublicationWidget();

public slots:
    /* called when somethinh is selected in the project view */
    void setResource(Nepomuk::Resource & resource);
    void clear();
    void showCreatePublication(bool showIt);

signals:
    /* notify connected editwidgets to update their info */
    void resourceChanged(Nepomuk::Resource & resource);

private slots:
    void newBibEntryTypeSelected(int index);
    void selectPublication();
    void newPublication();
    void removePublication();

private:
    void setupWidget();

    BibEntryType resourceTypeToEnum(Nepomuk::Resource & resource);
    QUrl EnumToResourceType(BibEntryType entryType);

    void selectLayout(BibEntryType entryType);
    void layoutArticle();
    void layoutBook();
    void layoutBachelorthesis();
    void layoutBooklet();
    void layoutCollection();
    void layoutProceedings();
    void layoutInproceedings();
    void layoutIncollection();
    void layoutManual();
    void layoutMastersthesis();
    void layoutMisc();
    void layoutPhdthesis();
    void layoutTechreport();
    void layoutUnpublished();
    void layoutPatent();
    void layoutElectronic();

    Nepomuk::Resource m_publication;
    Nepomuk::Resource m_document;

    QWidget *m_newPublicationWidget;
    QWidget *m_publicationWidget;
    KComboBox *entryTypeData;
    QWidget *copyrightWidget;
    QWidget *crossrefWidget;
    QWidget *doiWidget;
    QWidget *editionWidget;
    QWidget *editorWidget;
    QWidget *eprintWidget;
    QWidget *howpublishedWidget;
    QWidget *isbnWidget;
    QWidget *issnWidget;
    QWidget *journalWidget;
    QWidget *lccnWidget;
    QWidget *mrnumberWidget;
    QWidget *publicationDateWidget;
    QWidget *publisherWidget;
    QWidget *schoolWidget;
    QWidget *typeWidget;
    QWidget *urlWidget;
    QWidget *numberWidget; //not journal number
    QWidget *volumeWidget;  //not journal volume

    // wrong inplementation at the moment
    // need own edit widget t ocope
    // with the resource creation/linking
    QWidget *seriesWidget;
    //QWidget *procceedingsWidget;
    //QWidget *collectionWidget;
};

#endif // PUBLICATIONWIDGET_H
