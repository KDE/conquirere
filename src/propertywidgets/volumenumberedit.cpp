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

    // two different cases must be checked here
    // I. resource() is an article, with an attached Issue, from a Series
    // II. resource() is an Issue, from a Series

    // 1st get the issue
    Nepomuk::Resource issueResource;
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        issueResource = resource().property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
    }
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

    // check if the resource has a journalIssue attaced to it
    Nepomuk::Resource journalIssue = resource().property(Nepomuk::Vocabulary::NBIB::collection()).toResource();

    if(journalIssue.isValid()) {
        // in this case attach volume/number to the issue rather than the publication from resource()
        journalIssue.setProperty(propertyUrl(), text);
    }
    else {
        resource().setProperty(propertyUrl(), text);
    }
}

void VolumeNumberEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    // this needs a change in the propertyedit class
    // so we can fetch any value of a certain property
    // instead of searching for all occurrences of a Resourcetype
}
