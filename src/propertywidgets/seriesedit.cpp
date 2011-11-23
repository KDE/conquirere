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
}

void SeriesEdit::setupLabel()
{
    Nepomuk::Resource seriesResource = resource().property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    QString title = seriesResource.property(Nepomuk::Vocabulary::NIE::title()).toString();
    addPropertryEntry(title, resource().uri());

    setLabelText(title);
}

void SeriesEdit::updateResource(const QString & text)
{
    // remove the existing series from publication
    resource().removeProperty( propertyUrl() );

    if(text.isEmpty())
        return;

    // try to find the propertyurl of an already existing series
    QUrl propUrl = propertyEntry(text);

    qDebug() << "SeriesEdit::updateResource | resource" << propUrl << "for text" << text;

    if(propUrl.isValid()) {
        resource().addProperty( propertyUrl(), Nepomuk::Resource(propUrl));
    }
    else {
        // create a new series with the string s as title
        Nepomuk::Resource newSeries(QUrl(), findSeriesType());
        newSeries.addType(Nepomuk::Vocabulary::NBIB::Series()); // seems to be a bug, not the full hierachry will be set otherwise
        newSeries.addType(Nepomuk::Vocabulary::NIE::InformationElement());

        newSeries.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
        resource().addProperty( Nepomuk::Vocabulary::NBIB::inSeries() , newSeries);

        //add backlink
        newSeries.setProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), resource());
    }
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
