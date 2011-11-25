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

#include <QWidget>
#include <Nepomuk/Resource>

namespace Ui {
    class ListPartsWidget;
}

class ListPartsWidget : public QWidget
{
    Q_OBJECT

public:
    enum PartType {
        Chapter,
        Series,
        Collection
    };

    explicit ListPartsWidget(QWidget *parent = 0);
    virtual ~ListPartsWidget();

    /**
      * sets the resource for this widget
      *
      * @pre publication must be a @c nbib:Publication
      */
    void setResource(Nepomuk::Resource resource);

    Nepomuk::Resource selectedPart() const;

signals:
    void resourceUpdated(Nepomuk::Resource resource);

private slots:
    void editPart();
    void addPart();
    void removePart();

private:
    QString showChapterString(Nepomuk::Resource publication);
    QString showSeriesOfString(Nepomuk::Resource publication);
    QString showArticleString(Nepomuk::Resource publication);

    void addChapter();
    void editChapter(Nepomuk::Resource editResource);
    void removeChapter(Nepomuk::Resource chapter);

    void addSeries();
    void editSeries(Nepomuk::Resource editResource);
    void removeSeries(Nepomuk::Resource publication);

    void addCollection();
    void editCollection(Nepomuk::Resource editResource);
    void removeCollection(Nepomuk::Resource article);

    Ui::ListPartsWidget *ui;

    Nepomuk::Resource m_resource;
    PartType m_partType;
};

#endif // LISTPARTSWIDGET_H
