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

#include "publicationedit.h"

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/simpleresource.h"
#include <KDE/KJob>
#include "sro/nbib/publication.h"
#include "sro/nbib/reference.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NUAO>
#include <Nepomuk/Variant>

#include <KDE/KDebug>
#include <QtCore/QDateTime>

using namespace Nepomuk::Vocabulary;

PublicationEdit::PublicationEdit(QWidget *parent)
    : PropertyEdit(parent)
{
    setUseDetailDialog(true);
}

void PublicationEdit::setupLabel()
{
    Nepomuk::Resource publication = resource().property( NBIB::publication() ).toResource();

    QString title = publication.property( NIE::title() ).toString();

    kDebug() << "set title" << title;

    setLabelText(title);
}

void PublicationEdit::updateResource(const QString & newPublicationTitle)
{
    Nepomuk::Resource currentPublication = resource().property( NBIB::publication() ).toResource();
    QString curentTitle = currentPublication.property(NIE::title()).toString();

    if(newPublicationTitle == curentTitle) {
        return; // nothing changed
    }

    if(currentPublication.exists()) {
        // remove the crosslink reference <-> publication
        QList<QUrl> resourceUris; resourceUris << resource().uri();
        QVariantList value; value << currentPublication.uri();
        Nepomuk::removeProperty(resourceUris, NBIB::publication(), value);

        resourceUris.clear(); resourceUris << currentPublication.uri();
        value.clear(); value << resource().uri();
        Nepomuk::removeProperty(resourceUris, NBIB::reference(), value);
    }

    if(newPublicationTitle.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new Publication and merge it to the right place
    Nepomuk::SimpleResourceGraph graph;
    Nepomuk::SimpleResource referenceRes(resource().uri());
    Nepomuk::NBIB::Reference reference(referenceRes);
    //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
    QDateTime datetime = QDateTime::currentDateTimeUtc();
    referenceRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

    Nepomuk::NBIB::Publication newPublication;

    newPublication.setProperty(NIE::title(), newPublicationTitle.trimmed());
    reference.setPublication( newPublication.uri() );
    newPublication.addReference( reference.uri() );

    graph << newPublication << reference;

    m_changedResource = resource();
    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
}
