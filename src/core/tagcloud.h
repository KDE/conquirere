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

#ifndef TAGCLOUD_H
#define TAGCLOUD_H

#include <QObject>

#include <QtCore/QPair>
#include <QtCore/QList>
#include <QtCore/QUrl>

#include <Nepomuk2/Resource>

namespace Nepomuk2 {
    class ResourceWatcher;
}

class Library;

/**
  * @brief Create the tagcloud for a @c Library to display it on the @c WelcomeWidget
  *
  * Lists only the @c pimo:Topic for all @c nbib:Publications in the Library
  */
class TagCloud : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief TagCloud Constructor
     *
     * @param lib the library which tags should be fetched
     * @param parent some parent
     */
    explicit TagCloud(Library *lib, QObject *parent = 0);
    ~TagCloud();

    /**
     * @return Returns the list of tags with the label and number of occurrence
     */
    QList<QPair<int, QString> > tagCloud();

    /**
      * @brief Pause/unpause the tagcloud generation (starts/stops the ResourceWatcher)
      *
      * Used when large data is imported
      */
    void pauseUpdates(bool pause);

signals:
    /**
     * @brief emits when the cloud changed and the Welcome widget needs to update the data
     */
    void tagCloudChanged();

private slots:
    /**
     * @brief fetches the actual tags
     */
    void generateCloud();

    /**
     * @brief adds new resources to the watcher
     */
    void addToWatcher(Nepomuk2::Resource resource ,QList<QUrl> types);

private:
    /**
     * @brief setup the ResourceWatcher
     */
    void setup();

    Library *m_library;
    Nepomuk2::ResourceWatcher* m_newWatcher;
    Nepomuk2::ResourceWatcher* m_libWatcher;
    Nepomuk2::ResourceWatcher* m_changeWatcher;

    QList<QPair<int, QString> > m_tagCloud;
    bool m_pauseUpdates;
};

#endif // TAGCLOUD_H
