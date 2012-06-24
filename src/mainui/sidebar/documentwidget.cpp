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

#include "documentwidget.h"
#include "ui_documentwidget.h"

#include "listpublicationsdialog.h"

#include "mainui/mainwindow.h"
#include "mainui/librarymanager.h"

#include <Nepomuk2/DataManagement>
#include <KDE/KJob>

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Variant>

#include <KDE/KFileMetaDataWidget>
#include <KDE/KGlobalSettings>
#include <KDE/KMimeType>
#include <KDE/KIconLoader>

#include <KDE/KDebug>

using namespace Nepomuk2::Vocabulary;

DocumentWidget::DocumentWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::DocumentWidget)
{
    ui->setupUi(this);

    connect(this, SIGNAL(hasPublication(bool)), parent, SLOT(hasPublication(bool)));

    ui->line->setVisible(false);
    ui->icon->setVisible(false);
    ui->kfmdWidget->setVisible(false);
}

DocumentWidget::~DocumentWidget()
{
    delete ui;
}

void DocumentWidget::setLibraryManager(LibraryManager *lm)
{
    ui->editAnnot->setLibraryManager(lm);
    SidebarComponent::setLibraryManager(lm);
}

Nepomuk2::Resource DocumentWidget::resource()
{
    return m_document;
}

void DocumentWidget::setResource(Nepomuk2::Resource & resource)
{
    m_document = resource;

    ui->editAnnot->setResource( m_document );

    if(m_document.isValid()) {
        ui->line->setVisible(true);
        ui->icon->setVisible(true);
        ui->kfmdWidget->setVisible(true);

        //fetch the documents icon
        QString fileUrl = m_document.property(Nepomuk2::Vocabulary::NIE::url()).toString();

        QString mimeType = KMimeType::iconNameForUrl( fileUrl);

        KIconLoader kil;
        QPixmap fileIcon = kil.loadMimeTypeIcon(mimeType,  KIconLoader::Desktop);

        ui->icon->setPixmap(fileIcon);

        // show file meta data
        KFileItem kf(KFileItem::Unknown, KFileItem::Unknown, KUrl(fileUrl));
        kf.refresh();
        kf.refreshMimeType();
        KFileItemList kfil;
        kfil.append(kf);
        ui->kfmdWidget->setItems(kfil);

        Nepomuk2::Resource pa = m_document.property(Nepomuk2::Vocabulary::NBIB::publishedAs()).toResource();
        if(pa.isValid()) {
            emit hasPublication(true);
        }
        else {
            emit hasPublication(false);
        }
    }
    else {
        emit hasPublication(false);
        ui->line->setVisible(false);
        ui->icon->setVisible(false);
        ui->kfmdWidget->setVisible(false);
    }
}

void DocumentWidget::newButtonClicked()
{
    kDebug() << "TODO add file metadata";
}

void DocumentWidget::deleteButtonClicked()
{
    kDebug() << "TODO delete file + metadata";
}

void DocumentWidget::setPublication()
{
    QPointer<ListPublicationsDialog> lpd = new ListPublicationsDialog(this);
    lpd->setLibraryManager(libraryManager());

    int ret = lpd->exec();

    if(ret == QDialog::Accepted) {
        Nepomuk2::Resource publication = lpd->selectedPublication();

        QList<QUrl> resUri; resUri << m_document.resourceUri();
        QVariantList value; value << publication.resourceUri();
        KJob *job1 = Nepomuk2::addProperty(resUri, NBIB::publishedAs(), value);
        job1->exec(); //blocking call...

        resUri.clear(); resUri << publication.resourceUri();
        value.clear(); value << m_document.resourceUri();
        KJob *job2 = Nepomuk2::addProperty(resUri, NBIB::isPublicationOf(), value);
        job2->exec(); //blocking call...

        setResource(m_document);
        emit resourceCacheNeedsUpdate(m_document);
        emit resourceCacheNeedsUpdate(publication);
    }

    delete lpd;
}

void DocumentWidget::removePublication()
{
    Nepomuk2::Resource publication = m_document.property(Nepomuk2::Vocabulary::NBIB::publishedAs()).toResource();

    QList<QUrl> resourceUris; resourceUris << m_document.resourceUri();
    QVariantList value; value << publication.resourceUri();
    KJob *job1 = Nepomuk2::removeProperty(resourceUris, NBIB::publishedAs(), value);
    job1->exec(); //blocking call...

    resourceUris.clear(); resourceUris << publication.resourceUri();
    value.clear(); value << m_document.resourceUri();
    KJob *job2 = Nepomuk2::removeProperty(resourceUris, NBIB::isPublicationOf(), value);
    job2->exec(); //blocking call...

    //update
    setResource(m_document);
    emit resourceCacheNeedsUpdate(m_document);
    emit resourceCacheNeedsUpdate(publication);
}

