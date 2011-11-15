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

#include "tagcloud.h"

#include <Nepomuk/Tag>

#include <QtCore/QThread>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QTimer>

TagCloud::TagCloud(QObject *parent)
    : QObject(parent)
    , m_futureWatcher(0)
{
    m_missingUpdate = false;
}

void TagCloud::addResource(const Nepomuk::Resource &resource)
{
    m_resourceList.append(resource);
    updateTagCloud();
}

void TagCloud::updateResource(const Nepomuk::Resource &resource)
{
    updateTagCloud();
}

void TagCloud::removeResource(const QUrl &resourceUrl)
{
    int i = 0;
    foreach(Nepomuk::Resource r, m_resourceList) {
        if(!r.isValid()) {
            m_resourceList.removeAt(i);
        }
        i++;
    }
    updateTagCloud();
}

void TagCloud::updateTagCloud()
{
    if(m_futureWatcher && m_futureWatcher->isRunning()) {
        if(!m_missingUpdate) {
            m_missingUpdate = true;
            QTimer::singleShot(3000,this,SLOT(updateTagCloud()));
        }
        return;
    }

    QFuture<QList<QPair<int, QString> > > future = QtConcurrent::run(this, &TagCloud::createTagCloud, m_resourceList);
    m_futureWatcher = new QFutureWatcher<QList<QPair<int, QString> > >();

    m_futureWatcher->setFuture(future);

    connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(tagCloudUpdated()));

    m_missingUpdate = false;
}

void TagCloud::tagCloudUpdated()
{
    QList<QPair<int, QString> > newCloud = m_futureWatcher->future().result();

    m_tagCloud = newCloud;

    emit tagCloudChanged();
}

QList<QPair<int, QString> > TagCloud::tagCloud()
{
    return m_tagCloud;
}

bool sortTagPair(const QPair<int, QString> &s1, const QPair<int, QString> &s2)
{
    return s1.first > s2.first;
}

QList<QPair<int, QString> > TagCloud::createTagCloud(QList<Nepomuk::Resource> resourceList)
{
    // step one create a map with all tags and their ocurence count
    QMap<QString, int> cloudMap;
    foreach(Nepomuk::Resource r, resourceList) {

        QList<Nepomuk::Tag> tagList = r.tags();

        foreach(Nepomuk::Tag t, tagList) {
            int count = cloudMap.value(t.label(), 0);
            count ++;
            cloudMap.insert(t.label(), count);
        }
    }

    // step two sort the map by occurence into a List
    QList<QPair<int, QString> > cloudList;
    QMapIterator<QString, int> iterator(cloudMap);
    while (iterator.hasNext()) {
        iterator.next();
        cloudList.append(QPair<int, QString>(iterator.value(),iterator.key()));
    }

    qSort(cloudList.begin(), cloudList.end(), sortTagPair);

    return cloudList;
}
