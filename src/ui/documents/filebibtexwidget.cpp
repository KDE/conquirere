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

#include "filebibtexwidget.h"
#include "../semantic/labeledit.h"
#include "../semantic/contactedit.h"
#include "../semantic/addressedit.h"

#include "nbib.h"
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NIE>
#include <KComboBox>
#include <KGlobalSettings>
#include <Nepomuk/Variant>
#include <Nepomuk/File>
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Types/Ontology>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRegExp>

#include <QVariant>
#include <QDebug>

FileBibTexWidget::FileBibTexWidget(QWidget *parent)
    : QWidget(parent)
{
    hide();

    setupWidget();

    setFont(KGlobalSettings::smallestReadableFont());
}

FileBibTexWidget::~FileBibTexWidget()
{
}

void FileBibTexWidget::setResource(Nepomuk::Resource & resource)
{
    // what we get is a nfo::document or similar
    // what we want is the connected nbib::BibResource
    Nepomuk::Resource nr = resource.property(Nepomuk::Vocabulary::NBIB::bibResourceType()).toResource();
    if(!nr.isValid()) {
        //create a new resource if nothing is connected
        Nepomuk::Resource nb;
        QList<QUrl> types;
        types.append(Nepomuk::Vocabulary::NBIB::BibResource());
        nb.setTypes(types);

        // link document to resource
        resource.setProperty( Nepomuk::Vocabulary::NBIB::bibResourceType(), nb);
    }

    m_resource  = resource.property(Nepomuk::Vocabulary::NBIB::bibResourceType()).toResource();

    emit resourceChanged(m_resource);

    BibEntryType entryType = resourceTypeToEnum(m_resource);

    int index = entryTypeData->findData(entryType);
    entryTypeData->setCurrentIndex(index);

    selectLayout(entryType);

    show();
}

void FileBibTexWidget::clear()
{
    hide();
}

void FileBibTexWidget::newBibEntryTypeSelected(int index)
{
    KComboBox *kcb = qobject_cast<KComboBox *>(sender());
    BibEntryType entryType = (BibEntryType)kcb->itemData(index).toInt();

    selectLayout(entryType);

    // update resource
    QUrl newEntryUrl = EnumToResourceType(entryType);
    if(newEntryUrl.isValid()) {
        QList<QUrl>newtype;
        newtype.append(newEntryUrl);
        m_resource.setTypes(newtype);
    }
    else {
        QList<QUrl>newtype;
        newtype.append(Nepomuk::Vocabulary::NBIB::BibResource());
        m_resource.setTypes(newtype);
    }
}

void FileBibTexWidget::setupWidget()
{
    QVBoxLayout *layoutMain = new QVBoxLayout;
    layoutMain->setMargin(0);
    layoutMain->setSpacing(0);
    setLayout(layoutMain);

    QLabel *entryType = new QLabel(i18n("Entry type:"));
    entryType->setToolTip(i18n("The type of this document"));

    entryTypeData = new KComboBox();
    entryTypeData->setProperty("datatype", BibData_EntryType);
    entryTypeData->addItem(i18n("Unknown"),BibType_Unknown);
    entryTypeData->addItem(i18n("Article"),BibType_Article);
    entryTypeData->addItem(i18n("Book"),BibType_Book);
    entryTypeData->addItem(i18n("Booklet"),BibType_Booklet);
    entryTypeData->addItem(i18n("Proceedings"),BibType_Proceedings);
    entryTypeData->addItem(i18n("Inproceedings"),BibType_Inproceedings);
    entryTypeData->addItem(i18n("Collection"),BibType_Collection);
    entryTypeData->addItem(i18n("Incollection"),BibType_Incollection);
    entryTypeData->addItem(i18n("Manual"),BibType_Manual);
    entryTypeData->addItem(i18n("Mastersthesis"),BibType_Mastersthesis);
    entryTypeData->addItem(i18n("Bachelorhesis"),BibType_Bachelorthesis);
    entryTypeData->addItem(i18n("Misc"),BibType_Misc);
    entryTypeData->addItem(i18n("Patent"),BibType_Patent);
    entryTypeData->addItem(i18n("Phdthesis"),BibType_Phdthesis);
    entryTypeData->addItem(i18n("Techreport"),BibType_Techreport);
    entryTypeData->addItem(i18n("Unpublished"),BibType_Unpublished);
    entryTypeData->addItem(i18n("Electronic"),BibType_Electronic);
    entryType->setBuddy(entryTypeData);

    connect(entryTypeData, SIGNAL(currentIndexChanged(int)), this, SLOT(newBibEntryTypeSelected(int)));

    QHBoxLayout *layoutEntry = new QHBoxLayout;
    layoutEntry->addWidget(entryType);
    layoutEntry->addWidget(entryTypeData);
    layoutMain->addLayout(layoutEntry);

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
    layoutMain->addLayout(layoutTitle);

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
    layoutMain->addLayout(layoutAuthor);

    //####################################################################

    QFrame *headerLine = new QFrame();
    headerLine->setFrameShape(QFrame::HLine);
    headerLine->setFrameShadow(QFrame::Sunken);
    layoutMain->addWidget(headerLine);

    //####################################################################

    chapterWidget = new QWidget;
    QLabel *chapter = new QLabel(i18n("Chapter:"));
    chapter->setToolTip(i18n("The chapter number"));

//    LabelEdit *chapterData = new LabelEdit();
//    chapterData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::chapter() );
//    chapter->setBuddy(chapterData);
//    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), chapterData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutChapter = new QHBoxLayout;
    layoutChapter->setMargin(0);
    layoutChapter->setSpacing(0);
    layoutChapter->addWidget(chapter);
    //layoutChapter->addWidget(chapterData);
    chapterWidget->setLayout(layoutChapter);
    layoutMain->addWidget(chapterWidget);

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
    layoutMain->addWidget(seriesWidget);

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
    layoutMain->addWidget(publicationDateWidget);

    //####################################################################

    journalWidget = new QWidget;
    QLabel *journal = new QLabel(i18n("Journal:"));
    journal->setToolTip(i18n("The journal or magazine the work was published in"));


    ContactEdit *journalData = new ContactEdit();
    journalData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::inJournal() );
    journal->setBuddy(journalData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), journalData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutJournal = new QHBoxLayout;
    layoutJournal->setMargin(0);
    layoutJournal->setSpacing(0);
    layoutJournal->addWidget(journal);
    layoutJournal->addWidget(journalData);
    journalWidget->setLayout(layoutJournal);
    layoutMain->addWidget(journalWidget);

    //####################################################################

    volumeWidget = new QWidget;
    QLabel *volume = new QLabel(i18n("Volume:"));
    volume->setToolTip(i18n("The volume of a journal or multi-volume book"));

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
    layoutMain->addWidget(volumeWidget);

    //####################################################################

    numberWidget = new QWidget;
    QLabel *number = new QLabel(i18n("Number:"));
    number->setToolTip(i18n("The &quot;(issue) number&quot; of a journal, magazine, or tech-report, if applicable. (Most publications have a &quot;volume&quot;, but no &quot;number&quot; field.)"));

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
    layoutMain->addWidget(numberWidget);

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
    layoutMain->addWidget(publisherWidget);

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
    layoutMain->addWidget(editionWidget);

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
    layoutMain->addWidget(editorWidget);

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
    layoutMain->addWidget(eprintWidget);

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
    layoutMain->addWidget(howpublishedWidget);

    //####################################################################

    institutionWidget = new QWidget;
    QLabel *institution = new QLabel(i18n("Institution:"));
    institution->setToolTip(i18n("The institution that was involved in the publishing, but not necessarily the publisher"));

    //Todo institution part of publisher as role
    LabelEdit *institutionData = new LabelEdit();
    institutionData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::institution() );
    institution->setBuddy(institutionData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), institutionData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutInstitution = new QHBoxLayout;
    layoutInstitution->setMargin(0);
    layoutInstitution->setSpacing(0);
    layoutInstitution->addWidget(institution);
    layoutInstitution->addWidget(institutionData);
    institutionWidget->setLayout(layoutInstitution);
    layoutMain->addWidget(institutionWidget);

    //####################################################################

    organizationWidget = new QWidget;
    QLabel *organization = new QLabel(i18n("Organization:"));
    organization->setToolTip(i18n("The conference sponsor"));

    LabelEdit *organizationData = new LabelEdit();
    organizationData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::organization() );
    organization->setBuddy(organizationData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), organizationData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutOrganization = new QHBoxLayout;
    layoutOrganization->setMargin(0);
    layoutOrganization->setSpacing(0);
    layoutOrganization->addWidget(organization);
    layoutOrganization->addWidget(organizationData);
    organizationWidget->setLayout(layoutOrganization);
    layoutMain->addWidget(organizationWidget);

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
    layoutMain->addWidget(schoolWidget);

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
    layoutMain->addWidget(urlWidget);

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
    layoutMain->addWidget(typeWidget);

    //####################################################################

    QFrame *identifierLine = new QFrame();
    identifierLine->setFrameShape(QFrame::HLine);
    identifierLine->setFrameShadow(QFrame::Sunken);
    layoutMain->addWidget(identifierLine);

    //####################################################################

    crossrefWidget = new QWidget;
    QLabel *crossref = new QLabel(i18n("Crossref:"));
    crossref->setToolTip(i18n("The International Standard Book Number."));

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
    layoutMain->addWidget(crossrefWidget);

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
    layoutMain->addWidget(isbnWidget);

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
    layoutMain->addWidget(issnWidget);

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
    layoutMain->addWidget(lccnWidget);

    //####################################################################

    mrnumberWidget = new QWidget;
    QLabel *mrnumber = new QLabel(i18n("MRNumber:"));
    mrnumber->setToolTip(i18n("The Mathematical Reviews number."));

    LabelEdit *mrnumberData = new LabelEdit();
    mrnumberData->setPropertyUrl( Nepomuk::Vocabulary::NBIB::mrnumber() );
    mrnumber->setBuddy(mrnumberData);
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), mrnumberData, SLOT(setResource(Nepomuk::Resource&)));

    QHBoxLayout *layoutmrnumber = new QHBoxLayout;
    layoutmrnumber->setMargin(0);
    layoutmrnumber->setSpacing(0);
    layoutmrnumber->addWidget(mrnumber);
    layoutmrnumber->addWidget(mrnumberData);
    mrnumberWidget->setLayout(layoutmrnumber);
    layoutMain->addWidget(mrnumberWidget);

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
    layoutMain->addWidget(doiWidget);

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
    layoutMain->addWidget(copyrightWidget);

    //####################################################################

    QSpacerItem *s = new QSpacerItem(10,10,QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    layoutMain->insertSpacerItem(-1,s);
}

BibEntryType FileBibTexWidget::resourceTypeToEnum(Nepomuk::Resource & resource)
{
    Nepomuk::Resource checkType( resource.resourceType() );
    QString resourceLabel = checkType.genericLabel();
    qDebug() << "resourceTypeToEnum" << resourceLabel;
    //QString resourceLabel = resource.property(Nepomuk::Vocabulary::NBIB::bibResourceType()).toResource().genericLabel();

    if(resourceLabel == QLatin1String("Article")) {
        return BibType_Article;
    }
    if(resourceLabel == QLatin1String("Bachelorthesis")) {
        return BibType_Bachelorthesis;
    }
    if(resourceLabel == QLatin1String("Book")) {
        return BibType_Book;
    }
    if(resourceLabel == QLatin1String("Booklet")) {
        return BibType_Booklet;
    }
    if(resourceLabel == QLatin1String("Conference")) {
        return BibType_Booklet;
    }
    if(resourceLabel == QLatin1String("Inbook")) {
        return BibType_Inbook;
    }
    if(resourceLabel == QLatin1String("Inproceedings")) {
        return BibType_Inproceedings;
    }
    if(resourceLabel == QLatin1String("Incollection")) {
        return BibType_Incollection;
    }
    if(resourceLabel == QLatin1String("Manual")) {
        return BibType_Manual;
    }
    if(resourceLabel == QLatin1String("Mastersthesis")) {
        return BibType_Mastersthesis;
    }
    if(resourceLabel == QLatin1String("Misc")) {
        return BibType_Misc;
    }
    if(resourceLabel == QLatin1String("Phdthesis")) {
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
    return BibType_Unknown;
}

QUrl FileBibTexWidget::EnumToResourceType(BibEntryType entryType)
{
    switch(entryType) {
    case BibType_Article:
        return Nepomuk::Vocabulary::NBIB::Article();
        break;
    case BibType_Bachelorthesis:
        return Nepomuk::Vocabulary::NBIB::Bachelorthesis();
        break;
    case BibType_Book:
        return Nepomuk::Vocabulary::NBIB::Book();
        break;
    case BibType_Booklet:
        return Nepomuk::Vocabulary::NBIB::Booklet();
        break;
    case BibType_Inproceedings:
        return Nepomuk::Vocabulary::NBIB::Inproceedings();
        break;
    case BibType_Collection:
        return Nepomuk::Vocabulary::NBIB::Collection();
        break;
    case BibType_Incollection:
        return Nepomuk::Vocabulary::NBIB::Incollection();
        break;
    case BibType_Manual:
        return Nepomuk::Vocabulary::NBIB::Manual();
        break;
    case BibType_Mastersthesis:
        return Nepomuk::Vocabulary::NBIB::Mastersthesis();
        break;
    case BibType_Misc:
        return Nepomuk::Vocabulary::NBIB::Misc();
        break;
    case BibType_Phdthesis:
        return Nepomuk::Vocabulary::NBIB::Phdthesis();
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
    case BibType_Unknown:
        return Nepomuk::Vocabulary::NBIB::BibResource();
        break;
    case BibType_Inbook:
        return QUrl();
    }

    return QUrl();
}

void FileBibTexWidget::selectLayout(BibEntryType entryType)
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
        chapterWidget->setVisible(true);
        editionWidget->setVisible(true);
        editorWidget->setVisible(true);
        eprintWidget->setVisible(true);
        howpublishedWidget->setVisible(true);
        institutionWidget->setVisible(true);
        organizationWidget->setVisible(true);
        schoolWidget->setVisible(true);
        seriesWidget->setVisible(true);
        urlWidget->setVisible(true);
        typeWidget->setVisible(true);
    }
}

void FileBibTexWidget::layoutArticle()
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
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    howpublishedWidget->setVisible(false);
    institutionWidget->setVisible(false);
    organizationWidget->setVisible(false);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void FileBibTexWidget::layoutBook()
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
    chapterWidget->setVisible(true);
    editorWidget->setVisible(true);

    numberWidget->setVisible(false);
    eprintWidget->setVisible(false);
    howpublishedWidget->setVisible(false);
    institutionWidget->setVisible(false);
    organizationWidget->setVisible(false);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void FileBibTexWidget::layoutBooklet()
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
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    institutionWidget->setVisible(false);
    organizationWidget->setVisible(false);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void FileBibTexWidget::layoutCollection()
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
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    institutionWidget->setVisible(false);
    organizationWidget->setVisible(false);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void FileBibTexWidget::layoutIncollection()
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
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(true);
    eprintWidget->setVisible(false);
    institutionWidget->setVisible(false);
    organizationWidget->setVisible(true);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void FileBibTexWidget::layoutInproceedings()
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
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(true);
    eprintWidget->setVisible(false);
    institutionWidget->setVisible(false);
    organizationWidget->setVisible(true);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(true);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void FileBibTexWidget::layoutManual()
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
    chapterWidget->setVisible(false);
    editionWidget->setVisible(true);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    institutionWidget->setVisible(false);
    organizationWidget->setVisible(true);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}
void FileBibTexWidget::layoutBachelorthesis()
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
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    institutionWidget->setVisible(false);
    organizationWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void FileBibTexWidget::layoutMastersthesis()
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
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    institutionWidget->setVisible(false);
    organizationWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void FileBibTexWidget::layoutMisc()
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
    chapterWidget->setVisible(true);
    editionWidget->setVisible(true);
    editorWidget->setVisible(true);
    eprintWidget->setVisible(true);
    institutionWidget->setVisible(true);
    organizationWidget->setVisible(true);
    schoolWidget->setVisible(true);
    seriesWidget->setVisible(true);
    urlWidget->setVisible(true);
    typeWidget->setVisible(true);
}

void FileBibTexWidget::layoutPhdthesis()
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
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    institutionWidget->setVisible(false);
    organizationWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void FileBibTexWidget::layoutProceedings()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);
    organizationWidget->setVisible(true);

    howpublishedWidget->setVisible(false);
    publisherWidget->setVisible(true);
    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(true);
    eprintWidget->setVisible(false);
    institutionWidget->setVisible(false);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void FileBibTexWidget::layoutTechreport()
{
    mrnumberWidget->setVisible(true);
    lccnWidget->setVisible(true);
    doiWidget->setVisible(true);
    isbnWidget->setVisible(true);
    issnWidget->setVisible(true);
    copyrightWidget->setVisible(true);

    publicationDateWidget->setVisible(true);
    institutionWidget->setVisible(true);
    typeWidget->setVisible(true);
    numberWidget->setVisible(true);

    howpublishedWidget->setVisible(false);
    publisherWidget->setVisible(false);
    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    organizationWidget->setVisible(false);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
}

void FileBibTexWidget::layoutUnpublished()
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
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    institutionWidget->setVisible(false);
    organizationWidget->setVisible(false);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void FileBibTexWidget::layoutPatent()
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
    institutionWidget->setVisible(true);
    organizationWidget->setVisible(true);

    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    urlWidget->setVisible(false);
    typeWidget->setVisible(false);
}

void FileBibTexWidget::layoutElectronic()
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
    institutionWidget->setVisible(true);
    organizationWidget->setVisible(true);
    urlWidget->setVisible(true);

    journalWidget->setVisible(false);
    volumeWidget->setVisible(false);
    numberWidget->setVisible(false);
    chapterWidget->setVisible(false);
    editionWidget->setVisible(false);
    editorWidget->setVisible(false);
    eprintWidget->setVisible(false);
    schoolWidget->setVisible(false);
    seriesWidget->setVisible(false);
    typeWidget->setVisible(false);
}
