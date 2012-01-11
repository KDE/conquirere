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

#ifndef PROJECTSETTINGS_H
#define PROJECTSETTINGS_H

#include <QObject>

#include <Nepomuk/Thing>
#include <Nepomuk/Tag>

#include <KDE/KSharedConfigPtr>

#include "onlinestorage/storageglobals.h"

class ProjectSettings : public QObject
{
    Q_OBJECT
public:
    enum LibrarySpecialDir {
        BASE,
        DOCUMENTS,
        NOTES
    };

    explicit ProjectSettings(QObject *parent = 0);

    void setSettingsFile(const QString &projectFile);
    void loadSettings(const QString &projectFile);
    void loadSettings(const Nepomuk::Thing & pimoProject);

    void setPimoThing( Nepomuk::Resource &thing );
    Nepomuk::Thing projectThing() const;

    Nepomuk::Tag projectTag() const;

    void setName(const QString &name);
    QString name() const;

    void setDescription(const QString &description);
    QString description() const;

    void setProjectDir(const QString &path);
    QString projectDir(LibrarySpecialDir lsd = BASE) const;

    void setProviderSyncDetails(const ProviderSyncDetails &psd, const QString &uuid);
    void removeProviderSyncDetails(const QString &uuid);
    ProviderSyncDetails providerSyncDetails(const QString &uuid) const;
    QList<ProviderSyncDetails> allProviderSyncDetails() const;

signals:
    void projectDetailsChaned();
    void projectDirChanged(const QString &newPath);
    void newProviderAdded(const QString &uuid);
    void providerChanged(const QString &uuid);
    void providerRemoved(const QString &uuid);

private:
    KSharedConfigPtr m_projectConfig;

    Nepomuk::Thing m_pimoThing;
    Nepomuk::Tag m_projectTag;
};

#endif // PROJECTSETTINGS_H
