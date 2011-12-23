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

#include <QtGui/QStandardItemModel>
#include <QtCore/QUrl>

IssnEdit::IssnEdit(QWidget *parent)
    :PropertyEdit(parent)
{
    setPropertyCardinality(UNIQUE_PROPERTY);
}

void IssnEdit::setupLabel()
{
    Nepomuk::Resource series;
    Nepomuk::Resource issue;
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        issue = resource().property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
        series = issue.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    }
    else {
        series = resource().property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    }

    //get the connected journal for the publication
    QString issn = series.property(Nepomuk::Vocabulary::NBIB::issn()).toString();

    addPropertryEntry(issn, series.uri());

    setLabelText(issn);
}

void IssnEdit::updateResource(const QString & text)
{
    Nepomuk::Resource series;
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        Nepomuk::Resource issue = resource().property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
        series = issue.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    }
    else {
        series = resource().property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    }

    if(text.isEmpty()) {
        series.removeProperty(Nepomuk::Vocabulary::NBIB::issn());
        return;
    }

    // else change the issn
    if(series.isValid()) {
        series.setProperty(Nepomuk::Vocabulary::NBIB::issn(), text);
        return;
    }

    // series is not valid, as an issn can only be attached to a series, we create a series here
    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        Nepomuk::Resource issue = resource().property(Nepomuk::Vocabulary::NBIB::collection()).toResource();

        // no collection available for the article, create one
        if(!issue.isValid()) {
            issue = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Collection());

            resource().setProperty(Nepomuk::Vocabulary::NBIB::collection(), issue);
            issue.addProperty(Nepomuk::Vocabulary::NBIB::article(), resource());
        }

        series = issue.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();

        if(!series.isValid()) {
            QUrl seriesUrl = Nepomuk::Vocabulary::NBIB::Series();
            if(issue.hasType(Nepomuk::Vocabulary::NBIB::JournalIssue()))
                seriesUrl = Nepomuk::Vocabulary::NBIB::Journal();
            else if(issue.hasType(Nepomuk::Vocabulary::NBIB::MagazinIssue()))
                seriesUrl = Nepomuk::Vocabulary::NBIB::Magazin();
            else if(issue.hasType(Nepomuk::Vocabulary::NBIB::NewspaperIssue()))
                seriesUrl = Nepomuk::Vocabulary::NBIB::Newspaper();

            series = Nepomuk::Resource(QUrl(), seriesUrl);
            issue.setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), series);
            series.addProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), issue);
        }

        // now we have a valid series add the issn to it
         series.setProperty(Nepomuk::Vocabulary::NBIB::issn(), text);
         return;
    }
    // if we operate on something else than a article
    else {
        if(!series.isValid()) {
            QUrl seriesUrl = Nepomuk::Vocabulary::NBIB::Series();
            if(resource().hasType(Nepomuk::Vocabulary::NBIB::Book()))
                seriesUrl = Nepomuk::Vocabulary::NBIB::BookSeries();

            series = Nepomuk::Resource(QUrl(), seriesUrl);
            resource().setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), series);
            series.addProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), resource());
        }

        // now we have a valid series add the issn to it
         series.setProperty(Nepomuk::Vocabulary::NBIB::issn(), text);
         return;
    }
}

QStandardItemModel* IssnEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    return new QStandardItemModel;
}
