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
}

void EventEdit::setupLabel()
{
    QString title;
    Nepomuk::Resource event = resource().property(propertyUrl()).toResource();

    title = event.property(Nepomuk::Vocabulary::NIE::title()).toString();

    addPropertryEntry(title, resource().uri());

    setLabelText(title);
}

void EventEdit::updateResource(const QString & text)
{
    resource().removeProperty( propertyUrl() );

    if(text.isEmpty())
        return;

    // try to find the propertyurl of an already existing event
    QUrl propUrl = propertyEntry(text);

    if(propUrl.isValid()) {
        resource().addProperty( propertyUrl(), Nepomuk::Resource(propUrl));
    }
    else {
        // create a new event with the string text as title
        Nepomuk::Resource newEvent(QUrl(), Nepomuk::Vocabulary::PIMO::Event());
        newEvent.setProperty(Nepomuk::Vocabulary::NIE::title(), text);

        resource().addProperty( propertyUrl(), newEvent);
    }
}

QStandardItemModel* EventEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *parentItem = model->invisibleRootItem();

    foreach(const Nepomuk::Query::Result & r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().property(Nepomuk::Vocabulary::NIE::title()).toString());

        item->setData(r.resource().uri());

        parentItem->appendRow(item);
    }

    return model;
}
