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

#include "../../propertywidgets/stringedit.h"
#include "../../propertywidgets/contactedit.h"

#include "nbib.h"
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>
#include <KGlobalSettings>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>

#include <QDebug>

ReferenceWidget::ReferenceWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::ReferenceWidget)
{
    ui->setupUi(this);

    setFont(KGlobalSettings::smallestReadableFont());

    //set propertyURL of the edit elements
    ui->chapterEdit->setPropertyUrl( Nepomuk::Vocabulary::NBIB::hasChapter() );
    ui->citeKeyEdit->setPropertyUrl( Nepomuk::Vocabulary::NBIB::citeKey() );
    ui->pagesEdit->setPropertyUrl( Nepomuk::Vocabulary::NBIB::pages() );
    ui->publicationEdit->setPropertyUrl( Nepomuk::Vocabulary::NBIB::usePublication() );

    showCreateReference(true);

    //connect signal/slots
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->chapterEdit, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->citeKeyEdit, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->pagesEdit, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->publicationEdit, SLOT(setResource(Nepomuk::Resource&)));

    connect(ui->publicationEdit, SIGNAL(textChanged(QString)), this, SLOT(showChapter()));
}

void ReferenceWidget::setResource(Nepomuk::Resource & resource)
{
    // what we get is a nbib::BibReference
    if (resource.hasType(Nepomuk::Vocabulary::NBIB::BibReference()) ) {
        m_reference = resource;
        showCreateReference(false);

        emit resourceChanged(m_reference);
    }
    else {
        showCreateReference(true);
    }

}

void ReferenceWidget::clear()
{

}

void ReferenceWidget::showCreateReference(bool createRef)
{
    if(createRef) {
        ui->createRefLabel->setVisible(true);
        ui->createButton->setVisible(true);
        ui->removeButton->setVisible(false);
        ui->chapterEdit->setVisible(false);
        ui->citeKeyEdit->setVisible(false);
        ui->pagesEdit->setVisible(false);
        ui->publicationEdit->setVisible(false);

        ui->label_Chapter->setVisible(false);
        ui->label_CiteKey->setVisible(false);
        ui->label_Pages->setVisible(false);
        ui->label_Publication->setVisible(false);
    }
    else {
        ui->createRefLabel->setVisible(false);
        ui->createButton->setVisible(false);
        ui->removeButton->setVisible(true);
        ui->citeKeyEdit->setVisible(true);
        ui->pagesEdit->setVisible(true);
        ui->publicationEdit->setVisible(true);

        ui->label_CiteKey->setVisible(true);
        ui->label_Pages->setVisible(true);
        ui->label_Publication->setVisible(true);

        showChapter();
    }
}

void ReferenceWidget::showPublicationList()
{

}

void ReferenceWidget::showChapter()
{
    // show chapter is called when the label of the publication edit widget
    // changes, which only happens when the resource is adapted first
    // so check the resource
    QList<Nepomuk::Resource> list = ui->publicationEdit->propertyResources();

    if(list.isEmpty()) {
        ui->chapterEdit->setVisible(false);
        ui->label_Chapter->setVisible(false);
        return;
    }

    Nepomuk::Resource publication = list.first();
    if(publication.hasType(Nepomuk::Vocabulary::NBIB::Book())) {
        ui->chapterEdit->show();
        ui->chapterEdit->setVisible(true);
        ui->label_Chapter->setVisible(true);
    }
    else {
        ui->chapterEdit->setVisible(false);
        ui->label_Chapter->setVisible(false);
    }
}

void ReferenceWidget::createReference()
{
    // create a new reference
    Nepomuk::Resource newReference(QUrl(), Nepomuk::Vocabulary::NBIB::BibReference());

    setResource(newReference);
}

void ReferenceWidget::removeReference()
{
    m_reference.remove();
    showCreateReference(true);
}

