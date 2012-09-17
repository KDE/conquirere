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

#include "ontology/nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>

#include <KDE/KDebug>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

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

        //TODO: maybe make an option of the "series deletion" if it has no publication
        // if the series is not the series of any other publication delete it
        QList<Nepomuk2::Resource> publist = currentSeriesResource.property( NBIB::seriesOf()).toResourceList();
        if(publist.isEmpty() || (publist.size() == 1 && publist.first().uri() == resource().uri())) {
            Nepomuk2::removeResources(QList<QUrl>() << currentSeriesResource.uri());
        }
    }

    // if the text is empty just remove the series but nothing else
    if(text.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new Series and merge it to the right place
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Publication publication(resource().uri());

    Nepomuk2::NBIB::Series newSeries;
    QUrl subType = findSeriesType();
    if(!subType.isEmpty()) {
        newSeries.addType( subType );
    }

    newSeries.setTitle( text.trimmed() );
    newSeries.addSeriesOf( publication.uri() );
    publication.setInSeries( newSeries.uri() );
    // delete series if publication got deleted
    publication.addProperty(NAO::hasSubResource(), newSeries.uri());

    graph << newSeries << publication;

    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );
}

QUrl SeriesEdit::findSeriesType()
{
    //REFACTOR: move this in a global BibType Helper class with all the other static functions
    QUrl subType;
    if(resource().hasType(NBIB::Book()) || resource().hasType(NBIB::Booklet())) {
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
