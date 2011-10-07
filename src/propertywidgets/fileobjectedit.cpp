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

#include "fileobjectedit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Variant>
#include <Nepomuk/File>
#include <KFileDialog>
#include <KGlobalSettings>

#include <QDebug>

FileObjectEdit::FileObjectEdit(QWidget *parent) :
    PropertyEdit(parent)
{
}

void FileObjectEdit::setMode(Mode mode)
{
    m_mode = mode;

    if(m_mode == Local) {
        setDirectEdit(false);
        connect(this, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showFileSelection()));
    }
}

void FileObjectEdit::setupLabel()
{
    QList<Nepomuk::Resource> dataObjectList = resource().property(propertyUrl()).toResourceList();

    Nepomuk::Resource dataObject;

    foreach( Nepomuk::Resource nr, dataObjectList) {
        if(m_mode == Local && nr.hasType(Nepomuk::Vocabulary::NFO::FileDataObject())) {
            dataObject = nr;
            break;
        }
        if(m_mode == Remote && nr.hasType(Nepomuk::Vocabulary::NFO::RemoteDataObject())) {
            dataObject = nr;
            break;
        }
    }

    QString title;
    if(dataObject.isValid()) {
        title = dataObject.property(Nepomuk::Vocabulary::NIE::url()).toString();

        addPropertryEntry(title, dataObject.uri());
    }

    setLabelText(title);
}

void FileObjectEdit::updateResource(const QString & text)
{
    // we start by going through the list of already available nfo:***DataObject resources
    QList<Nepomuk::Resource> dataObjectList = resource().property(propertyUrl()).toResourceList();

    Nepomuk::Resource dataObject;

    foreach( Nepomuk::Resource nr, dataObjectList) {
        if(m_mode == Local && nr.hasType(Nepomuk::Vocabulary::NFO::FileDataObject())) {
            dataObject = nr;
            break;
        }
        if(m_mode == Remote && nr.hasType(Nepomuk::Vocabulary::NFO::RemoteDataObject())) {
            dataObject = nr;
            break;
        }
    }

    // now dataObject holds either the old Resource we need to change
    // or is not valid in which case we need to create a new resource

    // if it is valid, just change the nie:url
    if(dataObject.isValid()) {
        dataObject.setProperty(Nepomuk::Vocabulary::NIE::url(), text);
    }
    // otherwise create a new resource
    else {

        if(text.isEmpty())
            return;

        Nepomuk::Resource newDataObject;
        // first check if the resource exists somewhere already
        if(m_mode == Local) {
            KUrl url(text);
            Nepomuk::File nf(url);
            if(nf.isValid()) {
                qDebug() << nf;
                newDataObject = nf;
            }
        }
        else if(m_mode == Remote) {
            qWarning() << "fileobjectedit :: find a way to find existing nfo:RemoteDataObject resources";
        }

        // we couldn't find an existing resource with the url from "text" so we create a new one
        if(!newDataObject.isValid()) {

            // create list of all types from the hierarchie we want to add
            QList<QUrl> typeList;
            if(m_mode == Remote)
                typeList << Nepomuk::Vocabulary::NFO::RemoteDataObject();
            else
                typeList << Nepomuk::Vocabulary::NFO::FileDataObject();

            newDataObject.setTypes(typeList);
            newDataObject.setProperty(Nepomuk::Vocabulary::NIE::url(), text);
        }

        // now add this resource to the edit fileds resource
        resource().addProperty( propertyUrl(), newDataObject);
        //and the backreference
        //TODO breaks when propertyUrl() is not nbib::isPublicationOf
        newDataObject.setProperty(Nepomuk::Vocabulary::NBIB::publishedAs(), resource());
    }
}

void FileObjectEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    // for the local object it might make sense to add auto completion
    // but for the moment it will be omitted
}

void FileObjectEdit::showFileSelection()
{

    //select name and path of the project
    QString fileNameFromDialog = KFileDialog::getOpenFileName(KGlobalSettings::documentPath());

    if(fileNameFromDialog.isEmpty()) {
        return;
    }

    updateResource(fileNameFromDialog);
}
