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

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/simpleresource.h"
#include <KDE/KJob>
#include "sro/nbib/courtreporter.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <KDE/KDebug>

using namespace Nepomuk::Vocabulary;

CourtReporterEdit::CourtReporterEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void CourtReporterEdit::setupLabel()
{
    QString title;
    Nepomuk::Resource courtReporter = resource().property(NBIB::courtReporter()).toResource();
    title = courtReporter.property(NIE::title()).toString();

    setLabelText(title);
}

void CourtReporterEdit::updateResource(const QString & newCRTitle)
{
    Nepomuk::Resource currentCourtReporter = resource().property(NBIB::courtReporter()).toResource();

    QString curentTitle = currentCourtReporter.property(NIE::title()).toString();

    if(newCRTitle == curentTitle) {
        return; // nothing changed
    }

    if(currentCourtReporter.exists()) {
        // remove the crosslink CourtReporter <-> publication
        QList<QUrl> resourceUris; resourceUris << resource().uri();
        QVariantList value; value << currentCourtReporter.uri();
        Nepomuk::removeProperty(resourceUris, NBIB::courtReporter(), value);

        resourceUris.clear(); resourceUris << currentCourtReporter.uri();
        value.clear(); value << resource().uri();
        Nepomuk::removeProperty(resourceUris, NBIB::legalCase(), value);
    }

    if(newCRTitle.isEmpty()) {
        return;
    }

    // ok the user changed the text in the list
    // let the DMS create a new event and merge it to the right place
    Nepomuk::SimpleResourceGraph graph;
    Nepomuk::NBIB::CourtReporter newCourtReporter;

    newCourtReporter.setProperty(NIE::title(), newCRTitle.trimmed());

    graph << newCourtReporter;

    m_editedResource = resource();
    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(addCourtReporter(KJob*)));
}

void CourtReporterEdit::addCourtReporter(KJob *job)
{
    if( job->error() != 0) {
        kDebug() << "could not create new event" << job->errorString();
        return;
    }

    Nepomuk::StoreResourcesJob *srj = dynamic_cast<Nepomuk::StoreResourcesJob *>(job);

    // now get all the uris for the new event
    QList<QUrl> courtReporterUris;
    QVariantList courtReporterValues;
    foreach (QUrl uri, srj->mappings()) {
         courtReporterUris << uri;
         courtReporterValues << uri;
    }

    // add the crosslink reference <-> publicatio
    QList<QUrl> resourceUris; resourceUris << m_editedResource.uri();
    Nepomuk::setProperty(resourceUris, NBIB::courtReporter(), courtReporterValues);

    QVariantList value; value << m_editedResource.uri();
    Nepomuk::addProperty(courtReporterUris, NBIB::legalCase(), value);

    //TODO remove when resourcewatcher is working..
    emit resourceCacheNeedsUpdate(m_editedResource);
}
