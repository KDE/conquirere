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
#include "core/projectsettings.h"
#include "mainui/librarymanager.h"

#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include <KDE/KJob>
#include "sro/pimo/event.h"

#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Variant>

#include <KDE/KDebug>

using namespace Nepomuk::Vocabulary;
using namespace Soprano::Vocabulary;

EventWidget::EventWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::EventWidget)
{
    ui->setupUi(this);

    ui->editTags->setPropertyCardinality(PropertyEdit::MULTIPLE_PROPERTY);
    ui->editTags->setPropertyUrl( NAO::hasTag() );

    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(changeRating(int)));

    ui->editAttendee->setPropertyCardinality(PropertyEdit::MULTIPLE_PROPERTY);
    ui->editAttendee->setPropertyUrl( PIMO::attendee() );

    ui->editName->setPropertyCardinality(PropertyEdit::UNIQUE_PROPERTY);
    ui->editName->setPropertyUrl( NIE::title() );

    ui->editPlace->setPropertyCardinality(PropertyEdit::UNIQUE_PROPERTY);
    ui->editPlace->setPropertyUrl( NCO::addressLocation() );

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

void EventWidget::setLibraryManager(LibraryManager *lm)
{
    SidebarComponent::setLibraryManager(lm);
    ui->listPartsWidget->setLibraryManager(lm);
}

Nepomuk::Resource EventWidget::resource()
{
    return m_eventThing;
}

void EventWidget::setResource(Nepomuk::Resource & resource)
{
    // we start by getting the pimo:Thing of the rescource
    // as we list also ncal:Event stuff but do not want to add data to these events
    // we operate only on the Thing
    m_eventThing = resource.pimoThing();
    m_eventThing.addType(PIMO::Event());
    if(resource.resourceUri() == m_eventThing.resourceUri()) {
        m_eventResource = m_eventThing.property(PIMO::groundingOccurrence()).toResource();
    }
    else {
        m_eventResource = resource;
    }

    //now copy all tags from the ncal:event to the pimo:event where it belongs to
    //TODO aknadifeeder needs to be changed to respect pimo:Event and adds its tags there rather than to its ncal:Event
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
    //create a new resource with default name

    Nepomuk::SimpleResourceGraph graph;
    Nepomuk::PIMO::Event newEvent;
    newEvent.addType(NIE::InformationElement());

    newEvent.setProperty( NAO::prefLabel(), i18n("New Event"));
    newEvent.setProperty( NIE::title(), i18n("New Event"));

    graph << newEvent;

    //blocking graph save
    Nepomuk::StoreResourcesJob *srj = Nepomuk::storeResources(graph, Nepomuk::IdentifyNone);
    if( !srj->exec() ) {
        kWarning() << "could not new default series" << srj->errorString();
        return;
    }

    // get the pimo event from the return job mappings
    Nepomuk::Resource newEventResource = Nepomuk::Resource::fromResourceUri( srj->mappings().value( newEvent.uri() ) );

    Library *curUsedLib = libraryManager()->currentUsedLibrary();
    if(curUsedLib && curUsedLib->libraryType() == Library_Project) {
        curUsedLib->addResource( newEventResource );
    }

    setResource(newEventResource);
}

void EventWidget::deleteButtonClicked()
{
    QList<Nepomuk::Resource> pubList = m_eventThing.property(NBIB::eventPublication()).toResourceList();
    QList<QUrl> resUri;
    QVariantList value; value << m_eventThing.resourceUri();
    foreach(const Nepomuk::Resource &r, pubList) {
        resUri << r.resourceUri();
    }

    KJob *job1 = Nepomuk::addProperty(resUri, NBIB::event(), value);
    job1->exec(); // blocking wait ...

    foreach(const Nepomuk::Resource &r, pubList) {
        emit resourceCacheNeedsUpdate(r);
    }

    libraryManager()->systemLibrary()->deleteResource( m_eventThing );

    Nepomuk::Resource invalid;
    setResource(invalid);
}

void EventWidget::changeRating(int newRating)
{
    if(newRating == m_eventThing.rating() ) {
        return;
    }

    QList<QUrl> resourceUris; resourceUris << m_eventThing.resourceUri();
    QVariantList rating; rating <<  newRating;
    KJob *job = Nepomuk::setProperty(resourceUris, NAO::numericRating(), rating);

    if(job->exec()) {
        emit resourceCacheNeedsUpdate(m_eventThing);
    }
}

void EventWidget::subResourceUpdated()
{
    // emit event cache changes
    emit resourceCacheNeedsUpdate(m_eventThing);

    // also emit changes to the publications cache entries
    QList<Nepomuk::Resource> pubList = m_eventThing.property(NBIB::eventPublication()).toResourceList();

    foreach(const Nepomuk::Resource &r, pubList) {
        emit resourceCacheNeedsUpdate(r);
    }
}
