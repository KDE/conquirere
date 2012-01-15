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

#ifndef PROJECTSYNCSETTINGS_H
#define PROJECTSYNCSETTINGS_H

#include <QWidget>

#include "onlinestorage/providersettings.h"

#include <Akonadi/Collection>

namespace Ui {
    class ProjectSyncSettings;
}

class ProjectSettings;

class ProjectSyncSettings : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectSyncSettings(QWidget *parent = 0);
    ~ProjectSyncSettings();

    void setProjectSettings(ProjectSettings *ps);

signals:
    void contentChanged();
    void addContactCollection( const QList<ProviderSettings::AkonadiDetails> &contactCollections );
    void addEventCollection( const QList<ProviderSettings::AkonadiDetails> &eventCollections );

public slots:
    void resetSettings();
    void applySettings();

private slots:
    void updateFolderTextLabel(const QString &folder);
    void editProvider();
    void addProvider();
    void removeProvider();
    void akonadiContactCollectionFetched(const Akonadi::Collection::List &list);
    void akonadiEventCollectionFetched(const Akonadi::Collection::List &list);

private:
    void fetchAkonadiCollection();

    Ui::ProjectSyncSettings *ui;

    ProjectSettings *m_settings;
    QList<ProviderSettings::AkonadiDetails> m_eventList;
    QList<ProviderSettings::AkonadiDetails> m_contactList;
};

#endif // PROJECTSYNCSETTINGS_H
