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

#ifndef SERIESEDIT_H
#define SERIESEDIT_H

#include "propertyedit.h"

class QStandardItemModel;

/**
  * @brief Used to edit the @c nbib:Series of an nbib:Publication
  *
  * Shows/edits the @c nie:title of the @c nbib:Series. The series can be a bookseries, journal, magazin or newspaper.
  *
  * When a new resource must be created (for example the publication had no series set beforehand or the inserted name
  * for the sereis didi not match an already existing resource) This class relies on the findSeriesType() function
  * to define what type of series we need to create
  *
  */
class SeriesEdit : public PropertyEdit
{
public:
    SeriesEdit(QWidget *parent = 0);

protected:
    void setupLabel();

    virtual void updateResource( const QString & text );

private:
    /**
      * Intelligent function to determine what tpe of series will be right for the publication
      *
      * For example if the resource() is an nbib:JournalIssue the type must be nbib:Journal
      *
      * @return correctNepomuk::Vocabulary::NBIB::*type*
      */
    QUrl findSeriesType();
};

#endif // SERIESEDIT_H
