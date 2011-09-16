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

#include "project.h"

#include <KDE/KUrl>
#include <QtCore/QDir>
#include <QtCore/QFileInfoList>

#include <QtCore/QDebug>
#include <QtCore/QSettings>

#include <Nepomuk/ResourceManager>
#include <Nepomuk/File>
#include <Nepomuk/Variant>
#include <Nepomuk/Types/Class>
#include <Nepomuk/Tag>
#include <Nepomuk/Types/Property>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/PIMO>

#include <KDE/KIO/DeleteJob>

const QString DOCPATH = QLatin1String("documents");
const QString NOTEPATH = QLatin1String("notes");

Project::Project(QObject *parent) :
    QObject(parent)
{
}

Project::~Project()
{
    delete m_settings;
}

void Project::setName(const QString & name)
{
    m_name = name;
    m_projectTag = Nepomuk::Tag( m_name );
    m_projectTag.setLabel(m_name);
}

QString Project::name() const
{
    return m_name;
}

void Project::setPath(const QString & path)
{
    m_path = path;
}

QString Project::path() const
{
    return m_path;
}

void Project::createProject()
{
    initializeProjectFolder();
}

void Project::loadProject(const QString & projectFile)
{
    m_settings = new QSettings(projectFile,QSettings::IniFormat);
    m_settings->beginGroup(QLatin1String("Conquirere"));
    m_path = m_settings->value(QLatin1String("path")).toString();
    m_projectTag = Nepomuk::Tag(m_settings->value(QLatin1String("tag")).toString());
    m_settings->endGroup();

    // add new files in the folders
    scanProjectFolders();
}

void Project::deleteProject()
{
    m_projectTag.remove();

    KUrl path(m_path);

    KIO::del(m_path);
}

Nepomuk::Tag Project::projectTag() const
{
    return m_projectTag;
}

void Project::addDocument(const QFileInfo &fileInfo)
{
    // first check if the file is in the project path
    QString projectDocPath = m_path + DOCPATH;
    if(fileInfo.absolutePath() != projectDocPath) {
        qDebug() << "file" << fileInfo.fileName() << "not in project path ask to copy it?";
    }

    //now if the files do not already have the project tag, add it
    Nepomuk::Resource res( fileInfo.absoluteFilePath() );

    if( !res.tags().contains(m_projectTag) ) {
        res.addTag( m_projectTag );
    }
}

void Project::document()
{

}

QList<Nepomuk::File> Project::getProjectFiles()
{
    QList<Nepomuk::File> list2;

    foreach(Nepomuk::Resource res, m_projectTag.tagOf()) {
        if( res.isFile() ) {
            Nepomuk::File f = res.toFile();
            list2.append(f);
        }
    }

    return list2;
}

void Project::scanProjectFolders()
{
    QDir project;
    project.setPath(m_path);

    // scan for new documents
    project.cd(DOCPATH);

    QFileInfoList list = project.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.isFile()) {
            addDocument(fileInfo.absoluteFilePath());
        }
    }
}

void Project::initializeProjectFolder()
{
    QDir project;
    // first check if the folder exist and create it otherwise
    project.mkpath(m_path);

    project.setPath(m_path);

    // create the project .ini file
    m_settings = new QSettings(m_path + QLatin1String("/conquirere.ini"),QSettings::IniFormat);
    m_settings->beginGroup(QLatin1String("Conquirere"));
    m_settings->setValue(QLatin1String("path"), m_path);
    m_settings->setValue(QLatin1String("tag"), m_projectTag.uri());
    m_settings->endGroup();
    m_settings->beginGroup(QLatin1String("Settings"));
    m_settings->setValue(QLatin1String("copytoprojectfolder"), true);
    m_settings->endGroup();
    m_settings->sync();

    // now check if the used structure exist or create it.
    project.mkdir(DOCPATH);

    //in the case files did exist, scan and add them
    scanProjectFolders();
}
