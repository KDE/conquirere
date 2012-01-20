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

#include "publicationedit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>

PublicationEdit::PublicationEdit(QWidget *parent)
    : PropertyEdit(parent)
{
    setPropertyUrl(Nepomuk::Vocabulary::NBIB::publication());
    setUseDetailDialog(true);
}

void PublicationEdit::setupLabel()
{
    Nepomuk::Resource publication = resource().property(propertyUrl()).toResource();

    QString title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();

    addPropertryEntry(title, publication.resourceUri().toString());

    setLabelText(title);
}

void PublicationEdit::updateResource(const QString & text)
{
    // resource is a nbib:reference
    Nepomuk::Resource currentPublication = resource().property( propertyUrl() ).toResource();

    if(text.isEmpty()) {
        // remove the existing publication
        resource().removeProperty( propertyUrl() );
        // remove backlink too
        currentPublication.removeProperty( Nepomuk::Vocabulary::NBIB::reference(), resource());
        return;
    }

    QUrl propUrl = propertyEntry(text);
    Nepomuk::Resource selectedPublication = Nepomuk::Resource(propUrl);

    if(currentPublication.isValid()) {
        if(selectedPublication.isValid()) {
            //remove old links
            resource().removeProperty( propertyUrl() );
            currentPublication.removeProperty( Nepomuk::Vocabulary::NBIB::reference(), resource());

            //add the new links
            resource().addProperty( propertyUrl(), selectedPublication);
            selectedPublication.addProperty(Nepomuk::Vocabulary::NBIB::reference(), resource());
            return;
        }
        else {
            //rename if no publication with the new name already exist
            currentPublication.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
            return;
        }
    }

    // if no current publication exist, but we found one with the new name, link to it
    if(!selectedPublication.isValid()) {
        selectedPublication = Nepomuk::Resource (propUrl, Nepomuk::Vocabulary::NBIB::Publication());
        selectedPublication.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
    }

    resource().setProperty( propertyUrl(), selectedPublication);
    selectedPublication.addProperty(Nepomuk::Vocabulary::NBIB::reference(), resource());
}

QList<QStandardItem*> PublicationEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QList<QStandardItem*> results;

    foreach(const Nepomuk::Query::Result & r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().property(Nepomuk::Vocabulary::NIE::title()).toString());

        item->setData(r.resource().resourceUri().toString());

        results.append(item);
    }

    return results;
}
