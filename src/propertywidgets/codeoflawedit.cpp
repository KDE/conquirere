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

#include "codeoflawedit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>

CodeOfLawEdit::CodeOfLawEdit(QWidget *parent) :
    PropertyEdit(parent)
{
}

void CodeOfLawEdit::setupLabel()
{
    Nepomuk::Resource publication = resource().property(Nepomuk::Vocabulary::NBIB::codeOfLaw()).toResource();

    QString title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();

    addPropertryEntry(title, publication.uri());

    setLabelText(title);
}

void CodeOfLawEdit::updateResource(const QString & text)
{
    Nepomuk::Resource currentCodeOfLaw = resource().property( Nepomuk::Vocabulary::NBIB::codeOfLaw() ).toResource();

    if(text.isEmpty()) {
        resource().removeProperty( Nepomuk::Vocabulary::NBIB::codeOfLaw() );
        // remove backlink too
        currentCodeOfLaw.removeProperty( Nepomuk::Vocabulary::NBIB::legislation(), resource());
        return;
    }

    // add the selected publication
    QUrl propUrl = propertyEntry(text);
    Nepomuk::Resource newCodeOfLaw(propUrl);

    if(currentCodeOfLaw.isValid()) {
        if(newCodeOfLaw.isValid()) {
            // change link to new CodeOfLaw
            resource().removeProperty( Nepomuk::Vocabulary::NBIB::codeOfLaw() );
            currentCodeOfLaw.removeProperty( Nepomuk::Vocabulary::NBIB::legislation(), resource());

            resource().setProperty( Nepomuk::Vocabulary::NBIB::codeOfLaw(), newCodeOfLaw);
            newCodeOfLaw.addProperty( Nepomuk::Vocabulary::NBIB::legislation(), resource());
        }
        else {
            //rename current CodeOflaw
            currentCodeOfLaw.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
        }
        return;
    }

    // no current CodeOfLaw available

    if(!newCodeOfLaw.isValid()) {
        // create a new publication with the string text as title
        newCodeOfLaw = Nepomuk::Resource(propUrl, Nepomuk::Vocabulary::NBIB::CodeOfLaw());
        newCodeOfLaw.setProperty(Nepomuk::Vocabulary::NIE::title(), text);

    }
    resource().setProperty( Nepomuk::Vocabulary::NBIB::codeOfLaw(), newCodeOfLaw);
    newCodeOfLaw.addProperty( Nepomuk::Vocabulary::NBIB::legislation(), resource());
}

QStandardItemModel* CodeOfLawEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
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

