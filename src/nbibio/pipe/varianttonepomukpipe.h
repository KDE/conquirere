/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef VARIANTTONEPOMUKPIPE_H
#define VARIANTTONEPOMUKPIPE_H

#include <QtCore/QObject>
#include <QtCore/QVariantList>
#include <QtCore/QVariantMap>

#include <Nepomuk2/Resource>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>

class VariantToNepomukPipe : public QObject
{
    Q_OBJECT
public:
    explicit VariantToNepomukPipe(QObject *parent = 0);

    /**
      * Does the piping action
      *
      * @p resources list of publications
      */
    void pipeExport(QVariantList &publicationList);

    void setSyncDetails(const QString &url, const QString &userid, const QString &providerId);

    /**
      * if the @p projectThing is valid all imported data will be related via @c pimo:isRelated to the project
      */
    void setProjectPimoThing(Nepomuk2::Resource projectThing);

signals:
    void progress(int value);

private:
    void importNote(const QVariantMap &noteEntry);
    void importAttachment(const QVariantMap &attachmentEntry);

    void addZoteroSyncDetails(Nepomuk2::SimpleResourceGraph &graph, Nepomuk2::SimpleResource &mainResource, Nepomuk2::SimpleResource &referenceResource,const QVariantMap &item);

private:
    QString m_syncUrl;
    QString m_syncUserId;
    QString m_syncProviderId;
    Nepomuk2::Resource m_projectThing;
};

#endif // VARIANTTONEPOMUKPIPE_H
