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

#include "contactedit.h"

#include <QStandardItemModel>

#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Variant>

ContactEdit::ContactEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

ContactEdit::~ContactEdit()
{
}

void ContactEdit::setupLabel()
{
    QString labelText;

    if(hasMultipleCardinality()) {
        QList<Nepomuk::Resource> authorList = resource().property(propertyUrl()).toResourceList();

        foreach(Nepomuk::Resource r, authorList) {
            QString fullname = r.property(Nepomuk::Vocabulary::NCO::fullname()).toString();
            labelText.append(fullname);
            addPropertryEntry(fullname, r.resourceUri());

            labelText.append(QLatin1String("; "));
        }

        labelText.chop(2);
    }
    else {
        Nepomuk::Resource author = resource().property(propertyUrl()).toResource();

        QString fullname = author.property(Nepomuk::Vocabulary::NCO::fullname()).toString();
        labelText.append(fullname);
        addPropertryEntry(fullname, author.resourceUri());
    }

    setLabelText(labelText);
}

void ContactEdit::updateResource(const QString & text)
{
    // remove allexisting contacts
    resource().removeProperty( propertyUrl() );

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

    foreach(QString s, entryList) {
        s = s.trimmed();
        // try to find the propertyurl of an already existing contact
        QUrl propUrl = propertyEntry(s);
        if(propUrl.isValid()) {
            resource().addProperty( propertyUrl(), Nepomuk::Resource(propUrl));
        }
        else {
            // create a new contact with the string s as fullname
            Nepomuk::Resource newContact(propUrl, Nepomuk::Vocabulary::NCO::Contact());
            newContact.setProperty(Nepomuk::Vocabulary::NCO::fullname(), s);
            resource().addProperty( propertyUrl(), newContact);
        }
    }
}

void ContactEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *parentItem = model->invisibleRootItem();

    foreach(Nepomuk::Query::Result r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().genericLabel());
        // save the resource uri with the model item
        // this helps to identify the selected entry even if the generic label has
        // the same result on two different items
        // also it is not necessary to ask nepomuk for the resource used later again
        item->setData(r.resource().resourceUri());

        parentItem->appendRow(item);
    }

    setCompletionModel(model);
}
