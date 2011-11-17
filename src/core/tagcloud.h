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

#include <Nepomuk/Resource>

#include <QtCore/QFutureWatcher>
#include <QtCore/QUrl>
#include <QtCore/QPair>
#include <QtCore/QList>

class TagCloud : public QObject
{
    Q_OBJECT
public:
    explicit TagCloud(QObject *parent = 0);

    QList<QPair<int, QString> > tagCloud();

    void pauseUpdates(bool pause);

public slots:
    void addResource(const Nepomuk::Resource &resource);
    void updateResource(const Nepomuk::Resource &resource);
    void removeResource(const QUrl &resourceUrl);

    void updateTagCloud();

signals:
    void tagCloudChanged();

private slots:
    void tagCloudUpdated();

private:
    QList<QPair<int, QString> > createTagCloud(QList<Nepomuk::Resource> resourceList);

    QFutureWatcher<QList<QPair<int, QString> > > *m_futureWatcher;
    QList<QPair<int, QString> > m_tagCloud;
    QList<Nepomuk::Resource> m_resourceList;
    bool m_missingUpdate;
    bool m_pauseUpdates;
};

#endif // TAGCLOUD_H
