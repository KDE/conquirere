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

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/SimpleResource>

#include <KDE/KJob>
#include "sro/nco/organizationcontact.h"
#include "sro/nbib/publication.h"

#include "ontology/nbib.h"
#include <Nepomuk2/Vocabulary/NCO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>

#include <KDE/KDebug>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

OrganizationEdit::OrganizationEdit(QWidget *parent)
    : PropertyEdit(parent)
{
    m_lineEdit->setNepomukCompleterRange( NCO::OrganizationContact() );
    m_lineEdit->setNepomukCompleterLabel( NCO::fullname() );
}

void OrganizationEdit::setupLabel()
{
    QString title;

    //two different cases are handled here
    Nepomuk2::Resource organization;

    // I. the resource is an Article, means the organization is attached to the Collection
    // not the article itself
    if(resource().hasType(NBIB::Article())) {
        Nepomuk2::Resource collection = resource().property( NBIB::collection() ).toResource();
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
    Nepomuk2::Resource currentOrganization;
    // see above
    if(resource().hasType(NBIB::Article())) {
        Nepomuk2::Resource collection = resource().property( NBIB::collection() ).toResource();
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
        Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), NBIB::organization(), QVariantList() << currentOrganization.uri());

        Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), NAO::hasSubResource(), QVariantList() << currentOrganization.uri());
    }

    if(text.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new Organization and merge it to the right place
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Publication publication(resource().uri());

    Nepomuk2::NCO::OrganizationContact newOrganization;

    newOrganization.setProperty(NCO::fullname(), text.trimmed());
    publication.setOrganization( newOrganization.uri() );

    // delete organization if the publication got deleted (and no other resource is using it)
    publication.addProperty(NAO::hasSubResource(), newOrganization.uri());

    graph << newOrganization << publication;

    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );
}
