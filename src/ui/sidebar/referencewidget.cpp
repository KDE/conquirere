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

#include "../semantic/labeledit.h"
#include "../semantic/contactedit.h"

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
    ui->publicationEdit->setPropertyUrl( Nepomuk::Vocabulary::NBIB::hasResource() );

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

    }
    else {
        showCreateReference(true);
    }

    emit resourceChanged(m_reference);
}

void ReferenceWidget::clear()
{

}

void ReferenceWidget::showCreateReference(bool createRef)
{
    if(createRef) {
        //ui->newRefSpacer->setVisible(true);
        ui->createRefLabel->setVisible(true);
        ui->createButton->setVisible(true);
        ui->removeButton->setVisible(false);
    }
    else {
        ui->createRefLabel->setVisible(false);
        //ui->newRefSpacer->setVisible(false);
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
}

void ReferenceWidget::removeReference()
{

}

