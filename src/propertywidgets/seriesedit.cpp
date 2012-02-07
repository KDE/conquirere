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

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/simpleresource.h"
#include <KDE/KJob>
#include "sro/nbib/series.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <KDE/KDebug>

using namespace Nepomuk::Vocabulary;

SeriesEdit::SeriesEdit(QWidget *parent)
    : PropertyEdit(parent)
{
    setPropertyUrl(NBIB::inSeries());
}

void SeriesEdit::setupLabel()
{
    Nepomuk::Resource seriesResource = resource().property( NBIB::inSeries() ).toResource();
    QString title = seriesResource.property(NIE::title()).toString();
    addPropertryEntry(title, seriesResource.resourceUri().toString());

    setLabelText(title);
}

void SeriesEdit::updateResource(const QString & text)
{
    Nepomuk::Resource currentSeriesResource = resource().property( NBIB::inSeries() ).toResource();
    QString curentTitle = currentSeriesResource.property(NIE::title()).toString();

    if(text == curentTitle) {
        return; // nothing changed
    }

    if(currentSeriesResource.exists()) {
        // remove the crosslink series <-> resource
        QList<QUrl> resourceUris; resourceUris << resource().uri();
        QVariantList value; value << currentSeriesResource.uri();
        Nepomuk::removeProperty(resourceUris, NBIB::inSeries(), value);

        resourceUris.clear(); resourceUris << currentSeriesResource.uri();
        value.clear(); value << resource().uri();
        Nepomuk::removeProperty(resourceUris, NBIB::seriesOf(), value);
    }

    // if the text is empty, remove Series from resource and its backlink
    if(text.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new Series and merge it to the right place
    Nepomuk::SimpleResourceGraph graph;
    Nepomuk::NBIB::Series newSeries;
    QUrl subType = findSeriesType();
    if(!subType.isEmpty()) {
        newSeries.addType( subType );
    }

    newSeries.setProperty(NIE::title(), text.trimmed());

    graph << newSeries;

    m_editedResource = resource();
    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties), SIGNAL(result(KJob*)),this, SLOT(addSeries(KJob*)));
}

QUrl SeriesEdit::findSeriesType()
{
    QUrl subType;
    if(resource().hasType(NBIB::Book())) {
        subType = NBIB::BookSeries();
    }
    if(resource().hasType(NBIB::JournalIssue())) {
        subType = NBIB::Journal();
    }
    if(resource().hasType(NBIB::MagazinIssue())) {
        subType = NBIB::Magazin();
    }
    if(resource().hasType(NBIB::NewspaperIssue())) {
        subType = NBIB::Newspaper();
    }

    return subType;
}

void SeriesEdit::addSeries(KJob *job)
{
    if( job->error() != 0) {
        kDebug() << "could not create new series" << job->errorString();
        return;
    }

    Nepomuk::StoreResourcesJob *srj = dynamic_cast<Nepomuk::StoreResourcesJob *>(job);

    // now get all the uris for the new tags
    QList<QUrl> seriesUris;
    QVariantList seriesValues;
    foreach (QUrl uri, srj->mappings()) {
         seriesUris << uri;
         seriesValues << uri;
    }

    // add the crosslink series <-> resource
    QList<QUrl> resourceUris; resourceUris << m_editedResource.uri();
    Nepomuk::setProperty(resourceUris, NBIB::inSeries(), seriesValues);

    QVariantList value; value << m_editedResource.uri();
    Nepomuk::addProperty(seriesUris, NBIB::seriesOf(), value);

    //TODO remove when resourcewatcher is working..
    emit resourceCacheNeedsUpdate(m_editedResource);
}
