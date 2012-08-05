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

#include "issnedit.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/SimpleResource>

#include <KDE/KJob>
#include "sro/nbib/series.h"
#include "sro/nbib/collection.h"

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Variant>
#include <KDE/KDebug>

using namespace Nepomuk2::Vocabulary;

IssnEdit::IssnEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void IssnEdit::setupLabel()
{
    Nepomuk2::Resource series;
    Nepomuk2::Resource issue;
    if(resource().hasType(NBIB::Article())) {
        issue = resource().property(NBIB::collection()).toResource();
        series = issue.property(NBIB::inSeries()).toResource();
    }
    else {
        series = resource().property(NBIB::inSeries()).toResource();
    }

    //get the connected journal for the publication
    QString issn = series.property(NBIB::issn()).toString();

    setLabelText(issn);
}

void IssnEdit::updateResource(const QString & newIssnString)
{
    // first get the right series resource where the issn will be attached to
    Nepomuk2::Resource series;
    Nepomuk2::Resource issue;
    if(resource().hasType(NBIB::Article())) {
        issue = resource().property(NBIB::collection()).toResource();
        series = issue.property(NBIB::inSeries()).toResource();
    }
    else {
        series = resource().property(NBIB::inSeries()).toResource();
    }

    if( newIssnString == series.property(NBIB::issn()).toString()) {
        return;
    }

    // if we added the ISSN to something where no series existed
    // we need to create the right structure before we proceed
    if(resource().hasType(NBIB::Article())) {

        // no collection available for the article, create one
        if(!issue.exists()) {
            Nepomuk2::SimpleResourceGraph graph;
            Nepomuk2::NBIB::Collection newCollection;
            Nepomuk2::NBIB::Series newSeries;

            newCollection.setProperty(NIE::title(), i18n("Unknown Collection"));
            newSeries.setProperty(NIE::title(), i18n("Unknown Series"));

            newCollection.setProperty(NBIB::inSeries(), newSeries.uri() );
            newSeries.addSeriesOf( newCollection.uri() );

            graph << newCollection << newSeries;

            //blocking graph save
            Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNone );
            if( !srj->exec() ) {
                kWarning() << "could not create new issue and series" << srj->errorString();
                return;
            }

            Nepomuk2::Resource newCollectionResource = Nepomuk2::Resource::fromResourceUri( srj->mappings().value( newCollection.uri() ) );
            series = Nepomuk2::Resource::fromResourceUri( srj->mappings().value( newSeries.uri() ) );

            // because we could not create the links via the SimpleResource method, we add 2 additional calls to do them now
            QList<QUrl> resUri; resUri << resource().uri();
            QVariantList value; value << newCollectionResource.uri();
            Nepomuk2::setProperty(resUri, NBIB::collection(), value);

            resUri.clear(); resUri << newCollectionResource.uri();
            value.clear(); value << resource().uri();
            Nepomuk2::addProperty(resUri, NBIB::article(), value);
        }
        else if(!series.exists()) {
            Nepomuk2::SimpleResourceGraph graph;
            Nepomuk2::NBIB::Series newSeries;

            newSeries.setProperty(NIE::title(), i18n("Unknown Series"));

            graph << newSeries;

            //blocking graph save
            Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNone );
            if( !srj->exec() ) {
                kWarning() << "could not create new series" << srj->errorString();
                return;
            }

            series = Nepomuk2::Resource::fromResourceUri( srj->mappings().value( newSeries.uri() ) );

            // because we could not create the links via the SimpleResource method, we add 2 additional calls to do them now
            QList<QUrl> resUri; resUri << issue.uri();
            QVariantList value; value << series.uri();
            Nepomuk2::setProperty(resUri, NBIB::inSeries(), value);

            resUri.clear(); resUri << series.uri();
            value.clear(); value << issue.uri();
            Nepomuk2::addProperty(resUri, NBIB::seriesOf(), value);
        }
    }
    // if we operate on something else than an article and have no series we create it too
    else if(!series.exists()) {
        Nepomuk2::SimpleResourceGraph graph;
        Nepomuk2::NBIB::Series newSeries;

        newSeries.setProperty(NIE::title(), i18n("Unknown Series"));

        graph << newSeries;

        //blocking graph save
        Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNone );
        if( !srj->exec() ) {
            kWarning() << "could not create new series" << srj->errorString();
            return;
        }

        series = Nepomuk2::Resource::fromResourceUri( srj->mappings().value( newSeries.uri() ) );

        // because we could not create the links via the SimpleResource method, we add 2 additional calls to do them now
        QList<QUrl> resUri; resUri << resource().uri();
        QVariantList value; value << series.uri();
        Nepomuk2::setProperty(resUri, NBIB::inSeries(), value);

        resUri.clear(); resUri << series.uri();
        value.clear(); value << resource().uri();
        Nepomuk2::addProperty(resUri, NBIB::seriesOf(), value);
    }

    // now we have the valid seriws in the right connection add the issn to it
    QList<QUrl> resourceUris; resourceUris << series.uri();
    QVariantList value; value << newIssnString;
    m_changedResource = resource();

    connect(Nepomuk2::setProperty(resourceUris, NBIB::issn(), value),
            SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
}
