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
#include "propertywidgets/stringedit.h"
#include "propertywidgets/contactedit.h"
#include "listpublicationsdialog.h"
#include "listpartswidget.h"

#include "nbib.h"
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/PIMO>

#include <KDE/KGlobalSettings>
#include <KDE/KDialog>

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include <QtCore/QDebug>

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

    showCreateReference(true);

    //connect signal/slots
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->chapterEdit, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->citeKeyEdit, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->pagesEdit, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->publicationEdit, SLOT(setResource(Nepomuk::Resource&)));
    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(changeRating(int)));

    connect(ui->publicationEdit, SIGNAL(textChanged(QString)), this, SLOT(enableReferenceDetails()));
    connect(ui->publicationEdit, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showPublicationList()));
    connect(ui->chapterEdit, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showChapterList()));
}

void ReferenceWidget::setResource(Nepomuk::Resource & resource)
{
    // what we get is a nbib::BibReference
    if (resource.hasType(Nepomuk::Vocabulary::NBIB::Reference()) ) {
        m_reference = resource;
        showCreateReference(false);
        enableReferenceDetails();

        emit resourceChanged(m_reference);

        Nepomuk::Resource pub = m_reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
        ui->editRating->setRating(pub.rating());
    }
    else {
        showCreateReference(true);
        qDebug() << "nonvalid resource for the referencewidget";
    }

}

void ReferenceWidget::setLibrary(Library *p)
{
    SidebarComponent::setLibrary(p);

    //TODO remove and use ResourceWatcher later on
    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(subResourceUpdated()));
    connect(ui->chapterEdit, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
    connect(ui->citeKeyEdit, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
    connect(ui->pagesEdit, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
    connect(ui->publicationEdit, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
    connect(this, SIGNAL(resourceUpdated(Nepomuk::Resource)), p, SIGNAL(resourceUpdated(Nepomuk::Resource)));
}

void ReferenceWidget::subResourceUpdated()
{
    emit resourceUpdated(m_reference);
}

void ReferenceWidget::showCreateReference(bool createRef)
{
    if(createRef) {
        ui->frameWidget->setEnabled(false);
    }
    else {
        ui->frameWidget->setEnabled(true);

        enableReferenceDetails();
    }
}

void ReferenceWidget::showPublicationList()
{
    ListPublicationsDialog lpd;
    lpd.setSystemLibrary(library());

    int ret = lpd.exec();

    if(ret == QDialog::Accepted) {
        Nepomuk::Resource selectedPart = lpd.selectedPublication();

        m_reference.setProperty(Nepomuk::Vocabulary::NBIB::publication(), selectedPart );
        ui->publicationEdit->setResource(m_reference);
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
        //ui->chapterEdit->setResource(m_reference);

        QString pageStart = selectedPart.property(Nepomuk::Vocabulary::NBIB::pageStart() ).toString();
        QString pageEnd = selectedPart.property(Nepomuk::Vocabulary::NBIB::pageEnd() ).toString();
        QString pages = pageStart + QLatin1String("-") + pageEnd;
        m_reference.setProperty(Nepomuk::Vocabulary::NBIB::pages(), pages);

        setResource(m_reference);
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

    if(library()->libraryType() == Library_Project) {
        //relate the ref to the project
        newReference.setProperty(Nepomuk::Vocabulary::PIMO::isRelated() , library()->pimoLibrary());
    }
    newReference.setProperty(Nepomuk::Vocabulary::NBIB::citeKey(), i18n("new reference"));

    setResource(newReference);
}

void ReferenceWidget::deleteButtonClicked()
{
    m_reference.remove();
    showCreateReference(true);
}

void ReferenceWidget::changeRating(int newRating)
{
    Nepomuk::Resource pub = m_reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
    pub.setRating(newRating);
}
