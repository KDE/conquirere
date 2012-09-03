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

#include "kmultiitemedit.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/SimpleResource>

#include <KDE/KJob>
#include "sro/nbib/publication.h"
#include "sro/nbib/reference.h"

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>

#include <KDE/KDebug>
#include <QtCore/QDateTime>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

PublicationEdit::PublicationEdit(QWidget *parent)
    : PropertyEdit(parent)
{
    setUseDetailDialog(true);

    m_lineEdit->setNepomukCompleterLabel( NIE::title() );
    m_lineEdit->setNepomukCompleterRange( NBIB::Publication() );
}

void PublicationEdit::setupLabel()
{
    Nepomuk2::Resource publication = resource().property( NBIB::publication() ).toResource();

    QString title = publication.property( NIE::title() ).toString();

    setLabelText(title);
}

void PublicationEdit::updateResource(const QString & newPublicationTitle)
{
    Nepomuk2::Resource currentPublication = resource().property( NBIB::publication() ).toResource();
    QString curentTitle = currentPublication.property(NIE::title()).toString();

    if(newPublicationTitle == curentTitle) {
        return; // nothing changed
    }

    if(currentPublication.exists()) {
        // remove the crosslink reference <-> publication
        Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), NBIB::publication(), QVariantList() << currentPublication.uri());
        Nepomuk2::removeProperty(QList<QUrl>() << currentPublication.uri(), NBIB::reference(), QVariantList() << resource().uri());

        // remove subresource info
        Nepomuk2::removeProperty(QList<QUrl>() << currentPublication.uri(), NAO::hasSubResource(), QVariantList() << resource().uri());
    }

    if(newPublicationTitle.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new Publication and merge it to the right place
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Reference reference(resource().uri());
    Nepomuk2::NBIB::Publication newPublication;

    newPublication.setProperty(NIE::title(), newPublicationTitle.trimmed());
    reference.setPublication( newPublication.uri() );
    newPublication.addReference( reference.uri() );
    // delete reference when publication is removed
    newPublication.addProperty(NAO::hasSubResource(), resource().uri());

    graph << newPublication << reference;

    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );
}
