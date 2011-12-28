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

#include <Nepomuk/Resource>

namespace Ui {
    class SeriesWidget;
}

/**
  * @brief Widget to manipulate the @c nbib:Series which contain many other @c nbib:Publication
  *
  * Here new @c nbib:Series can be created and connected to other publication via @c nbib:inSeries and @c nbib:seriesOf
  *
  * For each series only the @c nie:title, nbib:issn, the rating will be altered directly in this widget.
  * The lublication list is handled by the @c ListPartsWidget
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
      *
      * @pre @p resource musst be of type @c nbib:Series
      */
    void setResource(Nepomuk::Resource & resource);

signals:
    /**
      * notify connected editwidgets to update their info
      */
    void resourceChanged(Nepomuk::Resource & resource);

private slots:
    /**
      * Change the @c type of the resource to the selected new type
      *
      * Also adjust all publication to the proper selection.
      * For example if the Series was a @c Journal and the publication a @c JournalIssue collection with several @c Article
      * and we change the series to be a @c Newspapger also the connected collection will be changed to a @c NewspaperIssue
      */
    void newSeriesTypeSelected(int index);

    /**
      * Creates a new @c nbib:Series resource with a default @c nie:title
      */
    void newButtonClicked();

    /**
      * Deletes the series and removes it from all connected publications
      */
    void deleteButtonClicked();

    /**
      * Updates the rating of the Series to @p newRating
      */
    void changeRating(int newRating);

private:
    void setupWidget();

    Ui::SeriesWidget *ui;
    Nepomuk::Resource m_series;
};

#endif // SERIESWIDGET_H
