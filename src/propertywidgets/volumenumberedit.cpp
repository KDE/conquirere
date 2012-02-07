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

#include "volumenumberedit.h"

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"

#include "nbib.h"
#include <Nepomuk/Resource>
#include <Nepomuk/Variant>

using namespace Nepomuk::Vocabulary;

VolumeNumberEdit::VolumeNumberEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void VolumeNumberEdit::setupLabel()
{
    QString string;

    // three different cases must be checked here
    Nepomuk::Resource issueResource;

    // I. resource() is an article, with an attached Collection, from a Series
    if(resource().hasType(NBIB::Article())) {
        issueResource = resource().property(NBIB::collection()).toResource();
    }
    else if(resource().hasType(NBIB::Legislation())) {
        // the number in this case is for the bill not the codeOfLaw
        if(propertyUrl() == NBIB::volume())
            issueResource = resource().property(NBIB::codeOfLaw()).toResource();
    }
    else if(resource().hasType(NBIB::LegalCaseDocument())) {
        // the number in this case is for the case not the courtreporter
        if(propertyUrl() == NBIB::volume())
            issueResource = resource().property(NBIB::courtReporter()).toResource();
    }
    // IV. resource() is an Collection, from a Series
    else if(resource().hasType(NBIB::Collection())) {
        issueResource = resource();
    }

    if(issueResource.isValid()) {
        // propertyUrl either leads to the issue number or volume number, see publicationwidget for the selection
        string = issueResource.property(propertyUrl()).toString();
    }
    else {
        // if no issue is attached, show issue/volume from publication instead
        string = resource().property(propertyUrl()).toString();
    }

    setLabelText(string);
}

void VolumeNumberEdit::updateResource(const QString & text)
{
    // the volume /number can either be attached directly to an nbib:Publication entity
    // happens for techreport for example

    // or it marks the volume/number of an JournalIssue or any other issue collection where an article was published in.
    // or the volume for the CodeOfLaw of an legislation

    // check if the resource has a Collection attached to it
    Nepomuk::Resource journalIssue = resource().property(NBIB::collection()).toResource();
    Nepomuk::Resource codeOfLaw = resource().property(NBIB::codeOfLaw()).toResource();
    Nepomuk::Resource courtReporter = resource().property(NBIB::courtReporter()).toResource();

    QList<QUrl> resourceUris;
    if(journalIssue.isValid()) {
        // in this case attach volume/number to the issue rather than the publication from resource()
        resourceUris << journalIssue.uri();
    }
    else if(codeOfLaw.isValid() && propertyUrl() == NBIB::volume()) {
        // in this case attach volume to the issue rather than the publication from resource()
        // the number is the bill number for the Legislation
        resourceUris << codeOfLaw.uri();
    }
    else if(courtReporter.isValid() && propertyUrl() == NBIB::volume()) {
        // in this case attach volume to the issue rather than the publication from resource()
        // the number is the docket number for the LegalCaseDocument
        resourceUris << courtReporter.uri();
    }
    else {
        resourceUris << resource().uri();
    }

    QVariantList value; value << text;
    m_changedResource = resource();
    connect(Nepomuk::setProperty(resourceUris, propertyUrl(), value),
            SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
}
