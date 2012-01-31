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

#include "mainui/librarymanager.h"
#include "notewidget.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Variant>

#include <KDE/KDialog>
#include <KDE/KIcon>

#include <QtGui/QListWidgetItem>

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
}

ListAnnotationsWidget::~ListAnnotationsWidget()
{
    delete ui;
}

void ListAnnotationsWidget::setLibraryManager(LibraryManager *lm)
{
    m_libraryManager = lm;
}

void ListAnnotationsWidget::setResource(Nepomuk::Resource resource)
{
    m_resource = resource;

    ui->listWidget->clear();

    // fill the widget with all pimo:notes
    QList<Nepomuk::Resource> resourceList = m_resource.property(Soprano::Vocabulary::NAO::isRelated()).toResourceList();

    foreach(const Nepomuk::Resource & r, resourceList) {
        if( !r.hasType( Nepomuk::Vocabulary::PIMO::Note() ) ) { continue; }

        QListWidgetItem *i = new QListWidgetItem();

        i->setIcon( KIcon(QLatin1String("view-pim-notes")) );
        i->setText( r.property(Nepomuk::Vocabulary::NIE::title()).toString() );
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

void ListAnnotationsWidget::editAnnotation()
{
    QListWidgetItem *i = ui->listWidget->currentItem();
    if(!i)  { return; }

    Nepomuk::Resource note(i->data(Qt::UserRole).toString());

    // open dialog to edit the note
    KDialog addAnnotationWidget;

    NoteWidget *nw = new NoteWidget();
    nw->setResource( note );
    nw->setLibraryManager(m_libraryManager);

    addAnnotationWidget.setMainWidget(nw);
    addAnnotationWidget.setInitialSize(QSize(400,300));

    addAnnotationWidget.exec();

    i->setText(note.property( Nepomuk::Vocabulary::NIE::title()).toString() );
    emit resourceCacheNeedsUpdate(m_resource);
}

void ListAnnotationsWidget::addAnnotation()
{
    // open dialog to create a new note
    KDialog addAnnotationWidget;

    NoteWidget *nw = new NoteWidget();
    Nepomuk::Resource note = Nepomuk::Resource();
    note.addType(Nepomuk::Vocabulary::PIMO::Note());
    nw->setResource( note );
    nw->setLibraryManager(m_libraryManager);

    addAnnotationWidget.setMainWidget(nw);
    addAnnotationWidget.setInitialSize(QSize(400,300));

    int ret = addAnnotationWidget.exec();

    if(ret == KDialog::Accepted) {
        Nepomuk::Resource note = nw->resource();

        m_resource.addProperty(Soprano::Vocabulary::NAO::isRelated(), note);
        note.addProperty(Soprano::Vocabulary::NAO::isRelated(), m_resource);

        QListWidgetItem *i = new QListWidgetItem();

        i->setIcon( KIcon(QLatin1String("view-pim-notes")) );
        i->setText( note.property(Nepomuk::Vocabulary::NIE::title()).toString() );
        i->setData(Qt::UserRole, note.resourceUri());
        ui->listWidget->addItem(i);
    }
    else {
        note.remove();
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

void ListAnnotationsWidget::removeAnnotation()
{
    QListWidgetItem *i = ui->listWidget->currentItem();
    if(!i) { return; }

    Nepomuk::Resource note(i->data(Qt::UserRole).toUrl());
    ui->listWidget->removeItemWidget(i);
    delete i;

    m_resource.removeProperty(Soprano::Vocabulary::NAO::isRelated(), note);
    note.remove();

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
