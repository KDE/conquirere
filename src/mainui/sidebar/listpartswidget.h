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
#include <Nepomuk2/Resource>

namespace Ui {
    class ListPartsWidget;
}

class LibraryManager;

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

    void setLibraryManager(LibraryManager *lm);

    /**
      * sets the resource for this widget
      *
      * @pre publication must be a @c nbib:Publication or @c nbib:Series
      */
    void setResource(Nepomuk2::Resource resource);

    /**
      * @returns the user selected resource
      */
    Nepomuk2::Resource selectedPart() const;

signals:
    /**
      * This signal gets thrown when the resource was changed and must be updated in the table model cache
      * redirects the signal from all propertywidgets
      *
      * Must be connected to the resourceUpdated signal from the widget it is contained in
      *
      * @todo This should be replaced by the Nepomuk2::ResourceWatcher later
      */
    void resourceCacheNeedsUpdate(Nepomuk2::Resource resource);

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
    QString showChapterString(Nepomuk2::Resource publication);
    QString showSeriesOfString(Nepomuk2::Resource publication);
    QString showArticleString(Nepomuk2::Resource publication);

    void addChapter();
    void editChapter(Nepomuk2::Resource editResource);
    void removeChapter(Nepomuk2::Resource chapter);
    void deleteChapter(Nepomuk2::Resource chapter);

    // collections to series
    void addToSeries();
    void editFromSeries(Nepomuk2::Resource editResource);
    void removeFromSeries(Nepomuk2::Resource publication);
    void deleteFromSeries(Nepomuk2::Resource publication);

    // articles to Collections
    void addToCollection();
    void editFromCollection(Nepomuk2::Resource editResource);
    void removeFromCollection(Nepomuk2::Resource article);
    void deleteFromCollection(Nepomuk2::Resource article);

    // publication for events
    void addToEvent();
    void editFromEvent(Nepomuk2::Resource editResource);
    void removeFromEvent(Nepomuk2::Resource publication);
    void deleteFromEvent(Nepomuk2::Resource publication);

    Ui::ListPartsWidget *ui;
    LibraryManager *m_libraryManager;

    Nepomuk2::Resource m_resource;
    PartType m_partType;
};

#endif // LISTPARTSWIDGET_H
