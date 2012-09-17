/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "projectsettings.h"

#include "core/library.h"

#include <Nepomuk2/Variant>
#include <Nepomuk2/File>
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Tag>

#include <KDE/KUrl>
#include <KDE/KConfigGroup>
#include <KDE/KIO/DeleteJob>
#include <KDE/KStandardDirs>

#include <KDE/KDebug>

#include <QtCore/QUuid>
#include <QtCore/QDir>

const QString DOCPATH = I18N_NOOP2("Name of the documents folder to store user library documents","documents");  /**< @todo make this configurable */
const QString NOTEPATH = I18N_NOOP2("Name of the notes folder to store user library documents","notes");     /**< @todo make this configurable */

ProjectSettings::ProjectSettings(Library *lib)
    : QObject(0)
    , m_library(lib)
{
}

void ProjectSettings::setSettingsFile(const QString &projectFile)
{
    m_projectConfig = KSharedConfig::openConfig( projectFile, KConfig::SimpleConfig );
}

void ProjectSettings::loadSettings(const QString &projectFile)
{
    m_projectConfig = KSharedConfig::openConfig( projectFile, KConfig::SimpleConfig );

    // read the nepomuk resources for this project
    KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
    m_pimoThing = Nepomuk2::Resource(generalGroup.readEntry("pimoProject", QString()));

    QString name = m_pimoThing.property(Soprano::Vocabulary::NAO::prefLabel()).toString();
    if(!name.isEmpty()) {
        m_projectTag = Nepomuk2::Tag( QUrl::toPercentEncoding(name) );
        if(!m_projectTag.exists()) {
            m_projectTag.setLabel( name );
        }

        kDebug() << "use project tag with name " << name << "valid?" << m_projectTag.exists() << m_projectTag.isValid();
    }

    if(!m_pimoThing.isValid() && m_library->libraryType() != Library_System) {
        kDebug() << "Warning loaded project without valid PIMO::Project() resource @ project" << name;
    }
}

void ProjectSettings::setPimoThing( Nepomuk2::Resource &thing )
{
    Q_ASSERT_X(thing.isValid(), "setPimoThing", "no valid thing used");

    KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
    generalGroup.writeEntry("pimoProject", thing.uri().toString());
    generalGroup.sync();

    m_pimoThing = thing;
}

Nepomuk2::Resource ProjectSettings::projectThing() const
{
    return m_pimoThing;
}

Nepomuk2::Resource ProjectSettings::projectTag() const
{
    return m_projectTag;
}

void ProjectSettings::setName(const QString &newName)
{
    if( newName == name())
        return;

    KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
    generalGroup.writeEntry("name", newName);
    generalGroup.sync();

    // check if a tag with the project name exist
    m_projectTag = Nepomuk2::Tag( QUrl::toPercentEncoding(name()) );

    // update the used tag for the project
    if(m_projectTag.exists()) {
        kDebug() << "project tag existed with name" << name() << ", rename it to" << newName;
        m_projectTag.removeProperty( Soprano::Vocabulary::NAO::prefLabel());
        m_projectTag.setLabel( newName );
        //m_projectTag.removeProperty( Soprano::Vocabulary::NAO::identifier());
        QStringList identifiers;
        identifiers << QUrl::toPercentEncoding(newName);
        m_projectTag.setIdentifiers(identifiers);
    }
    else {
        kDebug() << "no project Tag existed with name" << name() << ", create a new one" << newName;
        m_projectTag = Nepomuk2::Tag( QUrl::toPercentEncoding(newName) );
        m_projectTag.setLabel( newName.toUtf8() );
    }

    // update the project thing
    if(projectThing().isValid()) {
        projectThing().removeProperty( Soprano::Vocabulary::NAO::prefLabel() );
        projectThing().setProperty( Soprano::Vocabulary::NAO::prefLabel() , newName );
    }

    emit projectDetailsChanged(m_library);
}

QString ProjectSettings::name() const
{
    QString name;
    if(projectThing().isValid()) {
        name = projectThing().property( Soprano::Vocabulary::NAO::prefLabel() ).toString();
    }
    if(name.isEmpty()) {
        KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
        name = generalGroup.readEntry("name", QString());
    }

    if(name.isEmpty()) {
        name = i18n("unknown project name");
    }

    return name;
}

void ProjectSettings::setDescription(const QString &newDescription)
{
    if( newDescription == description())
        return;

    KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
    generalGroup.writeEntry("description", newDescription);
    generalGroup.sync();

    // update the project thing
    if(projectThing().isValid()) {
        projectThing().setProperty( Soprano::Vocabulary::NAO::description() , newDescription );
    }

    emit projectDetailsChanged(m_library);
}

QString ProjectSettings::description() const
{
    QString description;
    if(projectThing().isValid()) {
        description = projectThing().property( Soprano::Vocabulary::NAO::description() ).toString();
    }
    if(description.isEmpty()) {
        KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
        description = generalGroup.readEntry("description", QString());
    }

    return description;
}

void ProjectSettings::setProjectDir(const QString &path)
{
    if(path == projectDir())
        return;

    // update config file
    KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
    generalGroup.writeEntry("libraryDir", path);
    generalGroup.sync();

    if(!path.isEmpty()) {
        // create the folder
        QDir project;
        project.mkpath(path);
        project.setPath(path);
        project.mkdir(DOCPATH);
        project.mkdir(NOTEPATH);
    }

    emit projectDirChanged( projectDir() );
}

QString ProjectSettings::projectDir(LibrarySpecialDir lsd) const
{
    KConfigGroup generalGroup( m_projectConfig, "Conquirere" );

    QString baseDir = generalGroup.readEntry("libraryDir", QString());

    switch(lsd) {
    case DOCUMENTS:
        return baseDir + DOCPATH;
    case NOTES:
        return baseDir + NOTEPATH;
    case BASE:
    default:
        return baseDir;
    }
}

void ProjectSettings::deleteProjectDir()
{
    // delete the project dir
    KIO::DeleteJob *dj = KIO::del(KUrl(projectDir()));
    dj->exec();
    delete dj;
}

QString ProjectSettings::setProviderSyncDetails(const ProviderSyncDetails &psd, const QString &uuid)
{
    Q_ASSERT_X( psd.providerInfo, "setProviderSyncDetails", "no valid StorageInfo pointer" );

    QString validUuid = uuid;
    if(validUuid.isEmpty()) {
        // create new random id
        validUuid = QUuid::createUuid().toString();
    }

    KConfigGroup syncGroup( m_projectConfig, "SyncProvider" );
    KConfigGroup providerGroup( &syncGroup, validUuid );
    providerGroup.writeEntry(QLatin1String("provider"), psd.providerId);
    providerGroup.writeEntry(QLatin1String("name"), psd.userName);
    providerGroup.writeEntry(QLatin1String("url"), psd.url);
    providerGroup.writeEntry(QLatin1String("collection"), psd.collection);
    providerGroup.writeEntry(QLatin1String("askBeforeDeletion"), psd.askBeforeDeletion);
    providerGroup.writeEntry(QLatin1String("mergeStrategy"), (int)psd.mergeMode);
    providerGroup.writeEntry(QLatin1String("syncMode"), (int)psd.syncMode);
    providerGroup.writeEntry(QLatin1String("importAttachments"), psd.importAttachments);
    providerGroup.writeEntry(QLatin1String("exportAttachments"), psd.exportAttachments);
    providerGroup.writeEntry(QLatin1String("localStoragePath"), psd.localStoragePath);

    providerGroup.sync();
    syncGroup.sync();

    if(uuid.isEmpty())
        emit providerChanged(validUuid);
    else
        emit newProviderAdded(validUuid);

    return validUuid;
}

void ProjectSettings::removeProviderSyncDetails(const QString &uuid)
{
    Q_ASSERT_X( !uuid.isEmpty(), "removeproviderSyncDetails", "empty uuid" );

    KConfigGroup syncGroup( m_projectConfig, "SyncProvider" );
    KConfigGroup providerGroup( &syncGroup, uuid );
    providerGroup.deleteGroup();
    providerGroup.sync();
    syncGroup.sync();

    emit providerRemoved(uuid);
}

ProviderSyncDetails ProjectSettings::providerSyncDetails(const QString &uuid) const
{
    Q_ASSERT_X( !uuid.isEmpty(), "setProviderSyncDetails", "empty uuid" );

    KConfigGroup syncGroup( m_projectConfig, "SyncProvider" );
    KConfigGroup providerGroup( &syncGroup, uuid );

    ProviderSyncDetails psd;
    psd.uuid = uuid;
    psd.providerId = providerGroup.readEntry("provider", QString());
    psd.userName = providerGroup.readEntry("name", QString());
    psd.pwd = providerGroup.readEntry("pwd", QString());
    psd.url = providerGroup.readEntry("url", QString());
    psd.collection = providerGroup.readEntry("collection", QString());

    QString syncMode = providerGroup.readEntry("syncMode", QString());
    psd.syncMode = SyncMode ( syncMode.toInt() );

    QString mergeStrategy = providerGroup.readEntry("mergeStrategy", QString());
    psd.mergeMode = MergeStrategy ( mergeStrategy.toInt() );

    QString askBeforeDeletion = providerGroup.readEntry("askBeforeDeletion", QString());
    if(askBeforeDeletion == QLatin1String("false"))
        psd.askBeforeDeletion = false;
    else
        psd.askBeforeDeletion = true;

    QString importAttachments = providerGroup.readEntry("importAttachments", QString());
    if(importAttachments == QLatin1String("false"))
        psd.importAttachments = false;
    else
        psd.importAttachments = true;

    QString exportAttachments = providerGroup.readEntry("exportAttachments", QString());
    if(exportAttachments == QLatin1String("false"))
        psd.exportAttachments = false;
    else
        psd.exportAttachments = true;

    psd.localStoragePath = providerGroup.readEntry("localStoragePath", QString());

    return psd;
}

QList<ProviderSyncDetails> ProjectSettings::allProviderSyncDetails() const
{
    QList<ProviderSyncDetails> psdList;

    // read in all sync details
    KConfigGroup syncGroup( m_projectConfig, "SyncProvider" );
    QStringList providerList = syncGroup.groupList();
    foreach(const QString &providerUUid, providerList) {
        psdList.append(providerSyncDetails(providerUUid));
    }

    return psdList;
}
