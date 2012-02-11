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

#include "nbib.h"
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

#include <QtCore/QThread>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QTimer>

using namespace Nepomuk::Vocabulary;
using namespace Soprano::Vocabulary;

TagCloud::TagCloud(QObject *parent)
    : QObject(parent)
    , m_futureWatcher(0)
{
    m_missingUpdate = false;
    m_pauseUpdates = false;
}

TagCloud::~TagCloud()
{
    delete m_futureWatcher;
}

void TagCloud::addResource(const Nepomuk::Resource &resource)
{
    if(resource.hasType(NBIB::Publication()))
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
    foreach(const Nepomuk::Resource &r, m_resourceList) {
        if(!r.isValid()) {
            m_resourceList.removeAt(i);
        }
        i++;
    }
    updateTagCloud();
}

void TagCloud::updateTagCloud()
{
    if(!m_pauseUpdates) {
        QFuture<QList<QPair<int, QString> > > future = QtConcurrent::run(this, &TagCloud::createTagCloud, m_resourceList);
        m_futureWatcher = new QFutureWatcher<QList<QPair<int, QString> > >();

        m_futureWatcher->setFuture(future);

        connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(tagCloudUpdated()));
    }
}

void TagCloud::tagCloudUpdated()
{
    QList<QPair<int, QString> > newCloud = m_futureWatcher->future().result();

    m_tagCloud = newCloud;

    emit tagCloudChanged();

    //delete m_futureWatcher;
    //m_futureWatcher = 0;
}

QList<QPair<int, QString> > TagCloud::tagCloud()
{
    return m_tagCloud;
}

void TagCloud::pauseUpdates(bool pause)
{
    m_pauseUpdates = pause;

    if(!m_pauseUpdates) {
        updateTagCloud();
    }
}

bool sortTagPair(const QPair<int, QString> &s1, const QPair<int, QString> &s2)
{
    return s1.first > s2.first;
}

QList<QPair<int, QString> > TagCloud::createTagCloud(QList<Nepomuk::Resource> resourceList)
{
    // step one create a map with all tags and their ocurence count
    QMap<QString, int> cloudMap;
    foreach(const Nepomuk::Resource &r, resourceList) {

        QList<Nepomuk::Resource> tagList = r.property(NAO::hasTopic()).toResourceList();

        foreach(const Nepomuk::Resource &t, tagList) {
            QString topicLabel = t.property(PIMO::tagLabel()).toString();
            int count = cloudMap.value(topicLabel, 0);
            count ++;
            cloudMap.insert(topicLabel, count);
        }
    }

    // step two sort the map by ocurence into a List
    QList<QPair<int, QString> > cloudList;
    QMapIterator<QString, int> iterator(cloudMap);
    while (iterator.hasNext()) {
        iterator.next();
        cloudList.append(QPair<int, QString>(iterator.value(),iterator.key()));
    }

    qSort(cloudList.begin(), cloudList.end(), sortTagPair);

    return cloudList;
}
