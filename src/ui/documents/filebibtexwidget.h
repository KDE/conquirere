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

#ifndef FILEBIBTEXWIDGET_H
#define FILEBIBTEXWIDGET_H

#include <Nepomuk/Resource>

#include <QWidget>

enum BibEntryType {
    BibType_Unknown,           /**< Unknown entry type */
    BibType_Article,           /**< An article from a journal or magazine. */
    BibType_Bachelorthesis,    /**< A Bachelor thesis. */
    BibType_Book,              /**< A book with an explicit publisher. */
    BibType_Booklet,           /**< A work that is printed and bound, but without a named publisher or sponsoring institution. */
    BibType_Collection,        /**< A collection of different publications */
    BibType_Inbook,            /**< A part of a book, which may be a chapter (or section or whatever) and/or a range of pages. */
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
    BibData_Address,
    BibData_Annote,
    BibData_Author,
    BibData_Booktitle,
    BibData_Chapter,
    BibData_Crossref,
    BibData_Edition,
    BibData_Editor,
    BibData_Eprint,
    BibData_EntryType,
    BibData_Howpublished,
    BibData_Institution,
    BibData_Journal,
    BibData_Month,
    BibData_Note,
    BibData_Number,
    BibData_Organization,
    BibData_Pages,
    BibData_Publisher,
    BibData_School,
    BibData_Series,
    BibData_Title,
    BibData_Type,
    BibData_Url,
    BibData_Volume,
    BibData_Year
};

class QVBoxLayout;
class KComboBox;
class QHBoxLayout;

//todo rename to BibResourceWidget
// we really just create/manipulate the bibresource of a nfo:document
// the reference to the BibResource is handlet in a different widget
class FileBibTexWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileBibTexWidget(QWidget *parent = 0);
    ~FileBibTexWidget();

public slots:
    void setResource(Nepomuk::Resource & resource);
    void clear();

signals:
    void resourceChanged(Nepomuk::Resource & resource);

private slots:
    void newBibEntryTypeSelected(int index);

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
    void layoutInbook();
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

    Nepomuk::Resource m_resource;

    KComboBox *entryTypeData;
    QWidget *copyrightWidget;
    QWidget *crossrefWidget;
    QWidget *doiWidget;
    QWidget *editionWidget;
    QWidget *editorWidget;
    QWidget *eprintWidget;
    QWidget *howpublishedWidget;
    QWidget *institutionWidget;
    QWidget *isbnWidget;
    QWidget *issnWidget;
    QWidget *journalWidget;
    QWidget *lccnWidget;
    QWidget *mrnumberWidget;
    QWidget *numberWidget;
    QWidget *organizationWidget;
    QWidget *publicationDateWidget;
    QWidget *publisherWidget;
    QWidget *schoolWidget;
    QWidget *typeWidget;
    QWidget *urlWidget;
    QWidget *volumeWidget;

    // wrong inplementation at the moment
    // need own edit widget t ocope
    // with the resource creation/linking
    QWidget *seriesWidget;
    QWidget *chapterWidget;
    //QWidget *procceedingsWidget;
    //QWidget *collectionWidget;
};

#endif // FILEBIBTEXWIDGET_H
