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

#include "serieswidget.h"
#include "ui_serieswidget.h"
#include "../core/library.h"

#include "propertywidgets/stringedit.h"
#include "publicationwidget.h"
#include "listpartswidget.h"

#include "nbib.h"
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>

#include <KDE/KDialog>

#include <QtCore/QDebug>

SeriesWidget::SeriesWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::SeriesWidget)
{
    ui->setupUi(this);

    setupWidget();
}

SeriesWidget::~SeriesWidget()
{
    delete ui;
}

void SeriesWidget::setLibrary(Library *p)
{
    SidebarComponent::setLibrary(p);

    //TODO remove and use ResourceWatcher later on
    connect(ui->editIssn, SIGNAL(resourceUpdated(Nepomuk::Resource)), p, SIGNAL(resourceUpdated(Nepomuk::Resource)));
    connect(ui->editTitle, SIGNAL(resourceUpdated(Nepomuk::Resource)), p, SIGNAL(resourceUpdated(Nepomuk::Resource)));

    connect(this, SIGNAL(resourceUpdated(Nepomuk::Resource)), p, SIGNAL(resourceUpdated(Nepomuk::Resource)));
}

void SeriesWidget::setResource(Nepomuk::Resource & resource)
{
    m_series = resource;

    //check if the resource is a valid series
    if(!m_series.isValid()) {
        ui->editIssn->setEnabled(false);
        ui->editTitle->setEnabled(false);
        ui->editType->setEnabled(false);
    }
    else {
        ui->editIssn->setEnabled(true);
        ui->editTitle->setEnabled(true);
        ui->editType->setEnabled(true);
    }

    emit resourceChanged(m_series);

    SeriesType seriesType = SeriesTypeFromUrl(m_series);

    int index = ui->editType->findData(seriesType);
    ui->editType->setCurrentIndex(index);

    ui->listPartWidget->setResource(m_series);
}

void SeriesWidget::newSeriesTypeSelected(int index)
{
    qDebug() << "TODO SeriesWidget::newSeriesTypeSelected";
}

void SeriesWidget::newButtonClicked()
{
    //create a new resource
    Nepomuk::Resource nb = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Publication());
    QList<QUrl> types;
    types.append(Nepomuk::Vocabulary::NBIB::Publication());
    nb.setTypes(types);

    setResource(nb);
}

void SeriesWidget::deleteButtonClicked()
{
    qDebug() << "remove all links from the deleted series";

    m_series.remove();

    setResource(m_series);
}

void SeriesWidget::changeRating(int newRating)
{
    m_series.setRating(newRating);

    emit resourceUpdated(m_series);
}

void SeriesWidget::setupWidget()
{
    int i=0;
    foreach(const QString &s, SeriesTypeTranslation) {
        ui->editType->addItem(s,(SeriesType)i);
        i++;
    }

    connect(ui->editType, SIGNAL(currentIndexChanged(int)), this, SLOT(newSeriesTypeSelected(int)));

    ui->editIssn->setPropertyUrl( Nepomuk::Vocabulary::NBIB::issn() );
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editIssn, SLOT(setResource(Nepomuk::Resource&)));
    ui->editTitle->setPropertyUrl( Nepomuk::Vocabulary::NIE::title() );
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editTitle, SLOT(setResource(Nepomuk::Resource&)));

    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(changeRating(int)));

    connect(ui->listPartWidget, SIGNAL(resourceUpdated(Nepomuk::Resource)), this, SIGNAL(resourceUpdated(Nepomuk::Resource)));
}
