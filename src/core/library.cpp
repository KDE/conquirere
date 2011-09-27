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

#include "library.h"
#include "../core/resourcemodel.h"
#include "../mainui/projecttreewidget.h"

#include <KDE/KUrl>
#include <QtCore/QDir>
#include <QtCore/QFileInfoList>

#include <QtCore/QDebug>
#include <QtCore/QSettings>

#include <Nepomuk/Variant>
#include <Nepomuk/Tag>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/PIMO>

#include <KDE/KIO/DeleteJob>

const QString DOCPATH = QLatin1String("documents");  /**< @todo make this configurable */
const QString NOTEPATH = QLatin1String("notes");     /**< @todo make this configurable */

Library::Library(LibraryType type)
    : QObject(0)
    , m_settings(0)
    , m_libraryType(type)
{
    setupModels();
}

Library::~Library()
{
    delete m_settings;

    foreach(QAbstractTableModel *atm, m_resources) {
        delete atm;
    }

    m_resources.clear();
}

LibraryType Library::libraryType() const
{
    return m_libraryType;
}

void Library::setName(const QString & name)
{
    if(m_libraryType == Library_System) {
        qWarning() << "can't set the name for the system library";
        return;
    }

    m_name = name;
}

QString Library::name() const
{
    if(m_libraryType == Library_System) {
        return QLatin1String("System Library");
    }
    else {
        return m_name;
    }
}

void Library::setPath(const QString & path)
{
    m_path = path;
}

QString Library::path() const
{
    return m_path;
}

void Library::createLibrary()
{
    if(m_libraryType == Library_System) {
        qWarning() << "can't create system library";
        return;
    }

    // when a new library is created it is realized as pimo:Project
    QString identifier = QLatin1String("Conquirere Library:") + m_name;

    m_pimoLibrary = Nepomuk::Resource(identifier, Nepomuk::Vocabulary::PIMO::Project());

    m_pimoLibrary.setProperty( Nepomuk::Vocabulary::NIE::title() , m_name);

    // relate each library to the conquiere pimo:thing
    // this allows us to find all existing projects
    Nepomuk::Resource cp(QLatin1String("Conquirere Library"));

    m_pimoLibrary.setProperty( Nepomuk::Vocabulary::PIMO::isRelated() , cp);

    m_libraryTag = Nepomuk::Tag( m_name );
    m_libraryTag.setLabel(m_name);

    QDir project;
    // first check if the folder exist and create it otherwise
    project.mkpath(m_path);
    project.setPath(m_path);

    // create the project .ini file
    m_settings = new QSettings(m_path + QLatin1String("/conquirere.ini"),QSettings::IniFormat);
    m_settings->beginGroup(QLatin1String("Conquirere"));
    m_settings->setValue(QLatin1String("name"), name());
    m_settings->setValue(QLatin1String("path"), m_path);
    m_settings->setValue(QLatin1String("pimoProject"), m_pimoLibrary.uri());
    m_settings->endGroup();
    m_settings->beginGroup(QLatin1String("Settings"));
    m_settings->setValue(QLatin1String("copytoprojectfolder"), true);
    m_settings->endGroup();
    m_settings->sync();

    // now check if the used structure exist or create it.
    project.mkdir(DOCPATH);

    //in the case files did exist, scan and add them
    scanLibraryFolders();
}

void Library::loadLibrary(const QString & projectFile)
{
    m_settings = new QSettings(projectFile,QSettings::IniFormat);
    m_settings->beginGroup(QLatin1String("Conquirere"));
    m_path = m_settings->value(QLatin1String("path")).toString();
    m_name = m_settings->value(QLatin1String("name")).toString();
    m_pimoLibrary = Nepomuk::Resource(m_settings->value(QLatin1String("pimoProject")).toString());
    m_settings->endGroup();

    // add new files in the folders
    scanLibraryFolders();

    m_libraryType = Library_Project;

    m_libraryTag = Nepomuk::Tag( m_name );
    m_libraryTag.setLabel(m_name);
}

void Library::deleteLibrary()
{
    m_pimoLibrary.remove();
    m_libraryTag.remove();

    KUrl path(m_path);

    KIO::del(m_path);
}

Nepomuk::Resource Library::pimoLibrary() const
{
    return m_pimoLibrary;
}

bool Library::isInPath(const QString &filename)
{
    return filename.contains(m_path);
}

void Library::addResource(Nepomuk::Resource & res)
{
    if(m_libraryType == Library_System) {
        qWarning() << "can't add resources to system library";
        return;
    }

    Nepomuk::Resource relatesTo = res.property( Nepomuk::Vocabulary::PIMO::isRelated()).toResource();

    if ( relatesTo == m_pimoLibrary) {
        qDebug() << "resource " <<  res.genericLabel() << "is alread related to ::" << m_pimoLibrary.genericLabel();
    }
    else {
        res.setProperty( Nepomuk::Vocabulary::PIMO::isRelated() , m_pimoLibrary);
    }
}

void Library::addDocument(const QFileInfo &fileInfo)
{
    if(m_libraryType == Library_System) {
        qWarning() << "can't add documents to system library";
        return;
    }

    // first check if the file is in the project path
    QString projectDocPath = m_path + QLatin1String("/") + DOCPATH;
    if(fileInfo.absolutePath() != projectDocPath) {
        qDebug() << "file" << fileInfo.fileName() << "not in project path ask to copy it?";
    }

    //now if the files do not already have the project tag, add it
    Nepomuk::Resource res( fileInfo.absoluteFilePath() );

    addResource(res);
}

QAbstractTableModel* Library::viewModel(ResourceSelection selection)
{
    return m_resources.value(selection);
}

void Library::connectFetchIndicator(ProjectTreeWidget *treeWidget)
{
    //TODO crete different models for each ResourceSelection
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
            connect(m, SIGNAL(updatefetchDataFor(ResourceSelection,bool, Library *)),
                    treeWidget, SLOT(fetchDataFor(ResourceSelection,bool, Library *)));

            m->startFetchData();
        }
            break;
        }
    }
}

void Library::scanLibraryFolders()
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

void Library::setupModels()
{
    ResourceModel *DocumentModel = new ResourceModel;
    DocumentModel->setLibrary(this);
    DocumentModel->setResourceType(Resource_Document);
    m_resources.insert(Resource_Document, DocumentModel);

    ResourceModel *WebsiteModel = new ResourceModel;
    WebsiteModel->setLibrary(this);
    WebsiteModel->setResourceType(Resource_Website);
    m_resources.insert(Resource_Website, WebsiteModel);

    ResourceModel *ReferencesModel = new ResourceModel;
    ReferencesModel->setLibrary(this);
    ReferencesModel->setResourceType(Resource_Reference);
    m_resources.insert(Resource_Reference, ReferencesModel);

    ResourceModel *PublicationModel = new ResourceModel;
    PublicationModel->setLibrary(this);
    PublicationModel->setResourceType(Resource_Publication);
    m_resources.insert(Resource_Publication, PublicationModel);

    ResourceModel *NoteModel = new ResourceModel;
    NoteModel->setLibrary(this);
    NoteModel->setResourceType(Resource_Note);
    m_resources.insert(Resource_Note, NoteModel);

    if(m_libraryType == Library_Project) {
        ResourceModel *MailModel = new ResourceModel;
        MailModel->setLibrary(this);
        MailModel->setResourceType(Resource_Mail);
        m_resources.insert(Resource_Mail, MailModel);

        ResourceModel *MediaModel = new ResourceModel;
        MediaModel->setLibrary(this);
        MediaModel->setResourceType(Resource_Media);
        m_resources.insert(Resource_Media, MediaModel);
    }
}

