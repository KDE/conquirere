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

#include "core/library.h"
#include "core/projectsettings.h"
#include "mainui/librarymanager.h"

#include "propertywidgets/contactedit.h"

#include "referencewidget.h"
#include "eventwidget.h"
#include "serieswidget.h"
#include "contactdialog.h"
#include "addchapterdialog.h"
#include "listpublicationsdialog.h"
#include "listcitedsources.h"
#include "sidebarwidget.h"

#include "nbibio/conquirere.h"

#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include <KDE/KJob>
#include "sro/nbib/publication.h"

#include "nbib.h"
#include <Nepomuk/Thing>
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NUAO>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>
#include <KDE/KComboBox>
#include <KDE/KDialog>
#include <KDE/KDebug>

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtCore/QPointer>
#include <QtCore/QUrl>

using namespace Nepomuk::Vocabulary;
using namespace Soprano::Vocabulary;

PublicationWidget::PublicationWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::PublicationWidget)
{
    ui->setupUi(this);

    ui->tabWidget->setEnabled(false);

    setupWidget();

    if(qobject_cast<SidebarWidget*>(parent)) {
        connect(this, SIGNAL(hasReference(bool)), parent, SLOT(hasReference(bool)));
    }
}

PublicationWidget::~PublicationWidget()
{
    saveAnnotationContent();
    delete ui;
}

void PublicationWidget::setLibraryManager(LibraryManager *lm)
{
    SidebarComponent::setLibraryManager(lm);

    ui->editCitedSources->setLibraryManager(lm);
    ui->listPartsWidget->setLibraryManager(lm);
    ui->editAnnot->setLibraryManager(lm);
    ui->editFileObject->setLibraryManager(lm);
}

void PublicationWidget::setResource(Nepomuk::Resource & resource)
{
    m_publication = resource;

    //check if the resource is a valid publication
    if(!m_publication.exists()) {
        ui->tabWidget->setEnabled(false);
    }
    else {
        ui->tabWidget->setEnabled(true);
    }

    QList<Nepomuk::Resource> references = m_publication.property(NBIB::reference()).toResourceList();

    if(references.isEmpty()) {
        emit hasReference(false);
    }
    else {
        emit hasReference(true);
    }

    BibEntryType entryType = BibEntryTypeFromUrl(m_publication);

    int index = ui->editEntryType->findData(entryType);
    ui->editEntryType->setCurrentIndex(index);

    QString abstract = m_publication.property(NBIB::abstract()).toString();
    ui->editAbstract->document()->setPlainText(abstract);

    saveAnnotationContent(); // save previous annotation content, to be save here
    ui->editAnnotText->document()->clear();

    ui->editRating->setRating(m_publication.rating());

    selectLayout(entryType);

    ui->listPartsWidget->setResource(m_publication);

   ui->editTitle->setResource(m_publication);
   ui->editAuthors->setResource(m_publication);
   ui->editEditor->setResource(m_publication);
   ui->editAssignee->setResource(m_publication);
   ui->editDate->setResource(m_publication);
   ui->editFilingDate->setResource(m_publication);
   ui->editPublisher->setResource(m_publication);
   ui->editOrganization->setResource(m_publication);
   ui->editFileObject->setResource(m_publication);
   ui->editTags->setResource(m_publication);
   ui->editTopics->setResource(m_publication);

   ui->editShortTitle->setResource(m_publication);
   ui->editTranslator->setResource(m_publication);
   ui->editContributor->setResource(m_publication);
   ui->editReviewedAuthor->setResource(m_publication);
   ui->editCoSponsor->setResource(m_publication);
   ui->editCounsel->setResource(m_publication);
   ui->editCommenter->setResource(m_publication);
   ui->editAttorneyAgent->setResource(m_publication);
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

   ui->editAnnot->setResource(m_publication);
   ui->editCitedSources->setResource(m_publication);
}

Nepomuk::Resource PublicationWidget::resource()
{
    return m_publication;
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
        newtype.append(NIE::InformationElement());
        newtype.append(NBIB::Publication());
        newtype.append(newEntryUrl);

        // add another hierarchy if the newEntryUrl is not a direct subclass of NBIB::Publication()
        switch(entryType) {
        case BibType_Dictionary:
            newtype.append(NBIB::Book());
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
            newtype.append(NBIB::Collection());
            break;
        case BibType_Bachelorthesis:
        case BibType_Mastersthesis:
        case BibType_Phdthesis:
            newtype.append(NBIB::Thesis());
            break;
        case BibType_Techreport:
            newtype.append(NBIB::Report());
            break;
        case BibType_BlogPost:
        case BibType_ForumPost:
        case BibType_WebPage:
            newtype.append(NBIB::Article());
            break;
        case BibType_Bill:
        case BibType_Statute:
            newtype.append(NBIB::Legislation());
            newtype.append(NBIB::LegalDocument());
            break;
        case BibType_Decision:
        case BibType_Brief:
            newtype.append(NBIB::LegalCaseDocument());
            newtype.append(NBIB::LegalDocument());
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
        case BibType_Map:
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
        if(m_publication.hasType(NBIB::Collection())) {
            Nepomuk::Resource seriesResource = m_publication.property((NBIB::inSeries())).toResource();

            if(seriesResource.isValid()) {
                if(m_publication.hasType(NBIB::JournalIssue())) {
                    Nepomuk::Resource x(QUrl(), NBIB::Journal());
                    seriesResource.setTypes(x.types());
                }
                else if(m_publication.hasType(NBIB::NewspaperIssue())) {
                    Nepomuk::Resource x(QUrl(), NBIB::Newspaper());
                    seriesResource.setTypes(x.types());
                }
                else if(m_publication.hasType(NBIB::MagazinIssue())) {
                    Nepomuk::Resource x(QUrl(), NBIB::Magazin());
                    seriesResource.setTypes(x.types());
                }
                else {
                    Nepomuk::Resource x(QUrl(), NBIB::Series());
                    seriesResource.setTypes(x.types());
                }
            }
        }
        else if(m_publication.hasType(NBIB::Book())) {
            Nepomuk::Resource seriesResource = m_publication.property((NBIB::inSeries())).toResource();
            if(seriesResource.isValid()) {
                Nepomuk::Resource x(QUrl(), NBIB::BookSeries());
                seriesResource.setTypes(x.types());
            }
        }

        //DEBUG If we change collection type (forum/blog/website) also change ArticleType (forumpost/blogpost/webpage)
    }
}

void PublicationWidget::newButtonClicked()
{
    //create a new resource with default name

    Nepomuk::SimpleResourceGraph graph;
    Nepomuk::NBIB::Publication newPublication;

    newPublication.setProperty( Nepomuk::Vocabulary::NIE::title(), i18n("New Publication"));

    graph << newPublication;
    //blocking graph save
    Nepomuk::StoreResourcesJob *srj = Nepomuk::storeResources(graph, Nepomuk::IdentifyNone);
    if( !srj->exec() ) {
        kWarning() << "could not new default series" << srj->errorString();
        return;
    }

    // get the pimo project from the return job mappings
    Nepomuk::Resource newPublicationResource = Nepomuk::Resource::fromResourceUri( srj->mappings().value( newPublication.uri() ) );

    Library *curUsedLib = libraryManager()->currentUsedLibrary();
    if(curUsedLib && curUsedLib->libraryType() == Library_Project) {
        curUsedLib->addResource( newPublicationResource );
    }

    setResource(newPublicationResource);
}

void PublicationWidget::deleteButtonClicked()
{
    libraryManager()->systemLibrary()->deleteResource(m_publication);

    Nepomuk::Resource emptyResource;
    setResource(emptyResource);
}

void PublicationWidget::subResourceUpdated(Nepomuk::Resource resource)
{
    if(resource.resourceUri() != m_publication.resourceUri())
        emit resourceCacheNeedsUpdate(resource);

    emit resourceCacheNeedsUpdate(m_publication);

    // also update the cache entry for any kind of connected resource
    QList<Nepomuk::Resource> refs = m_publication.property(NBIB::reference()).toResourceList();
    foreach(const Nepomuk::Resource &r, refs) {
        emit resourceCacheNeedsUpdate(r);
    }

    Nepomuk::Resource event = m_publication.property(NBIB::event()).toResource();
    if(event.isValid()) {
        emit resourceCacheNeedsUpdate(event);
    }

    Nepomuk::Resource series = m_publication.property(NBIB::inSeries()).toResource();
    if(series.isValid()) {
        emit resourceCacheNeedsUpdate(series);
    }

    QList<Nepomuk::Resource> articles = m_publication.property(NBIB::article()).toResourceList();
    foreach(const Nepomuk::Resource &a, articles) {
        emit resourceCacheNeedsUpdate(a);
    }

    Nepomuk::Resource collection = m_publication.property(NBIB::collection()).toResource();
    if(collection.isValid()) {
        emit resourceCacheNeedsUpdate(collection);
    }
}

void PublicationWidget::addReference()
{
    QPointer<KDialog> showRefWidget = new KDialog(this);

    Nepomuk::Resource tempRef(QUrl(), NBIB::Reference());
    tempRef.setProperty(NBIB::publication(), m_publication);

    ReferenceWidget *rw = new ReferenceWidget(showRefWidget);
    rw->setLibraryManager( libraryManager() );
    rw->newButtonClicked();
    Nepomuk::Resource tmpReference = rw->resource();

    QList<QUrl> resourceUris; resourceUris << tmpReference.resourceUri();
    QVariantList value; value << m_publication.resourceUri();
    KJob *job = Nepomuk::setProperty(resourceUris, NBIB::publication(), value);
    job->exec(); //blocking to ensure resource is fully updated


    showRefWidget->setMainWidget(rw);
    showRefWidget->setInitialSize(QSize(300,300));

    int ret = showRefWidget->exec();

    if(ret == KDialog::Accepted) {
        QList<QUrl> resourceUris; resourceUris << m_publication.resourceUri();
        QVariantList value; value <<  tmpReference.resourceUri();
        KJob *job = Nepomuk::setProperty(resourceUris, NBIB::reference(), value);
        job->exec(); //blocking to ensure resource is fully updated

        emit resourceCacheNeedsUpdate(m_publication);
    }
    else {
        // remove temp citation again
        libraryManager()->systemLibrary()->deleteResource( tmpReference );
    }
}

void PublicationWidget::removeReference()
{
    QList<QAction*> actionCollection;
    QMenu removeReference;

    QList<Nepomuk::Resource> referenceList = m_publication.property(NBIB::reference()).toResourceList();

    foreach(const Nepomuk::Resource &r, referenceList) {
        QAction *a = new QAction(r.property(NBIB::citeKey()).toString(), this);
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

    if(!a) { return; }

    QList<QUrl> resourceUris; resourceUris << m_publication.resourceUri();
    QVariantList value; value << a->data().toString();
    KJob *job = Nepomuk::removeProperty(resourceUris, NBIB::reference(), value);
    job->exec(); // blocking wait till resource is updated

    QList<Nepomuk::Resource> references = m_publication.property(NBIB::reference()).toResourceList();

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
    QList<QUrl> resourceUris; resourceUris << m_publication.resourceUri();
    QVariantList value; value <<  ui->editAbstract->document()->toPlainText();
    Nepomuk::setProperty(resourceUris, NBIB::abstract(), value);
}

void PublicationWidget::discardContentChanges()
{
    QString abstract = m_publication.property(NBIB::abstract()).toString();
    ui->editAbstract->document()->setPlainText(abstract);
}

void PublicationWidget::newAnnotationSelected(Nepomuk::Resource & noteResource)
{
    saveAnnotationContent();

    m_currentAnnotation = noteResource;
    QString content = noteResource.property(NIE::htmlContent()).toString();
    if(content.isEmpty()) {
        content = noteResource.property(NIE::plainTextContent()).toString();
    }

    ui->editAnnotText->document()->setHtml(content);
}

void PublicationWidget::saveAnnotationContent()
{
    if(!m_currentAnnotation.isValid())
        return;

    QList<QUrl> resUri; resUri << m_currentAnnotation.resourceUri();

    QVariantList value; value << ui->editAnnotText->document()->toPlainText();
    Nepomuk::setProperty(resUri, NIE::plainTextContent(), value);

    value.clear(); value << ui->editAnnotText->document()->toHtml();
    Nepomuk::setProperty(resUri, NIE::htmlContent(), value);

    emit resourceCacheNeedsUpdate(m_currentAnnotation);
}

void PublicationWidget::changeRating(int newRating)
{
    if(newRating == m_publication.rating() ) {
        return;
    }

    QList<QUrl> resourceUris; resourceUris << m_publication.resourceUri();
    QVariantList rating; rating <<  newRating;
    Nepomuk::setProperty(resourceUris, Soprano::Vocabulary::NAO::numericRating(), rating);

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
    connect(ui->editFileObject, SIGNAL(openDocument(Nepomuk::Resource&,bool)), this, SIGNAL(openDocument(Nepomuk::Resource&,bool)));

    // Basics section
    ui->editTitle->setPropertyUrl( NIE::title() );
    ui->editAuthors->setPropertyUrl( NCO::creator() );
    ui->editAuthors->setUseDetailDialog(true);
    connect(ui->editAuthors, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editEditor->setPropertyUrl( NBIB::editor() );
    ui->editEditor->setUseDetailDialog(true);
    connect(ui->editEditor, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editAssignee->setPropertyUrl( NBIB::assignee() );
    ui->editDate->setPropertyUrl( NBIB::publicationDate() );
    ui->editFilingDate->setPropertyUrl( NBIB::filingDate() );
    ui->editPublisher->setPropertyUrl( NCO::publisher() );
    ui->editPublisher->setUseDetailDialog(true);
    connect(ui->editPublisher, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editTags->setPropertyCardinality(PropertyEdit::MULTIPLE_PROPERTY);
    ui->editTags->setPropertyUrl( NAO::hasTag() );
    ui->editTopics->setPropertyCardinality(PropertyEdit::MULTIPLE_PROPERTY);
    ui->editTopics->setPropertyUrl( NAO::hasTopic() );

    //other section
    ui->editShortTitle->setPropertyUrl( NBIB::shortTitle() );
    ui->editTranslator->setPropertyUrl( NBIB::translator() );
    ui->editTranslator->setUseDetailDialog(true);
    connect(ui->editTranslator, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editContributor->setPropertyUrl( NBIB::contributor() );
    ui->editContributor->setUseDetailDialog(true);
    connect(ui->editContributor, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editReviewedAuthor->setPropertyUrl( NBIB::reviewedAuthor() );
    ui->editReviewedAuthor->setUseDetailDialog(true);
    connect(ui->editReviewedAuthor, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editCoSponsor->setPropertyUrl( NBIB::coSponsor() );
    ui->editCoSponsor->setUseDetailDialog(true);
    connect(ui->editCoSponsor, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editCounsel->setPropertyUrl( NBIB::counsel() );
    ui->editCounsel->setUseDetailDialog(true);
    connect(ui->editCounsel, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editCommenter->setPropertyUrl( NBIB::commenter() );
    ui->editCommenter->setUseDetailDialog(true);
    connect(ui->editCommenter, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editAttorneyAgent->setPropertyUrl( NBIB::attorneyAgent() );
    ui->editAttorneyAgent->setUseDetailDialog(true);
    connect(ui->editAttorneyAgent, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
    ui->editCopyright->setPropertyUrl( NIE::copyright() );
    ui->editLastAccessed->setPropertyUrl( NUAO::lastUsage());
    ui->editLanguage->setPropertyUrl( NIE::language());

    // Extra section
    ui->editEvent->setPropertyUrl( NBIB::event() );
    ui->editEvent->setUseDetailDialog(true);
    connect(ui->editEvent, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showDetailDialog(Nepomuk::Resource&,QUrl)));
    ui->editSeries->setUseDetailDialog(true);
    connect(ui->editSeries, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showDetailDialog(Nepomuk::Resource&,QUrl)));
    ui->editEdition->setPropertyUrl( NBIB::edition() );
    ui->editCollection->setPropertyUrl( NBIB::collection() );
    ui->editCollection->setUseDetailDialog(true);
    connect(ui->editCollection, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showDetailDialog(Nepomuk::Resource&,QUrl)));
    ui->editCode->setPropertyUrl( NBIB::codeOfLaw() );
    ui->editCode->setUseDetailDialog(true);
    connect(ui->editCode, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showDetailDialog(Nepomuk::Resource&,QUrl)));
    ui->editCourtReporter->setPropertyUrl( NBIB::courtReporter() );
    ui->editCourtReporter->setUseDetailDialog(true);
    connect(ui->editCourtReporter, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showDetailDialog(Nepomuk::Resource&,QUrl)));
    ui->editVolume->setPropertyUrl( NBIB::volume() );
    ui->editNumber->setPropertyUrl( NBIB::number() );
    ui->editApplicationNumber->setPropertyUrl( NBIB::applicationNumber() );
    ui->editPriorityNumbers->setPropertyUrl( NBIB::priorityNumbers() );
    ui->editPublicLawNumber->setPropertyUrl( NBIB::publicLawNumber() );
    ui->editReferences->setPropertyUrl( NBIB::patentReferences() );
    ui->editLegalStatus->setPropertyUrl( NBIB::legalStatus() );
    ui->editHistory->setPropertyUrl( NBIB::history() );
    ui->editScale->setPropertyUrl( NBIB::mapScale() );
    ui->editHowPublished->setPropertyUrl( NBIB::publicationMethod() );
    ui->editType->setPropertyUrl( NBIB::publicationType() );

    // identification section
    ui->editArchive->setPropertyUrl( NBIB::archive() );
    ui->editArchiveLocation->setPropertyUrl( NBIB::archiveLocation() );
    ui->editLibCatalog->setPropertyUrl( NBIB::libraryCatalog() );
    ui->editEprint->setPropertyUrl( NBIB::eprint() );
    ui->editISBN->setPropertyUrl( NBIB::isbn() );
    ui->editISSN->setPropertyUrl( NBIB::issn() );
    ui->editLCCN->setPropertyUrl( NBIB::lccn() );
    ui->editMRNumber->setPropertyUrl( NBIB::mrNumber() );
    ui->editPubMed->setPropertyUrl( NBIB::pubMed() );
    ui->editDOI->setPropertyUrl( NBIB::doi() );

    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(changeRating(int)));

    //TODO remove and use ResourceWatcher later on
    connect(ui->editTitle, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editAuthors, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editEditor, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editAssignee, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editDate, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editFilingDate, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editPublisher, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editTags, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editTopics, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    connect(ui->editShortTitle, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editTranslator, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editContributor, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editReviewedAuthor, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editCommenter, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editCoSponsor, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editCounsel, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editAttorneyAgent, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
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
    connect(ui->editCitedSources, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));
    connect(ui->editAnnot, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated(Nepomuk::Resource)));

    connect(ui->editAnnot, SIGNAL(selectedAnnotation(Nepomuk::Resource&)), this, SLOT(newAnnotationSelected(Nepomuk::Resource&)));
}

void PublicationWidget::editContactDialog(Nepomuk::Resource & resource, const QUrl & propertyUrl)
{
    QPointer<ContactDialog> cd = new ContactDialog(this);
    cd->setResource(resource, propertyUrl);

    cd->exec();

    delete cd;

    // because this slot gets called from different PropertyEditWidgets
    // we find the right une (the sender()) and update the main resource, so the changed contacs will be reloaded
    ContactEdit *ce = dynamic_cast<ContactEdit *>(sender());
    ce->setResource(resource);
}

void PublicationWidget::showDetailDialog(Nepomuk::Resource & resource, const QUrl & propertyUrl)
{
    Nepomuk::Resource changedResource = resource.property(propertyUrl).toResource();

    // first if the resource is valid, we just want to edit it
    if(changedResource.isValid()) {
        QPointer<KDialog> addIssueWidget = new KDialog(this);
        if(changedResource.hasType(PIMO::Event())) {
            EventWidget *ew = new EventWidget(addIssueWidget);
            ew->setLibraryManager(libraryManager());
            ew->setResource(changedResource);
            addIssueWidget->setMainWidget(ew);
        }
        else if(changedResource.hasType(NBIB::Series())) {
            SeriesWidget *sw = new SeriesWidget(addIssueWidget);
            sw->setLibraryManager(libraryManager());
            sw->setResource(changedResource);
            addIssueWidget->setMainWidget(sw);
        }
        else {
            PublicationWidget *pw = new PublicationWidget(addIssueWidget);
            pw->setLibraryManager(libraryManager());
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
    // a) create a new one
    // b) select from a list of existing resources

    //get the range of the property (so what we are allowed to enter)
    //Nepomuk::Resource nr(propertyUrl);
    //Nepomuk::Resource range = nr.property(QUrl(QLatin1String("http://www.w3.org/2000/01/rdf-schema#range"))).toResource();
    // not working sadly :/
kDebug() << propertyUrl;
    QPointer<ListPublicationsDialog> lpd = new ListPublicationsDialog(this);
    if(propertyUrl == NBIB::inSeries()) {
        lpd->setListMode(Resource_Series, BibEntryType(Max_SeriesTypes));
    }
    else if(propertyUrl == NBIB::codeOfLaw()) {
        lpd->setListMode(Resource_Publication, BibType_CodeOfLaw);
    }
    else if(propertyUrl == NBIB::courtReporter()) {
        lpd->setListMode(Resource_Publication, BibType_CourtReporter);
    }
    else if(propertyUrl == NBIB::collection()) {
        lpd->setListMode(Resource_Publication, BibType_Collection);
    }
    else if(propertyUrl == NBIB::event()) {
        lpd->setListMode(Resource_Event, Max_BibTypes);
    }
    else {
        lpd->setListMode(Resource_Reference, Max_BibTypes);
    }

    lpd->setLibraryManager(libraryManager());

    int ret = lpd->exec();

    if(ret == QDialog::Accepted) {
        Nepomuk::Resource selectedPart = lpd->selectedPublication();

        if(propertyUrl == NBIB::event()) {
            // switch from ncal:Event to pimo:Event
            QString eventTitle = selectedPart.property(NIE::title()).toString();
            QList<Nepomuk::Tag> ncalTags = selectedPart.tags();
            QList<QUrl> resourceUris; resourceUris << selectedPart.resourceUri();
            QVariantList value; value << eventTitle;

            selectedPart = selectedPart.pimoThing();
            selectedPart.addType(PIMO::Event());
            selectedPart.addType(NIE::InformationElement());

            Nepomuk::setProperty(resourceUris, NIE::title(), value);
            Nepomuk::setProperty(resourceUris, NAO::prefLabel(), value);

            foreach(const Nepomuk::Tag &t, ncalTags)
                selectedPart.addTag(t);
        }

        // add forward link
        QList<QUrl> resourceUris; resourceUris << resource.resourceUri();
        QVariantList value; value << selectedPart.resourceUri();
        KJob *job = Nepomuk::setProperty(resourceUris, propertyUrl, value);
        job->exec(); //blocking to ensure we udate the resource

        // here I need to take into account, that backlinks must be handled somehow
        QUrl backwardLink;
        if(propertyUrl == NBIB::inSeries()) {
            backwardLink = NBIB::seriesOf();
        }
        else if(propertyUrl == NBIB::codeOfLaw()) {
            backwardLink = NBIB::legislation();
        }
        else if(propertyUrl == NBIB::courtReporter()) {
            backwardLink = NBIB::legalCase();
        }
        else if(propertyUrl == NBIB::collection()) {
            backwardLink = NBIB::article();
        }
        else if(propertyUrl == NBIB::event()) {
            backwardLink = NBIB::eventPublication();
        }

        // add backward link
        resourceUris.clear(); resourceUris << selectedPart.resourceUri();
        value.clear(); value << resource.resourceUri();
        KJob *job2 = Nepomuk::setProperty(resourceUris, backwardLink, value);
        job2->exec(); //blocking to ensure we udate the resource

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
    case BibType_WebPage:
        layoutArticle();
        break;
    case BibType_ForumPost:
    case BibType_BlogPost:
        layoutArticleExtra();
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
        layoutStandard();
        break;
    case BibType_Patent:
        layoutPatent();
        break;
    case BibType_CodeOfLaw:
        layoutCodeOfLaw();
        break;
    case BibType_CourtReporter:
        layoutCourtReporter();
        break;
    case BibType_Legislation:
    case BibType_Bill:
    case BibType_Statute:
        layoutLegislation();
        break;
    case BibType_Map:
        layoutMap();
        break;
    case BibType_Brief:
    case BibType_Decision:
    case BibType_LegalCaseDocument:
        layoutCase();
        break;
    case Max_BibTypes:
        break;
    }
}

void PublicationWidget::layoutArticle()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(true);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(true);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutArticleExtra()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(false);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(false);
    ui->editOrganization->setVisible(false);

    //Extra
    ui->editCollection->setVisible(true);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(true);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(true);
}

void PublicationWidget::layoutBook()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(false);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutCollection()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(false);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutThesis()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(false);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(false);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(false);
    ui->editNumber->setVisible(false);
    ui->editEdition->setVisible(false);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(true);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutReport()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(true);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutElectronic()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(true);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutScript()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(true);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutUnpublished()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editDate->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(false);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(false);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(false);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(true);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(true);
}

void PublicationWidget::layoutMisc()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editDate->setVisible(true);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(true);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(true);
}

void PublicationWidget::layoutManual()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(false);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(false);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutStandard()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(true);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutPatent()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(false);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(true);
    ui->editFilingDate->setVisible(true);
    ui->editPublisher->setVisible(false);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(false);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(false);
    ui->editApplicationNumber->setVisible(true);
    ui->editPriorityNumbers->setVisible(true);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(true);
    ui->editLegalStatus->setVisible(true);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(false);
    ui->editType->setVisible(false);

    //Other
    ui->editReviewedAuthor->setVisible(false);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(true);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutCodeOfLaw()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(false);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutCourtReporter()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(true);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(true);
    ui->editType->setVisible(true);

    //Other
    ui->editReviewedAuthor->setVisible(false);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutLegislation()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(false);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(false);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(true);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(false);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(true);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(true);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(false);
    ui->editType->setVisible(false);

    //Other
    ui->editReviewedAuthor->setVisible(false);
    ui->editCoSponsor->setVisible(true);
    ui->editCounsel->setVisible(true);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutMap()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(false);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(true);
    ui->editOrganization->setVisible(false);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(false);
    ui->editVolume->setVisible(false);
    ui->editNumber->setVisible(false);
    ui->editEdition->setVisible(true);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(false);
    ui->editScale->setVisible(true);
    ui->editHowPublished->setVisible(false);
    ui->editType->setVisible(false);

    //Other
    ui->editReviewedAuthor->setVisible(false);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(false);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}

void PublicationWidget::layoutCase()
{
    //Basics
    ui->editAuthors->setVisible(true);
    ui->editEditor->setVisible(false);
    ui->editDate->setVisible(true);
    ui->editAssignee->setVisible(false);
    ui->editFilingDate->setVisible(false);
    ui->editPublisher->setVisible(false);
    ui->editOrganization->setVisible(true);

    //Extra
    ui->editCollection->setVisible(false);
    ui->editCode->setVisible(false);
    ui->editCourtReporter->setVisible(true);
    ui->editVolume->setVisible(true);
    ui->editNumber->setVisible(true);
    ui->editEdition->setVisible(false);
    ui->editApplicationNumber->setVisible(false);
    ui->editPriorityNumbers->setVisible(false);
    ui->editPublicLawNumber->setVisible(false);
    ui->editReferences->setVisible(false);
    ui->editLegalStatus->setVisible(false);
    ui->editHistory->setVisible(true);
    ui->editScale->setVisible(false);
    ui->editHowPublished->setVisible(false);
    ui->editType->setVisible(false);

    //Other
    ui->editReviewedAuthor->setVisible(false);
    ui->editCoSponsor->setVisible(false);
    ui->editCounsel->setVisible(true);
    ui->editAttorneyAgent->setVisible(false);
    ui->editCommenter->setVisible(false);
}
