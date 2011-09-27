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

#ifndef LIBRARY_H
#define LIBRARY_H

#include "../globals.h"

#include <QObject>
#include <Nepomuk/Resource>
#include <Nepomuk/Tag>
#include <QFileInfo>
#include <QString>

class QSettings;
class ProjectTreeWidget;
class QAbstractTableModel;

class Library : public QObject
{
    Q_OBJECT
public:
    explicit Library(LibraryType type);

    LibraryType libraryType() const;

    void setName(const QString & name);
    QString name() const;

    void setPath(const QString & path);
    QString path() const;

    void createLibrary();
    void loadLibrary(const QString & projectFile);
    void deleteLibrary();

    Nepomuk::Resource pimoLibrary() const;

    bool isInPath(const QString &filename);

    void addResource(Nepomuk::Resource & res);
    void addDocument(const QFileInfo &fileInfo);

    QAbstractTableModel* viewModel(ResourceSelection selection);
    void connectFetchIndicator(ProjectTreeWidget *treeWidget);

public slots:
    void scanLibraryFolders();

private:
    void initializeLibraryFolder();
    void setupModels();

    LibraryType m_libraryType;
    QString m_name;
    QString m_path;

    Nepomuk::Resource m_pimoProject;

    Nepomuk::Tag m_projectTag;
    QSettings *m_settings;

    QMap<ResourceSelection, QAbstractTableModel*> m_resources;

};

#endif // LIBRARY_H
