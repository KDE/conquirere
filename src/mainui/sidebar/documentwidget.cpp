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

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <KDE/KFileMetaDataWidget>
#include <KDE/KGlobalSettings>
#include <KDE/KMimeType>
#include <KDE/KIconLoader>

#include <KDE/KDebug>

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

Nepomuk::Resource DocumentWidget::resource()
{
    return m_document;
}

void DocumentWidget::setResource(Nepomuk::Resource & resource)
{
    m_document = resource;

    ui->editAnnot->setResource( m_document );

    if(m_document.isValid()) {
        ui->line->setVisible(true);
        ui->icon->setVisible(true);
        ui->kfmdWidget->setVisible(true);

        //fetch the documents icon
        QString fileUrl = m_document.property(Nepomuk::Vocabulary::NIE::url()).toString();

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

        Nepomuk::Resource pa = m_document.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();
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
    ListPublicationsDialog lpd;
    lpd.setLibraryManager(libraryManager());

    int ret = lpd.exec();

    if(ret == QDialog::Accepted) {
        Nepomuk::Resource publication = lpd.selectedPublication();

        m_document.setProperty(Nepomuk::Vocabulary::NBIB::publishedAs(), publication);
        publication.addProperty(Nepomuk::Vocabulary::NBIB::isPublicationOf(), m_document);
        setResource(m_document);
        emit resourceCacheNeedsUpdate(m_document);
        emit resourceCacheNeedsUpdate(publication);
    }
}

void DocumentWidget::removePublication()
{
    Nepomuk::Resource publication = m_document.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();

    m_document.removeProperty(Nepomuk::Vocabulary::NBIB::publishedAs());
    publication.removeProperty(Nepomuk::Vocabulary::NBIB::isPublicationOf(), m_document);

    //update
    setResource(m_document);
    emit resourceCacheNeedsUpdate(m_document);
    emit resourceCacheNeedsUpdate(publication);
}

