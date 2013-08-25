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

#ifndef REFERENCEWIDGET_H
#define REFERENCEWIDGET_H

#include "sidebarcomponent.h"

#include <Nepomuk2/Resource>

namespace Ui {
    class ReferenceWidget;
}

/**
  * @brief shows a widget to manipulate the @c nbib:Reference for a @c nbib:Publication
  *
  */
class ReferenceWidget : public SidebarComponent
{
    Q_OBJECT
public:
    explicit ReferenceWidget(QWidget *parent = 0);

    void setLibraryManager(LibraryManager *lm);
    Nepomuk2::Resource resource();

public slots:
    /**
      * Sets the recource that will be altered here
      *
      * @pre @p resource must be of type @c nbib:Reference
      */
    void setResource(Nepomuk2::Resource & resource);

    void newButtonClicked();

private slots:
    void showPublicationList(Nepomuk2::Resource & resource, const QUrl & propertyUrl);
    void showChapterList();
    void showCiteKeySuggetion();

    /**
      * enables/disables the widgets if a valid publication is set or not
      */
    void enableReferenceDetails();
    void deleteButtonClicked();
    void changeRating(uint newRating);

private:
    Nepomuk2::Resource m_reference;
    Ui::ReferenceWidget *ui;
};

#endif // REFERENCEWIDGET_H
