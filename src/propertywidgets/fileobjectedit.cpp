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

#include "adddataobject.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Variant>
#include <Nepomuk/File>

#include <KDE/KFileDialog>
#include <KDE/KGlobalSettings>

#include <QtCore/QDebug>

FileObjectEdit::FileObjectEdit(QWidget *parent)
    : PropertyEdit(parent)
    , m_mode(Local)
{
    setDirectEdit(false);
    setUseDetailDialog(true);

    connect(this, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showFileSelection()));
}

void FileObjectEdit::setMode(Mode mode)
{
    m_mode = mode;
}

void FileObjectEdit::setupLabel()
{
    QList<Nepomuk::Resource> dataObjectList = resource().property(propertyUrl()).toResourceList();

    QString dataStringList;
    foreach(const Nepomuk::Resource & nr, dataObjectList) {
        QString url = nr.property(Nepomuk::Vocabulary::NIE::url()).toString();

        if(m_mode == Local && nr.type() == Nepomuk::Vocabulary::NFO::FileDataObject().toString()) { //nr.hasType(Nepomuk::Vocabulary::NFO::FileDataObject())) {
            dataStringList.append(url);
            dataStringList.append(QLatin1String("; "));
            continue;
        }
        else if(m_mode == Remote && nr.type() == Nepomuk::Vocabulary::NFO::RemoteDataObject().toString()) { //nr.hasType(Nepomuk::Vocabulary::NFO::RemoteDataObject())) {
            dataStringList.append(url);
            dataStringList.append(QLatin1String("; "));
            continue;
        }
        else if(m_mode == Website && nr.type() == Nepomuk::Vocabulary::NFO::Website().toString()) { //nr.hasType(Nepomuk::Vocabulary::NFO::Website())) {
            dataStringList.append(url);
            dataStringList.append(QLatin1String("; "));
            continue;
        }
    }

    dataStringList.chop(2);

    setLabelText(dataStringList);
}

void FileObjectEdit::updateResource(const QString & text)
{
    // is done externally via AddDataObject
}

QStandardItemModel* FileObjectEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    // for the local object it might make sense to add auto completion
    // but for the moment it will be omitted

    return 0;
}

void FileObjectEdit::showFileSelection()
{
    AddDataObject ado;

    ado.setMode(m_mode);
    ado.setResource(resource());
    ado.fillListWidget();

    ado.exec();

    setupLabel();
    //resourceUpdatedExternally();
}
