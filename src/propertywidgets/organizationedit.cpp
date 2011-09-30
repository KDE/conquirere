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

#include "organizationedit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Variant>

#include <QStandardItemModel>
#include <QUrl>

OrganizationEdit::OrganizationEdit(QWidget *parent)
    :PropertyEdit(parent)
{
}

void OrganizationEdit::setupLabel()
{
    Nepomuk::Resource organization = resource().property(propertyUrl()).toResource();

    QString title = organization.property(Nepomuk::Vocabulary::NCO::fullname()).toString();

    addPropertryEntry(title, resource().resourceUri());

    setLabelText(title);
}

void OrganizationEdit::updateResource(const QString & text)
{
    // remove the existing organizatzion
    resource().removeProperty( propertyUrl() );

    // try to find the propertyurl of an already existing organizatzion
    QUrl propUrl = propertyEntry(text);

    if(propUrl.isValid()) {
        resource().addProperty( propertyUrl(), Nepomuk::Resource(propUrl));
    }
    else {
        // create a new journalisiue with the string s as journal title
        Nepomuk::Resource newOrganizatzion(QUrl(), Nepomuk::Vocabulary::NCO::OrganizationContact());
        newOrganizatzion.setProperty(Nepomuk::Vocabulary::NCO::fullname(), text);

        resource().addProperty( propertyUrl(), newOrganizatzion);
    }
}

void OrganizationEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *parentItem = model->invisibleRootItem();

    foreach(Nepomuk::Query::Result r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().property(Nepomuk::Vocabulary::NCO::fullname()).toString());

        item->setData(r.resource().resourceUri());

        parentItem->appendRow(item);
    }

    setCompletionModel(model);
}
