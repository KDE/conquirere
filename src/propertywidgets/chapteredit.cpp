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

#include "chapteredit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <QStandardItemModel>

ChapterEdit::ChapterEdit(QWidget *parent)
    :PropertyEdit(parent)
{
}

void ChapterEdit::setupLabel()
{
    Nepomuk::Resource chapter = resource().property(propertyUrl()).toResource();

    QString title = chapter.property(Nepomuk::Vocabulary::NIE::title()).toString();

    addPropertryEntry(title, chapter.resourceUri());

    setLabelText(title);
}

void ChapterEdit::updateResource(const QString & text)
{
    // we restrict editing of the chapter
    // it is only possible to select a chapter when a book publication is added
    // to the resource (which is a nbib:BibReference)

    // remove the existing chapter
    resource().removeProperty( propertyUrl() );

    // add the selected chapter
    QUrl propUrl = propertyEntry(text);

    if(propUrl.isValid()) {
        resource().addProperty( propertyUrl(), Nepomuk::Resource(propUrl));
    }
    else {
        // create a new chapter with the string s as title
        Nepomuk::Resource newChapter(propUrl, Nepomuk::Vocabulary::NBIB::Chapter());
        newChapter.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
        resource().addProperty( propertyUrl(), newChapter);
    }

    // connect the chapter to the book
    Nepomuk::Resource bookResource = resource().property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();

    bookResource.setProperty(Nepomuk::Vocabulary::NIE::hasLogicalPart(),
                             resource().property( Nepomuk::Vocabulary::NBIB::hasChapter()).toResource());
    resource().setProperty(Nepomuk::Vocabulary::NIE::isLogicalPartOf(), bookResource);

}

void ChapterEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    // entries contain all Chapter from any book (that are created)
    // thats not so good ;)

    // idea, filter all entries to include only the once of the current resource()->usePublication Book entry
    // even better filter plainTextContent for all chapters available
    // alot better use prefilled nbib:contents
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *parentItem = model->invisibleRootItem();

    foreach(Nepomuk::Query::Result r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().property(Nepomuk::Vocabulary::NIE::title()).toString());

        item->setData(r.resource().resourceUri());

        parentItem->appendRow(item);
    }

    setCompletionModel(model);
}
