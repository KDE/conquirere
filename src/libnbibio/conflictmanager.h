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

#ifndef CONFLICTMANAGER_H
#define CONFLICTMANAGER_H

#include <QObject>

#include <Nepomuk/Resource>
#include <QList>

class ConflictManager : public QObject
{
    Q_OBJECT
public:

    struct Conflict {
        Nepomuk::Resource entry;
        QList<Nepomuk::Resource> conflictEntries;
    };

    explicit ConflictManager(QObject *parent = 0);

    Nepomuk::Resource addEntry(Nepomuk::Resource entry);
    QList<ConflictManager::Conflict> publicationConflicts();
    QList<ConflictManager::Conflict> referenceConflicts();

    bool hasConflicts();

private:
    QList<Nepomuk::Resource> conflictCheckPublication(Nepomuk::Resource entry);
    QList<Nepomuk::Resource> conflictCheckReference(Nepomuk::Resource entry);
    QUrl resourceToUrl(Nepomuk::Resource & resource);

    QList<ConflictManager::Conflict> m_publicationConflicts;
    QList<ConflictManager::Conflict> m_referenceConflicts;
};

#endif // CONFLICTMANAGER_H
