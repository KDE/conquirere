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
#include "../mainui/resourcemodel.h"
#include "../globals.h"
#include "../mainui/projecttreewidget.h"

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
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/PIMO>

#include <KDE/KIO/DeleteJob>

const QString DOCPATH = QLatin1String("documents");
const QString NOTEPATH = QLatin1String("notes");

Project::Project(QObject *parent) :
    QObject(parent)
{
    // create global nepomuk pimo:thing for this program
    // in case it does not exist yet
    Nepomuk::Resource project(QLatin1String("Conquirere Project"), Nepomuk::Vocabulary::PIMO::Thing());
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
    // when a new project is created it is realized as pimo:Project
    QString identifier = QLatin1String("Conquirere Project:") + m_name;

    m_pimoProject = Nepomuk::Resource(identifier, Nepomuk::Vocabulary::PIMO::Project());

    m_pimoProject.setProperty( Nepomuk::Vocabulary::NIE::title() , m_name);

    // relate each project to the conquiere pimo:thing
    // this allows us to find all existing projects
    Nepomuk::Resource cp(QLatin1String("Conquirere Project"));

    m_pimoProject.setProperty( Nepomuk::Vocabulary::PIMO::isRelated() , cp);

    // scan the project folder and add all documents to this project
    initializeProjectFolder();
}

void Project::loadProject(const QString & projectFile)
{
    m_settings = new QSettings(projectFile,QSettings::IniFormat);
    m_settings->beginGroup(QLatin1String("Conquirere"));
    m_path = m_settings->value(QLatin1String("path")).toString();
    m_name = m_settings->value(QLatin1String("name")).toString();
    m_pimoProject = Nepomuk::Resource(m_settings->value(QLatin1String("pimoProject")).toString());
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

Nepomuk::Resource Project::pimoProject() const
{
    return m_pimoProject;
}


bool Project::isInPath(const QString &filename)
{
    return filename.contains(m_path);
}

void Project::addDocument(const QFileInfo &fileInfo)
{
    // first check if the file is in the project path
    QString projectDocPath = m_path + QLatin1String("/") + DOCPATH;
    if(fileInfo.absolutePath() != projectDocPath) {
        qDebug() << "file" << fileInfo.fileName() << "not in project path ask to copy it?";
    }

    //now if the files do not already have the project tag, add it
    Nepomuk::Resource res( fileInfo.absoluteFilePath() );

    Nepomuk::Resource relatesTo = res.property( Nepomuk::Vocabulary::PIMO::isRelated()).toResource();

    if ( relatesTo == m_pimoProject) {
        qDebug() << "document " <<  fileInfo.fileName() << "is alread related to ::" << m_pimoProject.genericLabel();
    }
    else {
        res.setProperty( Nepomuk::Vocabulary::PIMO::isRelated() , m_pimoProject);
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

QAbstractTableModel* Project::viewModel(ResourceSelection selection)
{
    return m_resources.value(selection);
}

void Project::connectFetchIndicator(ProjectTreeWidget *treeWidget)
{
    foreach (QAbstractTableModel *model, m_resources) {
        switch(m_resources.key(model)) {
        case Resource_Document:
        case Resource_Mail:
        case Resource_Media:
        case Resource_Reference:
        case Resource_Publication:
        case Resource_Website:
        case Resource_Note:
        {
            ResourceModel *m = qobject_cast<ResourceModel *>(model);
            connect(m, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
                    treeWidget, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));

            m->startFetchData();
        }
            break;
        }
    }
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
    m_settings->setValue(QLatin1String("name"), name());
    m_settings->setValue(QLatin1String("path"), m_path);
    m_settings->setValue(QLatin1String("pimoProject"), m_pimoProject.uri());
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

void Project::setupModels()
{
    ResourceModel *DocumentModel = new ResourceModel;
    DocumentModel->setProject(this);
    DocumentModel->setResourceType(Resource_Document);

    m_resources.insert(Resource_Document, DocumentModel);

    //    ResourceModel *MailModel = new ResourceModel;
    //    MailModel->setProject(this);
    //    MailModel->setResourceType(Resource_Mail);
    //    connect(MailModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
    //            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    //    MailModel->startFetchData();

    //    m_projectMediaModel = new ResourceModel;
    //    m_projectMediaModel->setProject(this);
    //    m_projectMediaModel->setResourceType(Resource_Media);
    //    connect(m_projectMediaModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
    //            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    //    m_projectMediaModel->startFetchData();

    //    m_projectWebsiteModel = new ResourceModel;
    //    m_projectWebsiteModel->setProject(this);
    //    m_projectWebsiteModel->setResourceType(Resource_Website);
    //    connect(m_projectWebsiteModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
    //            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    //    m_projectWebsiteModel->startFetchData();

    //    m_projectReferencesModel = new ResourceModel;
    //    m_projectReferencesModel->setProject(this);
    //    m_projectReferencesModel->setResourceType(Resource_Reference);
    //    connect(m_projectReferencesModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
    //            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    //    m_projectReferencesModel->startFetchData();

    //    m_projectPublicationModel = new ResourceModel;
    //    m_projectPublicationModel->setProject(this);
    //    m_projectPublicationModel->setResourceType(Resource_Publication);
    //    connect(m_projectPublicationModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
    //            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    //    m_projectPublicationModel->startFetchData();

    //    m_projectNoteModel = new ResourceModel;
    //    m_projectNoteModel->setProject(this);
    //    m_projectNoteModel->setResourceType(Resource_Note);
    //    connect(m_projectNoteModel, SIGNAL(updatefetchDataFor(LibraryType,ResourceSelection,bool)),
    //            m_projectTree, SLOT(fetchDataFor(LibraryType,ResourceSelection,bool)));
    //    m_projectNoteModel->startFetchData();
}
