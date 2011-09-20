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

    //connect signal/slots
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->chapterEdit, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->citeKeyEdit, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->pagesEdit, SLOT(setResource(Nepomuk::Resource&)));
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->publicationEdit, SLOT(setResource(Nepomuk::Resource&)));


    showCreateReference(true);
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
    }
    else {
        ui->createRefLabel->setVisible(false);
        ui->createButton->setVisible(false);
        ui->removeButton->setVisible(true);
    }
}

void ReferenceWidget::showPublicationList()
{

}

void ReferenceWidget::createReference()
{
    qDebug() << "create ref";

    /*
    // create a new reference
    QList<QUrl> types;
    types.append( Nepomuk::Vocabulary::NBIB::BibReference() );

    Nepomuk::Resource ref;
    ref.setTypes(types); // set it to the type BibReference

    QString citeKey = ui->chapterEdit->getLabelText();
    if(!citeKey.isEmpty()) {
        ref.setProperty( Nepomuk::Vocabulary::NBIB::citeKey(), citeKey );
    }

    QString pages = ui->pagesEdit->getLabelText();
    if(!pages.isEmpty()) {
        ref.setProperty( Nepomuk::Vocabulary::NBIB::citeKey(), pages );
    }

    QString publication = ui->publicationEdit->getLabelText();
    if(!publication.isEmpty()) {
        ref.setProperty( Nepomuk::Vocabulary::NBIB::citeKey(), pages );
    }


    QString chapter = ui->chapterEdit->getLabelText();
    */

}

void ReferenceWidget::removeReference()
{

}

