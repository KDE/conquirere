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

#include "core/librarymanager.h"

#include "propertywidgets/stringedit.h"
#include "propertywidgets/contactedit.h"

#include "publicationwidget.h"
#include "listpublicationsdialog.h"
#include "listpartswidget.h"

#include "nbibio/pipe/nepomuktovariantpipe.h"
#include "nbibio/bibtex/bibtexvariant.h"
#include <kbibtex/idsuggestions.h>

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <KDE/KJob>
#include "sro/nbib/reference.h"

#include "nbib.h"
#include <Nepomuk2/Variant>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

#include <KDE/KGlobalSettings>
#include <KDE/KDialog>
#include <KDE/KInputDialog>
#include <KDE/KDebug>

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtCore/QPointer>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

ReferenceWidget::ReferenceWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::ReferenceWidget)
{
    ui->setupUi(this);

    //set propertyURL of the edit elements
    ui->publicationEdit->setPropertyUrl( NBIB::publication() );
    ui->citeKeyEdit->setPropertyUrl( NBIB::citeKey() );
    ui->citeKeyEdit->setPropertyCardinality(PropertyEdit::UNIQUE_PROPERTY);
    ui->citeKeyEdit->setUseDetailDialog(true);
    ui->pagesEdit->setPropertyUrl( NBIB::pages() );
    ui->pagesEdit->setPropertyCardinality(PropertyEdit::UNIQUE_PROPERTY);

    //connect signal/slots
    connect(ui->editRating, SIGNAL(ratingChanged(uint)), this, SLOT(changeRating(uint)));

    connect(ui->publicationEdit, SIGNAL(textChanged(QString)), this, SLOT(enableReferenceDetails()));
    connect(ui->publicationEdit, SIGNAL(externalEditRequested(Nepomuk2::Resource&,QUrl)), this, SLOT(showPublicationList(Nepomuk2::Resource&,QUrl)));
    connect(ui->chapterEdit, SIGNAL(externalEditRequested(Nepomuk2::Resource&,QUrl)), this, SLOT(showChapterList()));
    connect(ui->citeKeyEdit, SIGNAL(externalEditRequested(Nepomuk2::Resource&,QUrl)), this, SLOT(showCiteKeySuggetion()));
}

void ReferenceWidget::setLibraryManager(LibraryManager *lm)
{
    ui->editAnnot->setLibraryManager(lm);
    SidebarComponent::setLibraryManager(lm);
}

Nepomuk2::Resource ReferenceWidget::resource()
{
    return m_reference;
}

void ReferenceWidget::setResource(Nepomuk2::Resource & resource)
{
    if(resource.isValid()) {
        setEnabled(true);
        m_reference = resource;
        enableReferenceDetails();

        Nepomuk2::Resource pub = m_reference.property(NBIB::publication()).toResource();
        ui->editRating->setRating( (int) pub.rating()); //unit setRating is deprecated
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

void ReferenceWidget::showPublicationList(Nepomuk2::Resource & reference, const QUrl & propertyUrl)
{
    Nepomuk2::Resource changedResource = reference.property(propertyUrl).toResource();

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
        Nepomuk2::Resource selectedPublication = lpd->selectedPublication();

        // do the crosslinking via DMS
        QList<QUrl> resUri; resUri << reference.uri();
        QVariantList value; value << selectedPublication.uri();
        KJob* job1 = Nepomuk2::setProperty(resUri, NBIB::publication(), value);
        job1->exec(); // blocking wait so we are sure we updated the resource

        resUri.clear(); resUri << selectedPublication.uri();
        value.clear(); value << reference.uri();
        KJob* job2 = Nepomuk2::addProperty(resUri, NBIB::reference(), value);
        job2->exec(); // blocking wait so we are sure we updated the resource
        Nepomuk2::addProperty(resUri, NAO::hasSubResource(), value);

        setResource(m_reference); // updates this widget
    }

    delete lpd;
}

void ReferenceWidget::showChapterList()
{
    QPointer<KDialog> kd = new KDialog(this);

    ListPartsWidget *lpw = new ListPartsWidget(kd);
    Nepomuk2::Resource publication = m_reference.property(NBIB::publication()).toResource();
    lpw->setResource(publication);
    lpw->setLibraryManager(libraryManager());

    kd->setMainWidget(lpw);

    int ret = kd->exec();

    if(ret == KDialog::Accepted) {
        Nepomuk2::Resource selectedPart = lpw->selectedPart();

        // do the crosslinking via DMS
        QList<QUrl> resUri; resUri << m_reference.uri();
        QVariantList value; value << selectedPart.uri();
        KJob* job1 = Nepomuk2::setProperty(resUri, NBIB::referencedPart(), value);
        job1->exec(); // blocking wait so we are sure we updated the resource

        QString pageStart = selectedPart.property(NBIB::pageStart() ).toString();
        QString pageEnd = selectedPart.property(NBIB::pageEnd() ).toString();
        QString pages = pageStart + QLatin1String("-") + pageEnd;

        resUri.clear(); resUri << m_reference.uri();
        value.clear(); value << pages;
        KJob* job2 = Nepomuk2::setProperty(resUri, NBIB::pages(), value);
        job2->exec(); // blocking wait so we are sure we updated the resource

        setResource(m_reference); // updates this widget
    }
}

void ReferenceWidget::showCiteKeySuggetion()
{
    // transform current reference to bibtex

    NepomukToVariantPipe ntvp;
    ntvp.pipeExport( QList<Nepomuk2::Resource>() << m_reference);
    QVariantList list = ntvp.variantList();

    File *f = BibTexVariant::fromVariant(list);

    Entry *entry = dynamic_cast<Entry *>(f->first().data());
    if(!entry) { qDebug() << "cast not possible"; return; }

    IdSuggestions ids;
    QString selection = KInputDialog::getItem( i18n("Select a new citekey"),
                                               i18n("Select a citekey from one of these suggestions"),
                                               ids.formatIdList(*entry) );

    if(!selection.isEmpty()) {
        // do the crosslinking via DMS
        QList<QUrl> resUri; resUri << m_reference.uri();
        QVariantList value; value << selection;
        KJob* job1 = Nepomuk2::setProperty(resUri, NBIB::citeKey(), value);
        job1->exec(); // blocking wait so we are sure we updated the resource

        ui->citeKeyEdit->setLabelText(selection);
    }

    delete f;
}

void ReferenceWidget::enableReferenceDetails()
{
    //check if a valid publication exist
    Nepomuk2::Resource publication = m_reference.property(NBIB::publication()).toResource();

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
    // first ask the user which publication he want to reference
    // never a good idea to have a reference that does not belong to a publication
    QPointer<ListPublicationsDialog> lpd = new ListPublicationsDialog(this);
    lpd->setListMode(BibGlobals::Resource_Publication, BibGlobals::Max_BibTypes);
    lpd->setLibraryManager(libraryManager());

    int ret = lpd->exec();

    if(ret != KDialog::Accepted) {

        delete lpd;
        return;
    }

    // if he selected something, retrive his selection
    Nepomuk2::Resource selectedPublication = lpd->selectedPublication();

    // create a new reference
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Reference newReference;

    newReference.setProperty( Nepomuk2::Vocabulary::NIE::title(), i18n("New Reference"));

    graph << newReference;
    //blocking graph save
    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNone);
    if( !srj->exec() ) {
        kWarning() << "could not new default series" << srj->errorString();
        return;
    }

    // get the reference resource from the return job mappings
    Nepomuk2::Resource newReferenceResource = Nepomuk2::Resource::fromResourceUri( srj->mappings().value( newReference.uri() ) );

    // do the crosslinking via DMS
    QList<QUrl> resUri; resUri << newReferenceResource.uri();
    QVariantList value; value << selectedPublication.uri();
    KJob* job1 = Nepomuk2::setProperty(resUri, NBIB::publication(), value);
    job1->exec(); // blocking wait so we are sure we updated the resource

    resUri.clear(); resUri << selectedPublication.uri();
    value.clear(); value << newReferenceResource.uri();
    KJob* job2 = Nepomuk2::addProperty(resUri, NBIB::reference(), value);
    job2->exec(); // blocking wait so we are sure we updated the resource
    Nepomuk2::addProperty(resUri, NAO::hasSubResource(), value);

    Library *curUsedLib = libraryManager()->currentUsedLibrary();
    if(curUsedLib && curUsedLib->libraryType() == BibGlobals::Library_Project) {
        curUsedLib->addResource( newReferenceResource );
    }

    setResource(newReferenceResource); // updates this widget

    delete lpd;
}

void ReferenceWidget::deleteButtonClicked()
{
    libraryManager()->systemLibrary()->deleteResource(m_reference);

    Nepomuk2::Resource emptyResource;
    setResource(emptyResource);
}

void ReferenceWidget::changeRating(uint newRating)
{
    Nepomuk2::Resource publication = m_reference.property(NBIB::publication()).toResource();

    if(newRating != publication.rating()) {
        publication.setRating(newRating);
    }
}
