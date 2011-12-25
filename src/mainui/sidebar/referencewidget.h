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
#include <Nepomuk/Resource>

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

    void setLibrary(Library *p);

public slots:
    void setResource(Nepomuk::Resource & resource);
    void showCreateReference(bool showIt);

    /**
      * Sends changes in the connected property widgets to the connected table model to update the cache
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void subResourceUpdated();

signals:
    /**
      * notify connected editwidgets to update their info
      */
    void resourceChanged(Nepomuk::Resource & resource);

private slots:
    void showPublicationList(Nepomuk::Resource & resource, const QUrl & propertyUrl);
    void showChapterList();

    /**
      * enables/disables the widgets is a valid publication is set or not
      */
    void enableReferenceDetails();
    void newButtonClicked();
    void deleteButtonClicked();
    void changeRating(int newRating);

private:
    Nepomuk::Resource m_reference;
    Ui::ReferenceWidget *ui;
};

#endif // REFERENCEWIDGET_H
