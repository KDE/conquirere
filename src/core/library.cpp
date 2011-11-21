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

#include "models/nepomukmodel.h"
#include "models/referencemodel.h"
#include "models/publicationmodel.h"
#include "models/publicationfiltermodel.h"
#include "models/documentmodel.h"
#include "models/notemodel.h"
#include "models/bookmarkmodel.h"
#include "models/mailmodel.h"

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

#include <QtCore/QDir>
#include <QtCore/QFileInfoList>
#include <QtCore/QSettings>

#include <QtCore/QDebug>

Library::Library(LibraryType type)
    : QObject(0)
    , m_settings(0)
    , m_libraryType(type)
    , m_tagCloud(0)
{
    m_tagCloud = new TagCloud;
    m_initialImportFinished = 0;
    m_tagCloud->pauseUpdates(true);
}

Library::~Library()
{
    delete m_settings;

    foreach(QSortFilterProxyModel *atm, m_resources) {
        delete atm->sourceModel();
        delete atm;
    }

    m_resources.clear();
    delete m_tagCloud;
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
    m_pimoLibrary.setProperty( Soprano::Vocabulary::NAO::description() , m_description);

    // relate each library to the conquiere pimo:thing
    // this allows us to find all existing projects
    //DEBUG this creates duplicates resources for the conquire collection
    // find a way to retrieve the unique resource
    KSharedConfigPtr config = KSharedConfig::openConfig("conquirererc");
    KConfigGroup generalGroup = config->group("General");
    QString NepomukCollection = generalGroup.readEntry( "NepomukCollection", QString() );

    Nepomuk::Resource conquiereCollections = Nepomuk::Resource(NepomukCollection);
    m_pimoLibrary.setProperty( Nepomuk::Vocabulary::PIMO::isRelated() , conquiereCollections);

    // create a tag with the project name
    // this way we can relate publications/documents etc via PIMO::isRelated or the tag
    m_libraryTag = Nepomuk::Tag( m_name );
    m_libraryTag.setLabel(m_name);

    // create the project .ini file
    QString projectPath = KStandardDirs::locateLocal("appdata", QLatin1String("projects"));
    QString iniFile = projectPath + QLatin1String("/conquirere.ini");
    m_settings = new QSettings(iniFile,QSettings::IniFormat);
    m_settings->beginGroup(QLatin1String("Conquirere"));
    m_settings->setValue(QLatin1String("name"), name());
    m_settings->setValue(QLatin1String("description"), description());
    m_settings->setValue(QLatin1String("pimoProject"), m_pimoLibrary.uri());
    m_settings->endGroup();
    //m_settings->beginGroup(QLatin1String("Settings"));
    //m_settings->endGroup();
    m_settings->sync();

    Nepomuk::Resource settingsFile = Nepomuk::File(KUrl(iniFile));
    settingsFile.setProperty(Nepomuk::Vocabulary::PIMO::occurrence(), m_pimoLibrary);
    m_pimoLibrary.setProperty(Nepomuk::Vocabulary::PIMO::groundingOccurrence(), settingsFile );

    setupModels();
}

void Library::loadLibrary(const QString & projectFile)
{
    m_settings = new QSettings(projectFile,QSettings::IniFormat);
    m_settings->beginGroup(QLatin1String("Conquirere"));
    m_description = m_settings->value(QLatin1String("description")).toString();
    m_name = m_settings->value(QLatin1String("name")).toString();

    m_pimoLibrary = Nepomuk::Resource(m_settings->value(QLatin1String("pimoProject")).toString());
    m_settings->endGroup();

    if(!m_pimoLibrary.isValid()) {
        // DEBUG this creates duplicates resources for the conquire collection
        // find a way to retrieve the unique resource
        KSharedConfigPtr config = KSharedConfig::openConfig("conquirererc");
        KConfigGroup generalGroup = config->group("General");
        QString NepomukCollection = generalGroup.readEntry( "NepomukCollection", QString() );

        Nepomuk::Resource conquiereCollections = Nepomuk::Resource(NepomukCollection);
        m_pimoLibrary.setProperty( Nepomuk::Vocabulary::PIMO::isRelated() , conquiereCollections);
    }

    m_libraryType = Library_Project;

    m_libraryTag = Nepomuk::Tag( m_name );
    m_libraryTag.setLabel(m_name);
    setupModels();
}

void loadLibrary(const Nepomuk::Resource & pimoProject)
{
    Nepomuk::File settingsFile = pimoProject.property( Nepomuk::Vocabulary::PIMO::isRelated()).toResource();

    loadLibrary(settingsFile.url().pathOrUrl());
}

void Library::deleteLibrary()
{
    m_pimoLibrary.remove();
    m_libraryTag.remove();
}

Nepomuk::Resource Library::pimoLibrary() const
{
    return m_pimoLibrary;
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

    NoteModel *noteModel = new NoteModel;
    noteModel->setLibrary(this);
    QSortFilterProxyModel *noteFilter = new QSortFilterProxyModel;
    noteFilter->setSourceModel(noteModel);
    m_resources.insert(Resource_Note, noteFilter);
    connectModelToTagCloud(noteModel);

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
