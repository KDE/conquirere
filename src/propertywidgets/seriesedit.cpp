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

#include "seriesedit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>

SeriesEdit::SeriesEdit(QWidget *parent)
    :PropertyEdit(parent)
{
    setPropertyUrl(Nepomuk::Vocabulary::NBIB::inSeries());
}

void SeriesEdit::setupLabel()
{
    Nepomuk::Resource seriesResource = resource().property( propertyUrl() ).toResource();
    QString title = seriesResource.property(Nepomuk::Vocabulary::NIE::title()).toString();
    addPropertryEntry(title, seriesResource.uri());

    setLabelText(title);
}

void SeriesEdit::updateResource(const QString & text)
{
    Nepomuk::Resource currentSeriesResource = resource().property( propertyUrl() ).toResource();

    // if the text is empty, remove Series and its backlink
    if(text.isEmpty()) {
        resource().removeProperty( propertyUrl() );
        currentSeriesResource.removeProperty( Nepomuk::Vocabulary::NBIB::seriesOf(), resource() );
        return;
    }

    // try to find the propertyurl of an already existing series
    QUrl propUrl = propertyEntry(text);
    Nepomuk::Resource newSeriesResource = Nepomuk::Resource(propUrl);

    // if the text is not empty and we have a valid series
    // rename the series if no other series resource with the new name exist
    if(currentSeriesResource.isValid()) {
        if(!newSeriesResource.isValid()) {
            currentSeriesResource.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
            return;
        }
        // we found a valid series resource with the new name, do not rename current series but select new one
        else {
            // remove old links
            resource().removeProperty( propertyUrl() );
            currentSeriesResource.removeProperty( Nepomuk::Vocabulary::NBIB::seriesOf(), resource() );

            // add new ones
            resource().setProperty( propertyUrl(), newSeriesResource);
            newSeriesResource.addProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), resource());
            return;
        }
    }

    // if we had no valid series connected to the resource jus tconnect the new series or create a new one
    if(!newSeriesResource.isValid()) {
        // create a new series with the string s as title
        newSeriesResource = Nepomuk::Resource(QUrl(), findSeriesType());
        // seems to be a bug, not the full hierachry will be set otherwise
        newSeriesResource.addType(Nepomuk::Vocabulary::NBIB::Series());
        newSeriesResource.addType(Nepomuk::Vocabulary::NIE::InformationElement());

        newSeriesResource.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
    }

    resource().setProperty( propertyUrl(), newSeriesResource);
    newSeriesResource.addProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), resource());
}

QStandardItemModel* SeriesEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
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

QUrl SeriesEdit::findSeriesType()
{
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Book())) {
        return Nepomuk::Vocabulary::NBIB::BookSeries();
    }
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::JournalIssue())) {
        return Nepomuk::Vocabulary::NBIB::Journal();
    }
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::MagazinIssue())) {
        return Nepomuk::Vocabulary::NBIB::Magazin();
    }
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::NewspaperIssue())) {
        return Nepomuk::Vocabulary::NBIB::Newspaper();
    }

    // otherwise simply use the general nbib:Series
    return Nepomuk::Vocabulary::NBIB::Series();
}
