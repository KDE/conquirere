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

#include "config/conquirere.h"

#include "models/nepomukmodel.h"
#include "models/referencemodel.h"
#include "models/publicationmodel.h"
#include "models/publicationfiltermodel.h"
#include "models/documentmodel.h"
#include "models/notemodel.h"
#include "models/seriesmodel.h"
#include "models/seriesfiltermodel.h"
#include "models/eventmodel.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/StoreResourcesJob>
#include <KDE/KJob>

#include <Nepomuk2/Variant>
#include <Nepomuk2/Resource>
#include <Nepomuk2/Tag>
#include <Nepomuk2/File>

#include "sro/pimo/project.h"
#include "sro/nao/tag.h"
#include "sro/nie/dataobject.h"
#include "sro/nfo/document.h"

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NFO>

#include <KDE/KUrl>
#include <KDE/KStandardDirs>
#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KIO/CopyJob>
#include <KDE/KIO/DeleteJob>
#include <KDE/KDebug>

#include <QtCore/QUuid>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QFileInfoList>
#include <QtGui/QSortFilterProxyModel>

const QString DOCPATH = I18N_NOOP2("Name of the documents folder to store user library documents","documents");  /**< @todo TODO: make this configurable */
const QString NOTEPATH = I18N_NOOP2("Name of the notes folder to store user library documents","notes");     /**< @todo TODO: make this configurable */

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

Library::Library()
    : QObject(0)
    , m_libraryType(BibGlobals::Library_Project)
    , m_dirWatcher(0)
    , m_tagCloud(0)
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

Nepomuk2::Resource Library::createLibrary(const QString & name, const QString & description, const QString & path)
{
    Nepomuk2::Resource projectThing;

    // We create a new pimo:Project and tag via the DMS system.
    // this ensures we handle duplicates and set the nao:maintainedBy correctly
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::PIMO::Project newProject;
    newProject.setProperty( NAO::prefLabel() , name );
    newProject.setProperty( NAO::description() , description );

    Nepomuk2::NAO::Tag newTag;
    newTag.setProperty( NAO::identifier(), QUrl::toPercentEncoding(name) );
    newTag.addPrefLabel( name );
    newProject.addProperty(NAO::hasSubResource(), newTag.uri() ); // remove tag when project is deleted

    graph << newTag << newProject;

    //blocking graph save
    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties);
    if( !srj->exec() ) {
        kWarning() << "Could not create pimo:Project" << srj->errorString();
        return projectThing;
    }
    else {
        // get the pimo project from the return job mappings
        projectThing = Nepomuk2::Resource( srj->mappings().value( newProject.uri() ) );
    }

    // check if a library path is set
    // we create the path if not available
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

QString Library::createIniFile(Nepomuk2::Resource &pimoProject, const QString & path)
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
    projectSettings->setName( pimoProject.property( NAO::prefLabel()).toString() );
    projectSettings->setDescription( pimoProject.property( NAO::description()).toString() );

    // connect the nepomuk data for the settingsfile to the pimo::project
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NFO::Document iniFileSimpleResource;
    iniFileSimpleResource.addType(NIE::DataObject());
    iniFileSimpleResource.addType(NIE::InformationElement());
    QString settingsFileName = QLatin1String("file://") + iniFile;
    iniFileSimpleResource.setProperty( NIE::url(), settingsFileName );
    iniFileSimpleResource.setProperty( NIE::title(), i18n("Conquirere settings for project: %1", pimoProject.property( NAO::prefLabel()).toString()) );

    graph << iniFileSimpleResource;

    //blocking graph save
    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNone, Nepomuk2::OverwriteProperties);
    if( !srj->exec() ) {
        kWarning() << "could not create project ini resource file" << srj->errorString();
    }
    else {
        kDebug() << "save successful" << srj->mappings();
        // get the resource from the return job mappings
        pimoProject.addProperty( PIMO::groundingOccurrence(), srj->mappings().value( iniFileSimpleResource.uri() ) );
    }

    return iniFile;
}

void Library::loadLibrary(const QString & projectFile, BibGlobals::LibraryType type)
{
    Q_ASSERT_X( !m_tagCloud, "loadLibrary", "tagCloud exist already ... means we loaded this library beforehand already");

    m_libraryType = type;
    m_projectSettings->loadSettings(projectFile);

    m_dirWatcher = new DirWatcher();
    m_dirWatcher->setLibrary(this);

    connect(m_projectSettings, SIGNAL(projectDirChanged(QString)), m_dirWatcher, SLOT(changeListenDir(QString)));

    m_tagCloud = new TagCloud(this);
    //m_tagCloud->pauseUpdates(true);

    setupModels();
}

void Library::loadSystemLibrary( )
{
    QString inipath = KStandardDirs::locateLocal("appdata", QLatin1String("system"));
    QString iniFile = inipath + QLatin1String("/system_library.ini");

    QFileInfo fi(iniFile);
    if(!fi.exists()) {
        kDebug() << "System ini file not found. Create a default one";
        m_projectSettings->setSettingsFile(iniFile);
        m_projectSettings->setName(i18nc("name of the system library","System Library"));
    }

    loadLibrary(iniFile, BibGlobals::Library_System);
}

void Library::loadLibrary(Nepomuk2::Resource & pimoProject)
{
    QList<Nepomuk2::Resource> settingsFiles = pimoProject.property(Nepomuk2::Vocabulary::PIMO::groundingOccurrence()).toResourceList();

    Nepomuk2::File iniFile;
    foreach( const Nepomuk2::Resource &r, settingsFiles) {
        iniFile = r;
        //DEBUG we save inifiles in StandardDirs::locateLocal("appdata", QLatin1String("projects"));
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

BibGlobals::LibraryType Library::libraryType() const
{
    return m_libraryType;
}

void Library::deleteLibrary()
{
   QList<Nepomuk2::Resource> gos = m_projectSettings->projectThing().property(Nepomuk2::Vocabulary::PIMO::groundingOccurrence()).toResourceList();
   QList<QUrl> uris;

    // delete all groundingOccurences, in our case this should be only the .ini files
    foreach(const Nepomuk2::Resource &r, gos) {
        Nepomuk2::File iniFile = r;
        KIO::DeleteJob *dj = KIO::del(iniFile.url(), KIO::HideProgressInfo);
        dj->exec();
        delete dj;
        uris << r.uri();
    }

    uris << m_projectSettings->projectThing().uri() << m_projectSettings->projectTag().uri();

    connect(Nepomuk2::removeResources( uris, Nepomuk2::RemoveSubResoures ),
            SIGNAL(result(KJob*)), this, SLOT(nepomukDMSfinishedInfo(KJob*)));
}

void Library::addResource(const Nepomuk2::Resource & res)
{
    if(m_libraryType == BibGlobals::Library_System) {
        kWarning() << "can't add resources to system library";
        return;
    }

    QList<Nepomuk2::Resource> relatesTo = res.property( Soprano::Vocabulary::NAO::isRelated()).toResourceList();

    foreach(const Nepomuk2::Resource & rt, relatesTo) {
        if ( rt == m_projectSettings->projectThing()) {
            return;
        }
    }

    QList<QUrl> publicationUrisToAddProject;
    publicationUrisToAddProject << res.uri();
    QVariantList projectValue;
    projectValue <<  m_projectSettings->projectThing().uri();

    // small special case, if the resource was a reference add also the publication to the project
    if(res.hasType(Nepomuk2::Vocabulary::NBIB::Reference())) {
        Nepomuk2::Resource pub = res.property(Nepomuk2::Vocabulary::NBIB::publication()).toResource();
        publicationUrisToAddProject << pub.uri();
    }

    // if resource was a file (pdf for example) add also the known publication data
    // and the publication references
    if(res.hasType(Nepomuk2::Vocabulary::NFO::Document())) {
        Nepomuk2::Resource pub = res.property(Nepomuk2::Vocabulary::NBIB::publishedAs()).toResource();
        if(pub.isValid()) {
            publicationUrisToAddProject << pub.uri();

            // get references
            QList<Nepomuk2::Resource> refs = pub.property(Nepomuk2::Vocabulary::NBIB::reference()).toResourceList();

            foreach(const Nepomuk2::Resource r, refs) {
                publicationUrisToAddProject << r.uri();
            }
        }
    }

    // if we added a publication, add also all its references and the pdf to the project
    if(res.hasType(Nepomuk2::Vocabulary::NBIB::Publication())) {

        QList<Nepomuk2::Resource> refs = res.property(Nepomuk2::Vocabulary::NBIB::reference()).toResourceList();
        kDebug() << "##### add isRelated to reference" << refs.size();

        foreach(const Nepomuk2::Resource r, refs) {
            publicationUrisToAddProject << r.uri();
        }

        Nepomuk2::Resource file = res.property(Nepomuk2::Vocabulary::NBIB::isPublicationOf()).toResource();
        if(file.isValid()) {
            publicationUrisToAddProject << file.uri();
        }
    }

    Nepomuk2::addProperty(publicationUrisToAddProject, Soprano::Vocabulary::NAO::isRelated(), projectValue);
}

void Library::removeResource(const Nepomuk2::Resource & res)
{
    Q_ASSERT_X( m_libraryType == BibGlobals::Library_Project, "removeResource", "can't remove resources from system library");

    QList<QUrl> resourceUris;
    resourceUris << res.uri();

    QList<Nepomuk2::Resource> subResources = res.property(Soprano::Vocabulary::NAO::hasSubResource()).toResourceList();
    foreach(const Nepomuk2::Resource &r, subResources) {
        resourceUris << r.uri();
    }

    QVariantList propertyUris;
    propertyUris << m_projectSettings->projectThing().uri();
    Nepomuk2::removeProperty(resourceUris, NAO::isRelated(), propertyUris);
}

void Library::deleteResource(const Nepomuk2::Resource & resource )
{
    QList<QUrl> removeResourcesUris;
    removeResourcesUris << resource.uri();

    //FIXME: This should be done by nao:hasSubresource automatically. No need to find the connected resources
/*
    //check if the resource is in a collection that has no other articles attached to it anymore
    Nepomuk2::Resource collection = resource.property(NBIB::collection()).toResource();
    if( collection.exists() ) {
        QList<Nepomuk2::Resource> articles = collection.property(NBIB::article()).toResourceList();
        if(articles.size() <= 1) {
            removeResourcesUris << collection.uri();
        }

        // check if the collection is in a Series that has no other publication attached to it anymore
        Nepomuk2::Resource series = collection.property(NBIB::inSeries()).toResource();
        if( series.exists() ) {
            QList<Nepomuk2::Resource> seriesPublication = series.property(NBIB::seriesOf()).toResourceList();
            if(seriesPublication.size() <= 1) {
                removeResourcesUris << series.uri();
            }
        }
    }

    // check if the resource is in a Series that has no other publication attached to it anymore
    Nepomuk2::Resource series = resource.property(NBIB::inSeries()).toResource();
    if( series.exists() ) {
        QList<Nepomuk2::Resource> seriesPublication = series.property(NBIB::seriesOf()).toResourceList();
        if(seriesPublication.size() <= 1) {
            removeResourcesUris << series.uri();
        }
    }
    */

    // now delete everything and its subresources

    connect(Nepomuk2::removeResources( removeResourcesUris, Nepomuk2::RemoveSubResoures ),
            SIGNAL(result(KJob*)), this, SLOT(nepomukDMSfinishedInfo(KJob*)));
}

QSortFilterProxyModel* Library::viewModel(BibGlobals::ResourceSelection selection)
{
    return m_resources.value(selection);
}

QMap<BibGlobals::ResourceSelection, QSortFilterProxyModel*> Library::viewModels()
{
    return m_resources;
}

TagCloud *Library::tagCloud()
{
    return m_tagCloud;
}

void Library::nepomukDMSfinishedInfo(KJob *job)
{
    if(job->error()) {
        kWarning() << "Error during Nepomuk DMS call:";
        kWarning() << job->errorString();
    }
}

void Library::setupModels()
{
    DocumentModel *documentModel = new DocumentModel(this);
    documentModel->setLibrary(this);
    QSortFilterProxyModel *documentFilter = new QSortFilterProxyModel;
    documentFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    documentFilter->setSourceModel(documentModel);
    m_resources.insert(BibGlobals::Resource_Document, documentFilter);

    ReferenceModel *referencesModel = new ReferenceModel;
    referencesModel->setLibrary(this);
    PublicationFilterModel *referenceFilter = new PublicationFilterModel;
    referenceFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    referenceFilter->setSourceModel(referencesModel);
    m_resources.insert(BibGlobals::Resource_Reference, referenceFilter);

    PublicationModel *publicationModel = new PublicationModel;
    publicationModel->setLibrary(this);
    PublicationFilterModel *publicationFilter = new PublicationFilterModel;
    publicationFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    publicationFilter->setSourceModel(publicationModel);
    m_resources.insert(BibGlobals::Resource_Publication, publicationFilter);

    SeriesModel *seriesModel = new SeriesModel;
    seriesModel->setLibrary(this);
    SeriesFilterModel *seriesFilter = new SeriesFilterModel;
    seriesFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    seriesFilter->setSourceModel(seriesModel);
    m_resources.insert(BibGlobals::Resource_Series, seriesFilter);

    NoteModel *noteModel = new NoteModel;
    noteModel->setLibrary(this);
    QSortFilterProxyModel *noteFilter = new QSortFilterProxyModel;
    noteFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    noteFilter->setSourceModel(noteModel);
    m_resources.insert(BibGlobals::Resource_Note, noteFilter);

    EventModel *eventModel = new EventModel;
    eventModel->setLibrary(this);
    QSortFilterProxyModel *eventFilter = new QSortFilterProxyModel;
    eventFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    eventFilter->setSourceModel(eventModel);
    m_resources.insert(BibGlobals::Resource_Event, eventFilter);
}
