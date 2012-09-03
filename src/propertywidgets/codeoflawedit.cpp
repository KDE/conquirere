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
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>

#include <KDE/KDebug>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

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
        Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), NBIB::codeOfLaw(), QVariantList() << currentCodeOfLaw.uri());
        Nepomuk2::removeProperty(QList<QUrl>() << currentCodeOfLaw.uri(), NBIB::legislation(), QVariantList() << resource().uri());

        // remove subresource
        Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), NAO::hasSubResource(), QVariantList() << currentCodeOfLaw.uri());

        //TODO: maybe make an option of the "codeOfLaw deletion" if it has no legislation
        // if the codeOfLaw has no othe rlegislation attached to it, delete it
        QList<Nepomuk2::Resource> legislationList = currentCodeOfLaw.property( NBIB::legislation()).toResourceList();
        if(legislationList.isEmpty() || (legislationList.size() == 1 && legislationList.first().uri() == resource().uri())) {
            Nepomuk2::removeResources(QList<QUrl>() << currentCodeOfLaw.uri());
        }
    }

    if(newCodeOfLawTitle.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new event and merge it to the right place
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Legislation legislation(resource().uri());

    Nepomuk2::NBIB::CodeOfLaw newCodeOfLaw;

    newCodeOfLaw.setProperty(NIE::title(), newCodeOfLawTitle.trimmed());

    newCodeOfLaw.addLegislation( legislation.uri() );
    legislation.setCodeOfLaw( newCodeOfLaw.uri() );
    legislation.addProperty(NAO::hasSubResource(), newCodeOfLaw.uri() );

    graph << newCodeOfLaw << legislation;

    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );
}
