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

#ifndef DATEEDIT_H
#define DATEEDIT_H

#include "propertyedit.h"

#include <QtCore/QDate>

class QMenu;
class QWidgetAction;
class KDatePicker;

/**
  * @brief Edits the @c nbib:publicationDate or the @c nbib:accessDate of an @c nbib:Publication
  *
  * The date is saved as Qt:IsoDate @c "yyyy-MM-ddTHH:mm:ssZ"
  */
class DateEdit : public PropertyEdit
{
    Q_OBJECT
public:
    DateEdit(QWidget *parent = 0);
    virtual ~DateEdit();

private slots:
    void detailEditRequested();
    void dateChanged(QDate date);
    void editingFinished();
    void editingAborted();

private:
    virtual void mousePressEvent ( QMouseEvent * event );
    void setupLabel();

    void updateResource( const QString & text );

    QMenu *m_dateMenu;
    QWidgetAction *m_datePickerAction;
    KDatePicker *m_datePicker;
};

#endif // DATEEDIT_H
