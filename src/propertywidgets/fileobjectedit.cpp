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

#include "fileobjectedit.h"
#include "propertywidgets/ui_fileobjectedit.h"

#include "fileobjecteditdialog.h"

#include "dms-copy/datamanagement.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Variant>

#include <KDE/KDebug>

#include <QtCore/QPointer>

using namespace Nepomuk::Vocabulary;

FileObjectEdit::FileObjectEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileObjectEdit)
{
    ui->setupUi(this);

    ui->fileEdit->setIcon(KIcon("document-edit"));
    ui->fileAdd->setIcon(KIcon("list-add"));
    ui->fileRemove->setIcon(KIcon("list-remove"));

    connect(ui->fileEdit, SIGNAL(clicked()), this, SLOT(fileObjectEdit()));
    connect(ui->fileAdd, SIGNAL(clicked()), this, SLOT(fileObjectAdd()));
    connect(ui->fileRemove, SIGNAL(clicked()), this, SLOT(fileObjectRemove()));
}

FileObjectEdit::~FileObjectEdit()
{
    delete ui;
}

void FileObjectEdit::setLibraryManager(LibraryManager *lm)
{
    m_libraryManager = lm;
}

void FileObjectEdit::setResource(Nepomuk::Resource & resource)
{
    m_publication = resource;
    fillFileObjectWidget();
}

void FileObjectEdit::fillFileObjectWidget()
{
    ui->fileListWidget->clear();

    QList<Nepomuk::Resource> resources = m_publication.property(NBIB::isPublicationOf()).toResourceList();
    resources.append( m_publication.property(NIE::links()).toResourceList() );

    foreach(const Nepomuk::Resource &r, resources) {
        QListWidgetItem *i = new QListWidgetItem(ui->fileListWidget);

        addItemInfo(i, r);

        ui->fileListWidget->addItem(i);
    }
}

void FileObjectEdit::fileObjectEdit()
{
    QListWidgetItem *i = ui->fileListWidget->currentItem();
    if(!i) { return; }

    Nepomuk::Resource resource = Nepomuk::Resource::fromResourceUri(i->data(Qt::UserRole).toUrl());

    QPointer<FileObjectEditDialog> foed = new FileObjectEditDialog(this);

    foed->setLibraryManager(m_libraryManager);
    foed->setPublication(m_publication);
    foed->setResource(resource);

    foed->exec();

    Nepomuk::Resource changedSource = foed->resource();

    addItemInfo(i, changedSource);

    delete foed;
}

void FileObjectEdit::fileObjectAdd()
{
    QPointer<FileObjectEditDialog> foed = new FileObjectEditDialog(this);

    foed->setLibraryManager(m_libraryManager);
    foed->setPublication(m_publication);
    foed->createNewResource();

    int ret = foed->exec();

    if(ret == QDialog::Rejected) {
        // remove crosslink, but do not delete the resource
        QList<QUrl> publication; publication << m_publication.resourceUri();
        QVariantList uris; uris << foed->resource().resourceUri();
        Nepomuk::removeProperty( publication, NIE::links(), uris );
        Nepomuk::removeProperty( publication, NBIB::isPublicationOf(), uris );
        return;
    }

    QListWidgetItem *i = new QListWidgetItem(ui->fileListWidget);

    Nepomuk::Resource changedSource = foed->resource();

    addItemInfo(i, changedSource);

    ui->fileListWidget->addItem(i);

    setResource(m_publication);

    delete foed;
}

void FileObjectEdit::fileObjectRemove()
{
    QListWidgetItem *i = ui->fileListWidget->currentItem();
    if(!i) { return; }

    Nepomuk::Resource resource = Nepomuk::Resource::fromResourceUri(i->data(Qt::UserRole).toUrl());
    ui->fileListWidget->removeItemWidget(i);
    delete i;

    if(resource.hasType(NFO::Website())) {
        QList<QUrl> resourceUris; resourceUris << m_publication.uri();
        QVariantList value; value <<  resource.uri();
        Nepomuk::removeProperty(resourceUris, NIE::links(), value);
    }
    else {
        QList<QUrl> resourceUris; resourceUris << m_publication.uri();
        QVariantList value; value <<  resource.uri();
        Nepomuk::removeProperty(resourceUris, NBIB::isPublicationOf(), value);

        resourceUris.clear(); resourceUris << resource.uri();
        value.clear(); value <<  m_publication.uri();
        Nepomuk::removeProperty(resourceUris, NBIB::publishedAs(), value);
    }
}

void FileObjectEdit::addItemInfo(QListWidgetItem *i, const Nepomuk::Resource &resource)
{
    QString icon;
    KUrl url(resource.property(NIE::url()).toString());
    QString showString = url.prettyUrl(KUrl::RemoveTrailingSlash);
    if(resource.hasType(NFO::RemoteDataObject())) {
        icon = QLatin1String("application-x-smb-server");
    }
    else if(resource.hasType(NFO::WebDataObject()) || resource.hasType(NFO::Website())) {
        icon = QLatin1String("applications-internet");
    }
    else {
        QString name = resource.property(NFO::fileName()).toString();
        if(!name.isEmpty()) {
            showString = name;
        }
        if(showString.isEmpty()) {
            showString = resource.property(NIE::title()).toString();
        }
        icon = QLatin1String("application-pdf"); //TODO create right icon from mimetype as retrieved by nepomuk
    }

    i->setIcon( KIcon(icon) );
    i->setText(showString);
    i->setData(Qt::UserRole, resource.resourceUri());
}
