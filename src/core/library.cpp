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
#include "projectsettings.h"
#include "tagcloud.h"
#include "dirwatcher.h"

#include "onlinestorage/storageinfo.h"
#include "onlinestorage/storageglobals.h"
#include "onlinestorage/zotero/zoteroinfo.h"
#include "onlinestorage/kbibtexfile/kbtfileinfo.h"

#include "models/nepomukmodel.h"
#include "models/referencemodel.h"
#include "models/publicationmodel.h"
#include "models/publicationfiltermodel.h"
#include "models/documentmodel.h"
#include "models/notemodel.h"
#include "models/bookmarkmodel.h"
#include "models/mailmodel.h"
#include "models/seriesmodel.h"
#include "models/seriesfiltermodel.h"
#include "models/eventmodel.h"

#include <Nepomuk/Variant>
#include <Nepomuk/Resource>
#include <Nepomuk/Tag>
#include <Nepomuk/File>
#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/PIMO>

#include <KDE/KUrl>
#include <KDE/KStandardDirs>
#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KIO/CopyJob>
#include <KDE/KIO/DeleteJob>

#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtCore/QFileInfoList>

#include <QtCore/QDebug>

const QString DOCPATH = I18N_NOOP2("Name of the documents folder to store user library documents","documents");  /**< @todo make this configurable */
const QString NOTEPATH = I18N_NOOP2("Name of the notes folder to store user library documents","notes");     /**< @todo make this configurable */

Library::Library()
    : QObject(0)
    , m_libraryType(Library_Project)
    , m_projectSettings(new ProjectSettings())
    , m_dirWatcher(0)
    , m_tagCloud(0)
{
}

Library::~Library()
{
    foreach(QSortFilterProxyModel *atm, m_resources) {
        delete atm->sourceModel();
        delete atm;
    }

    m_resources.clear();
    delete m_tagCloud;
    delete m_dirWatcher;
    delete m_projectSettings;
}

Nepomuk::Thing Library::createLibrary(const QString & name, const QString & description, const QString & path)
{
    // when a new library is created it is realized as pimo:Project
    QString identifier = QLatin1String("Conquirere Library:") + name;
    Nepomuk::Thing projectThing = Nepomuk::Thing(identifier, Nepomuk::Vocabulary::PIMO::Project());
    projectThing.setProperty( Nepomuk::Vocabulary::NIE::title() , name);
    projectThing.setProperty( Soprano::Vocabulary::NAO::description() , description);
    projectThing.setProperty( Soprano::Vocabulary::NAO::identifier() , identifier);

    // relate each library to the conquiere pimo:thing
    // this allows us to find all existing projects
    //TODO instead of reading resource uri from file use the NAO::identifier
    KSharedConfigPtr config = KSharedConfig::openConfig("conquirererc");
    KConfigGroup generalGroup = config->group("General");
    QString NepomukCollection = generalGroup.readEntry( "NepomukCollection", QString() );

    Nepomuk::Resource conquiereCollections = Nepomuk::Resource(NepomukCollection);
    conquiereCollections.addProperty( Nepomuk::Vocabulary::PIMO::isRelated() , projectThing);

    // create a tag with the project name
    // this way we can relate publications/documents etc via PIMO::isRelated or the tag
    Nepomuk::Tag libraryTag = Nepomuk::Tag( name );
    libraryTag.setLabel(name);

    // check if a library path is set
    // we create the path if not available then
    QString inipath;
    if(!path.isEmpty()) {
        QDir project;
        project.mkpath(path);
        project.setPath(path);
        project.mkdir(DOCPATH);
        project.mkdir(NOTEPATH);
        inipath = path;
    }
    else {
        inipath = KStandardDirs::locateLocal("appdata", QLatin1String("projects"));
    }

    // create the project .ini file
    QString iniFile = inipath + QLatin1String("/") + name + QLatin1String(".ini");

    ProjectSettings *projectSettings = new ProjectSettings;
    projectSettings->setSettingsFile(iniFile);
    projectSettings->setName(name);
    projectSettings->setDescription(description);
    projectSettings->setProjectDir(path);
    projectSettings->setPimoThing(projectThing);

    // connect the nepomuk data for the settingsfile to the pimo:.project
    Nepomuk::File settingsFile = Nepomuk::File(KUrl(iniFile));

    //DEBUG the next 2 steps are necessary beacuse nepomuk did not fetch the newly created ini file at this point
    QString settingsFileName = QLatin1String("file://") + iniFile;
    settingsFile.setProperty(Nepomuk::Vocabulary::NIE::url(), settingsFileName);

    projectThing.addGroundingOccurrence(settingsFile);

    return projectThing;
}

void Library::loadLibrary(const QString & projectFile, LibraryType type)
{
    Q_ASSERT_X( !m_tagCloud, "loadLibrary", "tagCloud exist already ... means we loaded this library beforehand already");

    m_libraryType = type;
    m_projectSettings->loadSettings(projectFile);

    m_dirWatcher = new DirWatcher();
    m_dirWatcher->setLibrary(this);

    connect(m_projectSettings, SIGNAL(projectDirChanged(QString)), m_dirWatcher, SLOT(changeListenDir(QString)));

    m_tagCloud = new TagCloud;
    m_initialImportFinished = 0;
    m_tagCloud->pauseUpdates(true);

    setupModels();
}

void Library::loadSystemLibrary()
{
    QString inipath = KStandardDirs::locateLocal("appdata", QLatin1String("system"));
    QString iniFile = inipath + QLatin1String("/system_library.ini");

    QFileInfo fi(iniFile);
    if(!fi.exists()) {
        kDebug() << "System ini file not found. Create a default one";
        m_projectSettings->setSettingsFile(iniFile);
        m_projectSettings->setName(i18nc("name of the system library","System Library"));
    }

    loadLibrary(iniFile, Library_System);
}

void Library::loadLibrary(const Nepomuk::Thing & pimoProject)
{
    QList<Nepomuk::Resource> settingsFiles = pimoProject.groundingOccurrences();

    Nepomuk::File iniFile;
    //if(settingsFiles.size() > 1) {
        // This happens if we move the the folder and keep the old settingfiles intact
        // usually the old file is removed and should be deleted from nepomuk
        // otherwise we should filter modification date and take the newest
    //}
    iniFile = settingsFiles.first();

    loadLibrary(iniFile.url().pathOrUrl());
}

ProjectSettings * Library::settings()
{
    return m_projectSettings;
}

LibraryType Library::libraryType() const
{
    return m_libraryType;
}

void Library::deleteLibrary()
{
    // remove the library from the conquiere pimo:thing
    KSharedConfigPtr config = KSharedConfig::openConfig("conquirererc");
    KConfigGroup generalGroup = config->group("General");
    QString NepomukCollection = generalGroup.readEntry( "NepomukCollection", QString() );
    Nepomuk::Resource conquiereCollections = Nepomuk::Resource(NepomukCollection);
    conquiereCollections.removeProperty( Nepomuk::Vocabulary::PIMO::isRelated() , m_projectSettings->projectThing());

    QList<Nepomuk::Resource> gos = m_projectSettings->projectThing().groundingOccurrences();

    // delete all groundOccurences, in our case this should be only the .ini files
    foreach(Nepomuk::Resource r, gos) {
        Nepomuk::File iniFile = r;
        KIO::DeleteJob *dj = KIO::del(iniFile.url(), KIO::HideProgressInfo);
        dj->exec();
        delete dj;
        r.remove();
    }

    // remove nepomuk resources for it
    m_projectSettings->projectThing().remove();
    m_projectSettings->projectTag().remove();
}

void Library::addResource(Nepomuk::Resource & res)
{
    if(m_libraryType == Library_System) {
        qWarning() << "can't add resources to system library";
        return;
    }

    Nepomuk::Resource relatesTo = res.property( Nepomuk::Vocabulary::PIMO::isRelated()).toResource();

    if ( relatesTo != m_projectSettings->projectThing()) {
        res.addProperty( Nepomuk::Vocabulary::PIMO::isRelated() , m_projectSettings->projectThing());
    }
}

void Library::removeResource(Nepomuk::Resource & res)
{
    if(m_libraryType == Library_System) {
        qWarning() << "can't remove resources from system library";
        return;
    }

    res.removeProperty( Nepomuk::Vocabulary::PIMO::isRelated() , m_projectSettings->projectThing());
}

void Library::updateCacheData()
{
    foreach(QSortFilterProxyModel *atm, m_resources) {
        NepomukModel *nm = qobject_cast<NepomukModel *>(atm->sourceModel());
        nm->updateCacheData();
    }
}

QSortFilterProxyModel* Library::viewModel(ResourceSelection selection)
{
    return m_resources.value(selection);
}

QMap<ResourceSelection, QSortFilterProxyModel*> Library::viewModels()
{
    return m_resources;
}

TagCloud *Library::tagCloud()
{
    return m_tagCloud;
}

void Library::finishedInitialImport()
{
    m_initialImportFinished++;

    if(m_initialImportFinished == m_resources.size()) {
        m_tagCloud->pauseUpdates(false);
    }
}

void Library::setupModels()
{
    DocumentModel *documentModel = new DocumentModel;
    documentModel->setLibrary(this);
    QSortFilterProxyModel *documentFilter = new QSortFilterProxyModel;
    documentFilter->setSourceModel(documentModel);
    m_resources.insert(Resource_Document, documentFilter);
    connectModelToTagCloud(documentModel);

    BookmarkModel *bookmarkModel = new BookmarkModel;
    bookmarkModel->setLibrary(this);
    QSortFilterProxyModel *bookmarkFilter = new QSortFilterProxyModel;
    bookmarkFilter->setSourceModel(bookmarkModel);
    m_resources.insert(Resource_Website, bookmarkFilter);
    connectModelToTagCloud(bookmarkModel);

    ReferenceModel *referencesModel = new ReferenceModel;
    referencesModel->setLibrary(this);
    PublicationFilterModel *referenceFilter = new PublicationFilterModel;
    referenceFilter->setSourceModel(referencesModel);
    m_resources.insert(Resource_Reference, referenceFilter);
    connectModelToTagCloud(referencesModel);

    PublicationModel *publicationModel = new PublicationModel;
    publicationModel->setLibrary(this);
    PublicationFilterModel *publicationFilter = new PublicationFilterModel;
    publicationFilter->setSourceModel(publicationModel);
    m_resources.insert(Resource_Publication, publicationFilter);
    connectModelToTagCloud(publicationModel);

    SeriesModel *seriesModel = new SeriesModel;
    seriesModel->setLibrary(this);
    SeriesFilterModel *seriesFilter = new SeriesFilterModel;
    seriesFilter->setSourceModel(seriesModel);
    m_resources.insert(Resource_Series, seriesFilter);
    connectModelToTagCloud(seriesModel);

    NoteModel *noteModel = new NoteModel;
    noteModel->setLibrary(this);
    QSortFilterProxyModel *noteFilter = new QSortFilterProxyModel;
    noteFilter->setSourceModel(noteModel);
    m_resources.insert(Resource_Note, noteFilter);
    connectModelToTagCloud(noteModel);

    EventModel *eventModel = new EventModel;
    eventModel->setLibrary(this);
    QSortFilterProxyModel *eventFilter = new QSortFilterProxyModel;
    eventFilter->setSourceModel(eventModel);
    m_resources.insert(Resource_Event, eventFilter);
    connectModelToTagCloud(eventModel);

    if(m_libraryType == Library_Project) {
        MailModel *mailModel = new MailModel;
        mailModel->setLibrary(this);
        QSortFilterProxyModel *mailFilter = new QSortFilterProxyModel;
        mailFilter->setSourceModel(mailModel);
        m_resources.insert(Resource_Mail, mailFilter);
        connectModelToTagCloud(mailModel);

        /*
        ResourceModel *MediaModel = new ResourceModel;
        MediaModel->setLibrary(this);
        m_resources.insert(Resource_Media, MediaModel);
        connectModelToTagCloud(MediaModel);
        */
    }
}

void Library::connectModelToTagCloud(NepomukModel *model)
{
    connect(model, SIGNAL(resourceAdded(Nepomuk::Resource)), m_tagCloud, SLOT(addResource(Nepomuk::Resource)));
    connect(model, SIGNAL(resourceRemoved(QUrl)), m_tagCloud, SLOT(removeResource(QUrl)));
    connect(model, SIGNAL(resourceUpdated(Nepomuk::Resource)), m_tagCloud, SLOT(updateResource(Nepomuk::Resource)));
    connect(model, SIGNAL(queryFinished()), this, SLOT(finishedInitialImport()));
}
