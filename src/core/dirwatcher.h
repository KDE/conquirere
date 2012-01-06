/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef DIRWATCHER_H
#define DIRWATCHER_H

#include <QObject>

#include <KDE/KUrl>
#include <KDE/KFileItemList>

class Library;
class KDirLister;
class QDBusInterface;

class DirWatcher : public QObject
{
    Q_OBJECT
public:
    explicit DirWatcher(QObject *parent = 0);
    ~DirWatcher();

    void setLibrary(Library *lib);

private slots:
    void itemsAdded (const KUrl &directoryUrl, const KFileItemList &items);
    void itemsDeleted (const KFileItemList &items);

private:
    Library *m_library;
    KDirLister *m_kdl;
    QDBusInterface *m_nepomukDBus;
};

#endif // DIRWATCHER_H
