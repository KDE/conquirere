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

#include "nbibsync.h"

NBibSync::NBibSync(QObject *parent)
    : QObject(parent)
    , m_askBeforeDeletion(false)
    , m_mergeStrategy(Manual)
    , m_syncSteps(0)
    , m_curStep(0)
{
    qRegisterMetaType<SyncDetails>("SyncDetails");
    qRegisterMetaType<QList<SyncDetails> >("QList<SyncDetails>");
}

void NBibSync::setUserName(const QString &name)
{
    m_name = name;
}

void NBibSync::setPassword(const QString &pwd)
{
    m_pwd = pwd;
}

void NBibSync::setUrl(const QString &url)
{
    m_url = url;
}

void NBibSync::setCollection(const QString &collection)
{
    m_collection = collection;
}

void NBibSync::askBeforeDeletion(bool ask)
{
    m_askBeforeDeletion = ask;
}

void NBibSync::mergeStrategy( MergeStrategy strategy)
{
    m_mergeStrategy = strategy;
}

void NBibSync::calculateProgress(int value)
{
    qreal curProgress = ((qreal)value * 1.0/m_syncSteps);

    curProgress += (qreal)(100.0/m_syncSteps) * m_curStep;

    emit progress(curProgress);
}