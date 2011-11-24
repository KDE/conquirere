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

    addPropertryEntry(issn, resource().uri());

    setLabelText(issn);
}

void IssnEdit::updateResource(const QString & text)
{
    Nepomuk::Resource series;
    Nepomuk::Resource issue;

    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        issue = resource().property(Nepomuk::Vocabulary::NBIB::collection()).toResource();

        if(!issue.isValid()) {
            issue = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::JournalIssue());
            //DEBUG this seems wrong, but is currently the only way to preserve type hierarchy
            QList<QUrl>newtype;
            newtype.append(Nepomuk::Vocabulary::NIE::InformationElement());
            newtype.append(Nepomuk::Vocabulary::NBIB::Publication());
            newtype.append(Nepomuk::Vocabulary::NBIB::Collection());
            newtype.append(issue.types());
            issue.setTypes(newtype);

            // connect article<->journal issue
            resource().setProperty(Nepomuk::Vocabulary::NBIB::collection(), issue);
            issue.addProperty(Nepomuk::Vocabulary::NBIB::article(), resource());
            series = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Journal());
        }
        else {
            series = issue.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
        }
    }
    else {
        series = resource().property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
    }

    if(!series.isValid()) {
        series = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Journal());
    }

    series.setProperty(Nepomuk::Vocabulary::NBIB::issn(), text);

    if(resource().hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        issue.setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), series);
        series.setProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), issue);
    }
    else {
        resource().setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), series);
        series.setProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), resource());
    }
}

QStandardItemModel* IssnEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    return new QStandardItemModel;
}
