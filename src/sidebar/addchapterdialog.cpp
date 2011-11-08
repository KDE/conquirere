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

#include "addchapterdialog.h"
#include "ui_addchapterdialog.h"

#include "propertywidgets/contactedit.h"
#include "contactdialog.h"

#include "nbib.h"
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>

AddChapterDialog::AddChapterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddChapterDialog)
{
    ui->setupUi(this);

    ui->chapterAuthor->setPropertyUrl( Nepomuk::Vocabulary::NCO::creator() );
    ui->chapterAuthor->setUseDetailDialog(true);
    connect(ui->chapterAuthor, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk::Resource&,QUrl)));
}

AddChapterDialog::~AddChapterDialog()
{
    delete ui;
}

void AddChapterDialog::setResource(Nepomuk::Resource resource)
{
    m_resource = resource;

    //fill the fields
    QString chapterNumer = m_resource.property(Nepomuk::Vocabulary::NBIB::chapterNumber()).toString();
    ui->chapterNumber->setText(chapterNumer);

    QString chapterTitle = m_resource.property(Nepomuk::Vocabulary::NIE::title()).toString();
    ui->chapterTitle->setText(chapterTitle);

    ui->chapterAuthor->setResource(resource);

    QString pageStart = m_resource.property(Nepomuk::Vocabulary::NBIB::pageStart()).toString();
    ui->pageStart->setValue(pageStart.toInt());

    QString pageEnd = m_resource.property(Nepomuk::Vocabulary::NBIB::pageEnd()).toString();
    ui->pageEnd->setValue(pageEnd.toInt());
}

void AddChapterDialog::setPublication(Nepomuk::Resource resource)
{
    m_publication = resource;
}

Nepomuk::Resource AddChapterDialog::resource()
{
    return m_resource;
}

void AddChapterDialog::accept()
{

    if(!m_resource.isValid()) {
        m_resource = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Chapter());
    }

    //if we accept, save all values into the resource
    QString chapterNumer = ui->chapterNumber->text();
    if(!chapterNumer.isEmpty()) {
        m_resource.setProperty(Nepomuk::Vocabulary::NBIB::chapterNumber(), chapterNumer);
    }

    QString chapterTitle = ui->chapterTitle->text();
    if(!chapterTitle.isEmpty()) {
        m_resource.setProperty(Nepomuk::Vocabulary::NIE::title(), chapterTitle);
    }

    //chapterAuthor is handled by the property edit widget itself.
    QString pageStart = QString::number(ui->pageStart->value());
    if(!pageStart.isEmpty()) {
        m_resource.setProperty(Nepomuk::Vocabulary::NBIB::pageStart(), pageStart);
    }

    QString pageEnd = QString::number(ui->pageEnd->value());
    if(!pageEnd.isEmpty()) {
        m_resource.setProperty(Nepomuk::Vocabulary::NBIB::pageEnd(), pageEnd);
    }

    // connect to publication
    if(m_publication.isValid() && m_resource.isValid()) {
        qDebug() << "add " << chapterTitle << ":: uri" << m_resource.uri();
        m_resource.setProperty(Nepomuk::Vocabulary::NBIB::documentPartOf(), m_publication);
        m_publication.addProperty(Nepomuk::Vocabulary::NBIB::documentPart(), m_resource);
    }

    QDialog::accept();
}

void AddChapterDialog::editContactDialog(Nepomuk::Resource & resource, const QUrl & propertyUrl)
{
    ContactDialog cd;
    cd.setResource(resource, propertyUrl);

    cd.exec();

    ui->chapterAuthor->setResource(resource);
}
