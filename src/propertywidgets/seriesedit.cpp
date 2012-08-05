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

#include "kmultiitemedit.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/SimpleResource>

#include <KDE/KJob>
#include "sro/nbib/series.h"
#include "sro/nbib/publication.h"

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NUAO>
#include <Nepomuk2/Variant>

#include <KDE/KDebug>
#include <QtCore/QDateTime>

using namespace Nepomuk2::Vocabulary;

SeriesEdit::SeriesEdit(QWidget *parent)
    : PropertyEdit(parent)
{
    setPropertyUrl(NBIB::inSeries());

    m_lineEdit->setNepomukCompleterRange( NBIB::Series() );
    m_lineEdit->setNepomukCompleterLabel( NIE::title() );
}

void SeriesEdit::setupLabel()
{
    Nepomuk2::Resource seriesResource = resource().property( NBIB::inSeries() ).toResource();
    QString title = seriesResource.property(NIE::title()).toString();

    setLabelText(title);
}

void SeriesEdit::updateResource(const QString & text)
{
    Nepomuk2::Resource currentSeriesResource = resource().property( NBIB::inSeries() ).toResource();
    QString curentTitle = currentSeriesResource.property(NIE::title()).toString();

    if(text == curentTitle) {
        return; // nothing changed
    }

    if(currentSeriesResource.exists()) {
        // remove the crosslink series <-> resource
        QList<QUrl> resourceUris; resourceUris << resource().uri();
        QVariantList value; value << currentSeriesResource.uri();
        Nepomuk2::removeProperty(resourceUris, NBIB::inSeries(), value);

        resourceUris.clear(); resourceUris << currentSeriesResource.uri();
        value.clear(); value << resource().uri();
        Nepomuk2::removeProperty(resourceUris, NBIB::seriesOf(), value);
    }

    // if the text is empty, remove Series from resource and its backlink
    if(text.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new Series and merge it to the right place
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::SimpleResource publicationRes(resource().uri());
    Nepomuk2::NBIB::Publication publication(publicationRes);
    //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
    QDateTime datetime = QDateTime::currentDateTimeUtc();
    publicationRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

    Nepomuk2::NBIB::Series newSeries;
    QUrl subType = findSeriesType();
    if(!subType.isEmpty()) {
        newSeries.addType( subType );
    }

    newSeries.setProperty(NIE::title(), text.trimmed());
    newSeries.addSeriesOf( publication.uri() );
    publication.setInSeries( newSeries.uri() );

    graph << newSeries << publication;

    m_changedResource = resource();
    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
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
