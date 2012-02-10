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

#include "organizationedit.h"

#include "kmultiitemedit.h"

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/simpleresource.h"
#include <KDE/KJob>
#include "sro/nco/organizationcontact.h"
#include "sro/nbib/publication.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NUAO>
#include <Nepomuk/Variant>

#include <KDE/KDebug>
#include <QtCore/QDateTime>

using namespace Nepomuk::Vocabulary;

OrganizationEdit::OrganizationEdit(QWidget *parent)
    : PropertyEdit(parent)
{
    m_lineEdit->setNepomukCompleterRange( NCO::OrganizationContact() );
    m_lineEdit->setNepomukCompleterLabel( NCO::fullname() );
}

void OrganizationEdit::setupLabel()
{
    QString title;

    //different cases are handled here
    Nepomuk::Resource organization;

    // I. the resource is an Article, means the organization is attached to the Collection
    // not the article itself
    if(resource().hasType(NBIB::Article())) {
        Nepomuk::Resource collection = resource().property( NBIB::collection() ).toResource();
        organization = collection.property( NBIB::organization() ).toResource();
    }
    // otherwise the organization is directly connected to the publication
    // institution for a thesis, court for a case, and so on
    else {
        organization = resource().property( NBIB::organization() ).toResource();
    }

    title = organization.property( NCO::fullname() ).toString();

    setLabelText(title);
}

void OrganizationEdit::updateResource(const QString & text)
{
    Nepomuk::Resource currentOrganization;
    // see above
    if(resource().hasType(NBIB::Article())) {
        Nepomuk::Resource collection = resource().property( NBIB::collection() ).toResource();
        currentOrganization = collection.property( NBIB::organization() ).toResource();
    }
    else {
        currentOrganization = resource().property( NBIB::organization() ).toResource();
    }

    QString curentTitle = currentOrganization.property(NCO::fullname()).toString();

    if(text == curentTitle) {
        return; // nothing changed
    }

    if(currentOrganization.exists()) {
        // remove the crosslink reference <-> publication
        QList<QUrl> resourceUris; resourceUris << resource().uri();
        QVariantList value; value << currentOrganization.uri();
        Nepomuk::removeProperty(resourceUris, NBIB::organization(), value);
    }

    if(text.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new Organization and merge it to the right place
    Nepomuk::SimpleResourceGraph graph;
    Nepomuk::SimpleResource publicationRes(resource().uri());
    Nepomuk::NBIB::Publication publication(publicationRes);
    //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
    QDateTime datetime = QDateTime::currentDateTimeUtc();
    publicationRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

    Nepomuk::NCO::OrganizationContact newOrganization;

    newOrganization.setProperty(NCO::fullname(), text.trimmed());
    publication.setOrganization( newOrganization.uri() );

    graph << newOrganization << publication;

    m_changedResource = resource();
    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
}
