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

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/simpleresource.h"
#include <KDE/KJob>
#include "sro/nbib/codeoflaw.h"
#include "sro/nbib/legislation.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NUAO>
#include <Nepomuk/Variant>

#include <KDE/KDebug>
#include <QtCore/QDateTime>

using namespace Nepomuk::Vocabulary;

CodeOfLawEdit::CodeOfLawEdit(QWidget *parent) :
    PropertyEdit(parent)
{
}

void CodeOfLawEdit::setupLabel()
{
    Nepomuk::Resource codeOfLaw = resource().property(NBIB::codeOfLaw()).toResource();

    QString title = codeOfLaw.property(NIE::title()).toString();

    setLabelText(title);
}

void CodeOfLawEdit::updateResource(const QString & newCodeOfLawTitle)
{
    Nepomuk::Resource currentCodeOfLaw = resource().property(NBIB::codeOfLaw()).toResource();

    QString curentTitle = currentCodeOfLaw.property(NIE::title()).toString();

    if(newCodeOfLawTitle == curentTitle) {
        return; // nothing changed
    }

    if(currentCodeOfLaw.exists()) {
        // remove the crosslink event <-> publication
        QList<QUrl> resourceUris; resourceUris << resource().uri();
        QVariantList value; value << currentCodeOfLaw.uri();
        Nepomuk::removeProperty(resourceUris, NBIB::codeOfLaw(), value);

        resourceUris.clear(); resourceUris << currentCodeOfLaw.uri();
        value.clear(); value << resource().uri();
        Nepomuk::removeProperty(resourceUris, NBIB::legislation(), value);
    }

    if(newCodeOfLawTitle.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new event and merge it to the right place
    Nepomuk::SimpleResourceGraph graph;
    Nepomuk::SimpleResource legislationRes(resource().uri());
    Nepomuk::NBIB::Legislation legislation(legislationRes);
    //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
    QDateTime datetime = QDateTime::currentDateTimeUtc();
    legislationRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

    Nepomuk::NBIB::CodeOfLaw newCodeOfLaw;

    newCodeOfLaw.setProperty(NIE::title(), newCodeOfLawTitle.trimmed());

    newCodeOfLaw.addLegislation( legislation.uri() );
    legislation.setCodeOfLaw( newCodeOfLaw.uri() );

    graph << newCodeOfLaw << legislation;

    m_changedResource = resource();
    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
}
