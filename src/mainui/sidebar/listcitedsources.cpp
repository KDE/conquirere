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

#include "listcitedsources.h"
#include "ui_listcitedsources.h"

#include "mainui/librarymanager.h"
#include "listpublicationsdialog.h"
#include "referencewidget.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <KDE/KDialog>
#include <KDE/KIcon>
#include <KDE/KDebug>

#include <QtGui/QListWidgetItem>
#include <QtCore/QPointer>

ListCitedSources::ListCitedSources(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListCitedSources)
{
    ui->setupUi(this);

    ui->findMore->setVisible(false);

    ui->editPart->setIcon(KIcon("document-edit"));
    ui->addPart->setIcon(KIcon("list-add"));
    ui->removePart->setIcon(KIcon("list-remove"));

    connect(ui->editPart, SIGNAL(clicked()), this, SLOT(editReference()));
    connect(ui->findMore, SIGNAL(clicked()), this, SLOT(findeMoreReferences()));
    connect(ui->addPart, SIGNAL(clicked()), this, SLOT(addReference()));
    connect(ui->removePart, SIGNAL(clicked()), this, SLOT(removeReference()));
}

ListCitedSources::~ListCitedSources()
{
    delete ui;
}

void ListCitedSources::setLibraryManager(LibraryManager *lm)
{
    m_libraryManager = lm;
}

void ListCitedSources::setResource(Nepomuk::Resource resource)
{
    m_resource = resource;

    ui->listWidget->clear();

    // fill the widget with all pimo:notes
    QList<Nepomuk::Resource> resourceList = m_resource.property(Nepomuk::Vocabulary::NBIB::citedReference()).toResourceList();

    foreach(const Nepomuk::Resource & r, resourceList) {
        QListWidgetItem *i = new QListWidgetItem();
        Nepomuk::Resource publication = r.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();

        if(!publication.exists()) {
            kWarning() << "cited reference without publication!";
            continue;
        }

//        i->setIcon( KIcon(QLatin1String("view-pim-notes")) );
        QString citeKey = r.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
        QString title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
        QString showText = QLatin1String("[") + citeKey + QLatin1String("] ") + title;
        i->setText( showText );
        i->setData(Qt::UserRole, r.resourceUri());
        ui->listWidget->addItem(i);
    }

    if(ui->listWidget->count() == 0) {
        ui->editPart->setEnabled(false);
        ui->removePart->setEnabled(false);
    }
    else {
        ui->editPart->setEnabled(true);
        ui->removePart->setEnabled(true);
    }
}

void ListCitedSources::editReference()
{
    QListWidgetItem *i = ui->listWidget->currentItem();
    if(!i)  { return; }

    Nepomuk::Resource reference(i->data(Qt::UserRole).toString());

    // open dialog to edit the reference
    KDialog editReferenceWidget;

    ReferenceWidget *rw = new ReferenceWidget();
    rw->setResource( reference );
    rw->setLibraryManager(m_libraryManager);

    editReferenceWidget.setMainWidget(rw);
    editReferenceWidget.setInitialSize(QSize(400,300));

    editReferenceWidget.exec();

    Nepomuk::Resource publication = reference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
    if(!publication.exists()) {
        kWarning() << "cited reference without publication!";
    }

    QString citeKey = reference.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
    QString title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
    QString showText = QLatin1String("[") + citeKey + QLatin1String("] ") + title;
    i->setText( showText );

    emit resourceCacheNeedsUpdate(m_resource);
}

void ListCitedSources::findeMoreReferences()
{

}

void ListCitedSources::addReference()
{
    QPointer<ListPublicationsDialog> lpd = new ListPublicationsDialog(this);

    lpd->setListMode(Resource_Reference, Max_BibTypes);
    lpd->setLibraryManager(m_libraryManager);

    int ret = lpd->exec();

    if(ret == QDialog::Accepted) {
        Nepomuk::Resource selectedReference = lpd->selectedPublication();

        Nepomuk::Resource publication = selectedReference.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
        if(!publication.exists()) {
            kWarning() << "cited reference without publication!";
        }
        else {
            m_resource.addProperty(Nepomuk::Vocabulary::NBIB::citedReference(), selectedReference);

            QListWidgetItem *i = new QListWidgetItem();
//            i->setIcon( KIcon(QLatin1String("view-pim-notes")) );

            QString citeKey = selectedReference.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
            QString title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
            QString showText = QLatin1String("[") + citeKey + QLatin1String("] ") + title;
            i->setText( showText );

            i->setData(Qt::UserRole, selectedReference.resourceUri());
            ui->listWidget->addItem(i);
        }

    }

    if(ui->listWidget->count() == 0) {
        ui->editPart->setEnabled(false);
        ui->removePart->setEnabled(false);
    }
    else {
        ui->editPart->setEnabled(true);
        ui->removePart->setEnabled(true);
    }

    emit resourceCacheNeedsUpdate(m_resource);
}

void ListCitedSources::removeReference()
{
    QListWidgetItem *i = ui->listWidget->currentItem();
    if(!i) { return; }

    Nepomuk::Resource newReference(i->data(Qt::UserRole).toUrl());
    ui->listWidget->removeItemWidget(i);
    delete i;

    m_resource.removeProperty(Nepomuk::Vocabulary::NBIB::citedReference(), newReference);

    ui->listWidget->setCurrentRow(0);

    emit resourceCacheNeedsUpdate(m_resource);

    if(ui->listWidget->count() == 0) {
        ui->editPart->setEnabled(false);
        ui->removePart->setEnabled(false);
    }
    else {
        ui->editPart->setEnabled(true);
        ui->removePart->setEnabled(true);
        ui->listWidget->setCurrentRow(0);
    }
}
