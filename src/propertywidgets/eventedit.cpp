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

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/SimpleResource>

#include <KDE/KJob>
#include "sro/ncal/event.h"
#include "sro/nbib/publication.h"

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>

#include <KDE/KDebug>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

EventEdit::EventEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void EventEdit::setupLabel()
{
    Nepomuk2::Resource event = resource().property(NBIB::event()).toResource();

    QString title;
    title = event.property(NIE::title()).toString();
    if(title.isEmpty()) {
        title = event.property(NAO::prefLabel()).toString();
    }

    setLabelText(title);
}

void EventEdit::updateResource(const QString & newEventTitle)
{
    Nepomuk2::Resource currentEvent = resource().property(NBIB::event()).toResource();

    QString curentTitle;

    curentTitle = currentEvent.property(NIE::title()).toString();
    if(curentTitle.isEmpty()) {
        curentTitle = currentEvent.property(NAO::prefLabel()).toString();
    }

    if(newEventTitle == curentTitle) {
        return; // nothing changed
    }

    if(currentEvent.exists()) {
        // remove the crosslink event <-> publication
        Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), NBIB::event(), QVariantList() << currentEvent.uri());
        Nepomuk2::removeProperty(QList<QUrl>() << currentEvent.uri(), NBIB::eventPublication(), QVariantList() << resource().uri());

        // remove hasSubresource
        Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), NAO::hasSubResource(), QVariantList() << currentEvent.uri());

        //TODO: maybe make an option of the "event deletion" if it has no publication and is not in akonadi
        // if the event has no other publication and is not in akonadi tha ndelete it
//        QList<Nepomuk2::Resource> publist = currentEvent.property( NBIB::eventPublication()).toResourceList();
//        if(publist.isEmpty() || (publist.size() == 1 && publist.first().uri() == resource().uri())) {
//            //FIXME: check if event has akonadi information
//            Nepomuk2::removeResources(QList<QUrl>() << currentEvent.uri());
//        }
    }

    if(newEventTitle.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new event and merge it to the right place
    Nepomuk2::SimpleResourceGraph graph;

    Nepomuk2::NBIB::Publication publication(resource().uri());
    Nepomuk2::NCAL::Event newEvent;

    newEvent.setProperty(NIE::title(), newEventTitle.trimmed());
    newEvent.setProperty(NAO::prefLabel(), newEventTitle.trimmed());

    newEvent.addProperty( NBIB::eventPublication(), publication );
    publication.setEvent( newEvent.uri() );
    publication.addProperty(NAO::hasSubResource(), newEvent.uri());

    graph << newEvent << publication;

    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );
}
