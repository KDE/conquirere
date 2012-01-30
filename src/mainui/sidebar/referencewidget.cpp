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

#include "nbib.h"
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

#include <KDE/KGlobalSettings>
#include <KDE/KDialog>

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

ReferenceWidget::ReferenceWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::ReferenceWidget)
{
    ui->setupUi(this);

    //set propertyURL of the edit elements
    ui->chapterEdit->setPropertyUrl( Nepomuk::Vocabulary::NBIB::referencedPart() );
    ui->chapterEdit->setPropertyCardinality(PropertyEdit::UNIQUE_PROPERTY);
    ui->chapterEdit->setUseDetailDialog(true);
    ui->citeKeyEdit->setPropertyUrl( Nepomuk::Vocabulary::NBIB::citeKey() );
    ui->citeKeyEdit->setPropertyCardinality(PropertyEdit::UNIQUE_PROPERTY);
    ui->pagesEdit->setPropertyUrl( Nepomuk::Vocabulary::NBIB::pages() );
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

void ReferenceWidget::setResource(Nepomuk::Resource & resource)
{
    if(resource.isValid()) {
        setEnabled(true);
        m_reference = resource;
        enableReferenceDetails();

        Nepomuk::Resource pub = m_reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
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
    Nepomuk::Resource publication = m_reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();

    emit resourceCacheNeedsUpdate(m_reference);
    emit resourceCacheNeedsUpdate(publication);
}

void ReferenceWidget::showPublicationList(Nepomuk::Resource & resource, const QUrl & propertyUrl)
{
    Nepomuk::Resource changedResource = resource.property(propertyUrl).toResource();

    // first if the resource is valid, we just want to edit it
    if(changedResource.isValid()) {
        KDialog addIssueWidget;

        PublicationWidget *pw = new PublicationWidget();
        pw->setLibraryManager(libraryManager());
        pw->setResource(changedResource);
        addIssueWidget.setMainWidget(pw);

        addIssueWidget.setInitialSize(QSize(400,300));
        addIssueWidget.exec();

        setResource(m_reference); // this updates the changes in the current widget again

        //update the cache
        subResourceUpdated();

        return;
    }

    //2nd if no valid resource is availabe the user want
    // a) create a new one ignored, user should enter the title into the field first and press edit then
    // b) select from a list of existing resources

    ListPublicationsDialog lpd;
    lpd.setListMode(Resource_Publication, Max_BibTypes);
    lpd.setSystemLibrary(libraryManager()->systemLibrary());
    lpd.setOpenLibraries(libraryManager()->openProjects());

    int ret = lpd.exec();

    if(ret == QDialog::Accepted) {
        Nepomuk::Resource selectedPart = lpd.selectedPublication();

        resource.setProperty(Nepomuk::Vocabulary::NBIB::publication(), selectedPart );
        selectedPart.addProperty(Nepomuk::Vocabulary::NBIB::reference(), resource );

        setResource(m_reference); // this updates the changes in the current widget again

        //update the cache
        subResourceUpdated();
    }
}

void ReferenceWidget::showChapterList()
{
    KDialog kd;

    ListPartsWidget lpw;
    Nepomuk::Resource publication = m_reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
    lpw.setResource(publication);

    kd.setMainWidget(&lpw);

    int ret = kd.exec();

    if(ret == KDialog::Accepted) {
        Nepomuk::Resource selectedPart = lpw.selectedPart();
        m_reference.setProperty(Nepomuk::Vocabulary::NBIB::referencedPart(), selectedPart);

        QString pageStart = selectedPart.property(Nepomuk::Vocabulary::NBIB::pageStart() ).toString();
        QString pageEnd = selectedPart.property(Nepomuk::Vocabulary::NBIB::pageEnd() ).toString();
        QString pages;
        if(!pageEnd.isEmpty())
            pages = pageStart + QLatin1String("-") + pageEnd;
        else
            pages = pageStart;

        m_reference.setProperty(Nepomuk::Vocabulary::NBIB::pages(), pages);

        setResource(m_reference);
        emit resourceCacheNeedsUpdate(m_reference);
    }
}

void ReferenceWidget::enableReferenceDetails()
{
    //check if a valid publication exist
    Nepomuk::Resource publication = m_reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();

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
    Nepomuk::Resource newReference(QUrl(), Nepomuk::Vocabulary::NBIB::Reference());

    Library *curUsedLib = libraryManager()->currentUsedLibrary();
    if(curUsedLib && curUsedLib->libraryType() == Library_Project) {
        //relate the ref to the project
        newReference.setProperty(Soprano::Vocabulary::NAO::isRelated() , curUsedLib->settings()->projectThing());
    }

    newReference.setProperty(Nepomuk::Vocabulary::NBIB::citeKey(), i18n("new reference"));

    setResource(newReference);
}

void ReferenceWidget::deleteButtonClicked()
{
    libraryManager()->systemLibrary()->deleteResource(m_reference);

    Nepomuk::Resource emptyResource;
    setResource(emptyResource);
}

void ReferenceWidget::changeRating(int newRating)
{
    Nepomuk::Resource publication = m_reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();

    if(newRating != publication.rating()) {
        publication.setRating(newRating);
        subResourceUpdated();
    }
}
