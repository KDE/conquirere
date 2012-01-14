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

    connect(ui->editTags, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
    connect(ui->editName, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
    connect(ui->editAttendee, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
    connect(ui->editPlace, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
    connect(ui->listPartsWidget, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SLOT(subResourceUpdated()));
}

EventWidget::~EventWidget()
{
    delete ui;
}

void EventWidget::setResource(Nepomuk::Resource & resource)
{
    m_eventThing = resource.pimoThing();
    if(resource.resourceUri() == m_eventThing.resourceUri()) {
        m_eventResource = m_eventThing.property(Nepomuk::Vocabulary::PIMO::groundingOccurrence()).toResource();
    }
    else {
        m_eventResource = resource;
    }

    //now copy all tags from the ncal:event to the pimo:event where it belongs to
    //TODO aknadifeeder needs to be changed to respect pimo:Event and adds its tags there rather thatn to its ncal:Event
    QList<Nepomuk::Tag> ncalTags = m_eventResource.tags();
    foreach(const Nepomuk::Tag &t, ncalTags)
        m_eventThing.addTag(t);

    if(!m_eventThing.isValid() && !m_eventResource.isValid()) {
        ui->frameWidget->setEnabled(false);
    }
    else {
        ui->frameWidget->setEnabled(true);
    }

    ui->editRating->setRating(m_eventThing.rating());
    ui->editTags->setResource(m_eventThing);
    ui->editAttendee->setResource(m_eventThing);
    ui->editName->setResource(m_eventThing);
    ui->editPlace->setResource(m_eventThing);
    ui->listPartsWidget->setResource(m_eventThing);
}

void EventWidget::newButtonClicked()
{
    m_eventThing = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::PIMO::Event());

    m_eventThing.setProperty(Nepomuk::Vocabulary::NIE::title(), i18n("New event title"));

    setResource(m_eventThing);
}

void EventWidget::deleteButtonClicked()
{
    QList<Nepomuk::Resource> pubList = m_eventThing.property(Nepomuk::Vocabulary::NBIB::eventPublication()).toResourceList();

    foreach(const Nepomuk::Resource &r, pubList) {
        r.removeProperty(Nepomuk::Vocabulary::NBIB::event(), m_eventThing);

        emit resourceCacheNeedsUpdate(r);
    }

    m_eventThing.remove();
    setResource(m_eventThing);
}

void EventWidget::changeRating(int newRating)
{
    if(newRating != m_eventThing.rating()) {
        m_eventThing.setRating(newRating);

        emit resourceCacheNeedsUpdate(m_eventThing);
        emit resourceCacheNeedsUpdate(m_eventResource);
    }
}

void EventWidget::subResourceUpdated()
{
    // emit event cache changes
    emit resourceCacheNeedsUpdate(m_eventThing);
    emit resourceCacheNeedsUpdate(m_eventResource);

    // also emit changes to the publications cache entries
    QList<Nepomuk::Resource> pubList = m_eventThing.property(Nepomuk::Vocabulary::NBIB::eventPublication()).toResourceList();

    foreach(const Nepomuk::Resource &r, pubList) {
        emit resourceCacheNeedsUpdate(r);
    }
}
