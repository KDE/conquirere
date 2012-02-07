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

#include "referencewidget.h"
#include "ui_referencewidget.h"

#include "core/library.h"
#include "core/projectsettings.h"

#include "mainui/librarymanager.h"

#include "propertywidgets/stringedit.h"
#include "propertywidgets/contactedit.h"

#include "publicationwidget.h"
#include "listpublicationsdialog.h"
#include "listpartswidget.h"

#include "dms-copy/datamanagement.h"
#include <KDE/KJob>
#include "dms-copy/storeresourcesjob.h"
#include "dms-copy/simpleresourcegraph.h"
#include "sro/nbib/reference.h"

#include "nbib.h"
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

#include <KDE/KGlobalSettings>
#include <KDE/KDialog>
#include <KDE/KDebug>

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtCore/QPointer>

using namespace Nepomuk::Vocabulary;
using namespace Soprano::Vocabulary;

ReferenceWidget::ReferenceWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::ReferenceWidget)
{
    ui->setupUi(this);

    //set propertyURL of the edit elements
    ui->citeKeyEdit->setPropertyUrl( NBIB::citeKey() );
    ui->citeKeyEdit->setPropertyCardinality(PropertyEdit::UNIQUE_PROPERTY);
    ui->pagesEdit->setPropertyUrl( NBIB::pages() );
    ui->pagesEdit->setPropertyCardinality(PropertyEdit::UNIQUE_PROPERTY);

    //connect signal/slots
    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(changeRating(int)));

    connect(ui->publicationEdit, SIGNAL(textChanged(QString)), this, SLOT(enableReferenceDetails()));
    connect(ui->publicationEdit, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showPublicationList(Nepomuk::Resource&,QUrl)));
    connect(ui->chapterEdit, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showChapterList()));

    //TODO remove and use ResourceWatcher later on
    connect(ui->chapterEdit, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
    connect(ui->citeKeyEdit, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
    connect(ui->pagesEdit, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
    connect(ui->publicationEdit, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
    connect(ui->editAnnot, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
}

void ReferenceWidget::setLibraryManager(LibraryManager *lm)
{
    ui->editAnnot->setLibraryManager(lm);
    SidebarComponent::setLibraryManager(lm);
}

Nepomuk::Resource ReferenceWidget::resource()
{
    return m_reference;
}

void ReferenceWidget::setResource(Nepomuk::Resource & resource)
{
    if(resource.isValid()) {
        setEnabled(true);
        m_reference = resource;
        enableReferenceDetails();

        Nepomuk::Resource pub = m_reference.property(NBIB::publication()).toResource();
        ui->editRating->setRating(pub.rating());
    }
    else {
        setEnabled(false);
    }

    ui->chapterEdit->setResource(m_reference);
    ui->citeKeyEdit->setResource(m_reference);
    ui->pagesEdit->setResource(m_reference);
    ui->publicationEdit->setResource(m_reference);
    ui->editAnnot->setResource(m_reference);
}

void ReferenceWidget::subResourceUpdated()
{
    Nepomuk::Resource publication = m_reference.property(NBIB::publication()).toResource();

    emit resourceCacheNeedsUpdate(m_reference);
    emit resourceCacheNeedsUpdate(publication);
}

void ReferenceWidget::showPublicationList(Nepomuk::Resource & reference, const QUrl & propertyUrl)
{
    Nepomuk::Resource changedResource = reference.property(propertyUrl).toResource();

    // first if the resource is valid, we just want to edit it
    if(changedResource.isValid()) {
        QPointer<KDialog> addIssueWidget = new KDialog(this);

        PublicationWidget *pw = new PublicationWidget();
        pw->setLibraryManager(libraryManager());
        pw->setResource(changedResource);
        addIssueWidget->setMainWidget(pw);

        addIssueWidget->setInitialSize(QSize(400,300));
        addIssueWidget->exec();

        setResource(m_reference); // this updates the changes in the current widget again

        //update the cache
        subResourceUpdated();

        delete addIssueWidget;

        return;
    }

    //2nd if no valid resource is availabe the user want:
    // a) create a new one
    // b) select from a list of existing resources

    QPointer<ListPublicationsDialog> lpd = new ListPublicationsDialog(this);
    lpd->setLibraryManager(libraryManager());

    int ret = lpd->exec();

    if(ret == KDialog::Accepted) {
        Nepomuk::Resource selectedPublication = lpd->selectedPublication();

        // do the crosslinking via DMS
        QList<QUrl> resUri; resUri << reference.uri();
        QVariantList value; value << selectedPublication.uri();
        KJob* job1 = Nepomuk::setProperty(resUri, NBIB::publication(), value);
        job1->exec(); // blocking wait so we are sure we updated the resource

        resUri.clear(); resUri << selectedPublication.uri();
        value.clear(); value << reference.uri();
        KJob* job2 = Nepomuk::addProperty(resUri, NBIB::reference(), value);
        job2->exec(); // blocking wait so we are sure we updated the resource
        Nepomuk::addProperty(resUri, NAO::hasSubResource(), value);

        setResource(m_reference); // updates this widget
        subResourceUpdated();
    }

    delete lpd;
}

void ReferenceWidget::showChapterList()
{
    QPointer<KDialog> kd = new KDialog(this);

    ListPartsWidget *lpw = new ListPartsWidget(kd);
    Nepomuk::Resource publication = m_reference.property(NBIB::publication()).toResource();
    lpw->setResource(publication);
    lpw->setLibraryManager(libraryManager());

    kd->setMainWidget(lpw);

    int ret = kd->exec();

    if(ret == KDialog::Accepted) {
        Nepomuk::Resource selectedPart = lpw->selectedPart();

        // do the crosslinking via DMS
        QList<QUrl> resUri; resUri << m_reference.uri();
        QVariantList value; value << selectedPart.uri();
        KJob* job1 = Nepomuk::setProperty(resUri, NBIB::referencedPart(), value);
        job1->exec(); // blocking wait so we are sure we updated the resource

        QString pageStart = selectedPart.property(NBIB::pageStart() ).toString();
        QString pageEnd = selectedPart.property(NBIB::pageEnd() ).toString();
        QString pages = pageStart + QLatin1String("-") + pageEnd;

        resUri.clear(); resUri << m_reference.uri();
        value.clear(); value << pages;
        KJob* job2 = Nepomuk::setProperty(resUri, NBIB::pages(), value);
        job2->exec(); // blocking wait so we are sure we updated the resource

        setResource(m_reference); // updates this widget
        emit resourceCacheNeedsUpdate(m_reference);
    }
}

void ReferenceWidget::enableReferenceDetails()
{
    //check if a valid publication exist
    Nepomuk::Resource publication = m_reference.property(NBIB::publication()).toResource();

    if(publication.isValid()) {
        ui->pagesEdit->setEnabled(true);
        ui->citeKeyEdit->setEnabled(true);
        ui->chapterEdit->setEnabled(true);
    }
    else {
        ui->pagesEdit->setEnabled(false);
        ui->citeKeyEdit->setEnabled(false);
        ui->chapterEdit->setEnabled(false);
    }
}

void ReferenceWidget::newButtonClicked()
{
    // create a new reference
    Nepomuk::SimpleResourceGraph graph;
    Nepomuk::NBIB::Reference newReference;

    newReference.setProperty( Nepomuk::Vocabulary::NIE::title(), i18n("New Reference"));

    graph << newReference;
    //blocking graph save
    Nepomuk::StoreResourcesJob *srj = Nepomuk::storeResources(graph, Nepomuk::IdentifyNone);
    if( !srj->exec() ) {
        kWarning() << "could not new default series" << srj->errorString();
        return;
    }

    // get the reference resource from the return job mappings
    Nepomuk::Resource newReferenceResource = Nepomuk::Resource::fromResourceUri( srj->mappings().value( newReference.uri() ) );

    Library *curUsedLib = libraryManager()->currentUsedLibrary();
    if(curUsedLib && curUsedLib->libraryType() == Library_Project) {
        curUsedLib->addResource( newReferenceResource );
    }

    setResource(newReferenceResource);
}

void ReferenceWidget::deleteButtonClicked()
{
    libraryManager()->systemLibrary()->deleteResource(m_reference);

    Nepomuk::Resource emptyResource;
    setResource(emptyResource);
}

void ReferenceWidget::changeRating(int newRating)
{
    Nepomuk::Resource publication = m_reference.property(NBIB::publication()).toResource();

    if(newRating != publication.rating()) {
        publication.setRating(newRating);
        subResourceUpdated();
    }
}
