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

#include "collectionedit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>
#include <QtGui/QInputDialog>
#include <QtCore/QUrl>

CollectionEdit::CollectionEdit(QWidget *parent) :
    PropertyEdit(parent)
{
    setPropertyUrl( Nepomuk::Vocabulary::NBIB::collection() );
    setUseDetailDialog(true);

    m_collectionType = Nepomuk::Vocabulary::NBIB::Collection();
    m_seriesType =  Nepomuk::Vocabulary::NBIB::Series();
}

void CollectionEdit::setupLabel()
{
    //check if resource() is an publication with attached Issue collection
    QString title;

    Nepomuk::Resource collectionResource = resource().property( propertyUrl() ).toResource();
    if(collectionResource.isValid()) {
        Nepomuk::Resource seriesResource = collectionResource.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();

        // we show the name of the collection
        // if it has no name we have a look at the series and show this name instead
        // makes sense for Article with Issue as collection we like to show the Name of the Journal for example
        title = collectionResource.property(Nepomuk::Vocabulary::NIE::title()).toString();
        if(title.isEmpty()) {
            title = seriesResource.property(Nepomuk::Vocabulary::NIE::title()).toString();
        }

        addPropertryEntry(title, collectionResource.resourceUri().toString());

        m_collectionType = collectionResource.resourceType(); // saves which subclass of collection is used
        m_seriesType = seriesResource.resourceType(); // saves which subclass of series is used
    }

    setLabelText(title);
}

void CollectionEdit::updateResource(const QString & text)
{
    Nepomuk::Resource currentColection = resource().property( propertyUrl() ).toResource();

    if(text.isEmpty()) {
        resource().removeProperty( propertyUrl() );
        currentColection.removeProperty(Nepomuk::Vocabulary::NBIB::article(), resource());
        return;
    }

    // ok text is not empty
    // try to find the propertyurl of an already existing collection (issue)
    QUrl propUrl = propertyEntry(text);
    Nepomuk::Resource newCollection = Nepomuk::Resource(propUrl);

    if(currentColection.isValid()) {
        if(newCollection.isValid()) {
            //remove old links
            resource().removeProperty( propertyUrl() );
            currentColection.removeProperty(Nepomuk::Vocabulary::NBIB::article(), resource());

            //add new collection
            resource().setProperty( propertyUrl(), newCollection);
            newCollection.addProperty(Nepomuk::Vocabulary::NBIB::article(), resource());
        }
        else {
            // rename the current collection
            currentColection.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
        }
        return;
    }

    // no currentCollection available

    if(!newCollection.isValid()) {
        // create a new collection with the string text as title
        newCollection = Nepomuk::Resource(QUrl(), m_collectionType);
        newCollection.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
        // any issue is also part of an series.
        // only true for MagazinIssue, NewspaperIssue, JournalIssue
        if( m_collectionType == Nepomuk::Vocabulary::NBIB::JournalIssue() ||
               m_collectionType == Nepomuk::Vocabulary::NBIB::NewspaperIssue() ||
               m_collectionType == Nepomuk::Vocabulary::NBIB::MagazinIssue() ) {

            Nepomuk::Resource newSeries(QUrl(), m_seriesType);
            newSeries.setProperty(Nepomuk::Vocabulary::NIE::title(), text);

            // seems to be a bug, not the full hierachry will be set so create it ourself
            if(m_seriesType != Nepomuk::Vocabulary::NBIB::Series()) {
                newSeries.addType(Nepomuk::Vocabulary::NBIB::Series());
            }
            newSeries.addType(Nepomuk::Vocabulary::NIE::InformationElement());

            // connect series and collection
            newCollection.setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), newSeries);
            newSeries.addProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), newCollection);
        }

    }

    //connect article and collection
    resource().addProperty( Nepomuk::Vocabulary::NBIB::collection(), newCollection);
    newCollection.addProperty( Nepomuk::Vocabulary::NBIB::article(), resource());
}

QList<QStandardItem*> CollectionEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QList<QStandardItem*> results;

    foreach(const Nepomuk::Query::Result & r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().property(Nepomuk::Vocabulary::NIE::title()).toString());

        item->setData(r.resource().resourceUri().toString());

        results.append(item);
    }

    return results;
}
