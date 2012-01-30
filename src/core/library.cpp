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

#include "nbib.h"
#include "sync.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/PIMO>

#include <KDE/KUrl>
#include <KDE/KStandardDirs>
#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KIO/CopyJob>
#include <KDE/KIO/DeleteJob>
#include <KDE/KDebug>

#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtCore/QFileInfoList>

const QString DOCPATH = I18N_NOOP2("Name of the documents folder to store user library documents","documents");  /**< @todo make this configurable */
const QString NOTEPATH = I18N_NOOP2("Name of the notes folder to store user library documents","notes");     /**< @todo make this configurable */

Library::Library()
    : QObject(0)
    , m_libraryType(Library_Project)
    , m_dirWatcher(0)
    , m_tagCloud(0)
    , m_initialImportFinished(false)
{
    m_projectSettings = new ProjectSettings(this);
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
    Nepomuk::Thing projectThing = Nepomuk::Thing( identifier );
    projectThing.addType( Nepomuk::Vocabulary::PIMO::Project() );
    projectThing.setProperty( Nepomuk::Vocabulary::NIE::title() , name);
    projectThing.setProperty( Soprano::Vocabulary::NAO::description() , description);
    projectThing.setProperty( Soprano::Vocabulary::NAO::identifier() , identifier);

    // create a tag with the project name
    // this way we can relate publications/documents etc via PIMO::isRelated or the tag
    Nepomuk::Tag libraryTag = Nepomuk::Tag( name );
    libraryTag.setLabel(name);

    // check if a library path is set
    // we create the path if not available then
    if(!path.isEmpty()) {
        QDir project;
        project.mkpath(path);
        project.setPath(path);
        project.mkdir(DOCPATH);
        project.mkdir(NOTEPATH);
    }

    createIniFile(projectThing, path);

    return projectThing;
}

QString Library::createIniFile(Nepomuk::Thing & pimoProject, const QString & path)
{
    kDebug() << "create ini file for " << pimoProject;

    // create the project .ini file
    QString inipath = KStandardDirs::locateLocal("appdata", QLatin1String("projects"));
    QString iniFile;

    // ensure the inifile name is not already available (unlikely with uuid, but not impossible
    QFileInfo fi;
    do {
        iniFile = inipath + QLatin1String("/") + QUuid::createUuid().toString() + QLatin1String(".ini");
        fi.setFile(iniFile);
    }
    while(fi.exists());

    ProjectSettings *projectSettings = new ProjectSettings(0);
    projectSettings->setSettingsFile(iniFile);
    projectSettings->setProjectDir(path);
    projectSettings->setPimoThing(pimoProject);
    projectSettings->setName( pimoProject.property(Nepomuk::Vocabulary::NIE::title()).toString() );
    projectSettings->setDescription( pimoProject.property(Soprano::Vocabulary::NAO::description()).toString() );

    // connect the nepomuk data for the settingsfile to the pimo::project
    Nepomuk::File settingsFile = Nepomuk::File(KUrl(iniFile));

    //DEBUG the next 2 steps are necessary beacuse nepomuk did not fetch the newly created ini file at this point
    QString settingsFileName = QLatin1String("file://") + iniFile;
    settingsFile.setProperty(Nepomuk::Vocabulary::NIE::url(), settingsFileName);

    pimoProject.addGroundingOccurrence(settingsFile);

    return iniFile;
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

void Library::loadLibrary(Nepomuk::Thing & pimoProject)
{
    QList<Nepomuk::Resource> settingsFiles = pimoProject.groundingOccurrences();

    Nepomuk::File iniFile;
    foreach( const Nepomuk::Resource &r, settingsFiles) {
        iniFile = r;
        //DEBUG we save inifiles in StandardDirs::locateLocal("appdata", QLatin1String("projects")); so
        if(iniFile.url().pathOrUrl().contains(QLatin1String("conquirere")))
            break;
    }

    if(iniFile.exists()) {
        loadLibrary(iniFile.url().pathOrUrl());
    }
    else {
        QString newIniFile = createIniFile( pimoProject );
        loadLibrary( newIniFile );
    }
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
    conquiereCollections.removeProperty( Soprano::Vocabulary::NAO::isRelated() , m_projectSettings->projectThing());

    QList<Nepomuk::Resource> gos = m_projectSettings->projectThing().groundingOccurrences();

    // delete all groundingOccurences, in our case this should be only the .ini files
    foreach(Nepomuk::Resource r, gos) {
        Nepomuk::File iniFile = r;
        KIO::DeleteJob *dj = KIO::del(iniFile.url(), KIO::HideProgressInfo);
        dj->exec();
        delete dj;
        r.remove();
    }

    // remove connection from the overall pimo collection to the deleted library
    conquiereCollections.removeProperty(Soprano::Vocabulary::NAO::isRelated(), m_projectSettings->projectThing());

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

    Nepomuk::Resource relatesTo = res.property( Soprano::Vocabulary::NAO::isRelated()).toResource();

    if ( relatesTo != m_projectSettings->projectThing()) {
        res.addProperty( Soprano::Vocabulary::NAO::isRelated() , m_projectSettings->projectThing());
    }
}

void Library::removeResource(Nepomuk::Resource & res)
{
    Q_ASSERT_X( m_libraryType == Library_Project, "removeResource", "can't remove resources from system library");

    res.removeProperty( Soprano::Vocabulary::NAO::isRelated() , m_projectSettings->projectThing());

    QList<Nepomuk::Resource> references = res.property(Nepomuk::Vocabulary::NBIB::reference()).toResourceList();
    foreach(Nepomuk::Resource r, references) {
        r.removeProperty( Soprano::Vocabulary::NAO::isRelated() , m_projectSettings->projectThing());
    }
}

void Library::deleteResource(Nepomuk::Resource & resource)
{
    if( resource.hasType(Nepomuk::Vocabulary::NBIB::Reference() ) ) {
        Nepomuk::Resource publication = resource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
        publication.removeProperty(Nepomuk::Vocabulary::NBIB::reference(), resource );

        emit resourceCacheNeedsUpdate(publication);
    }
    else if( resource.hasType(Nepomuk::Vocabulary::NBIB::Publication()) ){
        Nepomuk::Resource series = resource.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
        QList<Nepomuk::Resource> seriesPubilcations = series.property(Nepomuk::Vocabulary::NBIB::seriesOf()).toResourceList();
        if(seriesPubilcations.isEmpty()) {
            series.remove();
        }

        Nepomuk::Resource collection = resource.property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
        QList<Nepomuk::Resource> articles = collection.property(Nepomuk::Vocabulary::NBIB::article()).toResourceList();
        if(articles.isEmpty()) {
            collection.remove();
        }

        QList<Nepomuk::Resource> references = resource.property(Nepomuk::Vocabulary::NBIB::reference()).toResourceList();
        foreach(Nepomuk::Resource r, references) {
            r.remove();
        }

        QList<Nepomuk::Resource> documentParts = resource.property(Nepomuk::Vocabulary::NBIB::documentPart()).toResourceList();
        foreach(Nepomuk::Resource dp, documentParts) {
            dp.remove();
        }
    }

    // finally remove the resource from teh nepomuk storage
    resource.remove();
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
    DocumentModel *documentModel = new DocumentModel(this);
    documentModel->setLibrary(this);
    QSortFilterProxyModel *documentFilter = new QSortFilterProxyModel;
    documentFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    documentFilter->setSourceModel(documentModel);
    m_resources.insert(Resource_Document, documentFilter);
    connectModelToTagCloud(documentModel);

    BookmarkModel *bookmarkModel = new BookmarkModel;
    bookmarkModel->setLibrary(this);
    QSortFilterProxyModel *bookmarkFilter = new QSortFilterProxyModel;
    bookmarkFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    bookmarkFilter->setSourceModel(bookmarkModel);
    m_resources.insert(Resource_Website, bookmarkFilter);
    connectModelToTagCloud(bookmarkModel);

    ReferenceModel *referencesModel = new ReferenceModel;
    referencesModel->setLibrary(this);
    PublicationFilterModel *referenceFilter = new PublicationFilterModel;
    referenceFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    referenceFilter->setSourceModel(referencesModel);
    m_resources.insert(Resource_Reference, referenceFilter);
    connectModelToTagCloud(referencesModel);

    PublicationModel *publicationModel = new PublicationModel;
    publicationModel->setLibrary(this);
    PublicationFilterModel *publicationFilter = new PublicationFilterModel;
    publicationFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    publicationFilter->setSourceModel(publicationModel);
    m_resources.insert(Resource_Publication, publicationFilter);
    connectModelToTagCloud(publicationModel);

    SeriesModel *seriesModel = new SeriesModel;
    seriesModel->setLibrary(this);
    SeriesFilterModel *seriesFilter = new SeriesFilterModel;
    seriesFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    seriesFilter->setSourceModel(seriesModel);
    m_resources.insert(Resource_Series, seriesFilter);
    connectModelToTagCloud(seriesModel);

    NoteModel *noteModel = new NoteModel;
    noteModel->setLibrary(this);
    QSortFilterProxyModel *noteFilter = new QSortFilterProxyModel;
    noteFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    noteFilter->setSourceModel(noteModel);
    m_resources.insert(Resource_Note, noteFilter);
    connectModelToTagCloud(noteModel);

    EventModel *eventModel = new EventModel;
    eventModel->setLibrary(this);
    QSortFilterProxyModel *eventFilter = new QSortFilterProxyModel;
    eventFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    eventFilter->setSourceModel(eventModel);
    m_resources.insert(Resource_Event, eventFilter);
    connectModelToTagCloud(eventModel);

    if(m_libraryType == Library_Project) {
        MailModel *mailModel = new MailModel;
        mailModel->setLibrary(this);
        QSortFilterProxyModel *mailFilter = new QSortFilterProxyModel;
        mailFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
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
