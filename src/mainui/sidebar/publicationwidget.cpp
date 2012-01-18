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

#include "globals.h"

#include "propertywidgets/stringedit.h"
#include "propertywidgets/contactedit.h"
#include "propertywidgets/fileobjectedit.h"

#include "referencewidget.h"
#include "eventwidget.h"
#include "serieswidget.h"
#include "contactdialog.h"
#include "addchapterdialog.h"
#include "listpublicationsdialog.h"
#include "core/library.h"

#include "nbibio/conquirere.h"

#include "nbib.h"
#include <Nepomuk/Thing>
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NUAO>
#include <Nepomuk/Vocabulary/PIMO>
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
#include <QtCore/QPointer>

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

   ui->editTitle->setResource(m_publication);
   ui->editAuthors->setResource(m_publication);
   ui->editEditor->setResource(m_publication);
   ui->editAssignee->setResource(m_publication);
   ui->editDate->setResource(m_publication);
   ui->editFillingDate->setResource(m_publication);
   ui->editPublisher->setResource(m_publication);
   ui->editOrganization->setResource(m_publication);
   ui->editFileObject->setResource(m_publication);
   ui->editRemoteObject->setResource(m_publication);
   ui->editWebObject->setResource(m_publication);
   ui->editKeywords->setResource(m_publication);

   ui->editShortTitle->setResource(m_publication);
   ui->editTranslator->setResource(m_publication);
   ui->editContributor->setResource(m_publication);
   ui->editReviewedAuthor->setResource(m_publication);
   ui->editCopyright->setResource(m_publication);
   ui->editCopyright->setResource(m_publication);
   ui->editLastAccessed->setResource(m_publication);
   ui->editLanguage->setResource(m_publication);

   ui->editEdition->setResource(m_publication);
   ui->editEvent->setResource(m_publication);
   ui->editCollection->setResource(m_publication);
   ui->editSeries->setResource(m_publication);
   ui->editCode->setResource(m_publication);
   ui->editCourtReporter->setResource(m_publication);
   ui->editVolume->setResource(m_publication);
   ui->editNumber->setResource(m_publication);
   ui->editApplicationNumber->setResource(m_publication);
   ui->editPriorityNumbers->setResource(m_publication);
   ui->editPublicLawNumber->setResource(m_publication);
   ui->editReferences->setResource(m_publication);
   ui->editLegalStatus->setResource(m_publication);
   ui->editHistory->setResource(m_publication);
   ui->editScale->setResource(m_publication);
   ui->editHowPublished->setResource(m_publication);
   ui->editType->setResource(m_publication);

   ui->editArchive->setResource(m_publication);
   ui->editArchiveLocation->setResource(m_publication);
   ui->editLibCatalog->setResource(m_publication);
   ui->editEprint->setResource(m_publication);
   ui->editISBN->setResource(m_publication);
   ui->editISSN->setResource(m_publication);
   ui->editLCCN->setResource(m_publication);
   ui->editMRNumber->setResource(m_publication);
   ui->editPubMed->setResource(m_publication);
   ui->editDOI->setResource(m_publication);
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
        case BibType_Dictionary:
            newtype.append(Nepomuk::Vocabulary::NBIB::Book());
            break;
        case BibType_MagazinIssue:
        case BibType_NewspaperIssue:
        case BibType_JournalIssue:
        case BibType_Proceedings:
        case BibType_Encyclopedia:
        case BibType_CodeOfLaw:
        case BibType_CourtReporter:
        case BibType_WebSite:
        case BibType_Blog:
        case BibType_Forum:
            newtype.append(Nepomuk::Vocabulary::NBIB::Collection());
            break;
        case BibType_Bachelorthesis:
        case BibType_Mastersthesis:
        case BibType_Phdthesis:
            newtype.append(Nepomuk::Vocabulary::NBIB::Thesis());
            break;
        case BibType_Techreport:
            newtype.append(Nepomuk::Vocabulary::NBIB::Report());
            break;
        case BibType_BlogPost:
        case BibType_ForumPost:
        case BibType_WebPage:
            newtype.append(Nepomuk::Vocabulary::NBIB::Article());
            break;
        case BibType_Bill:
        case BibType_Statute:
            newtype.append(Nepomuk::Vocabulary::NBIB::Legislation());
            newtype.append(Nepomuk::Vocabulary::NBIB::LegalDocument());
            break;
        case BibType_Decision:
        case BibType_Brief:
            newtype.append(Nepomuk::Vocabulary::NBIB::LegalCaseDocument());
            newtype.append(Nepomuk::Vocabulary::NBIB::LegalDocument());
            break;
            //ignore the following cases, as they are a direct subtype of nbib:publication and as such no need to add
            // another hirachry entry is necessary
        case BibType_Article:
        case BibType_Book:
        case BibType_Booklet:
        case BibType_Collection:
        case BibType_Thesis:
        case BibType_Report:
        case BibType_Electronic:
        case BibType_Script:
        case BibType_Misc:
        case BibType_Standard:
        case BibType_Patent:
        case BibType_Legislation:
        case BibType_LegalCaseDocument:
        case BibType_Presentation:
        case BibType_Unpublished:
        case BibType_Manual:
        case Max_BibTypes:
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
        else if(m_publication.hasType(Nepomuk::Vocabulary::NBIB::Book())) {
            Nepomuk::Resource seriesResource = m_publication.property((Nepomuk::Vocabulary::NBIB::inSeries())).toResource();
            if(seriesResource.isValid()) {
                Nepomuk::Resource x(QUrl(), Nepomuk::Vocabulary::NBIB::BookSeries());
                seriesResource.setTypes(x.types());
            }
        }
    }
}

void PublicationWidget::newButtonClicked()
{
    //create a new resource
    Nepomuk::Resource nb;
    QList<QUrl> types;
    types.append(Nepomuk::Vocabulary::NBIB::Publication());
    nb.setTypes(types);
    nb.setProperty(Nepomuk::Vocabulary::NIE::title(), i18n("New Publication"));

    setResource(nb);
}

void PublicationWidget::deleteButtonClicked()
{
    m_publication.remove();

    setResource(m_publication);
}

void PublicationWidget::subResourceUpdated(Nepomuk::Resource resource)
{
    if(resource.resourceUri() != m_publication.resourceUri())
        emit resourceCacheNeedsUpdate(resource);

    emit resourceCacheNeedsUpdate(m_publication);

    // also update the cache entry for any kind of connected resource
    QList<Nepomuk::Resource> refs = m_publication.property(Nepomuk::Vocabulary::NBIB::reference()).toResourceList();
    foreach(const Nepomuk::Resource &r, refs) {
        emit resourceCacheNeedsUpdate(r);
    }

    Nepomuk::Resource event = m_publication.property(Nepomuk::Vocabulary::NBIB::event()).toResource();
    if(event.isValid()) {
        emit resourceCacheNeedsUpdate(event);
    }

    Nepomuk::Resource series = m_publication.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    if(series.isValid()) {
        emit resourceCacheNeedsUpdate(series);
    }

    QList<Nepomuk::Resource> articles = m_publication.property(Nepomuk::Vocabulary::NBIB::article()).toResourceList();
    foreach(const Nepomuk::Resource &a, articles) {
        emit resourceCacheNeedsUpdate(a);
    }

    Nepomuk::Resource collection = m_publication.property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
    if(collection.isValid()) {
        emit resourceCacheNeedsUpdate(collection);
    }
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
        emit resourceCacheNeedsUpdate(m_publication);
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

    emit resourceCacheNeedsUpdate(m_publication);
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

    subResourceUpdated(m_publication);
}

void PublicationWidget::setupWidget()
{
    int i=0;
    foreach(const QString &s, BibEntryTypeTranslation) {
        if(ConqSettings::hiddenNbibPublications().contains(i)) {
            i++;
            continue;
        }
        ui->editEntryType->addItem(s,(BibEntryType)i);
        i++;
    }

    connect(ui->editEntryType, SIGNAL(currentIndexChanged(int)), this, SLOT(newBibEntryTypeSelected(int)));

    // Basics section
    ui->editTitle->setPropertyUrl( Nepomuk::Vocabulary::NIE::title() );
    ui->editAuthors->setPropertyUrl( Nepomuk::Vocabulary::NCO::creator() );
    ui->editAuthors->setUseDetailDialog(true);
    connect(ui->editAuthors, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editEditor->setPropertyUrl( Nepomuk::Vocabulary::NBIB::editor() );
    ui->editEditor->setUseDetailDialog(true);
    connect(ui->editEditor, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editAssignee->setPropertyUrl( Nepomuk::Vocabulary::NBIB::assignee() );
    ui->editDate->setPropertyUrl( Nepomuk::Vocabulary::NBIB::publicationDate() );
    ui->editFillingDate->setPropertyUrl( Nepomuk::Vocabulary::NBIB::filingDate() );
    ui->editPublisher->setPropertyUrl( Nepomuk::Vocabulary::NCO::publisher() );
    ui->editPublisher->setUseDetailDialog(true);
    connect(ui->editPublisher, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editFileObject->setMode(FileObjectEdit::Local);
    ui->editFileObject->setPropertyUrl( Nepomuk::Vocabulary::NBIB::isPublicationOf() );
    ui->editRemoteObject->setMode(FileObjectEdit::Remote);
    ui->editRemoteObject->setPropertyUrl( Nepomuk::Vocabulary::NBIB::isPublicationOf() );
    ui->editWebObject->setMode(FileObjectEdit::Website);
    ui->editWebObject->setPropertyUrl( Nepomuk::Vocabulary::NBIB::isPublicationOf() );
    ui->editKeywords->setPropertyCardinality(PropertyEdit::MULTIPLE_PROPERTY);
    ui->editKeywords->setPropertyUrl( Soprano::Vocabulary::NAO::hasTag() );

    //other section
    ui->editShortTitle->setPropertyUrl( Nepomuk::Vocabulary::NBIB::shortTitle() );
    ui->editTranslator->setPropertyUrl( Nepomuk::Vocabulary::NBIB::translator() );
    ui->editTranslator->setUseDetailDialog(true);
    connect(ui->editTranslator, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editContributor->setPropertyUrl( Nepomuk::Vocabulary::NBIB::contributor() );
    ui->editContributor->setUseDetailDialog(true);
    connect(ui->editContributor, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editReviewedAuthor->setPropertyUrl( Nepomuk::Vocabulary::NBIB::reviewedAuthor() );
    ui->editReviewedAuthor->setUseDetailDialog(true);
    connect(ui->editReviewedAuthor, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));

    ui->editCopyright->setPropertyUrl( Nepomuk::Vocabulary::NIE::copyright() );
    ui->editLastAccessed->setPropertyUrl( Nepomuk::Vocabulary::NUAO::lastUsage());
    ui->editLanguage->setPropertyUrl( Nepomuk::Vocabulary::NIE::language());

    // Extra section
    ui->editEvent->setPropertyUrl( Nepomuk::Vocabulary::NBIB::event() );
    ui->editEvent->setUseDetailDialog(true);
    connect(ui->editEvent, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showDetailDialog(Nepomuk::Resource&,QUrl)));
    ui->editSeries->setUseDetailDialog(true);
    connect(ui->editSeries, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showDetailDialog(Nepomuk::Resource&,QUrl)));
    ui->editEdition->setPropertyUrl( Nepomuk::Vocabulary::NBIB::edition() );
    ui->editCollection->setPropertyUrl( Nepomuk::Vocabulary::NBIB::collection() );
    ui->editCollection->setUseDetailDialog(true);
    connect(ui->editCollection, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showDetailDialog(Nepomuk::Resource&,QUrl)));
    ui->editCode->setPropertyUrl( Nepomuk::Vocabulary::NBIB::codeOfLaw() );
    ui->editCode->setUseDetailDialog(true);
    connect(ui->editCode, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showDetailDialog(Nepomuk::Resource&,QUrl)));
    ui->editCourtReporter->setPropertyUrl( Nepomuk::Vocabulary::NBIB::courtReporter() );
    ui->editCourtReporter->setUseDetailDialog(true);
    connect(ui->editCourtReporter, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showDetailDialog(Nepomuk::Resource&,QUrl)));
    ui->editVolume->setPropertyUrl( Nepomuk::Vocabulary::NBIB::volume() );
    ui->editNumber->setPropertyUrl( Nepomuk::Vocabulary::NBIB::number() );
    ui->editApplicationNumber->setPropertyUrl( Nepomuk::Vocabulary::NBIB::applicationNumber() );
    ui->editPriorityNumbers->setPropertyUrl( Nepomuk::Vocabulary::NBIB::priorityNumbers() );
    ui->editPublicLawNumber->setPropertyUrl( Nepomuk::Vocabulary::NBIB::publicLawNumber() );
    ui->editReferences->setPropertyUrl( Nepomuk::Vocabulary::NBIB::patentReferences() );
    ui->editLegalStatus->setPropertyUrl( Nepomuk::Vocabulary::NBIB::legalStatus() );
    ui->editHistory->setPropertyUrl( Nepomuk::Vocabulary::NBIB::history() );
    ui->editScale->setPropertyUrl( Nepomuk::Vocabulary::NBIB::mapScale() );
    ui->editHowPublished->setPropertyUrl( Nepomuk::Vocabulary::NBIB::publicationMethod() );
    ui->editType->setPropertyUrl( Nepomuk::Vocabulary::NBIB::type() );

    // identification section
    ui->editArchive->setPropertyUrl( Nepomuk::Vocabulary::NBIB::archive() );
    ui->editArchiveLocation->setPropertyUrl( Nepomuk::Vocabulary::NBIB::archiveLocation() );
    ui->editLibCatalog->setPropertyUrl( Nepomuk::Vocabulary::NBIB::libraryCatalog() );
    ui->editEprint->setPropertyUrl( Nepomuk::Vocabulary::NBIB::eprint() );
    ui->editISBN->setPropertyUrl( Nepomuk::Vocabulary::NBIB::isbn() );
    ui->editISSN->setPropertyUrl( Nepomuk::Vocabulary::NBIB::issn() );
    ui->editLCCN->setPropertyUrl( Nepomuk::Vocabulary::NBIB::lccn() );
    ui->editMRNumber->setPropertyUrl( Nepomuk::Vocabulary::NBIB::mrNumber() );
    ui->editPubMed->setPropertyUrl( Nepomuk::Vocabulary::NBIB::pubMed() );
    ui->editDOI->setPropertyUrl( Nepomuk::Vocabulary::NBIB::doi() );

    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(changeRating(int)));

    //TODO remove and use ResourceWatcher later on
    connect(ui->editTitle, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editAuthors, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editEditor, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editAssignee, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editDate, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editFillingDate, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editPublisher, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editFileObject, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editRemoteObject, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editWebObject, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editKeywords, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    connect(ui->editShortTitle, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editTranslator, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editContributor, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editReviewedAuthor, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editCopyright, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editLastAccessed, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editLanguage, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    connect(ui->editEvent, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editSeries, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editEdition, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editCollection, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editCode, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editCourtReporter, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editVolume, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editNumber, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editApplicationNumber, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editPriorityNumbers, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editPublicLawNumber, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editReferences, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editLegalStatus, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editHistory, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editScale, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editHowPublished, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editType, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    connect(ui->editArchive, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editArchiveLocation, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editLibCatalog, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editEprint, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editISBN, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editISSN, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editLCCN, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editMRNumber, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editPubMed, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editDOI, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    connect(ui->listPartsWidget, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
}

void PublicationWidget::editContactDialog(Nepomuk::Resource & resource, const QUrl & propertyUrl)
{
    ContactDialog cd;
    cd.setResource(resource, propertyUrl);

    cd.exec();

    ContactEdit *ce = dynamic_cast<ContactEdit *>(sender());
    ce->setResource(resource);
}

void PublicationWidget::showDetailDialog(Nepomuk::Resource & resource, const QUrl & propertyUrl)
{
    Nepomuk::Resource changedResource = resource.property(propertyUrl).toResource();

    // first if the resource is valid, we just want to edit it
    if(changedResource.isValid()) {
        QPointer<KDialog> addIssueWidget = new KDialog(this);
        if(changedResource.hasType(Nepomuk::Vocabulary::PIMO::Event())) {
            EventWidget *pw = new EventWidget();
            pw->setResource(changedResource);
            addIssueWidget->setMainWidget(pw);
        }
        else if(changedResource.hasType(Nepomuk::Vocabulary::NBIB::Series())) {
            SeriesWidget *pw = new SeriesWidget();
            pw->setResource(changedResource);
            addIssueWidget->setMainWidget(pw);
        }
        else {
            PublicationWidget *pw = new PublicationWidget();
            pw->setResource(changedResource);
            addIssueWidget->setMainWidget(pw);
        }
        addIssueWidget->setInitialSize(QSize(500,300));
        addIssueWidget->exec();
        delete addIssueWidget;

        setResource(m_publication); // this updates the changes in the current widget again

        subResourceUpdated(m_publication);
        return;
    }

    //2nd if no valid resource is availabe the user want
    // a) create a new one ignored, user should enter the title into the field first and press edit then
    // b) select from a list of existing resources

    //get the range of the property (so what we are allowed to enter)
    //Nepomuk::Resource nr(propertyUrl);
    //Nepomuk::Resource range = nr.property(QUrl(QLatin1String("http://www.w3.org/2000/01/rdf-schema#range"))).toResource();
    // not working sadly :/

    QPointer<ListPublicationsDialog> lpd = new ListPublicationsDialog(this);
    if(propertyUrl == Nepomuk::Vocabulary::NBIB::inSeries()) {
        lpd->setListMode(Resource_Series, Max_BibTypes);
    }
    else if(propertyUrl == Nepomuk::Vocabulary::NBIB::codeOfLaw()) {
        lpd->setListMode(Resource_Publication, BibType_CodeOfLaw);
    }
    else if(propertyUrl == Nepomuk::Vocabulary::NBIB::courtReporter()) {
        lpd->setListMode(Resource_Publication, BibType_CourtReporter);
    }
    else if(propertyUrl == Nepomuk::Vocabulary::NBIB::collection()) {
        lpd->setListMode(Resource_Publication, BibType_Collection);
    }
    else if(propertyUrl == Nepomuk::Vocabulary::NBIB::event()) {
        lpd->setListMode(Resource_Event, Max_BibTypes);
    }
    else {
        lpd->setListMode(Resource_Reference, Max_BibTypes);
    }
    lpd->setSystemLibrary(library());

    int ret = lpd->exec();

    if(ret == QDialog::Accepted) {
        Nepomuk::Resource selectedPart = lpd->selectedPublication();

        // here I need to take into account, that backlinks must be handled somehow
        if(propertyUrl == Nepomuk::Vocabulary::NBIB::inSeries()) {
            resource.setProperty(propertyUrl, selectedPart );
            selectedPart.addProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), resource );
        }
        else if(propertyUrl == Nepomuk::Vocabulary::NBIB::codeOfLaw()) {
            resource.setProperty(propertyUrl, selectedPart );
            selectedPart.addProperty(Nepomuk::Vocabulary::NBIB::legislation(), resource );
        }
        else if(propertyUrl == Nepomuk::Vocabulary::NBIB::courtReporter()) {
            resource.setProperty(propertyUrl, selectedPart );
            selectedPart.addProperty(Nepomuk::Vocabulary::NBIB::legalCase(), resource );
        }
        else if(propertyUrl == Nepomuk::Vocabulary::NBIB::collection()) {
            resource.setProperty(propertyUrl, selectedPart );
            selectedPart.addProperty(Nepomuk::Vocabulary::NBIB::article(), resource );
        }
        else if(propertyUrl == Nepomuk::Vocabulary::NBIB::event()) {
            Nepomuk::Thing eventThing = selectedPart.pimoThing();

            // adapt pimo::thing, as it is highly likely that we had to create a new pimo::event
            eventThing.addType(Nepomuk::Vocabulary::PIMO::Event());
            QString eventTitle = selectedPart.property(Nepomuk::Vocabulary::NIE::title()).toString();
            eventThing.setProperty(Nepomuk::Vocabulary::NIE::title(), eventTitle);

            //TODO aknadifeeder needs to be changed to respect pimo:Event and adds its tags there rather thatn to its ncal:Event
            QList<Nepomuk::Tag> ncalTags = selectedPart.tags();
            foreach(const Nepomuk::Tag &t, ncalTags)
                eventThing.addTag(t);

            resource.setProperty(propertyUrl, eventThing );
            eventThing.addProperty(Nepomuk::Vocabulary::NBIB::eventPublication(), resource );
        }

        setResource(m_publication); // this updates the changes in the current widget again
        subResourceUpdated(m_publication);
        emit resourceCacheNeedsUpdate(selectedPart);
    }

    delete lpd;
}

void PublicationWidget::selectLayout(BibEntryType entryType)
{
    switch(entryType) {
    case BibType_Article:
    case BibType_ForumPost:
    case BibType_BlogPost:
    case BibType_WebPage:
        layoutArticle();
        break;
    case BibType_Book:
    case BibType_Booklet:
    case BibType_Dictionary:
        layoutBook();
        break;
    case BibType_Collection:
    case BibType_Encyclopedia:
    case BibType_JournalIssue:
    case BibType_NewspaperIssue:
    case BibType_MagazinIssue:
    case BibType_Proceedings:
    case BibType_Forum:
    case BibType_Blog:
    case BibType_WebSite:
        layoutCollection();
        break;
    case BibType_Bachelorthesis:
    case BibType_Mastersthesis:
    case BibType_Phdthesis:
    case BibType_Thesis:
        layoutThesis();
        break;
    case BibType_Report:
    case BibType_Techreport:
        layoutReport();
        break;
    case BibType_Electronic:
    case BibType_Presentation:
        layoutElectronic();
        break;
    case BibType_Script:
        layoutScript();
        break;
    case BibType_Unpublished:
        layoutUnpublished();
        break;
    case BibType_Misc:
        layoutMisc();
        break;
    case BibType_Manual:
        layoutManual();
        break;
    case BibType_Standard:
        layoutMisc(); //TODO what is necessary for a standard? ISO etc
        qWarning() << "BibType_Standard no layout available, what is necessary?";
        break;
    case BibType_Patent:
        layoutPatent();
        break;
    case BibType_CodeOfLaw:
        layoutCodeOfLaw();
        break;
    case BibType_CourtReporter:
        layoutMisc();
        qWarning() << "BibType_CourtReporter no layout available, what is necessary?";
        break;
    case BibType_Legislation:
        layoutMisc();
        qWarning() << "BibType_Legislation no layout available, what is necessary?";
        break;
    case BibType_Bill:
        layoutBill();
        break;
    case BibType_Map:
        layoutMap();
        break;
    case BibType_Statute:
        layoutStatute();
        break;
    case BibType_LegalCaseDocument:
        layoutCase();
        break;
    case BibType_Decision:
        layoutMisc();
        qWarning() << "BibType_Decision no layout available, what is necessary?";
        break;
    case BibType_Brief:
        layoutMisc();
        qWarning() << "BibType_Brief no layout available, what is necessary?";
        break;
    case Max_BibTypes:
        break;
    }
}

void PublicationWidget::layoutArticle()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(false);
    ui->editAssignee->setVisible(false);
    ui->editFillingDate->setVisible(false);

    //Extra
    ui->editEdition->setVisible(false);
    ui->editEvent->setVisible(true);
    ui->editCollection->setVisible(true);
    ui->editSeries->setVisible(true);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(false);
    ui->editType->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}

void PublicationWidget::layoutBook()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(false);
    ui->editAssignee->setVisible(false);
    ui->editFillingDate->setVisible(false);

    //Extra
    ui->editEdition->setVisible(true);
    ui->editEvent->setVisible(true);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(true);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}

void PublicationWidget::layoutThesis()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFillingDate->setVisible(false);

    //Extra
    ui->editEdition->setVisible(false);
    ui->editEvent->setVisible(false);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(true);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(false);
    ui->editNumber->setVisible(false);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}

void PublicationWidget::layoutReport()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFillingDate->setVisible(false);

    //Extra
    ui->editEdition->setVisible(true);
    ui->editEvent->setVisible(true);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(true);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}

void PublicationWidget::layoutCollection()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFillingDate->setVisible(false);

    //Extra
    ui->editEdition->setVisible(true);
    ui->editEvent->setVisible(true);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(true);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}

void PublicationWidget::layoutMisc()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);
    ui->editAssignee->setVisible(true);
    ui->editFillingDate->setVisible(true);

    //Extra
    ui->editEdition->setVisible(true);
    ui->editEvent->setVisible(true);
    ui->editCollection->setVisible(true);
    ui->editSeries->setVisible(true);
    ui->editCode->setVisible(true);
    ui->editCourtReporter->setVisible(true);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editApplicationNumber->setVisible(true);
    ui->editPriorityNumbers->setVisible(true);
    ui->editPublicLawNumber->setVisible(true);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);
    ui->editReferences->setVisible(true);
    ui->editLegalStatus->setVisible(true);
}

void PublicationWidget::layoutElectronic()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFillingDate->setVisible(false);

    //Extra
    ui->editEdition->setVisible(false);
    ui->editEvent->setVisible(true);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(true);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(false);
    ui->editNumber->setVisible(false);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}

void PublicationWidget::layoutScript()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFillingDate->setVisible(false);

    //Extra
    ui->editEdition->setVisible(true);
    ui->editEvent->setVisible(true);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(true);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(false);
    ui->editNumber->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}

void PublicationWidget::layoutUnpublished()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(false);
    ui->editDate->setVisible(false);
    ui->editPublisher->setVisible(false);
    ui->editOrganization->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFillingDate->setVisible(false);

    //Extra
    ui->editEdition->setVisible(false);
    ui->editEvent->setVisible(true);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(true);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(false);
    ui->editNumber->setVisible(false);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(false);
    ui->editType->setVisible(true);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}

void PublicationWidget::layoutManual()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(false);
    ui->editAssignee->setVisible(false);
    ui->editFillingDate->setVisible(false);

    //Extra
    ui->editEdition->setVisible(true);
    ui->editEvent->setVisible(false);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(true);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(false);
    ui->editNumber->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}

void PublicationWidget::layoutPatent()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editAssignee->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editFillingDate->setVisible(true);
    ui->editPublisher->setVisible(false);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editEdition->setVisible(false);
    ui->editEvent->setVisible(false);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(false);
    ui->editNumber->setVisible(false);
    ui->editApplicationNumber->setVisible(true);
    ui->editPriorityNumbers->setVisible(true);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(false);
    ui->editType->setVisible(false);
    ui->editReferences->setVisible(true);
    ui->editLegalStatus->setVisible(true);
}

void PublicationWidget::layoutBill()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(false);
    ui->editAssignee->setVisible(false);
    ui->editDate->setVisible(false);
    ui->editFillingDate->setVisible(false);
    ui->editPublisher->setVisible(false);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editEdition->setVisible(false);
    ui->editEvent->setVisible(true);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(false);
    ui->editCode->setVisible(true);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(false);
    ui->editType->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}

void PublicationWidget::layoutStatute()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(false);
    ui->editAssignee->setVisible(false);
    ui->editDate->setVisible(true);
    ui->editFillingDate->setVisible(false);
    ui->editPublisher->setVisible(false);
    ui->editOrganization->setVisible(false);

    //Extra
    ui->editEdition->setVisible(false);
    ui->editEvent->setVisible(true);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(false);
    ui->editCode->setVisible(true);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(false);
    ui->editNumber->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(true);
    ui->editHowPublished->setVisible(false);
    ui->editType->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}

void PublicationWidget::layoutCodeOfLaw()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(false);
    ui->editAssignee->setVisible(false);
    ui->editDate->setVisible(false);
    ui->editFillingDate->setVisible(false);
    ui->editPublisher->setVisible(false);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editEdition->setVisible(false);
    ui->editEvent->setVisible(true);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(false);
    ui->editCode->setVisible(true);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(false);
    ui->editType->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}

void PublicationWidget::layoutMap()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);
    ui->editAssignee->setVisible(true);
    ui->editFillingDate->setVisible(true);

    //Extra
    ui->editEdition->setVisible(true);
    ui->editEvent->setVisible(true);
    ui->editCollection->setVisible(true);
    ui->editSeries->setVisible(true);
    ui->editCode->setVisible(true);
    ui->editCourtReporter->setVisible(true);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editApplicationNumber->setVisible(true);
    ui->editPriorityNumbers->setVisible(true);
    ui->editPublicLawNumber->setVisible(true);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);
    ui->editReferences->setVisible(true);
    ui->editLegalStatus->setVisible(true);
}

void PublicationWidget::layoutCase()
{
    //Basics
    ui->editTitle->setVisible(true);
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(false);
    ui->editAssignee->setVisible(false);
    ui->editDate->setVisible(true);
    ui->editFillingDate->setVisible(false);
    ui->editPublisher->setVisible(false);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editEdition->setVisible(false);
    ui->editEvent->setVisible(false);
    ui->editCollection->setVisible(false);
    ui->editSeries->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(true);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editHowPublished->setVisible(false);
    ui->editType->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
}


