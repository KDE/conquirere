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

#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Variant>

#include <KDE/KDebug>

#include <QAbstractItemModel>
#include <QtGui/QStandardItemModel>
#include <QtGui/QCompleter>

ContactEdit::ContactEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void ContactEdit::setupLabel()
{
    QString labelText;

    if(hasMultipleCardinality()) {
        QList<Nepomuk::Resource> authorList = resource().property(propertyUrl()).toResourceList();

        foreach(const Nepomuk::Resource & r, authorList) {
            QString fullname = r.property(Nepomuk::Vocabulary::NCO::fullname()).toString();
            labelText.append(fullname.trimmed());
            addPropertryEntry(fullname, r.resourceUri().toString());

            labelText.append(QLatin1String("; "));
        }

        labelText.chop(2);
    }
    else {
        Nepomuk::Resource author = resource().property(propertyUrl()).toResource();

        QString fullname = author.property(Nepomuk::Vocabulary::NCO::fullname()).toString();
        labelText.append(fullname.trimmed());
        addPropertryEntry(fullname, author.resourceUri().toString());
    }

    setLabelText(labelText);
}

void ContactEdit::updateResource(const QString & text)
{
    // remove all existing contacts
    resource().removeProperty( propertyUrl() );

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

        if(s.trimmed().isEmpty())
            continue;

        // try to find the propertyurl of an already existing contact
        QUrl propContactUrl = propertyEntry(s.trimmed());
        if(propContactUrl.isValid()) {
            Nepomuk::Resource contact = Nepomuk::Resource(propContactUrl);
            kDebug() << "add existing contact" << contact.genericLabel();
            resource().addProperty( propertyUrl(), contact);
        }
        else {
            kDebug() << "try to find the name of the contact entered without complter in the complter model";
            QStandardItemModel *sim = dynamic_cast<QStandardItemModel *>(m_completer->model());
            if(!sim)
                kDebug() << "no QStandardItemModel";
            else {
                QList<QStandardItem *> siList = sim->findItems(s.trimmed());
                if(siList.isEmpty())
                    kDebug() << "did not find the contact" << s.trimmed();
                else {
                    kDebug() << "found matching contacts" << siList.size() << "for contact" << s.trimmed();
                    Nepomuk::Resource contact = Nepomuk::Resource(siList.first()->data(Qt::UserRole + 1).toUrl());
                    kDebug() << "add existing contact" << contact.genericLabel();
                    resource().addProperty( propertyUrl(), contact);
                    continue;
                }
            }

            // create a new contact with the string s as fullname
            Nepomuk::Resource newContact(propContactUrl, Nepomuk::Vocabulary::NCO::Contact());
            newContact.setProperty(Nepomuk::Vocabulary::NCO::fullname(), s.trimmed());
            resource().addProperty( propertyUrl(), newContact);
        }
    }
}

QList<QStandardItem*> ContactEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QList<QStandardItem*> results;

    foreach(const Nepomuk::Query::Result & r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().genericLabel());
        // save the resource uri with the model item
        // this helps to identify the selected entry even if the generic label has
        // the same result on two different items
        // also it is not necessary to ask nepomuk for the resource used later again
        item->setData(r.resource().resourceUri().toString().trimmed());

        results.append(item);
    }

    return results;
}
