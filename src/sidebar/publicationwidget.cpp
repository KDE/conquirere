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
#include "ui_publicationwidget.h"

#include "propertywidgets/stringedit.h"
#include "propertywidgets/contactedit.h"

#include "referencewidget.h"

#include "nbib.h"
#include <KComboBox>
#include <KDialog>
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <QDebug>
#include <QSpacerItem>

PublicationWidget::PublicationWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::PublicationWidget)
{
    ui->setupUi(this);

    ui->addReference->setIcon(KIcon(QLatin1String("list-add")));

    setupWidget();
}

PublicationWidget::~PublicationWidget()
{
    delete ui;
}

void PublicationWidget::setResource(Nepomuk::Resource & resource)
{
    m_publication = resource;

    //check if the resource has a publication attached
    if(!m_publication.isValid()) {
        ui->tabWidget->setEnabled(false);
    }
    else {
        ui->tabWidget->setEnabled(true);
    }

    emit resourceChanged(m_publication);

    BibEntryType entryType = resourceTypeToEnum(m_publication);

    int index = ui->editEntryType->findData(entryType);
    ui->editEntryType->setCurrentIndex(index);

    selectLayout(entryType);
}

void PublicationWidget::newBibEntryTypeSelected(int index)
{
    KComboBox *kcb = qobject_cast<KComboBox *>(sender());
    BibEntryType entryType = (BibEntryType)kcb->itemData(index).toInt();

    selectLayout(entryType);

    // update resource
    QUrl newEntryUrl = EnumToResourceType(entryType);
    if(newEntryUrl.isValid()) {
        // create the full hierarchy
        //DEBUG this seems wrong, but is currently the only way to preserve type hierarchy
        QList<QUrl>newtype;
        newtype.append(Nepomuk::Vocabulary::NIE::InformationElement());
        newtype.append(Nepomuk::Vocabulary::NBIB::Publication());
        newtype.append(newEntryUrl);

        // add another hierarchy if the newEntryUrl is not a direct subclass of NBIB::Publication()
        switch(entryType) {
        case BibType_JournalIssue:
            newtype.append(Nepomuk::Vocabulary::NBIB::Collection());
            break;
        }

        m_publication.setTypes(newtype);
    }
    else {
        //QList<QUrl>newtype;
        //newtype.append(Nepomuk::Vocabulary::NBIB::Publication());
        //m_publication.setTypes(newtype);
        qDebug() << "unknwon newEntryUrl url. this should never happen";
    }
}

void PublicationWidget::selectPublication()
{
    qDebug() << "select from a list of systemwide publications";
}

void PublicationWidget::newButtonClicked()
{
    //create a new resource if nothing is connected
    Nepomuk::Resource nb;
    QList<QUrl> types;
    types.append(Nepomuk::Vocabulary::NBIB::Publication());
    nb.setTypes(types);

    setResource(nb);
}

void PublicationWidget::deleteButtonClicked()
{
    // link document to resource
    m_publication.remove();

    setResource(m_publication);
}

void PublicationWidget::setDialogMode(bool dialogMode)
{
    ui->addReference->setVisible(false);
}

void PublicationWidget::addReference()
{
    KDialog showRefWidget;

    Nepomuk::Resource tempRef(QUrl(), Nepomuk::Vocabulary::NBIB::Reference());
    tempRef.setProperty(Nepomuk::Vocabulary::NBIB::publication(), m_publication);

    ReferenceWidget *rw = new ReferenceWidget();
    rw->setDialogMode(true);
    rw->setResource(tempRef);
    rw->setLibrary(library());

    showRefWidget.setMainWidget(rw);
    showRefWidget.setInitialSize(QSize(300,300));

    int ret = showRefWidget.exec();

    if(ret == KDialog::Accepted) {
        //m_publication.setProperty(Nepomuk::Vocabulary::NBIB::, tempRef);
    }
    else {
        // remove temp citation again
        tempRef.remove();
    }
}

void PublicationWidget::setupWidget()
{
    ui->editEntryType->setProperty("datatype", BibData_EntryType);
    ui->editEntryType->addItem(i18n("Misc"),BibType_Misc);
    ui->editEntryType->addItem(i18n("Article"),BibType_Article);
    ui->editEntryType->addItem(i18n("Book"),BibType_Book);
    ui->editEntryType->addItem(i18n("Booklet"),BibType_Booklet);
    ui->editEntryType->addItem(i18n("Collection"),BibType_Collection);
    ui->editEntryType->addItem(i18n("Proceedings"),BibType_Proceedings);
    ui->editEntryType->addItem(i18n("Inproceedings"),BibType_Inproceedings);
    ui->editEntryType->addItem(i18n("Journal Issue"),BibType_JournalIssue);
    ui->editEntryType->addItem(i18n("Bachelorhesis"),BibType_Bachelorthesis);
    ui->editEntryType->addItem(i18n("Mastersthesis"),BibType_Mastersthesis);
    ui->editEntryType->addItem(i18n("Phdthesis"),BibType_Phdthesis);
    ui->editEntryType->addItem(i18n("Manual"),BibType_Manual);
    ui->editEntryType->addItem(i18n("Techreport"),BibType_Techreport);
    ui->editEntryType->addItem(i18n("Unpublished"),BibType_Unpublished);
    ui->editEntryType->addItem(i18n("Electronic"),BibType_Electronic);
    ui->editEntryType->addItem(i18n("Patent"),BibType_Patent);

    connect(ui->editEntryType, SIGNAL(currentIndexChanged(int)), this, SLOT(newBibEntryTypeSelected(int)));

    ui->editAuthors->setPropertyUrl( Nepomuk::Vocabulary::NCO::creator() );
    ui->editCopyright->setPropertyUrl( Nepomuk::Vocabulary::NIE::copyright() );
    ui->editCrossref->setPropertyUrl( Nepomuk::Vocabulary::NIE::links() );
    ui->editDate->setPropertyUrl( Nepomuk::Vocabulary::NBIB::publicationDate() );
    ui->editDOI->setPropertyUrl( Nepomuk::Vocabulary::NBIB::doi() );
    ui->editEdition->setPropertyUrl( Nepomuk::Vocabulary::NBIB::edition() );
    ui->editEditor->setPropertyUrl( Nepomuk::Vocabulary::NBIB::editor() );
    ui->editEprint->setPropertyUrl( Nepomuk::Vocabulary::NBIB::eprint() );
    ui->editHowPublished->setPropertyUrl( Nepomuk::Vocabulary::NBIB::publicationMethod() );
    ui->editISBN->setPropertyUrl( Nepomuk::Vocabulary::NBIB::isbn() );
    ui->editISSN->setPropertyUrl( Nepomuk::Vocabulary::NBIB::issn() );
    ui->editJournal->setPropertyUrl( Nepomuk::Vocabulary::NBIB::journalIssue() );
    ui->editLCCN->setPropertyUrl( Nepomuk::Vocabulary::NBIB::lccn() );
    ui->editMRNumber->setPropertyUrl( Nepomuk::Vocabulary::NBIB::mrNumber() );
    ui->editNumber->setPropertyUrl( Nepomuk::Vocabulary::NBIB::issueNumber() );
    ui->editPublisher->setPropertyUrl( Nepomuk::Vocabulary::NCO::publisher() );
    ui->editSeries->setPropertyUrl( Nepomuk::Vocabulary::NBIB::inSeries() );
    ui->editTitle->setPropertyUrl( Nepomuk::Vocabulary::NIE::title() );
    ui->editType->setPropertyUrl( Nepomuk::Vocabulary::NBIB::type() );
    ui->editVolume->setPropertyUrl( Nepomuk::Vocabulary::NBIB::volume() );

    //connect signal/slots
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editAuthors, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editCopyright, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editCrossref, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editDate, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editDOI, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editEdition, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editEditor, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editEprint, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editHowPublished, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editISBN, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editISSN, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editJournal, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editLCCN, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editMRNumber, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editNumber, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editPublisher, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editSeries, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editTitle, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editType, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editVolume, SLOT(setResource(Nepomuk::Resource&)));
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
    if(resourceLabel == QLatin1String("JournalIssue")) {
        return BibType_JournalIssue;
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
    case BibType_JournalIssue:
        return Nepomuk::Vocabulary::NBIB::JournalIssue();
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
    case BibType_JournalIssue:
        layoutJournalIssue();
        break;
        layoutMisc();
    }
}

void PublicationWidget::layoutArticle()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editCopyright->setEnabled(true);

    ui->editJournal->setEnabled(true);
    ui->editDate->setEnabled(true);
    ui->editVolume->setEnabled(true);
    ui->editNumber->setEnabled(true);


    ui->editPublisher->setEnabled(false);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(false);
    ui->editEprint->setEnabled(false);
    ui->editHowPublished->setEnabled(false);



    ui->editSeries->setEnabled(false);
    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutBook()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editCopyright->setEnabled(true);

    ui->editJournal->setEnabled(false);
    ui->editDate->setEnabled(true);
    ui->editVolume->setEnabled(true);
    ui->editPublisher->setEnabled(true);
    ui->editEdition->setEnabled(true);

    ui->editEditor->setEnabled(true);

    ui->editNumber->setEnabled(false);
    ui->editEprint->setEnabled(false);
    ui->editHowPublished->setEnabled(false);



    ui->editSeries->setEnabled(false);

    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutBooklet()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);

    ui->editHowPublished->setEnabled(true);
    ui->editPublisher->setEnabled(true);

    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(false);
    ui->editEprint->setEnabled(false);



    ui->editSeries->setEnabled(false);

    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutCollection()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);

    ui->editHowPublished->setEnabled(true);
    ui->editPublisher->setEnabled(true);

    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(false);
    ui->editEprint->setEnabled(false);



    ui->editSeries->setEnabled(false);

    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutInproceedings()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);

    ui->editHowPublished->setEnabled(false);
    ui->editPublisher->setEnabled(true);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(true);
    ui->editEprint->setEnabled(false);



    ui->editSeries->setEnabled(true);

    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutManual()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);

    ui->editHowPublished->setEnabled(false);
    ui->editPublisher->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);

    ui->editEdition->setEnabled(true);
    ui->editEditor->setEnabled(false);
    ui->editEprint->setEnabled(false);



    ui->editSeries->setEnabled(false);

    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutBachelorthesis()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);


    ui->editHowPublished->setEnabled(false);
    ui->editPublisher->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(false);
    ui->editEprint->setEnabled(false);


    ui->editSeries->setEnabled(false);

    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutMastersthesis()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);


    ui->editHowPublished->setEnabled(false);
    ui->editPublisher->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(false);
    ui->editEprint->setEnabled(false);


    ui->editSeries->setEnabled(false);

    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutMisc()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);

    ui->editHowPublished->setEnabled(true);
    ui->editPublisher->setEnabled(true);
    ui->editJournal->setEnabled(true);
    ui->editVolume->setEnabled(true);
    ui->editNumber->setEnabled(true);

    ui->editEdition->setEnabled(true);
    ui->editEditor->setEnabled(true);
    ui->editEprint->setEnabled(true);



    ui->editSeries->setEnabled(true);

    ui->editType->setEnabled(true);
}

void PublicationWidget::layoutPhdthesis()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);


    ui->editHowPublished->setEnabled(false);
    ui->editPublisher->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(false);
    ui->editEprint->setEnabled(false);


    ui->editSeries->setEnabled(false);

    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutProceedings()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);


    ui->editHowPublished->setEnabled(false);
    ui->editPublisher->setEnabled(true);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(true);
    ui->editEprint->setEnabled(false);


    ui->editSeries->setEnabled(false);

    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutTechreport()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);

    ui->editType->setEnabled(true);
    ui->editNumber->setEnabled(true);

    ui->editHowPublished->setEnabled(false);
    ui->editPublisher->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(false);
    ui->editEprint->setEnabled(false);


    ui->editSeries->setEnabled(false);

}

void PublicationWidget::layoutUnpublished()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(false);
    ui->editISSN->setEnabled(false);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);

    ui->editHowPublished->setEnabled(false);
    ui->editPublisher->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(false);
    ui->editEprint->setEnabled(false);



    ui->editSeries->setEnabled(false);

    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutPatent()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(false);
    ui->editISSN->setEnabled(false);
    ui->editCopyright->setEnabled(false);

    ui->editDate->setEnabled(true);

    ui->editHowPublished->setEnabled(true);
    ui->editPublisher->setEnabled(true);



    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(false);
    ui->editEprint->setEnabled(false);

    ui->editSeries->setEnabled(false);

    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutElectronic()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(false);
    ui->editISSN->setEnabled(false);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);
    ui->editHowPublished->setEnabled(true);
    ui->editPublisher->setEnabled(true);




    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(false);
    ui->editEprint->setEnabled(false);

    ui->editSeries->setEnabled(false);
    ui->editType->setEnabled(false);
}

void PublicationWidget::layoutJournalIssue()
{
    ui->editMRNumber->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editDOI->setEnabled(true);
    ui->editISBN->setEnabled(false);
    ui->editISSN->setEnabled(false);
    ui->editCopyright->setEnabled(true);

    ui->editDate->setEnabled(true);
    ui->editHowPublished->setEnabled(true);
    ui->editPublisher->setEnabled(true);



    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(true);
    ui->editNumber->setEnabled(true);

    ui->editEdition->setEnabled(false);
    ui->editEditor->setEnabled(false);
    ui->editEprint->setEnabled(true);

    ui->editSeries->setEnabled(false);
    ui->editType->setEnabled(false);
}
