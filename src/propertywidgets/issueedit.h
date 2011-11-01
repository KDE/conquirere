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

#ifndef ISSUEEDIT_H
#define ISSUEEDIT_H

#include "propertyedit.h"

#include <QUrl>
/**
  * @brief Used to edit a single Issue
  *
  * Shows/edits the @c nie:title of the @c nbib:Series from the used @c nbib:Collection (issue)
  *
  * @pre resource input must be an nbib:Article
  */
class IssueEdit : public PropertyEdit
{
public:
    IssueEdit(QWidget *parent = 0);

protected:
    void setupLabel();

    virtual void createCompletionModel( const QList< Nepomuk::Query::Result > &entries );

    virtual void updateResource( const QString & text );

private slots:
    void detailEditRequested();

private:
    QUrl m_issueType;
    QUrl m_seriesType;
};

#endif // ISSUEEDIT_H
