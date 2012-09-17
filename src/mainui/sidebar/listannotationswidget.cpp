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

#include "listannotationswidget.h"
#include "ui_listannotationswidget.h"

#include "core/library.h"
#include "core/librarymanager.h"
#include "notewidget.h"

#include <Nepomuk2/DataManagement>
#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Variant>

#include <KDE/KDialog>
#include <KDE/KIcon>

#include <QtGui/QListWidgetItem>
#include <QtCore/QPointer>

using namespace Soprano::Vocabulary;
using namespace Nepomuk2::Vocabulary;

ListAnnotationsWidget::ListAnnotationsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ListAnnotationsWidget)
{
    ui->setupUi(this);

    ui->editPart->setIcon(KIcon("document-edit"));
    ui->addPart->setIcon(KIcon("list-add"));
    ui->removePart->setIcon(KIcon("list-remove"));

    connect(ui->editPart, SIGNAL(clicked()), this, SLOT(editAnnotation()));
    connect(ui->addPart, SIGNAL(clicked()), this, SLOT(addAnnotation()));
    connect(ui->removePart, SIGNAL(clicked()), this, SLOT(removeAnnotation()));

    connect(ui->listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
}

ListAnnotationsWidget::~ListAnnotationsWidget()
{
    delete ui;
}

void ListAnnotationsWidget::setLibraryManager(LibraryManager *lm)
{
    m_libraryManager = lm;
}

void ListAnnotationsWidget::setResource(Nepomuk2::Resource resource)
{
    m_resource = resource;

    ui->listWidget->clear();

    // fill the widget with all pimo:notes
    QList<Nepomuk2::Resource> resourceList = m_resource.property(NAO::isRelated()).toResourceList();

    foreach(const Nepomuk2::Resource & r, resourceList) {
        if( !r.hasType( PIMO::Note() ) ) { continue; }

        QListWidgetItem *i = new QListWidgetItem();

        i->setIcon( KIcon(QLatin1String("view-pim-notes")) );
        QString title = r.property(NAO::prefLabel()).toString();
        if(title.isEmpty()) {
            title = r.property(NIE::title()).toString();
        }
        i->setText( title );
        i->setData(Qt::UserRole, r.uri());
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

void ListAnnotationsWidget::editAnnotation()
{
    QListWidgetItem *i = ui->listWidget->currentItem();
    if(!i)  { return; }

    Nepomuk2::Resource note = Nepomuk2::Resource::fromResourceUri(i->data(Qt::UserRole).toString());

    // open dialog to edit the note
    QPointer<KDialog> addAnnotationWidget = new KDialog(this);

    NoteWidget *nw = new NoteWidget(addAnnotationWidget);
    nw->setResource( note );
    nw->setLibraryManager(m_libraryManager);

    addAnnotationWidget->setMainWidget(nw);
    addAnnotationWidget->setInitialSize(QSize(400,300));

    addAnnotationWidget->exec();

    delete addAnnotationWidget;

    QString title = note.property(NAO::prefLabel()).toString();
    if(title.isEmpty()) {
        title = note.property(NIE::title()).toString();
    }

    i->setText( title );
}

void ListAnnotationsWidget::addAnnotation()
{
    // open dialog to create a new note
    QPointer<KDialog> addAnnotationWidget = new KDialog(this);

    NoteWidget *nw = new NoteWidget(addAnnotationWidget);

    nw->setLibraryManager(m_libraryManager);
    nw->newButtonClicked();

    addAnnotationWidget->setMainWidget(nw);
    addAnnotationWidget->setInitialSize(QSize(400,300));

    int ret = addAnnotationWidget->exec();

    Nepomuk2::Resource note = nw->resource();
    if(ret == KDialog::Accepted) {
        // create the nao relations and declare it a subresource
        QList<QUrl> resUri; resUri << m_resource.uri();
        QVariantList value; value << note.uri();
        Nepomuk2::addProperty(resUri, NAO::isRelated(), value);

        resUri.clear(); resUri << note.uri();
        value.clear(); value << m_resource.uri();
        Nepomuk2::addProperty(resUri, NAO::isRelated(), value);

        resUri.clear(); resUri << m_resource.uri();
        value.clear(); value << note.uri();
        Nepomuk2::addProperty(resUri, NAO::hasSubResource(), value);

        QListWidgetItem *i = new QListWidgetItem();

        i->setIcon( KIcon(QLatin1String("view-pim-notes")) );
        QString title = note.property(NAO::prefLabel()).toString();
        if(title.isEmpty()) {
            title = note.property(NIE::title()).toString();
        }
        i->setText( title );
        i->setData(Qt::UserRole, note.uri());
        ui->listWidget->addItem(i);
    }
    else {
        m_libraryManager->systemLibrary()->deleteResource( note );
    }

    if(ui->listWidget->count() == 0) {
        ui->editPart->setEnabled(false);
        ui->removePart->setEnabled(false);
    }
    else {
        ui->editPart->setEnabled(true);
        ui->removePart->setEnabled(true);
    }

    delete addAnnotationWidget;
}

void ListAnnotationsWidget::removeAnnotation()
{
    QListWidgetItem *i = ui->listWidget->currentItem();
    if(!i) { return; }

    Nepomuk2::Resource note = Nepomuk2::Resource::fromResourceUri(i->data(Qt::UserRole).toUrl());
    m_libraryManager->systemLibrary()->deleteResource( note );

    ui->listWidget->removeItemWidget(i);
    delete i;

    ui->listWidget->setCurrentRow(0);

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

void ListAnnotationsWidget::selectionChanged()
{
    QListWidgetItem *i = ui->listWidget->currentItem();
    if(!i) { return; }

    Nepomuk2::Resource note = Nepomuk2::Resource::fromResourceUri(i->data(Qt::UserRole).toUrl());

    emit selectedAnnotation(note);
}
