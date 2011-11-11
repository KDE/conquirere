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

#include "adddataobject.h"
#include "ui_adddataobject.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/File>
#include <Nepomuk/Variant>

#include <KDE/KUrlRequester>

AddDataObject::AddDataObject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDataObject),
    m_kurlrequester(0)
{
    ui->setupUi(this);

    connect(ui->keditlistwidget, SIGNAL(added(QString)), this, SLOT(addItem(QString)));
    connect(ui->keditlistwidget, SIGNAL(removed(QString)), this, SLOT(removeItem(QString)));
}

AddDataObject::~AddDataObject()
{
    delete ui;
    delete m_kurlrequester;
}

void AddDataObject::setMode(FileObjectEdit::Mode mode)
{
    m_mode = mode;

    switch(m_mode) {
    case FileObjectEdit::Local: {
        ui->textlabel->setText(i18n("Select all files on the harddrive that represent this publication."));
        m_propertyUrl = Nepomuk::Vocabulary::NFO::FileDataObject();
        KEditListWidget::CustomEditor kurlrequester;
        m_kurlrequester = new KUrlRequester();
        kurlrequester.setLineEdit(m_kurlrequester->lineEdit());
        kurlrequester.setRepresentationWidget(m_kurlrequester);
        ui->keditlistwidget->setCustomEditor(kurlrequester);
    }
        break;
    case FileObjectEdit::Remote:
        ui->textlabel->setText(i18n("Add all remote locations that represent this publications."));
        m_propertyUrl = Nepomuk::Vocabulary::NFO::RemoteDataObject();
        break;
    case FileObjectEdit::Website:
        ui->textlabel->setText(i18n("Add all websites connected to this publication"));
        m_propertyUrl = Nepomuk::Vocabulary::NFO::Website();
        break;
    }
}

void AddDataObject::setResource(Nepomuk::Resource resource)
{
    m_resource = resource;
}

void AddDataObject::fillListWidget()
{
    QList<Nepomuk::Resource> dataObjectList = m_resource.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResourceList();

    foreach( const Nepomuk::Resource & nr, dataObjectList) {
        QString url = nr.property( Nepomuk::Vocabulary::NIE::url() ).toString();
        if(m_mode == FileObjectEdit::Local && nr.hasType(Nepomuk::Vocabulary::NFO::FileDataObject())) {
            ui->keditlistwidget->insertItem(url);
            continue;
        }
        if(m_mode == FileObjectEdit::Remote && nr.hasType(Nepomuk::Vocabulary::NFO::RemoteDataObject())) {
            ui->keditlistwidget->insertItem(url);
            continue;
        }
        if(m_mode == FileObjectEdit::Website && nr.hasType(Nepomuk::Vocabulary::NFO::Website())) {
            ui->keditlistwidget->insertItem(url);
            continue;
        }
    }
}

void AddDataObject::addItem(const QString & text)
{
    Nepomuk::Resource dataObject;

    if(m_mode == FileObjectEdit::Local) {
        // find the resource of the file
        KUrl url(text);
        Nepomuk::File nf(url);
        if(nf.isValid()) {
            dataObject = nf;
        }
        else {
            Nepomuk::Resource newFile(QUrl(), Nepomuk::Vocabulary::NFO::FileDataObject());
            dataObject = newFile;
            dataObject.setProperty(Nepomuk::Vocabulary::NIE::url(), text);
        }
    }
    else if(m_mode == FileObjectEdit::Remote) {
        qWarning() << "fileobjectedit :: find a way to find existing nfo:RemoteDataObject resources";
        Nepomuk::Resource newFile(QUrl(), Nepomuk::Vocabulary::NFO::RemoteDataObject());
        dataObject = newFile;
        dataObject.setProperty(Nepomuk::Vocabulary::NIE::url(), text);
    }
    //TODO change this to NFO::WebDataObject()
    else if(m_mode == FileObjectEdit::Website) {
        qWarning() << "fileobjectedit :: find a way to find existing nfo:WebDataObject resources";
        Nepomuk::Resource newFile(QUrl(), Nepomuk::Vocabulary::NFO::Website());
        dataObject = newFile;
        dataObject.setProperty(Nepomuk::Vocabulary::NIE::url(), text);
    }

    qDebug() << "add dataObject" << dataObject.uri() << dataObject.resourceUri();
    // connect new dataobject to resource
    m_resource.addProperty( Nepomuk::Vocabulary::NBIB::isPublicationOf(), dataObject);
    //and the backreference
    dataObject.setProperty(Nepomuk::Vocabulary::NBIB::publishedAs(), m_resource);
}

void AddDataObject::removeItem(const QString & text)
{
    QList<Nepomuk::Resource> dataObjectList = m_resource.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResourceList();

    foreach( const Nepomuk::Resource & nr, dataObjectList) {
        QString url = nr.property( Nepomuk::Vocabulary::NIE::url() ).toString();
        if( text == url) {
            m_resource.removeProperty( Nepomuk::Vocabulary::NBIB::isPublicationOf(), nr);
        }
        // if it is not a local nepomuk resource, delete the resource also
        if(m_mode != FileObjectEdit::Local) {
            //nr.remove();
        }
    }
}