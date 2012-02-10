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

#ifndef TAGEDIT_H
#define TAGEDIT_H

#include "propertyedit.h"

class KJob;

/**
  * @brief simple linedit to add/remove Tags @c nao:Tag or @c pimo:Topic to a resource
  */
class TagEdit : public PropertyEdit
{
    Q_OBJECT
public:
    explicit TagEdit(QWidget *parent = 0);

    void setPropertyUrl(const QUrl & m_propertyUrl);

private slots:
    void addTags(KJob *job);

private:
    void setupLabel();
    void updateResource( const QString & text );

    // cache the resource used for the asynchron change.
    // otherwise if we switch to a different resource while the KJob
    // hasn't finished yet, we add the tags to the wrong resource
    Nepomuk::Resource m_editedResource;
};

#endif // TAGEDIT_H
