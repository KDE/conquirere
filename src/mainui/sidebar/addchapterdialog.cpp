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

#include <Nepomuk2/DataManagement>
#include <KDE/KJob>
#include "sro/nbib/chapter.h"

#include "nbib.h"
#include <Nepomuk2/Variant>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NCO>

using namespace Nepomuk2::Vocabulary;

AddChapterDialog::AddChapterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddChapterDialog)
{
    ui->setupUi(this);

    ui->chapterAuthor->setPropertyUrl( NCO::creator() );
    ui->chapterAuthor->setUseDetailDialog(true);
    connect(ui->chapterAuthor, SIGNAL(externalEditRequested(Nepomuk2::Resource&,QUrl)), this, SLOT(editContactDialog(Nepomuk2::Resource&,QUrl)));
}

AddChapterDialog::~AddChapterDialog()
{
    delete ui;
}

void AddChapterDialog::setResource(Nepomuk2::Resource resource)
{
    m_resource = resource;

    //fill the fields
    QString chapterNumer = m_resource.property(NBIB::chapterNumber()).toString();
    ui->chapterNumber->setText(chapterNumer);

    QString chapterTitle = m_resource.property(NIE::title()).toString();
    ui->chapterTitle->setText(chapterTitle);

    ui->chapterAuthor->setResource(resource);

    QString pageStart = m_resource.property(NBIB::pageStart()).toString();
    ui->pageStart->setValue(pageStart.toInt());

    QString pageEnd = m_resource.property(NBIB::pageEnd()).toString();
    ui->pageEnd->setValue(pageEnd.toInt());
}

Nepomuk2::Resource AddChapterDialog::resource()
{
    return m_resource;
}

void AddChapterDialog::accept()
{
    if(!m_resource.exists()) {
        kWarning() << "try to return an invalid chapter resource";
    }

    QList<QUrl> resUri; resUri << m_resource.uri();
    QVariantList value; value << ui->chapterNumber->text();
    KJob *job = Nepomuk2::setProperty(resUri, NBIB::chapterNumber(), value);
    job->exec();// wait till changes are made .. to properly update listWidget afterwards again

    resUri.clear(); resUri << m_resource.uri();
    value.clear(); value << ui->chapterTitle->text();
    KJob *job2 = Nepomuk2::setProperty(resUri, NIE::title(), value);
    job2->exec();// wait till changes are made .. to properly update listWidget afterwards again

    resUri.clear(); resUri << m_resource.uri();
    value.clear(); value << ui->pageStart->text();
    Nepomuk2::setProperty(resUri, NBIB::pageStart(), value);

    resUri.clear(); resUri << m_resource.uri();
    value.clear(); value << ui->pageEnd->text();
    Nepomuk2::setProperty(resUri, NBIB::pageEnd(), value);

    QDialog::accept();
}

void AddChapterDialog::editContactDialog(Nepomuk2::Resource & resource, const QUrl & propertyUrl)
{
    ContactDialog cd;
    cd.setResource(resource, propertyUrl);
    cd.exec();

    ui->chapterAuthor->setResource(resource);
}
