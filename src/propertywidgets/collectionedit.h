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

#ifndef COLLECTIONEDIT_H
#define COLLECTIONEDIT_H

#include "propertyedit.h"

#include <QUrl>

class QStandardItemModel;

/**
  *
  * @pre propertyUrl() must return a nbib:Collection type
  */
class CollectionEdit : public PropertyEdit
{
    Q_OBJECT
public:
    explicit CollectionEdit(QWidget *parent = 0);

protected:
    /**
      * Shows the @c nie:title of the collection connected to the nbib:article
      */
    void setupLabel();

    /**
      * fills the QCompleter with the titles of all available nbib:collections
      */
    virtual QStandardItemModel* createCompletionModel( const QList< Nepomuk::Query::Result > &entries );

    /**
      * changes the @c nie:title
      */
    virtual void updateResource( const QString & text );

private slots:
    /**
      * opens a dialog to switch the type of collection/issue used
      *
      * Thus one can change from a standard nbib:collection to nbib:journalIssue or nbib:newspaperIssue
      * the Series will be changed automatically with it
      */
    void detailEditRequested();

private:
    QUrl m_collectionType;  /**< saves which subclass of collection is used */
    QUrl m_seriesType; /**< saves which subclass of series is used */

};

#endif // COLLECTIONEDIT_H
