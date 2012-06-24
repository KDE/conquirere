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

#include "globals.h"
#include "core/library.h"
#include "core/projectsettings.h"
#include "mainui/librarymanager.h"

#include "listpartswidget.h"

#include "nbibio/conquirere.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <KDE/KJob>
#include "sro/nbib/series.h"

#include "nbib.h"
#include <Nepomuk2/Variant>
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>

SeriesWidget::SeriesWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::SeriesWidget)
{
    ui->setupUi(this);

    setupWidget();
    setEnabled(false);
}

SeriesWidget::~SeriesWidget()
{
    delete ui;
}

void SeriesWidget::setLibraryManager(LibraryManager *lm)
{
    ui->editAnnot->setLibraryManager(lm);
    ui->listPartWidget->setLibraryManager(lm);
    SidebarComponent::setLibraryManager(lm);
}

Nepomuk2::Resource SeriesWidget::resource()
{
    return m_series;
}

void SeriesWidget::setResource(Nepomuk2::Resource & resource)
{
    m_series = resource;

    //check if the resource is a valid series
    if(!m_series.isValid()) {
        setEnabled(false);
    }
    else {
        setEnabled(true);
    }

    SeriesType seriesType = SeriesTypeFromUrl(m_series);

    int index = ui->editType->findData(seriesType);
    ui->editType->setCurrentIndex(index);

    ui->editTitle->setResource(m_series);
    ui->editIssn->setResource(m_series);
    ui->listPartWidget->setResource(m_series);
    ui->editAnnot->setResource(m_series);

    // set rating
    ui->editRating->setRating(m_series.rating());
}

void SeriesWidget::newSeriesTypeSelected(int index)
{
    // change the seriestype of the resource
    SeriesType entryType = (SeriesType)ui->editType->itemData(index).toInt();

    // update resource
    QUrl newEntryUrl = SeriesTypeURL.at(entryType);
     if(!m_series.hasType(newEntryUrl)) {
        // create the full hierarchy
        //DEBUG this seems wrong, but is currently the only way to preserve type hierarchy
        QList<QUrl>newtype;
        newtype.append(Nepomuk2::Vocabulary::NIE::InformationElement());
        newtype.append(newEntryUrl);

        // add another hierarchy if the newEntryUrl is not a direct subclass of NBIB::Series()
        switch(entryType) {
        case SeriesType_BookSeries:
        case SeriesType_Journal:
        case SeriesType_Magazin:
        case SeriesType_Newspaper:
            newtype.append(Nepomuk2::Vocabulary::NBIB::Series());
            break;
        case SeriesType_Series:
        case Max_SeriesTypes:
            break;
        }

        if(m_series.isValid()) {
            m_series.setTypes(newtype);
        }
        // when we change the series type, we must change the type of any related collection
        // in the case a collection is a JournalIssue / series the Journal etc.

        QList<Nepomuk2::Resource> collectionResource = m_series.property(Nepomuk2::Vocabulary::NBIB::seriesOf()).toResourceList();

        foreach(Nepomuk2::Resource r, collectionResource) { // krazy:exclude=foreach
            switch(entryType) {
            case SeriesType_Series:
            case Max_SeriesTypes:
                // don't change anything
                break;
            case SeriesType_BookSeries:
            {
                // this changes the resource from a collection to a Book
                // (might run into some bad stuff when articles are attached to it)
                //TODO check if we need to take special attention here
                Nepomuk2::Resource x(QUrl(), Nepomuk2::Vocabulary::NBIB::Book());
                r.setTypes(x.types());
                break;
            }
            case SeriesType_Journal:
            {
                Nepomuk2::Resource x(QUrl(), Nepomuk2::Vocabulary::NBIB::JournalIssue());
                r.setTypes(x.types());
                break;
            }
            case SeriesType_Magazin:
            {
                Nepomuk2::Resource x(QUrl(), Nepomuk2::Vocabulary::NBIB::MagazinIssue());
                r.setTypes(x.types());
                break;
            }
            case SeriesType_Newspaper:
            {
                Nepomuk2::Resource x(QUrl(), Nepomuk2::Vocabulary::NBIB::NewspaperIssue());
                r.setTypes(x.types());
                break;
            }
            }
        }

        emit resourceCacheNeedsUpdate(m_series);
    }
}

void SeriesWidget::newButtonClicked()
{
    //create a new resource with default name

    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Series newSeries;

    newSeries.setProperty( Nepomuk2::Vocabulary::NIE::title(), i18n("New Series"));

    graph << newSeries;
    //blocking graph save
    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNone);
    if( !srj->exec() ) {
        kWarning() << "could not new default series" << srj->errorString();
        return;
    }

    // get the pimo project from the return job mappings
    Nepomuk2::Resource newSeriesResource = Nepomuk2::Resource::fromResourceUri( srj->mappings().value( newSeries.uri() ) );

    Library *curUsedLib = libraryManager()->currentUsedLibrary();
    if(curUsedLib && curUsedLib->libraryType() == Library_Project) {
        curUsedLib->addResource( newSeriesResource );
    }

    setResource(newSeriesResource);
}

void SeriesWidget::deleteButtonClicked()
{
    libraryManager()->systemLibrary()->deleteResource( m_series );

    Nepomuk2::Resource invalid;

    setResource(invalid);
}

void SeriesWidget::changeRating(int newRating)
{
    if(newRating == m_series.rating() ) {
        return;
    }

    QList<QUrl> resourceUris; resourceUris << m_series.resourceUri();
    QVariantList rating; rating <<  newRating;
    KJob *job = Nepomuk2::setProperty(resourceUris, Soprano::Vocabulary::NAO::numericRating(), rating);

    if(job->exec()) {
        emit resourceCacheNeedsUpdate(m_series);
    }
}

void SeriesWidget::setupWidget()
{
    int i=0;
    foreach(const QString &s, SeriesTypeTranslation) {
        if(ConqSettings::hiddenNbibSeries().contains(i)) {
            i++;
            continue;
        }

        ui->editType->addItem(s,(SeriesType)i);
        i++;
    }

    connect(ui->editType, SIGNAL(currentIndexChanged(int)), this, SLOT(newSeriesTypeSelected(int)));

    ui->editIssn->setPropertyUrl( Nepomuk2::Vocabulary::NBIB::issn() );
    ui->editTitle->setPropertyUrl( Nepomuk2::Vocabulary::NIE::title() );

    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(changeRating(int)));

    // TODO this part should be removed when the resourceWatcher is working correctly
    connect(ui->listPartWidget, SIGNAL(resourceCacheNeedsUpdate(Nepomuk2::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk2::Resource)));
    connect(ui->editIssn, SIGNAL(resourceCacheNeedsUpdate(Nepomuk2::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk2::Resource)));
    connect(ui->editTitle, SIGNAL(resourceCacheNeedsUpdate(Nepomuk2::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk2::Resource)));
    connect(ui->editAnnot, SIGNAL(resourceCacheNeedsUpdate(Nepomuk2::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk2::Resource)));
}
