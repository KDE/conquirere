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

#include "journaledit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <QStandardItemModel>

JournalEdit::JournalEdit(QWidget *parent)
    :PropertyEdit(parent)
{
}


void JournalEdit::setupLabel()
{
    //get the connected jornal for the publication
    Nepomuk::Resource journal = resource().property(propertyUrl()).toResource();

    QString title = journal.property(Nepomuk::Vocabulary::NIE::title()).toString();

    addPropertryEntry(title, resource().resourceUri());

    setLabelText(title);
}

void JournalEdit::updateResource(const QString & text)
{
    // remove the existing journal
    resource().removeProperty( propertyUrl() );

    // try to find the propertyurl of an already existing journal
    QUrl propUrl = propertyEntry(text);

    if(propUrl.isValid()) {
        resource().addProperty( propertyUrl(), Nepomuk::Resource(propUrl));
    }
    else {
        // create a new journal with the string s as title
        Nepomuk::Resource newJournal(propUrl, Nepomuk::Vocabulary::NBIB::JournalIssue());
        newJournal.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
        resource().addProperty( propertyUrl(), newJournal);
    }
}

void JournalEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *parentItem = model->invisibleRootItem();

    foreach(Nepomuk::Query::Result r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().property(Nepomuk::Vocabulary::NIE::title()).toString());

        item->setData(r.resource().resourceUri());

        parentItem->appendRow(item);
    }

    setCompletionModel(model);
}
