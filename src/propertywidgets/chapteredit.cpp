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

#include "ontology/nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Variant>

using namespace Nepomuk2::Vocabulary;

ChapterEdit::ChapterEdit(QWidget *parent)
    : PropertyEdit(parent)
{
    setDirectEdit(false);
    setUseDetailDialog(true);
}

void ChapterEdit::setupLabel()
{
    Nepomuk2::Resource chapter = resource().property(NBIB::referencedPart()).toResource();
    // creates a string in the form
    // "1. Introduction" or just "Introduction"
    QString showString;
    QString title = chapter.property(NIE::title()).toString();
    QString number = chapter.property(NBIB::chapterNumber()).toString();

    showString.append(title);

    if(!number.isEmpty()) {
        showString.prepend(number + QLatin1String(". "));
    }

    setLabelText(showString);
}

void ChapterEdit::updateResource(const QString & newChapterTitle)
{
    Q_UNUSED(newChapterTitle)
    // direct edit is not allowed, so we do not need this
}
