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

#include "publicationedit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>

PublicationEdit::PublicationEdit(QWidget *parent)
    :PropertyEdit(parent)
{
}

void PublicationEdit::setupLabel()
{
    Nepomuk::Resource publication = resource().property(propertyUrl()).toResource();

    QString title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();

    addPropertryEntry(title, publication.uri());

    setLabelText(title);
}

void PublicationEdit::updateResource(const QString & text)
{
    // remove the existing publication
    Nepomuk::Resource publication = resource().property( propertyUrl() ).toResource();
    resource().removeProperty( propertyUrl() );
    // remove backlink too
    publication.removeProperty( Nepomuk::Vocabulary::NBIB::reference(), resource());

    if(text.isEmpty())
        return;

    // add the selected publication
    QUrl propUrl = propertyEntry(text);

    if(propUrl.isValid()) {
        resource().addProperty( propertyUrl(), Nepomuk::Resource(propUrl));
    }
    else {
        // create a new publication with the string s as title
        Nepomuk::Resource newPublication(propUrl, Nepomuk::Vocabulary::NBIB::Publication());
        newPublication.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
        resource().setProperty( propertyUrl(), newPublication);
        newPublication.addProperty( Nepomuk::Vocabulary::NBIB::reference(), resource());
    }
}

void PublicationEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *parentItem = model->invisibleRootItem();

    foreach(const Nepomuk::Query::Result & r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().property(Nepomuk::Vocabulary::NIE::title()).toString());

        item->setData(r.resource().uri());

        parentItem->appendRow(item);
    }

    setCompletionModel(model);
}
