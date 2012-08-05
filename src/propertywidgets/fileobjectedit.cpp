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

#include <Nepomuk2/DataManagement>

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NFO>
#include <Nepomuk2/Variant>

#include <KDE/KMimeType>
#include <KDE/KDebug>

#include <QtCore/QPointer>

using namespace Nepomuk2::Vocabulary;

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

    connect(ui->fileListWidget, SIGNAL(doubleClicked(QListWidgetItem*,QPoint)), this, SLOT(doubleClicked(QListWidgetItem*,QPoint)));
}

FileObjectEdit::~FileObjectEdit()
{
    delete ui;
}

void FileObjectEdit::setLibraryManager(LibraryManager *lm)
{
    m_libraryManager = lm;
}

void FileObjectEdit::setResource(Nepomuk2::Resource & resource)
{
    m_publication = resource;
    fillFileObjectWidget();
}

void FileObjectEdit::fillFileObjectWidget()
{
    ui->fileListWidget->clear();

    QList<Nepomuk2::Resource> resources = m_publication.property(NBIB::isPublicationOf()).toResourceList();
    resources.append( m_publication.property(NIE::links()).toResourceList() );

    foreach(const Nepomuk2::Resource &r, resources) {
        QListWidgetItem *i = new QListWidgetItem(ui->fileListWidget);

        addItemInfo(i, r);

        ui->fileListWidget->addItem(i);
    }
}

void FileObjectEdit::fileObjectEdit()
{
    QListWidgetItem *i = ui->fileListWidget->currentItem();
    if(!i) { return; }

    Nepomuk2::Resource resource = Nepomuk2::Resource::fromResourceUri(i->data(Qt::UserRole).toUrl());

    QPointer<FileObjectEditDialog> foed = new FileObjectEditDialog(this);

    foed->setLibraryManager(m_libraryManager);
    foed->setPublication(m_publication);
    foed->setResource(resource);

    foed->exec();

    Nepomuk2::Resource changedSource = foed->resource();

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
        QList<QUrl> publication; publication << m_publication.uri();
        QVariantList uris; uris << foed->resource().uri();
        Nepomuk2::removeProperty( publication, NIE::links(), uris );
        Nepomuk2::removeProperty( publication, NBIB::isPublicationOf(), uris );
        return;
    }

    QListWidgetItem *i = new QListWidgetItem(ui->fileListWidget);

    Nepomuk2::Resource changedSource = foed->resource();

    addItemInfo(i, changedSource);

    ui->fileListWidget->addItem(i);

    setResource(m_publication);

    delete foed;
}

void FileObjectEdit::fileObjectRemove()
{
    QListWidgetItem *i = ui->fileListWidget->currentItem();
    if(!i) { return; }

    Nepomuk2::Resource resource = Nepomuk2::Resource::fromResourceUri(i->data(Qt::UserRole).toUrl());
    ui->fileListWidget->removeItemWidget(i);
    delete i;

    if(resource.hasType(NFO::Website())) {
        QList<QUrl> resourceUris; resourceUris << m_publication.uri();
        QVariantList value; value <<  resource.uri();
        Nepomuk2::removeProperty(resourceUris, NIE::links(), value);
    }
    else {
        QList<QUrl> resourceUris; resourceUris << m_publication.uri();
        QVariantList value; value <<  resource.uri();
        Nepomuk2::removeProperty(resourceUris, NBIB::isPublicationOf(), value);

        resourceUris.clear(); resourceUris << resource.uri();
        value.clear(); value <<  m_publication.uri();
        Nepomuk2::removeProperty(resourceUris, NBIB::publishedAs(), value);
    }
}

void FileObjectEdit::doubleClicked(QListWidgetItem* item, QPoint point)
{
    Q_UNUSED(point);


    QListWidgetItem *i = ui->fileListWidget->currentItem();
    if(!i) { return; }

    Nepomuk2::Resource resource = Nepomuk2::Resource::fromResourceUri(i->data(Qt::UserRole).toUrl());

    emit openDocument(resource, true);
}

void FileObjectEdit::addItemInfo(QListWidgetItem *i, const Nepomuk2::Resource &resource)
{
    QString icon;
    KUrl url(resource.property(NIE::url()).toString());
    QString showString = url.prettyUrl(KUrl::RemoveTrailingSlash);


    if( resource.hasType(Nepomuk2::Vocabulary::NFO::Website()) // || resource.hasType(Nepomuk2::Vocabulary::NFO::WebDataObject())
        || url.scheme() == QLatin1String("http")) {
        icon = KMimeType::favIconForUrl(url);
        if(icon.isEmpty()) {
            icon = QLatin1String("text-html");
        }
    }
    else {
        KMimeType::Ptr mimeTypePtr = KMimeType::findByUrl(url);
        icon = mimeTypePtr->iconName();

        QString name = resource.property(NFO::fileName()).toString();
        if(!name.isEmpty()) {
            showString = name;
        }
        if(showString.isEmpty()) {
            showString = resource.property(NIE::title()).toString();
        }
    }

    i->setIcon( KIcon(icon) );
    i->setText(showString);
    i->setData(Qt::UserRole, resource.uri());
}
