/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "eventwidget.h"
#include "ui_eventwidget.h"

#include "core/library.h"

#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Variant>

#include <QtCore/QDebug>

EventWidget::EventWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::EventWidget)
{
    ui->setupUi(this);

    ui->editTags->setPropertyCardinality(PropertyEdit::MULTIPLE_PROPERTY);
    ui->editTags->setPropertyUrl( Soprano::Vocabulary::NAO::hasTag() );
    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(changeRating(int)));

    ui->editAttendee->setPropertyCardinality(PropertyEdit::MULTIPLE_PROPERTY);
    ui->editAttendee->setPropertyUrl( Nepomuk::Vocabulary::PIMO::attendee() );
    ui->editName->setPropertyCardinality(PropertyEdit::UNIQUE_PROPERTY);
    ui->editName->setPropertyUrl( Nepomuk::Vocabulary::NIE::title() );

    ui->editPlace->setPropertyCardinality(PropertyEdit::UNIQUE_PROPERTY);
    ui->editPlace->setPropertyUrl( Nepomuk::Vocabulary::NCO::addressLocation() );
}

EventWidget::~EventWidget()
{
    delete ui;
}

void EventWidget::setResource(Nepomuk::Resource & resource)
{
    m_event = resource;

    if(!m_event.isValid()) {
        ui->frameWidget->setEnabled(false);
    }
    else {
        ui->frameWidget->setEnabled(true);
    }

    ui->editRating->setRating(m_event.rating());
    ui->editTags->setResource(m_event);
    ui->editAttendee->setResource(m_event);
    ui->editName->setResource(m_event);
    ui->editPlace->setResource(m_event);
    ui->listPartsWidget->setResource(m_event);

    emit resourceChanged(m_event);
}

void EventWidget::setLibrary(Library *p)
{
    SidebarComponent::setLibrary(p);

    //TODO remove and use ResourceWatcher later on
    connect(ui->editTags, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), p, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editName, SIGNAL(textChanged(QString)), this, SLOT(subResourceUpdated()));
    connect(ui->editAttendee, SIGNAL(textChanged(QString)), this, SLOT(subResourceUpdated()));
    connect(ui->editPlace, SIGNAL(textChanged(QString)), this, SLOT(subResourceUpdated()));
    connect(this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), p, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
}

void EventWidget::newButtonClicked()
{
    m_event = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::PIMO::Event());

    m_event.setProperty(Nepomuk::Vocabulary::NIE::title(), i18n("New event title"));

    setResource(m_event);
}

void EventWidget::deleteButtonClicked()
{
    QList<Nepomuk::Resource> pubList = m_event.property(Nepomuk::Vocabulary::NBIB::eventPublication()).toResourceList();

    foreach(Nepomuk::Resource r, pubList) {
        r.removeProperty(Nepomuk::Vocabulary::NBIB::event(), m_event);

        emit resourceCacheNeedsUpdate(r);
    }

    m_event.remove();
    setResource(m_event);
}

void EventWidget::changeRating(int newRating)
{
    m_event.setRating(newRating);

    emit resourceCacheNeedsUpdate(m_event);
}

void EventWidget::subResourceUpdated()
{
    emit resourceCacheNeedsUpdate(m_event);
}
