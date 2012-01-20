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

#include "organizationedit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>
#include <QtCore/QUrl>

OrganizationEdit::OrganizationEdit(QWidget *parent)
    : PropertyEdit(parent)
{
    setPropertyUrl( Nepomuk::Vocabulary::NBIB::organization() );
}

void OrganizationEdit::setupLabel()
{
    QString title;

    //different cases are handled here
    Nepomuk::Resource organization;

    // I. the resource is an Article, means the organization is attached to the Collection
    // not the article itself
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        Nepomuk::Resource collection = resource().property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
        organization = collection.property(propertyUrl()).toResource();
    }
    // otherwise the organization is directly connected to the publication
    // institution for a thesis, court for a case, and so on
    else {
        organization = resource().property(propertyUrl()).toResource();
    }

    title = organization.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    addPropertryEntry(title, organization.resourceUri().toString());

    setLabelText(title);
}

void OrganizationEdit::updateResource(const QString & text)
{
    // the publication is the resource where the organization is attached to
    // it is the resource directly for most parts, but the Collection for an article
    Nepomuk::Resource publication;
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        publication = resource().property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
    }
    else {
        publication = resource();
    }

    // only remove organization if no text was specified
    if(text.isEmpty()) {
        publication.removeProperty(propertyUrl());
        return;
    }

    // else the text changed
    Nepomuk::Resource currentOrganization = publication.property(propertyUrl()).toResource();;

    // try to find the propertyurl of an already existing organizatzion
    QUrl propUrl = propertyEntry(text);
    Nepomuk::Resource newOrganizatzion = Nepomuk::Resource(propUrl);

    if(currentOrganization.isValid()) {
        if(newOrganizatzion.isValid()) {
            // switch to new organization
            publication.setProperty( propertyUrl(), newOrganizatzion);
        }
        else {
            // rename current organization
            currentOrganization.setProperty(Nepomuk::Vocabulary::NCO::fullname(), text);
        }
        return;
    }

    // if no current organization can be found
    // add existing organizatzion with the name of text
    // or create a new one
    if(!newOrganizatzion.isValid()) {
        newOrganizatzion = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::OrganizationContact());
        newOrganizatzion.setProperty(Nepomuk::Vocabulary::NCO::fullname(), text);
    }

    publication.setProperty( propertyUrl(), newOrganizatzion);
}

QList<QStandardItem*> OrganizationEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QList<QStandardItem*> results;

    foreach(const Nepomuk::Query::Result & r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().property(Nepomuk::Vocabulary::NCO::fullname()).toString());

        item->setData(r.resource().resourceUri().toString());

        results.append(item);
    }

    return results;
}
