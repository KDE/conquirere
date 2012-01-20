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

#include "nbib.h"
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Resource>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>

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
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        issueResource = resource().property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
    }
    else if(resource().hasType(Nepomuk::Vocabulary::NBIB::Legislation())) {
        // the number in this case is for the bill not the codeOfLaw
        if(propertyUrl() == Nepomuk::Vocabulary::NBIB::volume())
            issueResource = resource().property(Nepomuk::Vocabulary::NBIB::codeOfLaw()).toResource();
    }
    else if(resource().hasType(Nepomuk::Vocabulary::NBIB::LegalCaseDocument())) {
        // the number in this case is for the case not the courtreporter
        if(propertyUrl() == Nepomuk::Vocabulary::NBIB::volume())
            issueResource = resource().property(Nepomuk::Vocabulary::NBIB::courtReporter()).toResource();
    }
    // IV. resource() is an Collection, from a Series
    else if(resource().hasType(Nepomuk::Vocabulary::NBIB::Collection())) {
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

    // check if the resource has a Collection attaced to it
    Nepomuk::Resource journalIssue = resource().property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
    Nepomuk::Resource codeOfLaw = resource().property(Nepomuk::Vocabulary::NBIB::codeOfLaw()).toResource();
    Nepomuk::Resource courtReporter = resource().property(Nepomuk::Vocabulary::NBIB::courtReporter()).toResource();

    if(journalIssue.isValid()) {
        // in this case attach volume/number to the issue rather than the publication from resource()
        journalIssue.setProperty(propertyUrl(), text);
    }
    else if(codeOfLaw.isValid() && propertyUrl() == Nepomuk::Vocabulary::NBIB::volume()) {
        // in this case attach volume to the issue rather than the publication from resource()
        // the number is the bill number for the Legislation
        codeOfLaw.setProperty(propertyUrl(), text);
    }
    else if(courtReporter.isValid() && propertyUrl() == Nepomuk::Vocabulary::NBIB::volume()) {
        // in this case attach volume to the issue rather than the publication from resource()
        // the number is the docket number for the LegalCaseDocument
        courtReporter.setProperty(propertyUrl(), text);
    }
    else {
        resource().setProperty(propertyUrl(), text);
    }
}

QList<QStandardItem*> VolumeNumberEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    // this needs a change in the propertyedit class
    // so we can fetch any value of a certain property
    // instead of searching for all occurrences of a Resourcetype

    QList<QStandardItem*> results;

    return results;
}
