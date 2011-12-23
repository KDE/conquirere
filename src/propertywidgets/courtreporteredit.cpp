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

#include "courtreporteredit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>
#include <QtCore/QUrl>

CourtReporterEdit::CourtReporterEdit(QWidget *parent) :
    PropertyEdit(parent)
{
    setPropertyUrl( Nepomuk::Vocabulary::NBIB::courtReporter() );
}

void CourtReporterEdit::setupLabel()
{
    QString title;
    Nepomuk::Resource courtReporter = resource().property(propertyUrl()).toResource();
    title = courtReporter.property(Nepomuk::Vocabulary::NIE::title()).toString();

    addPropertryEntry(title, courtReporter.uri());

    setLabelText(title);
}

void CourtReporterEdit::updateResource(const QString & text)
{
    Nepomuk::Resource currentCourtReporter = resource().property( propertyUrl() ).toResource();
    if(text.isEmpty()) {
        resource().removeProperty(propertyUrl());
        currentCourtReporter.removeProperty( Nepomuk::Vocabulary::NBIB::legalCase() , resource());
        return;
    }

    // try to find the propertyurl of an already existing organizatzion
    QUrl propUrl = propertyEntry(text);
    Nepomuk::Resource newCourtReporter = Nepomuk::Resource(propUrl);

    if(currentCourtReporter.isValid()) {
        if(newCourtReporter.isValid()) {
            currentCourtReporter.removeProperty( Nepomuk::Vocabulary::NBIB::legalCase() , resource());

            // change links
            resource().setProperty( propertyUrl() , newCourtReporter);
            newCourtReporter.addProperty( Nepomuk::Vocabulary::NBIB::legalCase() , resource());
        }
        else {
            // rename
            currentCourtReporter.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
        }
        return;
    }

    // if no current courtReporter exist

    if(!newCourtReporter.isValid()) {
        newCourtReporter = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::CourtReporter());
        newCourtReporter.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
    }

    resource().setProperty( propertyUrl() , newCourtReporter);
    newCourtReporter.addProperty( Nepomuk::Vocabulary::NBIB::legalCase() , resource());
}

QStandardItemModel* CourtReporterEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
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
