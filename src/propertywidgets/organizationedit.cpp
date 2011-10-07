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
    QString title;
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Proceedings())) {
        Nepomuk::Resource organization = resource().property(propertyUrl()).toResource();

        title = organization.property(Nepomuk::Vocabulary::NCO::fullname()).toString();
    }
    else {
        Nepomuk::Resource proceedings = resource().property(Nepomuk::Vocabulary::NBIB::proceedings()).toResource();
        Nepomuk::Resource organization = proceedings.property(propertyUrl()).toResource();

        title = organization.property(Nepomuk::Vocabulary::NCO::fullname()).toString();
    }

    addPropertryEntry(title, resource().uri());

    setLabelText(title);
}

void OrganizationEdit::updateResource(const QString & text)
{
    // remove the existing organizatzion
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Proceedings())) {
        Nepomuk::Resource organization = resource().property(propertyUrl()).toResource();
        resource().removeProperty(organization.uri());
    }
    else {
        Nepomuk::Resource proceedings = resource().property(Nepomuk::Vocabulary::NBIB::proceedings()).toResource();
        Nepomuk::Resource organization = proceedings.property(propertyUrl()).toResource();
        resource().removeProperty(organization.uri());
    }

    if(text.isEmpty())
        return;

    // try to find the propertyurl of an already existing organizatzion
    QUrl propUrl = propertyEntry(text);

    if(propUrl.isValid()) {
        resource().addProperty( propertyUrl(), Nepomuk::Resource(propUrl));
    }
    else {
        // create a new organization with the string s as title
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

        item->setData(r.resource().uri());

        parentItem->appendRow(item);
    }

    setCompletionModel(model);
}
