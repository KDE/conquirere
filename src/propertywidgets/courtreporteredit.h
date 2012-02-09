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

#ifndef COURTREPORTEREDIT_H
#define COURTREPORTEREDIT_H

#include "propertyedit.h"

class KJob;

/**
  * @brief Used to Edit the @c nbib:CourtReporter of an nbib:Publication
  */
class CourtReporterEdit : public PropertyEdit
{
    Q_OBJECT
public:
    explicit CourtReporterEdit(QWidget *parent = 0);

private:
    void setupLabel();

    void updateResource( const QString & text );
};

#endif // COURTREPORTEREDIT_H
