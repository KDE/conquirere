/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "proceedingsedit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>
#include <QtCore/QUrl>

ProceedingsEdit::ProceedingsEdit(QWidget *parent)
    :PropertyEdit(parent)
{
}

void ProceedingsEdit::setupLabel()
{
    QString title;
    //get the connected proceedings for the inproceedings elements
    Nepomuk::Resource proceedings = resource().property(Nepomuk::Vocabulary::NBIB::collection()).toResource();

    if(proceedings.hasType(Nepomuk::Vocabulary::NBIB::Proceedings())) {
        title = proceedings.property(Nepomuk::Vocabulary::NIE::title()).toString();
        addPropertryEntry(title, resource().uri());
    }

    setLabelText(title);
}

void ProceedingsEdit::updateResource(const QString & text)
{
    // remove the existing proccedings
    resource().removeProperty( Nepomuk::Vocabulary::NBIB::collection() );

    if(text.isEmpty())
        return;

    // try to find the propertyurl of an already existing journal
    QUrl propUrl = propertyEntry(text);

    if(propUrl.isValid()) {
        resource().addProperty( propertyUrl(), Nepomuk::Resource(propUrl));
    }
    else {
        // create a new proceedings with the string text as title
        Nepomuk::Resource newProceedings(QUrl(), Nepomuk::Vocabulary::NBIB::Proceedings());
        newProceedings.setProperty(Nepomuk::Vocabulary::NIE::title(), text);

        resource().addProperty( Nepomuk::Vocabulary::NBIB::collection(), newProceedings);

        //connect inproceedings back to proceedings
        newProceedings.addProperty(Nepomuk::Vocabulary::NBIB::article(), resource());
    }
}

QStandardItemModel* ProceedingsEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *parentItem = model->invisibleRootItem();

    foreach(const Nepomuk::Query::Result & r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().property(Nepomuk::Vocabulary::NIE::title()).toString());

        item->setData(r.resource().uri());

        parentItem->appendRow(item);
    }

    return model;
}
