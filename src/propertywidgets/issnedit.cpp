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

#include "issnedit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <QStandardItemModel>
#include <QUrl>

IssnEdit::IssnEdit(QWidget *parent)
    :PropertyEdit(parent)
{
    setPropertyCardinality(UNIQUE_PROPERTY);
}

void IssnEdit::setupLabel()
{
    //get the connected journal for the publication
    Nepomuk::Resource series = resource().property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();

    QString issn = series.property(Nepomuk::Vocabulary::NBIB::issn()).toString();

    addPropertryEntry(issn, resource().resourceUri());

    setLabelText(issn);
}

void IssnEdit::updateResource(const QString & text)
{
    Nepomuk::Resource series = resource().property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();

    if(!series.isValid()) {
        series = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::BookSeries());
        resource().setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), series);
        series.setProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), resource());
    }

    series.setProperty(Nepomuk::Vocabulary::NBIB::issn(), text);
}

void IssnEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
}
