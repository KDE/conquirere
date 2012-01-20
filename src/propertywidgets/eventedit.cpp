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

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>
#include <QtCore/QUrl>

EventEdit::EventEdit(QWidget *parent)
    :PropertyEdit(parent)
{
    setPropertyUrl(Nepomuk::Vocabulary::NBIB::event());
}

void EventEdit::setupLabel()
{
    QString title;
    Nepomuk::Resource event = resource().property(propertyUrl()).toResource();

    title = event.property(Nepomuk::Vocabulary::NIE::title()).toString();

    addPropertryEntry(title, event.resourceUri().toString());

    setLabelText(title);
}

void EventEdit::updateResource(const QString & text)
{
    Nepomuk::Resource currentEvent = resource().property(propertyUrl()).toResource();

    if(text.isEmpty()) {
        resource().removeProperty( propertyUrl() );
        currentEvent.removeProperty(Nepomuk::Vocabulary::NBIB::eventPublication(), resource());
        return;
    }

    // try to find the propertyurl of an already existing event
    QUrl propUrl = propertyEntry(text);
    Nepomuk::Resource newEvent = Nepomuk::Resource(propUrl);

    if(currentEvent.isValid()) {
        if(newEvent.isValid()) {
            // remove old link
            resource().removeProperty( propertyUrl() );
            currentEvent.removeProperty(Nepomuk::Vocabulary::NBIB::eventPublication(), resource());

            // set link to new event
            resource().setProperty( propertyUrl(), newEvent);
            newEvent.addProperty(Nepomuk::Vocabulary::NBIB::eventPublication(), resource());
        }
        else {
            //rename existing event
            currentEvent.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
        }
        return;
    }

    // no current event available, set to newEvent or create a new event
    if(!newEvent.isValid()) {
        newEvent = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::PIMO::Event());
        newEvent.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
    }

    resource().setProperty( propertyUrl(), newEvent);
    newEvent.addProperty(Nepomuk::Vocabulary::NBIB::eventPublication(), resource());
}

QList<QStandardItem*> EventEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QList<QStandardItem*> results;

    foreach(const Nepomuk::Query::Result & r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().property(Nepomuk::Vocabulary::NIE::title()).toString());

        item->setData(r.resource().resourceUri().toString());

        results.append(item);
    }

    return results;
}
