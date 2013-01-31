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
#include "sro/nbib/article.h"
#include "sro/nbib/series.h"
#include "sro/nbib/collection.h"

#include "ontology/nbib.h"
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

    // in the case of an article, take the issn from the sereis which is connected to the collection of the article
    // Like the Series (Journal) form the Collection (Journalissue) from this article
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

        // no collection available for the article, create collection and the series
        if(!issue.exists()) {
            Nepomuk2::SimpleResourceGraph graph;
            Nepomuk2::NBIB::Collection newCollection;
            Nepomuk2::NBIB::Series newSeries;
            Nepomuk2::NBIB::Article article(resource().uri());

            newCollection.setProperty(NIE::title(), i18n("Unknown Collection"));
            newSeries.setProperty(NIE::title(), i18n("Unknown Series"));
            newSeries.setIssn( newIssnString );

            // add crosslinks for collection <-> series
            newCollection.setProperty(NBIB::inSeries(), newSeries.uri() );
            newSeries.addSeriesOf( newCollection.uri() );

            // add crosslinks for article <-> collection
            article.setCollection( newCollection.uri() );
            newCollection.addArticle( article.uri() );

            graph << newCollection << newSeries << article;

            // do not merge resources
            connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
                    SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );
        }
        // collection exist but no series yet, create the series
        else if(!series.exists()) {
            Nepomuk2::SimpleResourceGraph graph;
            Nepomuk2::NBIB::Collection collection(issue.uri());
            Nepomuk2::NBIB::Series newSeries;

            newSeries.setProperty(NIE::title(), i18n("Unknown Series"));
            newSeries.setIssn( newIssnString );

            // add crosslinks for collection <-> series
            collection.setProperty(NBIB::inSeries(), newSeries.uri() );
            newSeries.addSeriesOf( collection.uri() );

            graph << collection << newSeries;

            // do not merge resources
            connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
                    SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );
        }
        // collection and series exist, just set the issn to the series then
        else {
            connect(Nepomuk2::setProperty(QList<QUrl>() << series.uri(), NBIB::issn(), QVariantList() << newIssnString),
                    SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
        }
    }
    // if we operate on something else than an article and have no series we create it too
    else {
        if(!series.exists()) {
            Nepomuk2::SimpleResourceGraph graph;
            Nepomuk2::NBIB::Series newSeries;
            Nepomuk2::NBIB::Publication publication(resource().uri());

            newSeries.setProperty(NIE::title(), i18n("Unknown Series"));

            // add crosslinks for collection <-> series
            publication.setProperty(NBIB::inSeries(), newSeries.uri() );
            newSeries.addSeriesOf( publication.uri() );

            graph << newSeries << publication;

            // do not merge resources
            connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
                    SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );
        }
        // series exist, just set the issn to the series then
        else {
            connect(Nepomuk2::setProperty(QList<QUrl>() << series.uri(), NBIB::issn(), QVariantList() << newIssnString),
                    SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
        }
    }
}
