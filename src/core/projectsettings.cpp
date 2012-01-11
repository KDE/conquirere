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

#include "onlinestorage/zotero/zoteroinfo.h"
#include "onlinestorage/kbibtexfile/kbtfileinfo.h"

#include <Nepomuk/Variant>
#include <Nepomuk/File>
#include <Nepomuk/Vocabulary/NIE>

#include <KDE/KUrl>
#include <KDE/KConfigGroup>
#include <KDE/KIO/CopyJob>
#include <KDE/KStandardDirs>

#include <KDE/KDebug>

#include <QtCore/QUuid>
#include <QtCore/QDir>

const QString DOCPATH = I18N_NOOP2("Name of the documents folder to store user library documents","documents");  /**< @todo make this configurable */
const QString NOTEPATH = I18N_NOOP2("Name of the notes folder to store user library documents","notes");     /**< @todo make this configurable */

ProjectSettings::ProjectSettings(QObject *parent)
: QObject(parent)
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
    m_pimoThing = Nepomuk::Thing(generalGroup.readEntry("pimoProject", QString()));

    QString name = generalGroup.readEntry(QLatin1String("name"), QString());
    m_projectTag = Nepomuk::Tag( name );

    if(!m_pimoThing.isValid()) {
        kDebug() << "Warning loaded project without valid PIMO::Project() resource @ project" << name;
    }
}

void ProjectSettings::loadSettings(const Nepomuk::Thing & pimoProject)
{
    QList<Nepomuk::Resource> settingsFiles = pimoProject.groundingOccurrences();

    Nepomuk::File iniFile;
    //if(settingsFiles.size() > 1) {
    // This happens if we move the the folder and keep the old settingfiles intact
    // usually the old file is removed and should be deleted from nepomuk
    // otherwise we should filter modification date and take the newest
    //}
    iniFile = settingsFiles.first();

    loadSettings(iniFile.url().pathOrUrl());
}

void ProjectSettings::setPimoThing( Nepomuk::Resource &thing)
{
    Q_ASSERT_X(thing.isValid(), "setPimoThing", "no valid thing used");

    KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
    generalGroup.writeEntry("pimoProject", thing.resourceUri().toString());
    generalGroup.sync();

    m_pimoThing = thing;
}

Nepomuk::Thing ProjectSettings::projectThing() const
{
    return m_pimoThing;
}

Nepomuk::Tag ProjectSettings::projectTag() const
{
    return m_projectTag;
}

void ProjectSettings::setName(const QString &name)
{
    KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
    generalGroup.writeEntry("name", name);
    generalGroup.sync();

    emit projectDetailsChaned();
}

QString ProjectSettings::name() const
{
    KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
    return generalGroup.readEntry("name", QString());
}

void ProjectSettings::setDescription(const QString &description)
{
    KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
    generalGroup.writeEntry("description", description);
    generalGroup.sync();

    emit projectDetailsChaned();
}

QString ProjectSettings::description() const
{
    KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
    return generalGroup.readEntry("description", QString());
}

void ProjectSettings::setProjectDir(const QString &path)
{
    if(path == projectDir())
        return;

    QString systemPath = KStandardDirs::locateLocal("appdata", QLatin1String("projects"));

    QString newIniFile;
    QString oldIniFile = projectDir();
    if(oldIniFile.isEmpty())
        oldIniFile = systemPath;

    if(path.isEmpty()) {
        // move inifile to system space
        newIniFile = systemPath;
    }
    else {
        // move inifile to user selected space
        newIniFile = path;

        // create the folder
        QDir project;
        project.mkpath(path);
        project.mkdir(DOCPATH);
        project.mkdir(NOTEPATH);
    }

    oldIniFile = oldIniFile + QLatin1String("/") + name() + QLatin1String(".ini");
    newIniFile = newIniFile + QLatin1String("/") + name() + QLatin1String(".ini");

    // update config file
    KConfigGroup generalGroup( m_projectConfig, "Conquirere" );
    generalGroup.writeEntry("libraryDir", path);
    generalGroup.sync();

    // move ini file to new space
    KIO::CopyJob *cj = KIO::move(oldIniFile, newIniFile);
    cj->exec();
    delete cj;

    //update nepomuk data for the settingsfile to the pimo:.project
    Nepomuk::File settingsFile = Nepomuk::File(KUrl(newIniFile));
    QString settingsFileName = QLatin1String("file://") + newIniFile;
    settingsFile.setProperty(Nepomuk::Vocabulary::NIE::url(), settingsFileName);
    m_pimoThing.addGroundingOccurrence(settingsFile);

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

void ProjectSettings::setProviderSyncDetails(const ProviderSyncDetails &psd, const QString &uuid)
{
    Q_ASSERT_X( psd.providerInfo, "setProviderSyncDetails", "no valid StorageInfo pointer" );

    QString validUuid = uuid;
    if(validUuid.isEmpty()) {
        // create new random id
        validUuid = QUuid::createUuid().toString();
    }

    KConfigGroup syncGroup( m_projectConfig, "SyncProvider" );
    KConfigGroup providerGroup( &syncGroup, validUuid );
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
    syncGroup.sync();

    if(uuid.isEmpty())
        emit providerChanged(validUuid);
    else
        emit newProviderAdded(validUuid);
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

    QString providerType = providerGroup.readEntry("provider", QString());
    if(providerType == QLatin1String("zotero")) {
        psd.providerInfo = new ZoteroInfo;
    }
    else if(providerType == QLatin1String("kbibtexfile")) {
        psd.providerInfo = new KBTFileInfo;
    }
    else {
        qWarning() << providerType;
        qFatal("ProjectSettings::providerSyncDetails unknown sync provider found");
    }

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

    QString akonadiContactsUUid = providerGroup.readEntry("akonadiContactsUUid", QString());
    psd.akonadiContactsUUid = akonadiContactsUUid.toInt();

    QString akonadiEventsUUid = providerGroup.readEntry("akonadiEventsUUid", QString());
    psd.akonadiEventsUUid = akonadiEventsUUid.toInt();

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
