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

#include "stringedit.h"

#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>

StringEdit::StringEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void StringEdit::setupLabel()
{
    QString string = resource().property(propertyUrl()).toString();

    setLabelText(string);
}

void StringEdit::updateResource(const QString & text)
{
    // remove all existing string entries of this property
    resource().removeProperty( propertyUrl() );

    if(text.isEmpty())
        return;

    QStringList entryList;
    if(hasMultipleCardinality()) {
        entryList = text.split(QLatin1String(";"));
    }
    else {
        entryList.append(text);
    }

    foreach(const QString & s, entryList) {
        resource().addProperty(propertyUrl(), s.trimmed());
    }
}

QStandardItemModel* StringEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    // this needs a change in the propertyedit class
    // so we can fetch any value of a certain property
    // instead of searching for all occurrences of a Resourcetype

    return new QStandardItemModel;
}
