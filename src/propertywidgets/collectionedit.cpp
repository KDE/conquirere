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

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/simpleresource.h"
#include <KDE/KJob>
#include "sro/nbib/series.h"
#include "sro/nbib/collection.h"
#include "sro/nbib/article.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NUAO>
#include <Nepomuk/Variant>

#include <KDE/KDebug>
#include <QtCore/QDateTime>

using namespace Nepomuk::Vocabulary;

CollectionEdit::CollectionEdit(QWidget *parent)
    : PropertyEdit(parent)
{
    setUseDetailDialog(true);
}

void CollectionEdit::setupLabel()
{
    Nepomuk::Resource collectionResource = resource().property( NBIB::collection() ).toResource();

    QString title = collectionResource.property(NIE::title()).toString();

    setLabelText(title);
}

void CollectionEdit::updateResource(const QString & newCollectionTitle)
{
    Nepomuk::Resource currentCollection = resource().property(NBIB::collection()).toResource();

    QString curentTitle = currentCollection.property(NIE::title()).toString();

    if(newCollectionTitle == curentTitle) {
        return; // nothing changed
    }

    if(currentCollection.exists()) {
        // remove the crosslink collection <-> article
        QList<QUrl> resourceUris; resourceUris << resource().uri();
        QVariantList value; value << currentCollection.uri();
        Nepomuk::removeProperty(resourceUris, NBIB::collection(), value);

        resourceUris.clear(); resourceUris << currentCollection.uri();
        value.clear(); value << resource().uri();
        Nepomuk::removeProperty(resourceUris, NBIB::article(), value);
    }

    if(newCollectionTitle.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new event and merge it to the right place
    Nepomuk::SimpleResourceGraph graph;
    Nepomuk::SimpleResource articleRes(resource().uri());
    Nepomuk::NBIB::Article article(articleRes);
    //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
    QDateTime datetime = QDateTime::currentDateTimeUtc();
    articleRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

    Nepomuk::NBIB::Collection newCollection;

    newCollection.setProperty(NIE::title(), newCollectionTitle.trimmed());

    newCollection.addArticle( article.uri() );
    article.setCollection( newCollection.uri() );

    graph << newCollection << article;

    m_changedResource = resource();
    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
}
