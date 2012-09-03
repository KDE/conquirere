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

#include "courtreporteredit.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/SimpleResource>

#include <KDE/KJob>
#include "sro/nbib/courtreporter.h"
#include "sro/nbib/legalcasedocument.h"

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>

#include <KDE/KDebug>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

CourtReporterEdit::CourtReporterEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void CourtReporterEdit::setupLabel()
{
    QString title;
    Nepomuk2::Resource courtReporter = resource().property(NBIB::courtReporter()).toResource();
    title = courtReporter.property(NIE::title()).toString();

    setLabelText(title);
}

void CourtReporterEdit::updateResource(const QString & newCRTitle)
{
    Nepomuk2::Resource currentCourtReporter = resource().property(NBIB::courtReporter()).toResource();

    QString curentTitle = currentCourtReporter.property(NIE::title()).toString();

    if(newCRTitle == curentTitle) {
        return; // nothing changed
    }

    if(currentCourtReporter.exists()) {
        // remove the crosslink CourtReporter <-> publication
        Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), NBIB::courtReporter(), QVariantList() << currentCourtReporter.uri());
        Nepomuk2::removeProperty(QList<QUrl>() << currentCourtReporter.uri(), NBIB::legalCase(), QVariantList() << resource().uri());

        // remove subresource
        Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), NAO::hasSubResource(), QVariantList() << currentCourtReporter.uri());

        //TODO: maybe make an option of the "courtreport deletion" if it has no publication
        QList<Nepomuk2::Resource> legalCases = currentCourtReporter.property( NBIB::legalCase()).toResourceList();
        if(legalCases.isEmpty() || (legalCases.size() == 1 && legalCases.first().uri() == resource().uri())) {
            Nepomuk2::removeResources(QList<QUrl>() << currentCourtReporter.uri());
        }
    }

    if(newCRTitle.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new event and merge it to the right place
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::LegalCaseDocument legalCase(resource().uri());

    Nepomuk2::NBIB::CourtReporter newCourtReporter;

    newCourtReporter.setProperty(NIE::title(), newCRTitle.trimmed());

    newCourtReporter.addLegalCase( legalCase.uri() );
    legalCase.setCourtReporter( newCourtReporter.uri() );
    legalCase.addProperty( NAO::hasSubResource(), newCourtReporter.uri());

    graph << newCourtReporter << legalCase;

    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );
}
