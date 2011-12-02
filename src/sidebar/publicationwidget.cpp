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
#include "propertywidgets/fileobjectedit.h"

#include "referencewidget.h"
#include "contactdialog.h"
#include "addchapterdialog.h"
#include "../core/library.h"

#include "nbib.h"
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NUAO>
#include <Soprano/Vocabulary/NAO>
#include <KDE/KComboBox>
#include <KDE/KDialog>

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QMenu>
#include <QtGui/QAction>

#include <QtCore/QDebug>

PublicationWidget::PublicationWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::PublicationWidget)
{
    ui->setupUi(this);

    ui->tabWidget->setEnabled(false);

    setupWidget();

    if(parent) {
        connect(this, SIGNAL(hasReference(bool)), parent, SLOT(hasReference(bool)));
    }
}

PublicationWidget::~PublicationWidget()
{
    delete ui;
}

void PublicationWidget::setResource(Nepomuk::Resource & resource)
{
    m_publication = resource;

    //check if the resource is a valid publication
    if(!m_publication.isValid()) {
        ui->tabWidget->setEnabled(false);
    }
    else {
        ui->tabWidget->setEnabled(true);
    }

    emit resourceChanged(m_publication);

    QList<Nepomuk::Resource> references = m_publication.property(Nepomuk::Vocabulary::NBIB::reference()).toResourceList();

    if(references.isEmpty()) {
        emit hasReference(false);
    }
    else {
        emit hasReference(true);
    }

    BibEntryType entryType = BibEntryTypeFromUrl(m_publication);

    int index = ui->editEntryType->findData(entryType);
    ui->editEntryType->setCurrentIndex(index);

    QString abstract = m_publication.property(Nepomuk::Vocabulary::NBIB::abstract()).toString();
    ui->editAbstract->document()->setPlainText(abstract);

    //set the rating
    ui->editRating->setRating(m_publication.rating());

    selectLayout(entryType);

    ui->listPartsWidget->setResource(m_publication);
}

void PublicationWidget::newBibEntryTypeSelected(int index)
{
    KComboBox *kcb = qobject_cast<KComboBox *>(sender());
    BibEntryType entryType = (BibEntryType)kcb->itemData(index).toInt();

    selectLayout(entryType);

    // update resource
    QUrl newEntryUrl = BibEntryTypeURL.at(entryType);
    if(!m_publication.hasType(newEntryUrl)) {
        // create the full hierarchy
        //DEBUG this seems wrong, but is currently the only way to preserve type hierarchy
        QList<QUrl>newtype;
        newtype.append(Nepomuk::Vocabulary::NIE::InformationElement());
        newtype.append(Nepomuk::Vocabulary::NBIB::Publication());
        newtype.append(newEntryUrl);

        // add another hierarchy if the newEntryUrl is not a direct subclass of NBIB::Publication()
        switch(entryType) {
        case BibType_MagazinIssue:
        case BibType_NewspaperIssue:
        case BibType_JournalIssue:
            newtype.append(Nepomuk::Vocabulary::NBIB::Collection());
            break;
        case BibType_Bachelorthesis:
        case BibType_Mastersthesis:
        case BibType_Phdthesis:
            newtype.append(Nepomuk::Vocabulary::NBIB::Thesis());
            break;
        case BibType_BlogPost:
        case BibType_ForumPost:
        case BibType_WebPage:
            newtype.append(Nepomuk::Vocabulary::NBIB::Electronic());
            break;
        case BibType_Bill:
        case BibType_Decision:
        case BibType_Brief:
        case BibType_Statute:
            newtype.append(Nepomuk::Vocabulary::NBIB::LegalDocument());
            break;
        }

        if(m_publication.isValid()) {
            m_publication.setTypes(newtype);
        }

        // a special case when the new type is a collection or subclass of a collection
        // change also the type of any connected Series.
        // this ensures we don't end up with a JournalIssue from Magazin or NewspaperIssue from a Journal
        if(m_publication.hasType(Nepomuk::Vocabulary::NBIB::Collection())) {
            Nepomuk::Resource seriesResource = m_publication.property((Nepomuk::Vocabulary::NBIB::inSeries())).toResource();

            if(seriesResource.isValid()) {
                if(m_publication.hasType(Nepomuk::Vocabulary::NBIB::JournalIssue())) {
                    Nepomuk::Resource x(QUrl(), Nepomuk::Vocabulary::NBIB::Journal());
                    seriesResource.setTypes(x.types());
                }
                else if(m_publication.hasType(Nepomuk::Vocabulary::NBIB::NewspaperIssue())) {
                    Nepomuk::Resource x(QUrl(), Nepomuk::Vocabulary::NBIB::Newspaper());
                    seriesResource.setTypes(x.types());
                }
                else if(m_publication.hasType(Nepomuk::Vocabulary::NBIB::MagazinIssue())) {
                    Nepomuk::Resource x(QUrl(), Nepomuk::Vocabulary::NBIB::Magazin());
                    seriesResource.setTypes(x.types());
                }
                else {
                    Nepomuk::Resource x(QUrl(), Nepomuk::Vocabulary::NBIB::Series());
                    seriesResource.setTypes(x.types());
                }
            }
        }
    }
}

void PublicationWidget::selectPublication()
{
    qDebug() << "select from a list of systemwide publications";
}

void PublicationWidget::newButtonClicked()
{
    //create a new resource
    Nepomuk::Resource nb;
    QList<QUrl> types;
    types.append(Nepomuk::Vocabulary::NBIB::Publication());
    nb.setTypes(types);

    setResource(nb);
}

void PublicationWidget::deleteButtonClicked()
{
    m_publication.remove();

    setResource(m_publication);
}

void PublicationWidget::setLibrary(Library *p)
{
    SidebarComponent::setLibrary(p);

    //TODO remove and use ResourceWatcher later on
    connect(ui->editAuthors, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editCopyright, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editDate, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editDOI, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editEdition, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editEditor, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editEprint, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editHowPublished, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editISBN, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editISSN, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editJournal, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editLCCN, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editMRNumber, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editNumber, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editPublisher, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editSeries, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editTitle, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editType, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editVolume, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editFileObject, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editRemoteObject, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editOrganization, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editLastAccessed, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editKeywords, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editProceedings, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editPubMed, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));

    connect(this, SIGNAL(resourceUpdated(Nepomuk::Resource)), p, SIGNAL(resourceUpdated(Nepomuk::Resource)));
}

void PublicationWidget::subResourceUpdated(Nepomuk::Resource resource)
{
    Q_UNUSED(resource);
    emit resourceUpdated(m_publication);
}

void PublicationWidget::addReference()
{
    KDialog showRefWidget;

    Nepomuk::Resource tempRef(QUrl(), Nepomuk::Vocabulary::NBIB::Reference());
    tempRef.setProperty(Nepomuk::Vocabulary::NBIB::publication(), m_publication);

    ReferenceWidget *rw = new ReferenceWidget();
    rw->setResource(tempRef);
    rw->setLibrary(library());

    showRefWidget.setMainWidget(rw);
    showRefWidget.setInitialSize(QSize(300,300));

    int ret = showRefWidget.exec();

    if(ret == KDialog::Accepted) {
        m_publication.addProperty(Nepomuk::Vocabulary::NBIB::reference(), tempRef);
    }
    else {
        // remove temp citation again
        tempRef.remove();
    }
}

void PublicationWidget::removeReference()
{
    QList<QAction*> actionCollection;
    QMenu removeReference;

    QList<Nepomuk::Resource> referenceList = m_publication.property(Nepomuk::Vocabulary::NBIB::reference()).toResourceList();

    foreach(const Nepomuk::Resource &r, referenceList) {
        QAction *a = new QAction(r.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString(), this);
        a->setData(r.resourceUri());
        connect(a, SIGNAL(triggered(bool)),this, SLOT(removeFromSelectedReference()));
        removeReference.addAction(a);
        actionCollection.append(a);
    }

    removeReference.exec(QCursor::pos());

    qDeleteAll(actionCollection);
}

void PublicationWidget::removeFromSelectedReference()
{
    QAction *a = qobject_cast<QAction *>(sender());

    if(!a)
        return;

    Nepomuk::Resource reference = Nepomuk::Resource(a->data().toString());
    m_publication.removeProperty(Nepomuk::Vocabulary::NBIB::reference(), reference);

    reference.remove();


    QList<Nepomuk::Resource> references = m_publication.property(Nepomuk::Vocabulary::NBIB::reference()).toResourceList();

    if(references.isEmpty()) {
        emit hasReference(false);
    }
    else {
        emit hasReference(true);
    }

    emit resourceUpdated(m_publication);
}

void PublicationWidget::acceptContentChanges()
{
    QString abstract = ui->editAbstract->document()->toPlainText();
    m_publication.setProperty(Nepomuk::Vocabulary::NBIB::abstract(), abstract);
}

void PublicationWidget::discardContentChanges()
{
    QString abstract = m_publication.property(Nepomuk::Vocabulary::NBIB::abstract()).toString();
    ui->editAbstract->document()->setPlainText(abstract);
}

void PublicationWidget::acceptNoteChanges()
{
    QString note = ui->editNote->document()->toPlainText();
    m_publication.setProperty(Nepomuk::Vocabulary::NIE::description(), note);

    QString annote = ui->editAnnote->document()->toPlainText();
    m_publication.setProperty(Nepomuk::Vocabulary::NIE::comment(), annote);
}

void PublicationWidget::discardNoteChanges()
{
    QString note = m_publication.property(Nepomuk::Vocabulary::NIE::description()).toString();
    ui->editNote->document()->setPlainText(note);

    QString annote = m_publication.property(Nepomuk::Vocabulary::NIE::comment()).toString();
    ui->editAnnote->document()->setPlainText(annote);
}

void PublicationWidget::changeRating(int newRating)
{
    m_publication.setRating(newRating);

    emit resourceUpdated(m_publication);
}

void PublicationWidget::setupWidget()
{
    int i=0;
    foreach(const QString &s, BibEntryTypeTranslation) {
        ui->editEntryType->addItem(s,(BibEntryType)i);
        i++;
    }

    connect(ui->editEntryType, SIGNAL(currentIndexChanged(int)), this, SLOT(newBibEntryTypeSelected(int)));

    ui->editAuthors->setPropertyUrl( Nepomuk::Vocabulary::NCO::creator() );
    ui->editAuthors->setUseDetailDialog(true);
    connect(ui->editAuthors, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editCopyright->setPropertyUrl( Nepomuk::Vocabulary::NIE::copyright() );
    ui->editDate->setPropertyUrl( Nepomuk::Vocabulary::NBIB::publicationDate() );
    ui->editDOI->setPropertyUrl( Nepomuk::Vocabulary::NBIB::doi() );
    ui->editEdition->setPropertyUrl( Nepomuk::Vocabulary::NBIB::edition() );
    ui->editEditor->setPropertyUrl( Nepomuk::Vocabulary::NBIB::editor() );
    ui->editEditor->setUseDetailDialog(true);
    connect(ui->editEditor, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editEprint->setPropertyUrl( Nepomuk::Vocabulary::NBIB::eprint() );
    ui->editHowPublished->setPropertyUrl( Nepomuk::Vocabulary::NBIB::publicationMethod() );
    ui->editISBN->setPropertyUrl( Nepomuk::Vocabulary::NBIB::isbn() );
    ui->editISSN->setPropertyUrl( Nepomuk::Vocabulary::NBIB::issn() );
    ui->editJournal->setPropertyUrl( Nepomuk::Vocabulary::NBIB::seriesOf() );
    ui->editLCCN->setPropertyUrl( Nepomuk::Vocabulary::NBIB::lccn() );
    ui->editMRNumber->setPropertyUrl( Nepomuk::Vocabulary::NBIB::mrNumber() );
    ui->editNumber->setPropertyUrl( Nepomuk::Vocabulary::NBIB::number() );
    ui->editPublisher->setPropertyUrl( Nepomuk::Vocabulary::NCO::publisher() );
    ui->editPublisher->setUseDetailDialog(true);
    connect(ui->editPublisher, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editSeries->setPropertyUrl( Nepomuk::Vocabulary::NBIB::inSeries() );
    ui->editTitle->setPropertyUrl( Nepomuk::Vocabulary::NIE::title() );
    ui->editType->setPropertyUrl( Nepomuk::Vocabulary::NBIB::type() );
    ui->editVolume->setPropertyUrl( Nepomuk::Vocabulary::NBIB::volume() );
    ui->editFileObject->setMode(FileObjectEdit::Local);
    ui->editFileObject->setPropertyUrl( Nepomuk::Vocabulary::NBIB::isPublicationOf() );
    ui->editRemoteObject->setMode(FileObjectEdit::Remote);
    ui->editRemoteObject->setPropertyUrl( Nepomuk::Vocabulary::NBIB::isPublicationOf() );
    ui->editWebObject->setMode(FileObjectEdit::Website);
    ui->editWebObject->setPropertyUrl( Nepomuk::Vocabulary::NBIB::isPublicationOf() );
    ui->editOrganization->setPropertyUrl( Nepomuk::Vocabulary::NBIB::organization());
    ui->editLastAccessed->setPropertyUrl( Nepomuk::Vocabulary::NUAO::lastUsage());
    ui->editKeywords->setPropertyCardinality(PropertyEdit::MULTIPLE_PROPERTY);
    ui->editKeywords->setPropertyUrl( Soprano::Vocabulary::NAO::hasTag() );
    ui->editProceedings->setPropertyUrl( Nepomuk::Vocabulary::NBIB::collection() );
    ui->editPubMed->setPropertyUrl( Nepomuk::Vocabulary::NBIB::pubMed() );

    //connect signal/slots
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editAuthors, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editCopyright, SLOT(setResource(Nepomuk::Resource&)));
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
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editFileObject, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editRemoteObject, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editWebObject, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editOrganization, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editLastAccessed, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editKeywords, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editProceedings, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editPubMed, SLOT(setResource(Nepomuk::Resource&)));

    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(changeRating(int)));

    connect(ui->listPartsWidget, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SIGNAL(resourceUpdated(Nepomuk::Resource)));
}

void PublicationWidget::editContactDialog(Nepomuk::Resource & resource, const QUrl & propertyUrl)
{
    ContactDialog cd;
    cd.setResource(resource, propertyUrl);

    cd.exec();

    ui->editAuthors->setResource(resource);
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
    case BibType_Manual:
        layoutManual();
        break;
    case BibType_Thesis:
    case BibType_Phdthesis:
    case BibType_Mastersthesis:
    case BibType_Bachelorthesis:
        layoutThesis();
        break;
    case BibType_Misc:
        layoutMisc();
        break;
    case BibType_Proceedings:
        layoutProceedings();
        break;
    case BibType_Report:
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
    case BibType_ForumPost:
    case BibType_BlogPost:
    case BibType_WebPage:
        layoutElectronic();
        break;
    case BibType_Collection:
    case BibType_MagazinIssue:
    case BibType_NewspaperIssue:
    case BibType_JournalIssue:
        layoutCollection();
        break;
    default:
        qDebug() << "PublicationWidget::selectLayout use default layout";
        layoutMisc();
    }
}

void PublicationWidget::layoutArticle()
{
    //Basics
    ui->editTitle->setEnabled(true);
    ui->editAuthors->setEnabled(true);
    ui->editEditor->setEnabled(true);
    ui->editDate->setEnabled(true);
    ui->editPublisher->setEnabled(true);
    ui->editOrganization->setEnabled(true);

    //Extra
    ui->editSeries->setEnabled(false);
    ui->editEdition->setEnabled(false);
    ui->editProceedings->setEnabled(true);
    ui->editJournal->setEnabled(true);
    ui->editVolume->setEnabled(true);
    ui->editNumber->setEnabled(true);
    ui->editHowPublished->setEnabled(false);
    ui->editType->setEnabled(false);
    ui->editCopyright->setEnabled(true);
    ui->editLastAccessed->setEnabled(false);

    //identification
    ui->editEprint->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editLCCN->setEnabled(false);
    ui->editMRNumber->setEnabled(true);
    ui->editDOI->setEnabled(true);
}

void PublicationWidget::layoutBook()
{
    //Basics
    ui->editTitle->setEnabled(true);
    ui->editAuthors->setEnabled(true);
    ui->editEditor->setEnabled(true);
    ui->editDate->setEnabled(true);
    ui->editPublisher->setEnabled(true);
    ui->editOrganization->setEnabled(false);

    //Extra
    ui->editSeries->setEnabled(true);
    ui->editEdition->setEnabled(true);
    ui->editProceedings->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(true);
    ui->editNumber->setEnabled(false);
    ui->editHowPublished->setEnabled(false);
    ui->editType->setEnabled(false);
    ui->editCopyright->setEnabled(true);
    ui->editLastAccessed->setEnabled(false);

    //identification
    ui->editEprint->setEnabled(false);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editMRNumber->setEnabled(false);
    ui->editDOI->setEnabled(true);
}

void PublicationWidget::layoutBooklet()
{
    //Basics
    ui->editTitle->setEnabled(true);
    ui->editAuthors->setEnabled(true);
    ui->editEditor->setEnabled(false);
    ui->editDate->setEnabled(true);
    ui->editPublisher->setEnabled(false);
    ui->editOrganization->setEnabled(false);

    //Extra
    ui->editSeries->setEnabled(false);
    ui->editEdition->setEnabled(false);
    ui->editProceedings->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);
    ui->editHowPublished->setEnabled(true);
    ui->editType->setEnabled(false);
    ui->editCopyright->setEnabled(true);
    ui->editLastAccessed->setEnabled(false);

    //identification
    ui->editEprint->setEnabled(false);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editMRNumber->setEnabled(false);
    ui->editDOI->setEnabled(true);
}

void PublicationWidget::layoutCollection()
{
    //Basics
    ui->editTitle->setEnabled(true);
    ui->editAuthors->setEnabled(true);
    ui->editEditor->setEnabled(true);
    ui->editDate->setEnabled(true);
    ui->editPublisher->setEnabled(true);
    ui->editOrganization->setEnabled(true);

    //Extra
    ui->editSeries->setEnabled(true);
    ui->editEdition->setEnabled(false);
    ui->editProceedings->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(true);
    ui->editNumber->setEnabled(true);
    ui->editHowPublished->setEnabled(true);
    ui->editType->setEnabled(false);
    ui->editCopyright->setEnabled(true);
    ui->editLastAccessed->setEnabled(true);

    //identification
    ui->editEprint->setEnabled(false);
    ui->editISBN->setEnabled(false);
    ui->editISSN->setEnabled(true);
    ui->editLCCN->setEnabled(false);
    ui->editMRNumber->setEnabled(false);
    ui->editDOI->setEnabled(true);
}

void PublicationWidget::layoutManual()
{
    //Basics
    ui->editTitle->setEnabled(true);
    ui->editAuthors->setEnabled(true);
    ui->editEditor->setEnabled(false);
    ui->editDate->setEnabled(true);
    ui->editPublisher->setEnabled(false);
    ui->editOrganization->setEnabled(true);

    //Extra
    ui->editSeries->setEnabled(false);
    ui->editEdition->setEnabled(true);
    ui->editProceedings->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);
    ui->editHowPublished->setEnabled(false);
    ui->editType->setEnabled(false);
    ui->editCopyright->setEnabled(true);
    ui->editLastAccessed->setEnabled(false);

    //identification
    ui->editEprint->setEnabled(false);
    ui->editISBN->setEnabled(false);
    ui->editISSN->setEnabled(false);
    ui->editLCCN->setEnabled(false);
    ui->editMRNumber->setEnabled(false);
    ui->editDOI->setEnabled(true);
}

void PublicationWidget::layoutMisc()
{
    //Basics
    ui->editTitle->setEnabled(true);
    ui->editAuthors->setEnabled(true);
    ui->editEditor->setEnabled(true);
    ui->editDate->setEnabled(true);
    ui->editPublisher->setEnabled(true);
    ui->editOrganization->setEnabled(true);

    //Extra
    ui->editSeries->setEnabled(true);
    ui->editEdition->setEnabled(true);
    ui->editProceedings->setEnabled(true);
    ui->editJournal->setEnabled(true);
    ui->editVolume->setEnabled(true);
    ui->editNumber->setEnabled(true);
    ui->editHowPublished->setEnabled(true);
    ui->editType->setEnabled(true);
    ui->editCopyright->setEnabled(true);
    ui->editLastAccessed->setEnabled(true);

    //identification
    ui->editEprint->setEnabled(true);
    ui->editISBN->setEnabled(true);
    ui->editISSN->setEnabled(true);
    ui->editLCCN->setEnabled(true);
    ui->editMRNumber->setEnabled(true);
    ui->editDOI->setEnabled(true);
}

void PublicationWidget::layoutThesis()
{
    //Basics
    ui->editTitle->setEnabled(true);
    ui->editAuthors->setEnabled(true);
    ui->editEditor->setEnabled(false);
    ui->editDate->setEnabled(true);
    ui->editPublisher->setEnabled(true);
    ui->editOrganization->setEnabled(false);

    //Extra
    ui->editSeries->setEnabled(false);
    ui->editEdition->setEnabled(false);
    ui->editProceedings->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(true);
    ui->editHowPublished->setEnabled(true);
    ui->editType->setEnabled(true);
    ui->editCopyright->setEnabled(true);
    ui->editLastAccessed->setEnabled(false);

    //identification
    ui->editEprint->setEnabled(false);
    ui->editISBN->setEnabled(false);
    ui->editISSN->setEnabled(false);
    ui->editLCCN->setEnabled(false);
    ui->editMRNumber->setEnabled(false);
    ui->editDOI->setEnabled(true);
}

void PublicationWidget::layoutProceedings()
{
    //Basics
    ui->editTitle->setEnabled(true);
    ui->editAuthors->setEnabled(false);
    ui->editEditor->setEnabled(true);
    ui->editDate->setEnabled(true);
    ui->editPublisher->setEnabled(true);
    ui->editOrganization->setEnabled(true);

    //Extra
    ui->editSeries->setEnabled(false);
    ui->editEdition->setEnabled(false);
    ui->editProceedings->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);
    ui->editHowPublished->setEnabled(false);
    ui->editType->setEnabled(false);
    ui->editCopyright->setEnabled(true);
    ui->editLastAccessed->setEnabled(false);

    //identification
    ui->editEprint->setEnabled(false);
    ui->editISBN->setEnabled(false);
    ui->editISSN->setEnabled(false);
    ui->editLCCN->setEnabled(false);
    ui->editMRNumber->setEnabled(false);
    ui->editDOI->setEnabled(true);
}

void PublicationWidget::layoutTechreport()
{
    //Basics
    ui->editTitle->setEnabled(true);
    ui->editAuthors->setEnabled(true);
    ui->editEditor->setEnabled(false);
    ui->editDate->setEnabled(true);
    ui->editPublisher->setEnabled(true);
    ui->editOrganization->setEnabled(false);

    //Extra
    ui->editSeries->setEnabled(false);
    ui->editEdition->setEnabled(false);
    ui->editProceedings->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(true);
    ui->editHowPublished->setEnabled(false);
    ui->editType->setEnabled(true);
    ui->editCopyright->setEnabled(true);
    ui->editLastAccessed->setEnabled(false);

    //identification
    ui->editEprint->setEnabled(false);
    ui->editISBN->setEnabled(false);
    ui->editISSN->setEnabled(false);
    ui->editLCCN->setEnabled(false);
    ui->editMRNumber->setEnabled(false);
    ui->editDOI->setEnabled(true);
}

void PublicationWidget::layoutUnpublished()
{
    //Basics
    ui->editTitle->setEnabled(true);
    ui->editAuthors->setEnabled(true);
    ui->editEditor->setEnabled(false);
    ui->editDate->setEnabled(true);
    ui->editPublisher->setEnabled(false);
    ui->editOrganization->setEnabled(false);

    //Extra
    ui->editSeries->setEnabled(false);
    ui->editEdition->setEnabled(false);
    ui->editProceedings->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);
    ui->editHowPublished->setEnabled(false);
    ui->editType->setEnabled(false);
    ui->editCopyright->setEnabled(true);
    ui->editLastAccessed->setEnabled(false);

    //identification
    ui->editEprint->setEnabled(false);
    ui->editISBN->setEnabled(false);
    ui->editISSN->setEnabled(false);
    ui->editLCCN->setEnabled(false);
    ui->editMRNumber->setEnabled(false);
    ui->editDOI->setEnabled(false);
}

void PublicationWidget::layoutPatent()
{
    //Basics
    ui->editTitle->setEnabled(true);
    ui->editAuthors->setEnabled(true);
    ui->editEditor->setEnabled(false);
    ui->editDate->setEnabled(true);
    ui->editPublisher->setEnabled(true);
    ui->editOrganization->setEnabled(true);

    //Extra
    ui->editSeries->setEnabled(false);
    ui->editEdition->setEnabled(true);
    ui->editProceedings->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(true);
    ui->editHowPublished->setEnabled(false);
    ui->editType->setEnabled(false);
    ui->editCopyright->setEnabled(true);
    ui->editLastAccessed->setEnabled(false);

    //identification
    ui->editEprint->setEnabled(false);
    ui->editISBN->setEnabled(false);
    ui->editISSN->setEnabled(false);
    ui->editLCCN->setEnabled(false);
    ui->editMRNumber->setEnabled(false);
    ui->editDOI->setEnabled(true);
}

void PublicationWidget::layoutElectronic()
{
    //Basics
    ui->editTitle->setEnabled(true);
    ui->editAuthors->setEnabled(true);
    ui->editEditor->setEnabled(false);
    ui->editDate->setEnabled(true);
    ui->editPublisher->setEnabled(true);
    ui->editOrganization->setEnabled(true);

    //Extra
    ui->editSeries->setEnabled(false);
    ui->editEdition->setEnabled(false);
    ui->editProceedings->setEnabled(false);
    ui->editJournal->setEnabled(false);
    ui->editVolume->setEnabled(false);
    ui->editNumber->setEnabled(false);
    ui->editHowPublished->setEnabled(false);
    ui->editType->setEnabled(false);
    ui->editCopyright->setEnabled(true);
    ui->editLastAccessed->setEnabled(false);

    //identification
    ui->editEprint->setEnabled(false);
    ui->editISBN->setEnabled(false);
    ui->editISSN->setEnabled(false);
    ui->editLCCN->setEnabled(false);
    ui->editMRNumber->setEnabled(false);
    ui->editDOI->setEnabled(true);
}
