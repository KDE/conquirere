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
#include "sro/nbib/publication.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/NUAO>
#include <Nepomuk/Variant>

#include <KDE/KDebug>
#include <QtCore/QDateTime>

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

    Nepomuk::SimpleResource publicationRes(resource().uri());
    Nepomuk::NBIB::Publication publication(publicationRes);
    //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
    QDateTime datetime = QDateTime::currentDateTimeUtc();
    publicationRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

    Nepomuk::PIMO::Event newEvent;
    newEvent.addType(NIE::InformationElement());

    newEvent.setProperty(NIE::title(), newEventTitle.trimmed());
    newEvent.setProperty(NAO::prefLabel(), newEventTitle.trimmed());

    newEvent.addProperty( NBIB::eventPublication(), publication );
    publication.setEvent( newEvent.uri() );

    graph << newEvent << publication;

    m_changedResource = resource();
    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
}
