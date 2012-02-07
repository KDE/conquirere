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

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include <Nepomuk/Variant>

StringEdit::StringEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void StringEdit::setupLabel()
{
    QString stringLabel;
    if(hasMultipleCardinality()) {
        QList<Nepomuk::Resource> stringList = resource().property(propertyUrl()).toResourceList();

        foreach(const Nepomuk::Resource & r, stringList) {
            stringLabel.append(r.property(propertyUrl()).toString());
            stringLabel.append(QLatin1String("; "));
        }
        stringLabel.chop(2);
    }
    else {

    }
    stringLabel = resource().property(propertyUrl()).toString();

    setLabelText(stringLabel);
}

void StringEdit::updateResource(const QString & text)
{
    QStringList entryList;
    if(hasMultipleCardinality()) {
        entryList = text.split(QLatin1String(";"));
    }
    else {
        entryList.append(text);
    }

    QVariantList value;
    foreach(const QString & s, entryList) {
        value << s.trimmed();
    }

    QList<QUrl> resourceUris; resourceUris << resource().uri();
    m_changedResource = resource();
    connect(Nepomuk::setProperty(resourceUris, propertyUrl(), value),
            SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));
}
