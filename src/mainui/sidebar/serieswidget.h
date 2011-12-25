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

#ifndef SERIESWIDGET_H
#define SERIESWIDGET_H

#include "sidebarcomponent.h"
#include "globals.h"

#include <Nepomuk/Resource>

namespace Ui {
    class SeriesWidget;
}

/**
  * @brief Widget to manipulate the @c nbib:Series which contain many other @c nbib:Publication
  *
  */
class SeriesWidget : public SidebarComponent
{
    Q_OBJECT

public:
    explicit SeriesWidget(QWidget *parent = 0);
    virtual ~SeriesWidget();

    /** @todo remove and use ResourceWatcher later on */
    void setLibrary(Library *p);

public slots:
    /**
      * called when something is selected in the project view
      */
    void setResource(Nepomuk::Resource & resource);

signals:
    /**
      * notify connected editwidgets to update their info
      */
    void resourceChanged(Nepomuk::Resource & resource);

private slots:
    void newSeriesTypeSelected(int index);
    void newButtonClicked();
    void deleteButtonClicked();

    void changeRating(int newRating);

private:
    void setupWidget();

    Ui::SeriesWidget *ui;
    Nepomuk::Resource m_series;
};

#endif // SERIESWIDGET_H
