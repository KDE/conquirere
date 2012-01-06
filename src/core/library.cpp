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
#include "tagcloud.h"
#include "dirwatcher.h"
#include "backgroundsync.h"

#include "onlinestorage/storageinfo.h"
#include "onlinestorage/storageglobals.h"
#include "onlinestorage/zotero/zoteroinfo.h"
#include "onlinestorage/kbibtexfile/kbtfileinfo.h"

#include "nbibio/nbibsync.h"
#include "nbibio/synczoteronepomuk.h"

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
#include "../dms-copy/simpleresource.h"
#include "../dms-copy/simpleresourcegraph.h"

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

Library::Library(LibraryType type)
    : QObject(0)
    , m_libraryType(type)
    , m_dirWatcher(0)
    , m_backgroundSync(0)
    , m_tagCloud(0)
{
    m_tagCloud = new TagCloud;
    m_initialImportFinished = 0;
    m_tagCloud->pauseUpdates(true);
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

void Library::setDescription(const QString & description)
{
    m_description = description;
}

QString Library::description() const
{
    return m_description;
}

void Library::setLibraryDir(const QString & path)
{
    if(m_libraryDir == path)
        return;

    if(path.isEmpty()) {
        // move the inifile to user config place and rename the librarypath on its way
        QString oldIniFile = m_libraryDir + QLatin1String("/") + m_name + QLatin1String(".ini");
        QString inipath = KStandardDirs::locateLocal("appdata", QLatin1String("projects"));
        QString newIniFile = inipath + QLatin1String("/") + m_name + QLatin1String(".ini");

        KConfig libconfig( oldIniFile, KConfig::SimpleConfig );
        KConfigGroup libGroup( &libconfig, "Conquirere" );
        libGroup.deleteEntry(QLatin1String("libraryDir"));
        libconfig.sync();

        KIO::CopyJob *cj = KIO::move(oldIniFile, newIniFile);
        cj->exec();
        delete cj;

        //update nepomuk data for the settingsfile to the pimo:.project
        Nepomuk::File settingsFile = Nepomuk::File(KUrl(newIniFile));
        settingsFile.setProperty(Nepomuk::Vocabulary::PIMO::occurrence(), m_pimoLibrary);
        m_pimoLibrary.setProperty(Nepomuk::Vocabulary::PIMO::groundingOccurrence(), settingsFile );

        // do not remove old folder
    }
    // create new fodler structer or move existing one
    else {
        if(m_libraryDir.isEmpty()) {
            QString inipath = KStandardDirs::locateLocal("appdata", QLatin1String("projects"));
            QString oldIniFile = inipath + QLatin1String("/") + m_name + QLatin1String(".ini");
            QString newIniFile = path + QLatin1String("/") + m_name + QLatin1String(".ini");

            KConfig libconfig( oldIniFile, KConfig::SimpleConfig );
            KConfigGroup libGroup( &libconfig, "Conquirere" );
            libGroup.writeEntry(QLatin1String("libraryDir"), path);
            libconfig.sync();

            // now create folder and move init file to it
            QDir project;
            project.mkpath(path);
            project.setPath(path);
            project.mkdir(DOCPATH);
            project.mkdir(NOTEPATH);

            KIO::CopyJob *cj = KIO::move(oldIniFile, newIniFile);
            cj->exec();
            delete cj;

            //update nepomuk data for the settingsfile to the pimo:.project
            Nepomuk::File settingsFile = Nepomuk::File(KUrl(newIniFile));
            settingsFile.setProperty(Nepomuk::Vocabulary::PIMO::occurrence(), m_pimoLibrary);
            m_pimoLibrary.setProperty(Nepomuk::Vocabulary::PIMO::groundingOccurrence(), settingsFile );
        }
        // there was a previous folder, so we move it to the new destination
        else {
            KIO::CopyJob *cj = KIO::move(m_libraryDir, path);
            cj->exec();
            delete cj;

            // and update the path in the inifile
            QString newIniFile = path + QLatin1String("/") + m_name + QLatin1String(".ini");
            KConfig libconfig( newIniFile, KConfig::SimpleConfig );
            KConfigGroup libGroup( &libconfig, "Conquirere" );
            libGroup.writeEntry(QLatin1String("libraryDir"), path);
            libconfig.sync();

            //update nepomuk data for the settingsfile to the pimo:.project
            Nepomuk::File settingsFile = Nepomuk::File(KUrl(newIniFile));
            settingsFile.setProperty(Nepomuk::Vocabulary::PIMO::occurrence(), m_pimoLibrary);
            m_pimoLibrary.setProperty(Nepomuk::Vocabulary::PIMO::groundingOccurrence(), settingsFile );
        }
    }

    // aaaaand set the library variable to the new path
    m_libraryDir = path;

    delete m_dirWatcher;
    m_dirWatcher = 0;
    if(!m_libraryDir.isEmpty()) {
        m_dirWatcher = new DirWatcher();
        m_dirWatcher->setLibrary(this);
    }
}

QString Library::libraryDir() const
{
    return m_libraryDir;
}

QString Library::libraryDocumentDir() const
{
    QString documentDir;
    documentDir = m_libraryDir + QLatin1String("/") + DOCPATH;

    return documentDir;
}

void Library::addSyncProvider(NBibSync* provider)
{
    QString providerID = provider->uniqueProviderID();
    if(providerID.isEmpty()) {
        providerID = QUuid::createUuid().toString();
        provider->setUniqueProviderID(providerID);
    }

    m_backgroundSync->addSyncProvider(provider);

    // save sync details in the inifile
    QString inipath;
    if(!m_libraryDir.isEmpty()) {
        inipath = m_libraryDir;
    }
    else {
        inipath = KStandardDirs::locateLocal("appdata", QLatin1String("projects"));
    }

    ProviderSyncDetails psd = provider->providerSyncDetails();

    QString iniFile = inipath + QLatin1String("/") + m_name + QLatin1String(".ini");
    KConfig libconfig( iniFile, KConfig::SimpleConfig );
    KConfigGroup libGroup( &libconfig, "SyncProvider" );
    KConfigGroup providerGroup( &libGroup, providerID );
    providerGroup.writeEntry(QLatin1String("provider"), psd.providerInfo->providerId());
    providerGroup.writeEntry(QLatin1String("name"), psd.userName);
    providerGroup.writeEntry(QLatin1String("url"), psd.url);
    providerGroup.writeEntry(QLatin1String("collection"), psd.collection);
    providerGroup.writeEntry(QLatin1String("askBeforeDeletion"), psd.askBeforeDeletion);
    providerGroup.writeEntry(QLatin1String("mergeStrategy"), (int)psd.mergeMode);
    providerGroup.writeEntry(QLatin1String("syncMode"), (int)psd.syncMode);
    providerGroup.writeEntry(QLatin1String("importAttachments"), psd.importAttachments);
    providerGroup.writeEntry(QLatin1String("exportAttachments"), psd.exportAttachments);
    providerGroup.writeEntry(QLatin1String("akonadiContactsUUid"), psd.akonadiContactsUUid);
    providerGroup.writeEntry(QLatin1String("akonadiEventsUUid"), psd.akonadiEventsUUid);

    providerGroup.sync();
    libGroup.sync();
}

void Library::removeSyncProvider(NBibSync* provider)
{
    QString inipath;
    if(!m_libraryDir.isEmpty()) {
        inipath = m_libraryDir;
    }
    else {
        inipath = KStandardDirs::locateLocal("appdata", QLatin1String("projects"));
    }

    QString iniFile = inipath + QLatin1String("/") + m_name + QLatin1String(".ini");
    KConfig libconfig( iniFile, KConfig::SimpleConfig );
    KConfigGroup libGroup( &libconfig, "SyncProvider" );
    KConfigGroup providerGroup( &libGroup, provider->uniqueProviderID() );
    providerGroup.deleteGroup();
    libGroup.sync();

    m_backgroundSync->removeSyncProvider(provider);
}

BackgroundSync *Library::backgroundSync() const
{
    return m_backgroundSync;
}

void Library::createLibrary(const QString & name, const QString & description, const QString & path)
{
    // when a new library is created it is realized as pimo:Project
    QString identifier = QLatin1String("Conquirere Library:") + name;
    Nepomuk::Resource pimoLibrary = Nepomuk::Resource(identifier, Nepomuk::Vocabulary::PIMO::Project());
    pimoLibrary.setProperty( Nepomuk::Vocabulary::NIE::title() , name);
    pimoLibrary.setProperty( Soprano::Vocabulary::NAO::description() , description);

    // relate each library to the conquiere pimo:thing
    // this allows us to find all existing projects
    KSharedConfigPtr config = KSharedConfig::openConfig("conquirererc");
    KConfigGroup generalGroup = config->group("General");
    QString NepomukCollection = generalGroup.readEntry( "NepomukCollection", QString() );

    Nepomuk::Resource conquiereCollections = Nepomuk::Resource(NepomukCollection);
    conquiereCollections.addProperty( Nepomuk::Vocabulary::PIMO::isRelated() , pimoLibrary);
    pimoLibrary.addProperty( Nepomuk::Vocabulary::PIMO::isRelated() , conquiereCollections);

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

    KConfig libconfig( iniFile, KConfig::SimpleConfig );
    KConfigGroup libGroup( &libconfig, "Conquirere" );
    libGroup.writeEntry(QLatin1String("name"), name);
    libGroup.writeEntry(QLatin1String("description"), description);
    libGroup.writeEntry(QLatin1String("pimoProject"), pimoLibrary.resourceUri().toString());
    libGroup.writeEntry(QLatin1String("libraryDir"), path);
    libconfig.sync();

    // connect the nepomuk data for the settingsfile to the pimo:.project
    Nepomuk::File settingsFile = Nepomuk::File(KUrl(iniFile));
    settingsFile.setProperty(Nepomuk::Vocabulary::PIMO::occurrence(), pimoLibrary);
    pimoLibrary.setProperty(Nepomuk::Vocabulary::PIMO::groundingOccurrence(), settingsFile );

    // now everything is created, test the settings by loading the .ini file and all parts with it again
    loadLibrary(iniFile);
}

void Library::loadLibrary(const QString & projectFile)
{
    KConfig libconfig( projectFile, KConfig::SimpleConfig );
    KConfigGroup libGroup( &libconfig, "Conquirere" );
    m_description = libGroup.readEntry(QLatin1String("description"), QString());
    m_name = libGroup.readEntry(QLatin1String("name"), QString());
    m_libraryDir = libGroup.readEntry(QLatin1String("libraryDir"), QString());

    m_pimoLibrary = Nepomuk::Resource(libGroup.readEntry(QLatin1String("pimoProject"), QString()));

    if(!m_pimoLibrary.isValid()) {
        KSharedConfigPtr config = KSharedConfig::openConfig("conquirererc");
        KConfigGroup generalGroup = config->group("General");
        QString NepomukCollection = generalGroup.readEntry( "NepomukCollection", QString() );

        Nepomuk::Resource conquiereCollections = Nepomuk::Resource(NepomukCollection);
        conquiereCollections.addProperty( Nepomuk::Vocabulary::PIMO::isRelated() , m_pimoLibrary);
        m_pimoLibrary.addProperty( Nepomuk::Vocabulary::PIMO::isRelated() , conquiereCollections);
    }

    m_libraryType = Library_Project;

    m_libraryTag = Nepomuk::Tag( m_name );
    m_libraryTag.setLabel(m_name);

    m_backgroundSync = new BackgroundSync;

    // read in all sync details
    KConfigGroup syncGroup( &libconfig, "SyncProvider" );
    QStringList providerList = syncGroup.groupList();
    foreach(const QString &providerUUid, providerList) {
        NBibSync* syncProvider = 0;
        KConfigGroup providerGroup( &syncGroup, providerUUid );

        ProviderSyncDetails psd;

        QString providerType = providerGroup.readEntry(QLatin1String("provider"), QString());
        if(providerType == QLatin1String("zotero")) {
            psd.providerInfo = new ZoteroInfo;
            syncProvider = new SyncZoteroNepomuk;
        }
        else if(providerType == QLatin1String("kbibtexfile")) {
            psd.providerInfo = new KBTFileInfo;
            //syncProvider = new SyncZoteroNepomuk;
        }
        else {
            qWarning() << "Library::loadLibrary unknown sync provider found >>" << providerType;
            continue;
        }

        psd.userName = providerGroup.readEntry(QLatin1String("name"), QString());
        psd.pwd = providerGroup.readEntry(QLatin1String("pwd"), QString());
        psd.url = providerGroup.readEntry(QLatin1String("url"), QString());
        psd.collection = providerGroup.readEntry(QLatin1String("collection"), QString());

        QString syncMode = providerGroup.readEntry(QLatin1String("syncMode"), QString());
        psd.syncMode = SyncMode ( syncMode.toInt() );

        QString mergeStrategy = providerGroup.readEntry(QLatin1String("mergeStrategy"), QString());
        psd.mergeMode = MergeStrategy ( mergeStrategy.toInt() );

        QString askBeforeDeletion = providerGroup.readEntry(QLatin1String("askBeforeDeletion"), QString());
        if(askBeforeDeletion == QLatin1String("false"))
            psd.askBeforeDeletion = false;
        else
            psd.askBeforeDeletion = true;

        QString importAttachments = providerGroup.readEntry(QLatin1String("importAttachments"), QString());
        if(importAttachments == QLatin1String("false"))
            psd.importAttachments = false;
        else
            psd.importAttachments = true;

        QString exportAttachments = providerGroup.readEntry(QLatin1String("exportAttachments"), QString());
        if(exportAttachments == QLatin1String("false"))
            psd.exportAttachments = false;
        else
            psd.exportAttachments = true;

        QString akonadiContactsUUid = providerGroup.readEntry(QLatin1String("akonadiContactsUUid"), QString());
        psd.akonadiContactsUUid = akonadiContactsUUid.toInt();

        QString akonadiEventsUUid = providerGroup.readEntry(QLatin1String("akonadiEventsUUid"), QString());
        psd.akonadiEventsUUid = akonadiEventsUUid.toInt();

        syncProvider->setProviderDetails(psd);

        m_backgroundSync->addSyncProvider(syncProvider);
    }

    if(!m_libraryDir.isEmpty()) {
        m_dirWatcher = new DirWatcher();
        m_dirWatcher->setLibrary(this);
    }

    setupModels();
}

void Library::loadLibrary(const Nepomuk::Resource & pimoProject)
{
    Nepomuk::File settingsFile = pimoProject.property( Nepomuk::Vocabulary::PIMO::groundingOccurrence()).toResource();

    loadLibrary(settingsFile.url().pathOrUrl());
}

void Library::deleteLibrary()
{
    // remove the library from the conquiere pimo:thing
    KSharedConfigPtr config = KSharedConfig::openConfig("conquirererc");
    KConfigGroup generalGroup = config->group("General");
    QString NepomukCollection = generalGroup.readEntry( "NepomukCollection", QString() );
    Nepomuk::Resource conquiereCollections = Nepomuk::Resource(NepomukCollection);
    conquiereCollections.removeProperty( Nepomuk::Vocabulary::PIMO::isRelated() , m_pimoLibrary);

    Nepomuk::File settingsFile = m_pimoLibrary.property( Nepomuk::Vocabulary::PIMO::groundingOccurrence()).toResource();

    m_pimoLibrary.remove();
    m_libraryTag.remove();

    KIO::DeleteJob *dj = KIO::del(settingsFile.url());
    dj->exec();
    delete dj;
}

Nepomuk::Resource Library::pimoLibrary() const
{
    return m_pimoLibrary;
}

Nepomuk::Resource Library::pimoTag() const
{
    return m_libraryTag;
}

void Library::addResource(Nepomuk::Resource & res)
{
    if(m_libraryType == Library_System) {
        qWarning() << "can't add resources to system library";
        return;
    }

    Nepomuk::Resource relatesTo = res.property( Nepomuk::Vocabulary::PIMO::isRelated()).toResource();

    if ( relatesTo != m_pimoLibrary) {
        res.addProperty( Nepomuk::Vocabulary::PIMO::isRelated() , m_pimoLibrary);
    }
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
