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

#include "publicationwidget.h"

#include "../semantic/labeledit.h"
#include "../semantic/contactedit.h"
#include "../semantic/addressedit.h"

#include "nbib.h"
#include <KComboBox>
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <QDebug>
#include <QSpacerItem>

PublicationWidget::PublicationWidget(QWidget *parent) :
    SidebarComponent(parent)
{
    //hide();
    setFont(KGlobalSettings::smallestReadableFont());

    setupWidget();
    showCreatePublication(true);
    //show();
}

PublicationWidget::~PublicationWidget()
{
    delete entryTypeData;
    delete copyrightWidget;
    delete crossrefWidget;
    delete doiWidget;
    delete editionWidget;
    delete editorWidget;
    delete eprintWidget;
    delete howpublishedWidget;
    delete isbnWidget;
    delete issnWidget;
    delete journalWidget;
    delete lccnWidget;
    delete mrnumberWidget;
    delete publicationDateWidget;
    delete publisherWidget;
    delete schoolWidget;
    delete typeWidget;
    delete urlWidget;
    delete numberWidget; //not journal number
    delete volumeWidget;  //not journal volume
    delete m_publicationWidget;
    delete m_newPublicationWidget;
}

void PublicationWidget::setResource(Nepomuk::Resource & resource)
{
    m_document = resource;

    // what we get is a nfo::document or similar
    // what we want is the connected nbib::Publication
    Nepomuk::Resource nr = resource.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();

    //check if the resource has a publication attached
    if(!nr.isValid()) {
        showCreatePublication(true);
        m_publication  = resource;
    }
    else {
        showCreatePublication(false);
        m_publication  = nr;
    }

    emit resourceChanged(m_publication);

    BibEntryType entryType = resourceTypeToEnum(m_publication);

    int index = entryTypeData->findData(entryType);
    entryTypeData->setCurrentIndex(index);

    selectLayout(entryType);
}

void PublicationWidget::clear()
{
    showCreatePublication(true);
}

void PublicationWidget::showCreatePublication(bool showIt)
{
    m_newPublicationWidget->setVisible(showIt);
    // handle Publication parts
    m_publicationWidget->setVisible(!showIt);
}

void PublicationWidget::newBibEntryTypeSelected(int index)
{
    KComboBox *kcb = qobject_cast<KComboBox *>(sender());
    BibEntryType entryType = (BibEntryType)kcb->itemData(index).toInt();

    selectLayout(entryType);

    // update resource
    QUrl newEntryUrl = EnumToResourceType(entryType);
    if(newEntryUrl.isValid()) {
        QList<QUrl>newtype;
        newtype.append(newEntryUrl);
        m_publication.setTypes(newtype);
    }
    else {
        QList<QUrl>newtype;
        newtype.append(Nepomuk::Vocabulary::NBIB::Publication());
        m_publication.setTypes(newtype);
    }
}

void PublicationWidget::selectPublication()
{
    qDebug() << "select from a list of systemwide publications";
}

void PublicationWidget::newPublication()
{
    //create a new resource if nothing is connected
    Nepomuk::Resource nb;
    QList<QUrl> types;
    types.append(Nepomuk::Vocabulary::NBIB::Publication());
    nb.setTypes(types);

    // link document to resource
    m_publication.setProperty( Nepomuk::Vocabulary::NBIB::publishedAs(), nb);

    setResource(m_publication);
}

void PublicationWidget::removePublication()
{
    // link document to resource
    m_document.removeProperty( Nepomuk::Vocabulary::NBIB::publishedAs() );

    setResource(m_document);
}

void PublicationWidget::setupWidget()
{
    QVBoxLayout *layoutMain = new QVBoxLayout;
    layoutMain->setMargin(0);
    layoutMain->setSpacing(0);
    setLayout(layoutMain);

    //shown when new nbib:Publication is available
    m_newPublicationWidget = new QWidget();
    QVBoxLayout *layoutNewPublication = new QVBoxLayout;
    layoutNewPublication->setMargin(0);
    layoutNewPublication->setSpacing(0);
    m_newPublicationWidget->setLayout(layoutNewPublication);
    layoutMain->addWidget(m_newPublicationWidget);


    QLabel *newPublicationLabel = new QLabel();
    newPublicationLabel->setText(i18n("No Publication assigned to this document\nPlease select an existing one or create a new one."));
    newPublicationLabel->setWordWrap(true);
    layoutNewPublication->addWidget(newPublicationLabel);

    QPushButton *selectPublication = new QPushButton();
    selectPublication->setText(i18n("select"));
    layoutNewPublication->addWidget(selectPublication);
    connect(selectPublication, SIGNAL(clicked()), this, SLOT(selectPublication()));


    QPushButton *newPublication = new QPushButton();
    newPublication->setText(i18n("new"));
    layoutNewPublication->addWidget(newPublication);
    connect(newPublication, SIGNAL(clicked()), this, SLOT(newPublication()));

    QSpacerItem *s = new QSpacerItem(10,10,QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    layoutNewPublication->insertSpacerItem(-1,s);


    //####################################################################


    // Actual nbib:Publication section
    m_publicationWidget = new QWidget();
    QVBoxLayout *layoutPublication = new QVBoxLayout;
    layoutPublication->setMargin(0);
    layoutPublication->setSpacing(0);
    m_publicationWidget->setLayout(layoutPublication);

    layoutMain->addWidget(m_publicationWidget);

    QLabel *entryType = new QLabel(i18n("Entry type:"));
    entryType->setToolTip(i18n("The type of this document"));

    entryTypeData = new KComboBox();
    entryTypeData->setProperty("datatype", BibData_EntryType);
    entryTypeData->addItem(i18n("Misc"),BibType_Misc);
    entryTypeData->addItem(i18n("Article"),BibType_Article);
    entryTypeData->addItem(i18n("Book"),BibType_Book);
    entryTypeData->addItem(i18n("Booklet"),BibType_Booklet);
    entryTypeData->addItem(i18n("Collection"),BibType_Collection);
    entryTypeData->addItem(i18n("Incollection"),BibType_Incollection);
    entryTypeData->addItem(i18n("Proceedings"),BibType_Proceedings);
    entryTypeData->addItem(i18n("Inproceedings"),BibType_Inproceedings);
    entryTypeData->addItem(i18n("Bachelorhesis"),BibType_Bachelorthesis);
    entryTypeData->addItem(i18n("Mastersthesis"),BibType_Mastersthesis);
    entryTypeData->addItem(i18n("Phdthesis"),BibType_Phdthesis);
    entryTypeData->addItem(i18n("Manual"),BibType_Manual);
    entryTypeData->addItem(i18n("Techreport"),BibType_Techreport);
    entryTypeData->addItem(i18n("Unpublished"),BibType_Unpublished);
    entryTypeData->addItem(i18n("Electronic"),BibType_Electronic);
    entryTypeData->addItem(i18n("Patent"),BibType_Patent);
    entryType->setBuddy(entryTypeData);

    connect(entryTypeData, SIGNAL(currentIndexChanged(int)), this, SLOT(newBibEntryTypeSelected(int)));

    QHBoxLayout *layoutEntry = new QHBoxLayout;
    layoutEntry->addWidget(entryType);
    layoutEntry->addWidget(entryTypeData);
    layoutPublication->addLayout(layoutEntry);

    //####################################################################
    QLabel *title = new QLabel(i18n("Title:"));
    title->setToolTip(i18n("The title of the work"));

    LabelEdit *titleData = new LabelEdit();
    titleData->setPropertyUrl( Nepomuk::Vocabulary::NIE::title() );
    title->setBuddy(titleData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), titleData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutTitle = new QHBoxLayout;
    layoutTitle->addWidget(title);
    layoutTitle->addWidget(titleData);
    layoutPublication->addLayout(layoutTitle);

    //####################################################################

    QLabel *author = new QLabel(i18n("Authors:"));
    author->setToolTip(i18n("The name(s) of the author(s)"));

    ContactEdit *authorData = new ContactEdit();
    authorData->setPropertyUrl( Nepomuk::Vocabulary::NCO::creator() );
    author->setBuddy(authorData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), authorData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutAuthor = new QHBoxLayout;
    layoutAuthor->addWidget(author);
    layoutAuthor->addWidget(authorData);
    layoutPublication->addLayout(layoutAuthor);

    //####################################################################

    publicationDateWidget = new QWidget;
    QLabel *year = new QLabel(i18n("Publication Date:"));
    year->setToolTip(i18n("The year of publication (or, if unpublished, the year of creation)"));

    //TODO date edit
    LabelEdit *yearData = new LabelEdit();
    yearData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::publicationDate() );
    year->setBuddy(yearData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), yearData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutYear = new QHBoxLayout;
    layoutYear->setMargin(0);
    layoutYear->setSpacing(0);
    layoutYear->addWidget(year);
    layoutYear->addWidget(yearData);
    publicationDateWidget->setLayout(layoutYear);
    layoutPublication->addWidget(publicationDateWidget);

    //####################################################################

    QFrame *headerLine = new QFrame();
    headerLine->setFrameShape(QFrame::HLine);
    headerLine->setFrameShadow(QFrame::Sunken);
    layoutPublication->addWidget(headerLine);

    //####################################################################

    seriesWidget = new QWidget;
    QLabel *series = new QLabel(i18n("Series:"));
    series->setToolTip(i18n("The series of books the book was published in"));

//    LabelEdit *seriesData = new LabelEdit();
//    seriesData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::series() );
//    series->setBuddy(seriesData);
//    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), seriesData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutSeries = new QHBoxLayout;
    layoutSeries->setMargin(0);
    layoutSeries->setSpacing(0);
    layoutSeries->addWidget(series);
    //layoutSeries->addWidget(seriesData);
    seriesWidget->setLayout(layoutSeries);
    layoutPublication->addWidget(seriesWidget);

    //####################################################################

    journalWidget = new QWidget;
    QLabel *journal = new QLabel(i18n("Journal:"));
    journal->setToolTip(i18n("The journal or magazine the work was published in"));

    //TODO change journal to its own edit widget
    ContactEdit *journalData = new ContactEdit();
    journalData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::inJournalIssue() );
    journal->setBuddy(journalData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), journalData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutJournal = new QHBoxLayout;
    layoutJournal->setMargin(0);
    layoutJournal->setSpacing(0);
    layoutJournal->addWidget(journal);
    layoutJournal->addWidget(journalData);
    journalWidget->setLayout(layoutJournal);
    layoutPublication->addWidget(journalWidget);

    //####################################################################

    // volume of a journal is handlet by the JournalIssue
    volumeWidget = new QWidget;
    QLabel *volume = new QLabel(i18n("Volume:"));
    volume->setToolTip(i18n("The volume of a multi-volume book"));

    LabelEdit *volumeData = new LabelEdit();
    volumeData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::volume() );
    volume->setBuddy(volumeData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), volumeData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutVolume = new QHBoxLayout;
    layoutVolume->setMargin(0);
    layoutVolume->setSpacing(0);
    layoutVolume->addWidget(volume);
    layoutVolume->addWidget(volumeData);
    volumeWidget->setLayout(layoutVolume);
    layoutPublication->addWidget(volumeWidget);

    //####################################################################

    // number of a journal is handlet by the JournalIssue
    numberWidget = new QWidget;
    QLabel *number = new QLabel(i18n("Number:"));
    number->setToolTip(i18n("The &quot;(issue) number&quot; of a tech-report, if applicable. (Most publications have a &quot;volume&quot;, but no &quot;number&quot; field.)"));

    LabelEdit *numberData = new LabelEdit();
    numberData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::number() );
    number->setBuddy(numberData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), numberData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutNumber = new QHBoxLayout;
    layoutNumber->setMargin(0);
    layoutNumber->setSpacing(0);
    layoutNumber->addWidget(number);
    layoutNumber->addWidget(numberData);
    numberWidget->setLayout(layoutNumber);
    layoutPublication->addWidget(numberWidget);

    //####################################################################

    publisherWidget = new QWidget;
    QLabel *publisher = new QLabel(i18n("Publisher:"));
    publisher->setToolTip(i18n("The publisher's name"));

    ContactEdit *publisherData = new ContactEdit();
    publisherData->setPropertyUrl( Nepomuk::Vocabulary::NCO::publisher() );
    publisher->setBuddy(publisherData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), publisherData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutPublisher = new QHBoxLayout;
    layoutPublisher->setMargin(0);
    layoutPublisher->setSpacing(0);
    layoutPublisher->addWidget(publisher);
    layoutPublisher->addWidget(publisherData);
    publisherWidget->setLayout(layoutPublisher);
    layoutPublication->addWidget(publisherWidget);

    //####################################################################

    editionWidget = new QWidget;
    QLabel *edition = new QLabel(i18n("Edition:"));
    edition->setToolTip(i18n("The edition of a book, long form (such as &quot;first&quot; or &quot;second&quot;)"));

    LabelEdit *editionData = new LabelEdit();
    editionData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::edition() );
    edition->setBuddy(editionData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), editionData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutEdition = new QHBoxLayout;
    layoutEdition->setMargin(0);
    layoutEdition->setSpacing(0);
    layoutEdition->addWidget(edition);
    layoutEdition->addWidget(editionData);
    editionWidget->setLayout(layoutEdition);
    layoutPublication->addWidget(editionWidget);

    //####################################################################

    editorWidget = new QWidget;
    QLabel *editor = new QLabel(i18n("Editor:"));
    editor->setToolTip(i18n("The name(s) of the editor(s)"));

    ContactEdit *editorData = new ContactEdit();
    editorData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::editor() );
    editor->setBuddy(editorData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), editorData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layouteditor = new QHBoxLayout;
    layouteditor->addWidget(editor);
    layouteditor->addWidget(editorData);
    editorWidget->setLayout(layouteditor);
    layoutPublication->addWidget(editorWidget);

    //####################################################################

    eprintWidget = new QWidget;
    QLabel *eprint = new QLabel(i18n("E-print:"));
    eprint->setToolTip(i18n("A specification of an electronic publication, often a preprint or a technical report"));

    LabelEdit *eprintData = new LabelEdit();
    eprintData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::eprint() );
    eprint->setBuddy(eprintData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), eprintData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutEprint = new QHBoxLayout;
    layoutEprint->setMargin(0);
    layoutEprint->setSpacing(0);
    layoutEprint->addWidget(eprint);
    layoutEprint->addWidget(eprintData);
    eprintWidget->setLayout(layoutEprint);
    layoutPublication->addWidget(eprintWidget);

    //####################################################################

    howpublishedWidget = new QWidget;
    QLabel *howpublished = new QLabel(i18n("How published:"));
    howpublished->setToolTip(i18n("How it was published, if the publishing method is nonstandard"));

    LabelEdit *howpublishedData = new LabelEdit();
    howpublishedData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::howPublished() );
    howpublished->setBuddy(howpublishedData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), howpublishedData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutHowpublished = new QHBoxLayout;
    layoutHowpublished->setMargin(0);
    layoutHowpublished->setSpacing(0);
    layoutHowpublished->addWidget(howpublished);
    layoutHowpublished->addWidget(howpublishedData);
    howpublishedWidget->setLayout(layoutHowpublished);
    layoutPublication->addWidget(howpublishedWidget);

    //####################################################################

    schoolWidget = new QWidget;
    QLabel *school = new QLabel(i18n("School:"));
    school->setToolTip(i18n("The school where the thesis was written"));

    ContactEdit *schoolData = new ContactEdit();
    schoolData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::school() );
    school->setBuddy(schoolData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), schoolData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutSchool = new QHBoxLayout;
    layoutSchool->setMargin(0);
    layoutSchool->setSpacing(0);
    layoutSchool->addWidget(school);
    layoutSchool->addWidget(schoolData);
    schoolWidget->setLayout(layoutSchool);
    layoutPublication->addWidget(schoolWidget);

    //####################################################################

    urlWidget = new QWidget;
    QLabel *url = new QLabel(i18n("Url:"));
    url->setToolTip(i18n("The WWW address"));

    LabelEdit *urlData = new LabelEdit();
    urlData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::url() );
    url->setBuddy(urlData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), urlData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutUrl = new QHBoxLayout;
    layoutUrl->setMargin(0);
    layoutUrl->setSpacing(0);
    layoutUrl->addWidget(url);
    layoutUrl->addWidget(urlData);
    urlWidget->setLayout(layoutUrl);
    layoutPublication->addWidget(urlWidget);

    //####################################################################

    typeWidget = new QWidget;
    QLabel *type = new QLabel(i18n("Type:"));
    type->setToolTip(i18n("The type of tech-report, for example, &quot;Research Note&quot;"));

    LabelEdit *typeData = new LabelEdit();
    typeData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::type() );
    type->setBuddy(typeData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), typeData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutType = new QHBoxLayout;
    layoutType->setMargin(0);
    layoutType->setSpacing(0);
    layoutType->addWidget(type);
    layoutType->addWidget(urlData);
    typeWidget->setLayout(layoutType);
    layoutPublication->addWidget(typeWidget);

    //####################################################################

    QFrame *identifierLine = new QFrame();
    identifierLine->setFrameShape(QFrame::HLine);
    identifierLine->setFrameShadow(QFrame::Sunken);
    layoutPublication->addWidget(identifierLine);

    //####################################################################

    crossrefWidget = new QWidget;
    QLabel *crossref = new QLabel(i18n("Crossref:"));
    crossref->setToolTip(i18n("References found in the publication"));

    LabelEdit *crossrefData = new LabelEdit();
    crossrefData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::crossref() );
    crossref->setBuddy(crossrefData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), crossrefData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutcrossref = new QHBoxLayout;
    layoutcrossref->setMargin(0);
    layoutcrossref->setSpacing(0);
    layoutcrossref->addWidget(crossref);
    layoutcrossref->addWidget(crossrefData);
    crossrefWidget->setLayout(layoutcrossref);
    layoutPublication->addWidget(crossrefWidget);

    //####################################################################

    isbnWidget = new QWidget;
    QLabel *isbn = new QLabel(i18n("ISBN:"));
    isbn->setToolTip(i18n("The International Standard Book Number."));

    LabelEdit *isbnData = new LabelEdit();
    isbnData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::isbn() );
    isbn->setBuddy(isbnData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), isbnData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutIsbn = new QHBoxLayout;
    layoutIsbn->setMargin(0);
    layoutIsbn->setSpacing(0);
    layoutIsbn->addWidget(isbn);
    layoutIsbn->addWidget(isbnData);
    isbnWidget->setLayout(layoutIsbn);
    layoutPublication->addWidget(isbnWidget);

    //####################################################################

    issnWidget = new QWidget;
    QLabel *issn = new QLabel(i18n("ISSN:"));
    issn->setToolTip(i18n("The International Standard Serial Number. Used to identify a journal."));

    LabelEdit *issnData = new LabelEdit();
    issnData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::issn() );
    issn->setBuddy(issnData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), issnData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutIssn = new QHBoxLayout;
    layoutIssn->setMargin(0);
    layoutIssn->setSpacing(0);
    layoutIssn->addWidget(issn);
    layoutIssn->addWidget(issnData);
    issnWidget->setLayout(layoutIssn);
    layoutPublication->addWidget(issnWidget);

    //####################################################################

    lccnWidget = new QWidget;
    QLabel *lccn = new QLabel(i18n("LCCN:"));
    lccn->setToolTip(i18n("The Library of Congress Call Number."));

    LabelEdit *lccnData = new LabelEdit();
    lccnData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::lccn() );
    lccn->setBuddy(lccnData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), lccnData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutlccn = new QHBoxLayout;
    layoutlccn->setMargin(0);
    layoutlccn->setSpacing(0);
    layoutlccn->addWidget(lccn);
    layoutlccn->addWidget(lccnData);
    lccnWidget->setLayout(layoutlccn);
    layoutPublication->addWidget(lccnWidget);

    //####################################################################

    mrnumberWidget = new QWidget;
    QLabel *mrnumber = new QLabel(i18n("MRNumber:"));
    mrnumber->setToolTip(i18n("The Mathematical Reviews number."));

    LabelEdit *mrnumberData = new LabelEdit();
    mrnumberData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::mrNumber() );
    mrnumber->setBuddy(mrnumberData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), mrnumberData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutmrnumber = new QHBoxLayout;
    layoutmrnumber->setMargin(0);
    layoutmrnumber->setSpacing(0);
    layoutmrnumber->addWidget(mrnumber);
    layoutmrnumber->addWidget(mrnumberData);
    mrnumberWidget->setLayout(layoutmrnumber);
    layoutPublication->addWidget(mrnumberWidget);

    //####################################################################

    doiWidget = new QWidget;
    QLabel *doi = new QLabel(i18n("DOI:"));
    doi->setToolTip(i18n("The Digital object identifier."));

    LabelEdit *doiData = new LabelEdit();
    doiData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::doi() );
    doi->setBuddy(doiData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), doiData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutdoi = new QHBoxLayout;
    layoutdoi->setMargin(0);
    layoutdoi->setSpacing(0);
    layoutdoi->addWidget(doi);
    layoutdoi->addWidget(doiData);
    doiWidget->setLayout(layoutdoi);
    layoutPublication->addWidget(doiWidget);

    //####################################################################

    copyrightWidget = new QWidget;
    QLabel *copyright = new QLabel(i18n("Copyright:"));
    copyright->setToolTip(i18n("Copyright information."));

    LabelEdit *copyrightData = new LabelEdit();
    copyrightData->setPropertyUrl( Nepomuk::Vocabulary::NIE::copyright() );
    copyright->setBuddy(copyrightData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), copyrightData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutCopyright = new QHBoxLayout;
    layoutCopyright->setMargin(0);
    layoutCopyright->setSpacing(0);
    layoutCopyright->addWidget(copyright);
    layoutCopyright->addWidget(issnData);
    copyrightWidget->setLayout(layoutCopyright);
    layoutPublication->addWidget(copyrightWidget);

    //####################################################################

    QSpacerItem *s1 = new QSpacerItem(10,10,QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    layoutPublication->insertSpacerItem(-1,s1);

    QPushButton *removePublication = new QPushButton();
    removePublication->setText(i18n("remove Publication"));
    layoutNewPublication->addWidget(removePublication);
    connect(removePublication, SIGNAL(clicked()), this, SLOT(removePublication()));
    layoutPublication->addWidget(removePublication);
}

BibEntryType PublicationWidget::resourceTypeToEnum(Nepomuk::Resource & resource)
{
    Nepomuk::Resource checkType( resource.resourceType() );
    QString resourceLabel = checkType.genericLabel();

    if(resourceLabel == QLatin1String("Article")) {
        return BibType_Article;
    }
    if(resourceLabel == QLatin1String("BachelorThesis")) {
        return BibType_Bachelorthesis;
    }
    if(resourceLabel == QLatin1String("Book")) {
        return BibType_Book;
    }
    if(resourceLabel == QLatin1String("Booklet")) {
        return BibType_Booklet;
    }
    if(resourceLabel == QLatin1String("InProceedings")) {
        return BibType_Inproceedings;
    }
    if(resourceLabel == QLatin1String("InCollection")) {
        return BibType_Incollection;
    }
    if(resourceLabel == QLatin1String("Manual")) {
        return BibType_Manual;
    }
    if(resourceLabel == QLatin1String("MastersThesis")) {
        return BibType_Mastersthesis;
    }
    if(resourceLabel == QLatin1String("PhdThesis")) {
        return BibType_Phdthesis;
    }
    if(resourceLabel == QLatin1String("Proceedings")) {
        return BibType_Proceedings;
    }
    if(resourceLabel == QLatin1String("Techreport")) {
        return BibType_Techreport;
    }
    if(resourceLabel == QLatin1String("Unpublished")) {
        return BibType_Unpublished;
    }
    if(resourceLabel == QLatin1String("Patent")) {
        return BibType_Patent;
    }
    if(resourceLabel == QLatin1String("Website")) {
        return BibType_Electronic;
    }
    return BibType_Misc;
}

QUrl PublicationWidget::EnumToResourceType(BibEntryType entryType)
{
    switch(entryType) {
    case BibType_Article:
        return Nepomuk::Vocabulary::NBIB::Article();
        break;
    case BibType_Bachelorthesis:
        return Nepomuk::Vocabulary::NBIB::BachelorThesis();
        break;
    case BibType_Book:
        return Nepomuk::Vocabulary::NBIB::Book();
        break;
    case BibType_Booklet:
        return Nepomuk::Vocabulary::NBIB::Booklet();
        break;
    case BibType_Inproceedings:
        return Nepomuk::Vocabulary::NBIB::InProceedings();
        break;
    case BibType_Collection:
        return Nepomuk::Vocabulary::NBIB::Collection();
        break;
    case BibType_Incollection:
        return Nepomuk::Vocabulary::NBIB::InCollection();
        break;
    case BibType_Manual:
        return Nepomuk::Vocabulary::NBIB::Manual();
        break;
    case BibType_Mastersthesis:
        return Nepomuk::Vocabulary::NBIB::MastersThesis();
        break;
    case BibType_Misc:
        return Nepomuk::Vocabulary::NBIB::Publication();
        break;
    case BibType_Phdthesis:
        return Nepomuk::Vocabulary::NBIB::PhdThesis();
        break;
    case BibType_Proceedings:
        return Nepomuk::Vocabulary::NBIB::Proceedings();
        break;
    case BibType_Techreport:
        return Nepomuk::Vocabulary::NBIB::Techreport();
        break;
    case BibType_Unpublished:
        return Nepomuk::Vocabulary::NBIB::Unpublished();
        break;
    case BibType_Patent:
        return Nepomuk::Vocabulary::NBIB::Patent();
        break;
    case BibType_Electronic:
        return Nepomuk::Vocabulary::NBIB::Electronic();
        break;
    }

    return QUrl();
}

void PublicationWidget::selectLayout(BibEntryType entryType)
{
    switch(entryType) {
    case BibType_Article:
        layoutArticle();
        break;
    case BibType_Book:
        layoutBook();
        break;
    case BibType_Booklet:
        layoutBooklet();
        break;
    case BibType_Inproceedings:
        layoutInproceedings();
        break;
    case BibType_Incollection:
        layoutIncollection();
        break;
    case BibType_Manual:
        layoutManual();
        break;
    case BibType_Mastersthesis:
        layoutMastersthesis();
        break;
    case BibType_Misc:
        layoutMisc();
        break;
    case BibType_Phdthesis:
        layoutPhdthesis();
        break;
    case BibType_Proceedings:
        layoutProceedings();
        break;
    case BibType_Techreport:
        layoutTechreport();
        break;
    case BibType_Unpublished:
        layoutUnpublished();
        break;
    case BibType_Patent:
        layoutPatent();
        break;
    case BibType_Electronic:
        layoutElectronic();
        break;
    default:
        isbnWidget->setVisible(true);
        issnWidget->setVisible(true);
        copyrightWidget->setVisible(true);

        journalWidget->setVisible(true);
        publicationDateWidget->setVisible(true);
        volumeWidget->setVisible(true);

        publisherWidget->setVisible(true);

        numberWidget->setVisible(true);

        editionWidget->setVisible(true);
        editorWidget->setVisible(true);
        eprintWidget->setVisible(true);
        howpublishedWidget->setVisible(true);


        schoolWidget->setVisible(true);
        seriesWidget->setVisible(true);
        urlWidget->setVisible(true);
        typeWidget->setVisible(true);
    }
}

void PublicationWidget::layoutArticle()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    journalWidget->setVisible(true);
    publicationDateWidget->setVisible(true);
    volumeWidget->setVisible(true);
    numberWidget->setVisible(true);


    publisherWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    howpublishedWidget->setVisible(false);


    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void PublicationWidget::layoutBook()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    journalWidget->setVisible(false);
    publicationDateWidget->setVisible(true);
    volumeWidget->setVisible(true);
    publisherWidget->setVisible(true);
    editionWidget->setVisible(true);

    editorWidget->setVisible(true);

    numberWidget->setVisible(false);
    eprintWidget->setVisible(false);
    howpublishedWidget->setVisible(false);


    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void PublicationWidget::layoutBooklet()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);

    howpublishedWidget->setVisible(true);
    publisherWidget->setVisible(true);

    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);


    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void PublicationWidget::layoutCollection()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);

    howpublishedWidget->setVisible(true);
    publisherWidget->setVisible(true);

    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);


    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void PublicationWidget::layoutIncollection()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);

    howpublishedWidget->setVisible(false);
    publisherWidget->setVisible(true);
    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(true);
    eprintWidget->setVisible(false);


    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void PublicationWidget::layoutInproceedings()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);

    howpublishedWidget->setVisible(false);
    publisherWidget->setVisible(true);
    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(true);
    eprintWidget->setVisible(false);


    schoolWidget->setVisible(false);
    seriesWidget->setVisible(true);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void PublicationWidget::layoutManual()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);

    howpublishedWidget->setVisible(false);
    publisherWidget->setVisible(false);
    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);

    editionWidget->setVisible(true);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);


    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}
void PublicationWidget::layoutBachelorthesis()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);
    schoolWidget->setVisible(true);

    howpublishedWidget->setVisible(false);
    publisherWidget->setVisible(false);
    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);


    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void PublicationWidget::layoutMastersthesis()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);
    schoolWidget->setVisible(true);

    howpublishedWidget->setVisible(false);
    publisherWidget->setVisible(false);
    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);


    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void PublicationWidget::layoutMisc()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);

    howpublishedWidget->setVisible(true);
    publisherWidget->setVisible(true);
    journalWidget->setVisible(true);
    volumeWidget->setVisible(true);
    numberWidget->setVisible(true);

    editionWidget->setVisible(true);
    editorWidget->setVisible(true);
    eprintWidget->setVisible(true);


    schoolWidget->setVisible(true);
    seriesWidget->setVisible(true);
    urlWidget->setVisible(true);
    typeWidget->setVisible(true);
}

void PublicationWidget::layoutPhdthesis()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);
    schoolWidget->setVisible(true);

    howpublishedWidget->setVisible(false);
    publisherWidget->setVisible(false);
    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);


    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void PublicationWidget::layoutProceedings()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);


    howpublishedWidget->setVisible(false);
    publisherWidget->setVisible(true);
    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(true);
    eprintWidget->setVisible(false);

    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void PublicationWidget::layoutTechreport()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);

    typeWidget->setVisible(true);
    numberWidget->setVisible(true);

    howpublishedWidget->setVisible(false);
    publisherWidget->setVisible(false);
    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);

    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
}

void PublicationWidget::layoutUnpublished()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(false);
    issnWidget->setVisible(false);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);

    howpublishedWidget->setVisible(false);
    publisherWidget->setVisible(false);
    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);


    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void PublicationWidget::layoutPatent()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(false);
    issnWidget->setVisible(false);
    copyrightWidget->setVisible(false);

    publicationDateWidget->setVisible(true);

    howpublishedWidget->setVisible(true);
    publisherWidget->setVisible(true);



    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void PublicationWidget::layoutElectronic()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(false);
    issnWidget->setVisible(false);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);
    howpublishedWidget->setVisible(true);
    publisherWidget->setVisible(true);


    urlWidget->setVisible(true);

    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);

    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    typeWidget->setVisible(false);
}
