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
#include <QStandardItemModel>

#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Resource>
#include <Nepomuk/Variant>


VolumeNumberEdit::VolumeNumberEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void VolumeNumberEdit::setupLabel()
{
    QString string;
    // check if the resource has a journalissue attaced to it
    Nepomuk::Resource journalIssue = resource().property(Nepomuk::Vocabulary::NBIB::inJournalIssue()).toResource();

    if(journalIssue.isValid()) {
        string = journalIssue.property(propertyUrl()).toString();
    }
    else {
        string = resource().property(propertyUrl()).toString();
    }

    setLabelText(string);
}

void VolumeNumberEdit::updateResource(const QString & text)
{
    // check if the resource has a journalissue attaced to it
    Nepomuk::Resource journalIssue = resource().property(Nepomuk::Vocabulary::NBIB::inJournalIssue()).toResource();

    if(journalIssue.isValid()) {
        // remove all existing string entries of this property
        journalIssue.removeProperty( propertyUrl() );

        journalIssue.addProperty(propertyUrl(), text);
    }
    else {
        // remove all existing string entries of this property
        resource().removeProperty( propertyUrl() );

        resource().addProperty(propertyUrl(), text);
    }
}

void VolumeNumberEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    // this needs a change in the propertyedit class
    // so we can fetch any value of a certain property
    // instead of searching for all occurences of a Resourcetype
}
