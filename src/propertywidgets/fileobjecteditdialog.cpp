/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "fileobjecteditdialog.h"
#include "propertywidgets/ui_fileobjecteditdialog.h"

#include "stringedit.h"
#include "dateedit.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>

#include <KDE/KJob>
#include "sro/nfo/webdataobject.h"
#include "sro/nfo/website.h"

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NUAO>
#include <Nepomuk2/Vocabulary/NFO>
#include <Soprano/Vocabulary/RDF>
#include <Soprano/Vocabulary/NAO>

#include <Nepomuk2/Query/QueryServiceClient>
#include <Nepomuk2/Query/Result>
#include <Nepomuk2/Query/QueryParser>
#include <Nepomuk2/Variant>

#include <KDE/KInputDialog>
#include <KDE/KFileDialog>
#include <KDE/KDebug>

#include <QtCore/QUrl>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

FileObjectEditDialog::FileObjectEditDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FileObjectEditDialog)
{
    ui->setupUi(this);

    ui->editTitle->setPropertyUrl( NIE::title());
    ui->editLastAccessed->setPropertyUrl( NUAO::lastUsage());
    ui->urlSelector->setIcon(KIcon("document-open-remote"));

    connect(ui->editType, SIGNAL(currentIndexChanged(int)), this, SLOT(typeChanged(int)));
    connect(ui->urlSelector, SIGNAL(clicked()), this, SLOT(openUrlSelectionDialog()));
}

FileObjectEditDialog::~FileObjectEditDialog()
{
    delete ui;
}

void FileObjectEditDialog::setLibraryManager(LibraryManager *lm)
{
    ui->editNotes->setLibraryManager(lm);
}

void FileObjectEditDialog::setPublication(const Nepomuk2::Resource &r)
{
    m_publication = r;
}

void FileObjectEditDialog::setResource(Nepomuk2::Resource r)
{
    m_fileObject = r;

    ui->editTitle->setResource(m_fileObject);
    ui->editLastAccessed->setResource(m_fileObject);
    ui->editNotes->setResource(m_fileObject);

    ui->editUrl->setText( m_fileObject.property(NIE::url()).toString() );

    ui->editType->blockSignals(true);
    if( m_fileObject.hasType(NFO::Website())) {
        ui->editType->setCurrentIndex(2);
    }
    else if( m_fileObject.hasType(NFO::RemoteDataObject())) {
        ui->editType->setCurrentIndex(1);
    }
    else {
        ui->editType->setCurrentIndex(0);
    }
    ui->editType->blockSignals(false);
}

void FileObjectEditDialog::createNewResource()
{
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NFO::Website newWebsite;
    newWebsite.addType(NFO::WebDataObject());
    newWebsite.addType(NIE::InformationElement());

    newWebsite.setProperty( NIE::title(), i18n("New Website"));

    graph << newWebsite;
    //blocking graph save
    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNone);
    if( !srj->exec() ) {
        kWarning() << "could not create new default website" << srj->errorString();
        return;
    }

    // get the resource from the return job mappings
    Nepomuk2::Resource newWebsiteResource( srj->mappings().value( newWebsite.uri() ) );

    // add the crosslinking
    Nepomuk2::addProperty(QList<QUrl>() << m_publication.uri(), NIE::links(), QVariantList() << newWebsiteResource.uri());
    Nepomuk2::addProperty(QList<QUrl>() << m_publication.uri(), NAO::hasSubResource(), QVariantList() << newWebsiteResource.uri());

    setResource(newWebsiteResource);
}

Nepomuk2::Resource FileObjectEditDialog::resource()
{
    return m_fileObject;
}

void FileObjectEditDialog::saveAndMergeUrlChange()
{
    QString newUrl = ui->editUrl->fullText();
    QString existingUrl = m_fileObject.property(NIE::url()).toString();
    if(newUrl == existingUrl) {
        return;
    }

    if(!newUrl.isEmpty()) {
        QString query = "Select DISTINCT ?r where {"
                        "?r nie:url ?url . FILTER ( regex(?url, \"^" + newUrl + "$\"))"
                        "}";

        QList<Nepomuk2::Query::Result> queryResult = Nepomuk2::Query::QueryServiceClient::syncSparqlQuery(query);

        if(!queryResult.isEmpty() && queryResult.first().resource().uri() != m_fileObject.uri()) {
            kDebug() << "found a duplicate with url" << newUrl << "merge it";
            KJob *job = Nepomuk2::mergeResources(queryResult.first().resource().uri(), m_fileObject.uri());
            job->exec();

            if(job->error() != 0) {
                kDebug() << job->errorString() << job->errorText();
            }

            setResource(queryResult.first().resource());
        }
        else {
            kDebug() << "set url to " << newUrl;
            QList<QUrl> fileObjectUri; fileObjectUri << m_fileObject.uri();
            QVariantList fileObjectValue; fileObjectValue << newUrl;
            Nepomuk2::setProperty(fileObjectUri, NIE::url(), fileObjectValue);
        }
    }
}

void FileObjectEditDialog::typeChanged(int newType)
{
    bool switchToWebsite = true;
    bool switchToFile = true;

    QList<QUrl> currentTypes = m_fileObject.types();

    if(newType == 0) { // local file
        switchToWebsite = false;
        if(currentTypes.contains( NFO::FileDataObject() )) {
            switchToFile = false;
        }
        if(currentTypes.contains( NFO::Website() )) {
            switchToFile = true;
        }

        currentTypes.removeAll(NFO::Website());
        currentTypes.removeAll(NFO::WebDataObject());
        currentTypes.removeAll(NFO::RemoteDataObject());

        if(!currentTypes.contains( NFO::FileDataObject() )) {
            currentTypes.append( NFO::FileDataObject() );
        }
    }
    else if(newType == 1) { // remote file
        switchToWebsite = false;
        if(currentTypes.contains( NFO::FileDataObject() )) {
            switchToFile = false;
        }
        if(currentTypes.contains( NFO::Website() )) {
            switchToFile = true;
        }

        currentTypes.removeAll(NFO::Website());
        currentTypes.removeAll(NFO::WebDataObject());

        if(!currentTypes.contains( NFO::FileDataObject() )) {
            currentTypes.append( NFO::FileDataObject() );
        }

        if(!currentTypes.contains( NFO::RemoteDataObject() )) {
            currentTypes.append(NFO::RemoteDataObject());
        }
    }
    else if(newType == 2) { // nfo:website
        switchToFile = false;
        if(currentTypes.contains( NFO::FileDataObject() )) {
            switchToWebsite = true;
        }
        if(currentTypes.contains( NFO::Website() )) {
            switchToWebsite = false;
        }

        currentTypes.removeAll(NFO::FileDataObject());
        currentTypes.removeAll(NFO::RemoteDataObject());

        if(!currentTypes.contains( NFO::WebDataObject() )) {
            currentTypes.append( NFO::WebDataObject() );
        }
        if(!currentTypes.contains( NFO::Website() )) {
            currentTypes.append( NFO::Website() );
        }
    }

    QList<QUrl> publicationUri; publicationUri << m_publication.uri();
    QVariantList publicationValue; publicationValue << m_publication.uri();
    QList<QUrl> fileObjectUri; fileObjectUri << m_fileObject.uri();
    QVariantList fileObjectValue; fileObjectValue << m_fileObject.uri();
    QVariantList typeValue;

    foreach(const QUrl &url, currentTypes) {
        typeValue << url;
    }

    //m_fileObject.setTypes(currentTypes);

    // this appraoch is not working
    QList<QUrl> removeAllTypes; removeAllTypes << RDF::type();
    KJob *job = Nepomuk2::removeProperties(fileObjectUri, removeAllTypes);
    if(!job->exec() ) {
        kDebug() << job->errorString();
    }
    KJob *job2 = Nepomuk2::setProperty(fileObjectUri, RDF::type(), typeValue);
    if(!job->exec() ) {
        kDebug() << job2->errorString();
    }

    // change crosslink from nbib:publicationOf / nbib:isPublishedAs to nie:links
    if( switchToFile ) {
        Nepomuk2::removeProperty(publicationUri, NIE::links(), fileObjectValue);
        Nepomuk2::addProperty(publicationUri, NBIB::isPublicationOf(), fileObjectValue);
        Nepomuk2::addProperty(fileObjectUri, NBIB::publishedAs(), publicationValue);
    }
    else if( switchToWebsite ) {
        Nepomuk2::removeProperty(publicationUri, NBIB::isPublicationOf(), fileObjectValue);
        Nepomuk2::removeProperty(fileObjectUri, NBIB::publishedAs(), publicationValue);
        Nepomuk2::addProperty(publicationUri, NIE::links(), fileObjectValue);
    }
}

void FileObjectEditDialog::openUrlSelectionDialog()
{
    QString selectedUrl;

    if( ui->editType->currentIndex() == 2) {
        selectedUrl = KInputDialog::getText(i18n("Enter website url"),i18n("Please enter the url of the webseite you want to add"),
                                    ui->editUrl->fullText());
    }
    else {
        KUrl url = KFileDialog::getOpenUrl();
        selectedUrl = url.url();
    }

    ui->editUrl->setText(selectedUrl);
    saveAndMergeUrlChange();
}
