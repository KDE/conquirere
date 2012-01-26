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

#ifndef LISTPARTSWIDGET_H
#define LISTPARTSWIDGET_H

#include <QtGui/QWidget>
#include <Nepomuk/Resource>

namespace Ui {
    class ListPartsWidget;
}

/**
  * @brief This widget offers a simple way to edit/add/remove/create parts of a resource
  *
  * This widget is used to manipulate @c nbib:chapter of books and articles, the @c nbib:article from collections
  * such as proceedings or journal issues and the @c nbib:inSeriesOf to connect a series with its content
  * like a journal series with its single issues or a book series with all books.
  */
class ListPartsWidget : public QWidget
{
    Q_OBJECT

public:
    /**
      * Reflects what kind of reource is being manipulated
      *
      * Will be set in setResource() and defines what function will be called for the edit, add, remove create action
      */
    enum PartType {
        Chapter,
        Series,
        Collection,
        Publication
    };

    explicit ListPartsWidget(QWidget *parent = 0);
    virtual ~ListPartsWidget();

    /**
      * sets the resource for this widget
      *
      * @pre publication must be a @c nbib:Publication or @c nbib:Series
      */
    void setResource(Nepomuk::Resource resource);

    /**
      * @returns the user selected resource
      */
    Nepomuk::Resource selectedPart() const;

signals:
    /**
      * This signal gets thrown when the resource was changed and must be updated in the table model cache
      * redirects the signal from all propertywidgets
      *
      * Must be connected to the resourceUpdated signal from the widget it is contained in
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void resourceCacheNeedsUpdate(Nepomuk::Resource resource);

private slots:
    /**
      * called when the user clicks the edit button
      *
      * depending on @c m_partType one of the edit function is called.
      * @see editChapter()
      * @see editSeries()
      * @see editCollection()
      * @see editPublication()
      */
    void editPart();

    /**
      * called when the user clicks the add button
      *
      * depending on @c m_partType one of the edit function is called.
      * @see addChapter()
      * @see addSeries()
      * @see addCollection()
      * @see addPublication()
      */
    void addPart();

    /**
      * called when the user clicks the remove button
      *
      * depending on @c m_partType one of the edit function is called.
      * @see removeChapter()
      * @see removeSeries()
      * @see removeCollection()
      * @see removePublication()
      */
    void removePart();

    /**
      * called when the user clicks the delete button
      *
      * depending on @c m_partType one of the edit function is called.
      * @see deleteChapter()
      * @see deleteSeries()
      * @see deleteCollection()
      * @see deletePublication()
      */
    void deletePart();

private:
    QString showChapterString(Nepomuk::Resource publication);
    QString showSeriesOfString(Nepomuk::Resource publication);
    QString showArticleString(Nepomuk::Resource publication);

    void addChapter();
    void editChapter(Nepomuk::Resource editResource);
    void removeChapter(Nepomuk::Resource chapter);
    void deleteChapter(Nepomuk::Resource chapter);

    void addSeries();
    void editSeries(Nepomuk::Resource editResource);
    void removeSeries(Nepomuk::Resource publication);
    void deleteSeries(Nepomuk::Resource publication);

    void addCollection();
    void editCollection(Nepomuk::Resource editResource);
    void removeCollection(Nepomuk::Resource article);
    void deleteCollection(Nepomuk::Resource article);

    // for events
    void addPublication();
    void editPublication(Nepomuk::Resource editResource);
    void removePublication(Nepomuk::Resource publication);
    void deletePublication(Nepomuk::Resource publication);

    Ui::ListPartsWidget *ui;

    Nepomuk::Resource m_resource;
    PartType m_partType;
};

#endif // LISTPARTSWIDGET_H
