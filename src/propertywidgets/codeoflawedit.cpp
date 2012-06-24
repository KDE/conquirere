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

#include "codeoflawedit.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/SimpleResource>

#include <KDE/KJob>
#include "sro/nbib/codeoflaw.h"
#include "sro/nbib/legislation.h"

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NUAO>
#include <Nepomuk2/Variant>

#include <KDE/KDebug>
#include <QtCore/QDateTime>

using namespace Nepomuk2::Vocabulary;

CodeOfLawEdit::CodeOfLawEdit(QWidget *parent) :
    PropertyEdit(parent)
{
}

void CodeOfLawEdit::setupLabel()
{
    Nepomuk2::Resource codeOfLaw = resource().property(NBIB::codeOfLaw()).toResource();

    QString title = codeOfLaw.property(NIE::title()).toString();

    setLabelText(title);
}

void CodeOfLawEdit::updateResource(const QString & newCodeOfLawTitle)
{
    Nepomuk2::Resource currentCodeOfLaw = resource().property(NBIB::codeOfLaw()).toResource();

    QString curentTitle = currentCodeOfLaw.property(NIE::title()).toString();

    if(newCodeOfLawTitle == curentTitle) {
        return; // nothing changed
    }

    if(currentCodeOfLaw.exists()) {
        // remove the crosslink event <-> publication
        QList<QUrl> resourceUris; resourceUris << resource().resourceUri();
        QVariantList value; value << currentCodeOfLaw.resourceUri();
        Nepomuk2::removeProperty(resourceUris, NBIB::codeOfLaw(), value);

        resourceUris.clear(); resourceUris << currentCodeOfLaw.resourceUri();
        value.clear(); value << resource().resourceUri();
        Nepomuk2::removeProperty(resourceUris, NBIB::legislation(), value);
    }

    if(newCodeOfLawTitle.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new event and merge it to the right place
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::SimpleResource legislationRes(resource().resourceUri());
    Nepomuk2::NBIB::Legislation legislation(legislationRes);
    //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
    QDateTime datetime = QDateTime::currentDateTimeUtc();
    legislationRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

    Nepomuk2::NBIB::CodeOfLaw newCodeOfLaw;

    newCodeOfLaw.setProperty(NIE::title(), newCodeOfLawTitle.trimmed());

    newCodeOfLaw.addLegislation( legislation.uri() );
    legislation.setCodeOfLaw( newCodeOfLaw.uri() );

    graph << newCodeOfLaw << legislation;

    m_changedResource = resource();
    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
}
