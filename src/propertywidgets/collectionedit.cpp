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

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/SimpleResource>

#include <KDE/KJob>
#include "sro/nbib/series.h"
#include "sro/nbib/collection.h"
#include "sro/nbib/article.h"

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NUAO>
#include <Nepomuk2/Variant>

#include <KDE/KDebug>
#include <QtCore/QDateTime>

using namespace Nepomuk2::Vocabulary;

CollectionEdit::CollectionEdit(QWidget *parent)
    : PropertyEdit(parent)
{
    setUseDetailDialog(true);
}

void CollectionEdit::setupLabel()
{
    Nepomuk2::Resource collectionResource = resource().property( NBIB::collection() ).toResource();

    QString title = collectionResource.property(NIE::title()).toString();

    setLabelText(title);
}

void CollectionEdit::updateResource(const QString & newCollectionTitle)
{
    Nepomuk2::Resource currentCollection = resource().property(NBIB::collection()).toResource();

    QString curentTitle = currentCollection.property(NIE::title()).toString();

    if(newCollectionTitle == curentTitle) {
        return; // nothing changed
    }

    if(currentCollection.exists()) {
        // remove the crosslink collection <-> article
        QList<QUrl> resourceUris; resourceUris << resource().uri();
        QVariantList value; value << currentCollection.uri();
        Nepomuk2::removeProperty(resourceUris, NBIB::collection(), value);

        resourceUris.clear(); resourceUris << currentCollection.uri();
        value.clear(); value << resource().uri();
        Nepomuk2::removeProperty(resourceUris, NBIB::article(), value);
    }

    if(newCollectionTitle.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new event and merge it to the right place
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::SimpleResource articleRes(resource().uri());
    Nepomuk2::NBIB::Article article(articleRes);
    //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
    QDateTime datetime = QDateTime::currentDateTimeUtc();
    articleRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

    Nepomuk2::NBIB::Collection newCollection;

    newCollection.setProperty(NIE::title(), newCollectionTitle.trimmed());

    newCollection.addArticle( article.uri() );
    article.setCollection( newCollection.uri() );

    graph << newCollection << article;

    m_changedResource = resource();
    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
}
