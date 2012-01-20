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

#include <QtGui/QStandardItemModel>

ChapterEdit::ChapterEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void ChapterEdit::setupLabel()
{
    Nepomuk::Resource chapter = resource().property(propertyUrl()).toResource();

    QString title = chapter.property(Nepomuk::Vocabulary::NIE::title()).toString();

    addPropertryEntry(title, chapter.resourceUri().toString());

    setLabelText(title);
}

void ChapterEdit::updateResource(const QString & text)
{
    Nepomuk::Resource currentChapter = resource().property(propertyUrl()).toResource();

    if(text.isEmpty()) {
        resource().removeProperty( propertyUrl(), currentChapter );
        return;
    }

    // find existing chapter
    QUrl propUrl = propertyEntry(text);
    Nepomuk::Resource newChapter = Nepomuk::Resource(propUrl);

    if(currentChapter.isValid()) {
        if(newChapter.isValid()) {
            // link to new chapter
            resource().setProperty( propertyUrl(), newChapter);
        }
        else {
            //rename current Chapter
            currentChapter.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
        }
        return;
    }

    // now current chapter available
    if(!newChapter.isValid()) {
        newChapter = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Chapter());
        newChapter.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
    }

    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Reference())) {
        resource().setProperty( Nepomuk::Vocabulary::NBIB::referencedPart(), newChapter);
    }

    // connect the chapter to the book
    Nepomuk::Resource bookResource = resource().property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
    if(bookResource.isValid()) {
        bookResource.addProperty(Nepomuk::Vocabulary::NBIB::documentPart(), currentChapter);
        currentChapter.setProperty(Nepomuk::Vocabulary::NBIB::documentPartOf(), bookResource);
    }
}

QList<QStandardItem*> ChapterEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    // entries contain all Chapter from any book (that are created)
    // thats not so good ;)

    // idea, filter all entries to include only the once of the current resource()->usePublication entry
    // even better filter plainTextContent for all chapters available
    // a lot better use prefilled nbib:contents
    QList<QStandardItem*> results;

    foreach(const Nepomuk::Query::Result & r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().property(Nepomuk::Vocabulary::NIE::title()).toString());

        item->setData(r.resource().resourceUri().toString());

        results.append(item);
    }

    return results;
}
