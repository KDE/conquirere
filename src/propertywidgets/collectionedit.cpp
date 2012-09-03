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
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>

#include <KDE/KDebug>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

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
        Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), NBIB::collection(), QVariantList() << currentCollection.uri());
        Nepomuk2::removeProperty(QList<QUrl>() << currentCollection.uri(), NBIB::article(), QVariantList() << resource().uri());

        Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), NAO::hasSubResource(), QVariantList() << currentCollection.uri());


        //TODO: maybe make an option of the "collection deletion" if it has no publication
        // if the old collection has no other publication attached to it anymore
        QList<Nepomuk2::Resource> publist = currentCollection.property( NBIB::article()).toResourceList();
        if(publist.isEmpty() || (publist.size() == 1 && publist.first().uri() == resource().uri())) {
            Nepomuk2::removeResources(QList<QUrl>() << currentCollection.uri());
        }
    }

    if(newCollectionTitle.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new event and merge it to the right place
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Article article(resource().uri());

    Nepomuk2::NBIB::Collection newCollection;

    newCollection.setProperty(NIE::title(), newCollectionTitle.trimmed());

    newCollection.addArticle( article.uri() );
    article.setCollection( newCollection.uri() );
    article.addProperty(NAO::hasSubResource(), newCollection.uri());

    graph << newCollection << article;

    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );
}
