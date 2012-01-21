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

#include "tagedit.h"

#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Tag>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>

TagEdit::TagEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void TagEdit::setupLabel()
{
    QString labelText;

    QList<Nepomuk::Tag> tagList = resource().tags();

    foreach(const Nepomuk::Tag & t, tagList) {
        QString prefLabel = t.genericLabel();
        labelText.append(prefLabel);
        addPropertryEntry(prefLabel, t.resourceUri().toString());

        labelText.append(QLatin1String("; "));
    }

    labelText.chop(2);

    setLabelText(labelText);
}

void TagEdit::updateResource(const QString & text)
{
    // remove any tag first and add only what is specified in "text" again
    resource().removeProperty(Soprano::Vocabulary::NAO::hasTag());

    if(text.isEmpty())
        return;

    QStringList entryList;
    if(hasMultipleCardinality()) {
        // for the contact we get a list of contact names divided by ;
        // where each contact is also available as nepomuk:/res in the cache
        // if not, a new contact with the full name of "string" will be created
        entryList = text.split(QLatin1String(";"));
    }
    else {
        entryList.append(text);
    }

    foreach(const QString & s, entryList) {
        // try to find the propertyurl of an already existing contact
        QUrl propTagUrl = propertyEntry(s.trimmed());
        if(propTagUrl.isValid()) {
            Nepomuk::Tag tag = Nepomuk::Tag(propTagUrl);
            resource().addTag(tag);
        }
        else {
            // create a new contact with the string s as fullname
            Nepomuk::Tag newTag;
            //BUG Nepomuk::Tag does not add type Tag but twice type Resource!
            newTag.addType(Soprano::Vocabulary::NAO::Tag());
            newTag.setProperty(Soprano::Vocabulary::NAO::prefLabel(), s.trimmed());
            newTag.setProperty(Soprano::Vocabulary::NAO::identifier(), s.trimmed());
            resource().addTag(newTag);
        }
    }
}
