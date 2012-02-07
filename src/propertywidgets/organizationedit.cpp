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

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/simpleresource.h"
#include <KDE/KJob>
#include "sro/nco/organizationcontact.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Variant>

#include <KDE/KDebug>

using namespace Nepomuk::Vocabulary;

OrganizationEdit::OrganizationEdit(QWidget *parent)
    : PropertyEdit(parent)
{
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
    Nepomuk::NCO::OrganizationContact newOrganization;

    newOrganization.setProperty(NCO::fullname(), text.trimmed());

    graph << newOrganization;

    m_editedResource = resource();
    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(addOrganization(KJob*)));
}

void OrganizationEdit::addOrganization(KJob *job)
{
    if( job->error() != 0) {
        kDebug() << "could not create new oraganization" << job->errorString();
        return;
    }

    Nepomuk::StoreResourcesJob *srj = dynamic_cast<Nepomuk::StoreResourcesJob *>(job);

    // now get all the uris for the new tags
    QList<QUrl> publicationUris;
    QVariantList publicationValues;
    foreach (QUrl uri, srj->mappings()) {
         publicationUris << uri;
         publicationValues << uri;
    }

    Nepomuk::Resource resourceToAddOrg;
    // see above
    if(m_editedResource.hasType(NBIB::Article())) {
        resourceToAddOrg = resource().property( NBIB::collection() ).toResource();
    }
    else {
        resourceToAddOrg = m_editedResource;
    }

    // add the crosslink reference <-> publicatio
    QList<QUrl> resourceUris; resourceUris << resourceToAddOrg.uri();
    Nepomuk::setProperty(resourceUris, NBIB::organization(), publicationValues);

    //TODO remove when resourcewatcher is working..
    emit resourceCacheNeedsUpdate(m_editedResource);
    if(m_editedResource != resourceToAddOrg)
        emit resourceCacheNeedsUpdate(resourceToAddOrg);
}
