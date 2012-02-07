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

#include "eventedit.h"

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/simpleresource.h"
#include <KDE/KJob>
#include "sro/pimo/event.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Variant>

#include <KDE/KDebug>

using namespace Nepomuk::Vocabulary;
using namespace Soprano::Vocabulary;

EventEdit::EventEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void EventEdit::setupLabel()
{
    Nepomuk::Resource event = resource().property(NBIB::event()).toResource();

    QString title;
    title = event.property(NAO::prefLabel()).toString();
    if(title.isEmpty()) {
        title = event.property(NIE::title()).toString();
    }

    setLabelText(title);
}

void EventEdit::updateResource(const QString & newEventTitle)
{
    Nepomuk::Resource currentEvent = resource().property(NBIB::event()).toResource();

    QString curentTitle;

    curentTitle = currentEvent.property(NAO::prefLabel()).toString();
    if(curentTitle.isEmpty()) {
        curentTitle = currentEvent.property(NIE::title()).toString();
    }

    if(newEventTitle == curentTitle) {
        return; // nothing changed
    }

    if(currentEvent.exists()) {
        // remove the crosslink event <-> publication
        QList<QUrl> resourceUris; resourceUris << resource().uri();
        QVariantList value; value << currentEvent.uri();
        Nepomuk::removeProperty(resourceUris, NBIB::event(), value);

        resourceUris.clear(); resourceUris << currentEvent.uri();
        value.clear(); value << resource().uri();
        Nepomuk::removeProperty(resourceUris, NBIB::eventPublication(), value);
    }

    if(newEventTitle.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new event and merge it to the right place
    Nepomuk::SimpleResourceGraph graph;
    Nepomuk::PIMO::Event newEvent;
    newEvent.addType(NIE::InformationElement());

    newEvent.setProperty(NIE::title(), newEventTitle.trimmed());
    newEvent.setProperty(NAO::prefLabel(), newEventTitle.trimmed());

    graph << newEvent;

    m_editedResource = resource();
    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(addEvent(KJob*)));
}

void EventEdit::addEvent(KJob *job)
{
    if( job->error() != 0) {
        kDebug() << "could not create new event" << job->errorString();
        return;
    }

    Nepomuk::StoreResourcesJob *srj = dynamic_cast<Nepomuk::StoreResourcesJob *>(job);

    // now get all the uris for the new event
    QList<QUrl> eventUris;
    QVariantList eventValues;
    foreach (QUrl uri, srj->mappings()) {
         eventUris << uri;
         eventValues << uri;
    }

    // add the crosslink reference <-> publicatio
    QList<QUrl> resourceUris; resourceUris << m_editedResource.uri();
    Nepomuk::setProperty(resourceUris, NBIB::event(), eventValues);

    QVariantList value; value << m_editedResource.uri();
    Nepomuk::addProperty(eventUris, NBIB::eventPublication(), value);

    //TODO remove when resourcewatcher is working..
    emit resourceCacheNeedsUpdate(m_editedResource);
}
