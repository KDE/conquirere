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

#include "listpartswidget.h"
#include "ui_listpartswidget.h"

#include "addchapterdialog.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <QtGui/QListWidgetItem>

ListPartsWidget::ListPartsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListPartsWidget)
{
    ui->setupUi(this);

    connect(ui->editPart, SIGNAL(clicked()), this, SLOT(editPart()));
    connect(ui->addPart, SIGNAL(clicked()), this, SLOT(addPart()));
    connect(ui->removePart, SIGNAL(clicked()), this, SLOT(removePart()));
}

ListPartsWidget::~ListPartsWidget()
{
    delete ui;
}

void ListPartsWidget::setPublication(Nepomuk::Resource publication)
{
    m_publication = publication;

    ui->listWidget->clear();
    QList<Nepomuk::Resource> tocResources = m_publication.property(Nepomuk::Vocabulary::NBIB::documentPart()).toResourceList();

    foreach(const Nepomuk::Resource & r, tocResources) {
        QListWidgetItem *i = new QListWidgetItem();
        QString title = r.property(Nepomuk::Vocabulary::NIE::title()).toString();
        QString number = r.property(Nepomuk::Vocabulary::NBIB::chapterNumber()).toString();
        QString listEntry = QString ("%1 : %2").arg(number).arg(title);
        i->setText(listEntry);
        i->setData(Qt::UserRole, r.resourceUri());
        ui->listWidget->addItem(i);
    }
}

Nepomuk::Resource ListPartsWidget::selectedPart() const
{
    QListWidgetItem *i = ui->listWidget->currentItem();

    Nepomuk::Resource nepomukPart(i->data(Qt::UserRole).toString());

    return nepomukPart;
}

void ListPartsWidget::editPart()
{
    //get selected chapter
    QListWidgetItem *i = ui->listWidget->currentItem();

    Nepomuk::Resource tmpChapter(i->data(Qt::UserRole).toString());

    AddChapterDialog acd;
    acd.setResource(tmpChapter);

    acd.exec();

    //update chapter
    QString title = tmpChapter.property(Nepomuk::Vocabulary::NIE::title()).toString();
    QString number = tmpChapter.property(Nepomuk::Vocabulary::NBIB::chapterNumber()).toString();
    QString listEntry = QString ("%1 : %2").arg(number).arg(title);
    i->setText(listEntry);
}

void ListPartsWidget::addPart()
{
    AddChapterDialog acd;
    acd.setPublication(m_publication);

    int ret = acd.exec();
    Nepomuk::Resource chapter = acd.resource();

    if(ret == QDialog::Accepted) {
        QListWidgetItem *i = new QListWidgetItem();
        QString title = chapter.property(Nepomuk::Vocabulary::NIE::title()).toString();
        QString number = chapter.property(Nepomuk::Vocabulary::NBIB::chapterNumber()).toString();
        QString listEntry = QString ("%1 : %2").arg(number).arg(title);
        i->setText(listEntry);
        i->setData(Qt::UserRole, chapter.resourceUri());
        ui->listWidget->addItem(i);
    }
    else {
        chapter.remove();
    }
}

void ListPartsWidget::removePart()
{
    QListWidgetItem *i = ui->listWidget->currentItem();
    ui->listWidget->removeItemWidget(i);

    Nepomuk::Resource chapter(i->data(Qt::UserRole).toUrl());
    chapter.remove();
}

