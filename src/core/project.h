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

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QFileInfo>
#include <QString>

#include <Nepomuk/Tag>

class QSettings;

class Project : public QObject
{
    Q_OBJECT
public:
    explicit Project(QObject *parent = 0);
    ~Project();

    void setName(const QString & name);
    QString name() const;

    void setPath(const QString & path);
    QString path() const;

    void createProject();
    void loadProject(const QString & projectFile);
    void deleteProject();

    Nepomuk::Tag projectTag() const;

    bool isInPath(const QString &filename);

    void addDocument(const QFileInfo &fileInfo);
    void document();

    QList<Nepomuk::File> getProjectFiles();

public slots:
    void scanProjectFolders();

private:
    void initializeProjectFolder();

    QString m_name;
    QString m_path;
    Nepomuk::Tag m_projectTag;
    QSettings *m_settings;
};

#endif // PROJECT_H
