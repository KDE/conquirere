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
}

void CourtReporterEdit::setupLabel()
{
    QString title;
    Nepomuk::Resource courtReporter = resource().property(Nepomuk::Vocabulary::NBIB::courtReporter()).toResource();
    title = courtReporter.property(Nepomuk::Vocabulary::NIE::title()).toString();

    addPropertryEntry(title, courtReporter.uri());

    setLabelText(title);
}

void CourtReporterEdit::updateResource(const QString & text)
{
    //remove exsting courtReporter
    resource().removeProperty(Nepomuk::Vocabulary::NBIB::courtReporter());

    if(text.isEmpty())
        return;

    // try to find the propertyurl of an already existing organizatzion
    QUrl propUrl = propertyEntry(text);

    if(propUrl.isValid()) {
        resource().addProperty( propertyUrl(), Nepomuk::Resource(propUrl));
    }
    else {
        // create a new organization with the string text as fullname
        Nepomuk::Resource newCourtReporter(QUrl(), Nepomuk::Vocabulary::NBIB::CourtReporter());
        newCourtReporter.setProperty(Nepomuk::Vocabulary::NIE::title(), text);

        resource().addProperty( Nepomuk::Vocabulary::NBIB::courtReporter(), newCourtReporter);
    }
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
