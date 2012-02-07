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

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <KDE/KDebug>

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
    Nepomuk::NBIB::Collection newCollection;

    newCollection.setProperty(NIE::title(), newCollectionTitle.trimmed());

    graph << newCollection;

    m_editedResource = resource();
    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(addCollection(KJob*)));
}

void CollectionEdit::addCollection(KJob *job)
{
    if( job->error() != 0) {
        kDebug() << "could not create new collection" << job->errorString();
        return;
    }

    Nepomuk::StoreResourcesJob *srj = dynamic_cast<Nepomuk::StoreResourcesJob *>(job);

    // now get all the uris for the new collection
    QList<QUrl> collectionUris;
    QVariantList collectionValues;
    foreach (QUrl uri, srj->mappings()) {
         collectionUris << uri;
         collectionValues << uri;
    }

    // add the crosslink collection <-> article
    QList<QUrl> resourceUris; resourceUris << m_editedResource.uri();
    Nepomuk::setProperty(resourceUris, NBIB::collection(), collectionValues);

    QVariantList value; value << m_editedResource.uri();
    Nepomuk::addProperty(collectionUris, NBIB::article(), value);

    //TODO remove when resourcewatcher is working..
    emit resourceCacheNeedsUpdate(m_editedResource);
}
