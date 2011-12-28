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

#include "dirwatcher.h"

#include "library.h"

#include <KDE/KDirLister>

#include <Nepomuk/File>

DirWatcher::DirWatcher(QObject *parent)
    : QObject(parent)
    , m_library(0)
    , m_kdl(0)
{
}

DirWatcher::~DirWatcher()
{
    delete m_kdl;
}

void DirWatcher::setLibrary(Library *lib)
{
    m_library = lib;

    delete m_kdl;
    m_kdl = new KDirLister;

    connect(m_kdl, SIGNAL(itemsAdded(KUrl,KFileItemList)), this, SLOT(itemsAdded(KUrl,KFileItemList)));
    connect(m_kdl, SIGNAL(itemsDeleted(KFileItemList)), this, SLOT(itemsDeleted(KFileItemList)));

    m_kdl->openUrl(KUrl(m_library->libraryDocumentDir()));
}

void DirWatcher::itemsAdded (const KUrl &directoryUrl, const KFileItemList &items)
{
    Q_UNUSED(directoryUrl);

    foreach(const KFileItem &file, items) {
        Nepomuk::File addedFile(file.url());
        if(!addedFile.isValid()) {
            qDebug() << file << "is not a valid nepomuk resource";
        }

        m_library->addResource(addedFile);
    }
}

void DirWatcher::itemsDeleted (const KFileItemList &items)
{
    foreach(const KFileItem &file, items) {
        Nepomuk::File addedFile(file.url());
        addedFile.remove();
    }
}
