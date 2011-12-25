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

#ifndef ADDCHAPTERDIALOG_H
#define ADDCHAPTERDIALOG_H

#include <QDialog>

#include <Nepomuk/Resource>

namespace Ui {
    class AddChapterDialog;
}

/**
  * @brief Small dialog to add details to a new or existing chapter
  *
  * Allows to add the @c nbib:number, @ nbib:title, @c nbib:author, @c nbib:pageStart and @c nbib:pageEnd
  */
class AddChapterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddChapterDialog(QWidget *parent = 0);
    virtual ~AddChapterDialog();

    /**
      * Sets the DocumentPart resource. This must be valid
      *
      * This is the part that is being edited
      *
      * @pre valid @c nbib:Chapter resource
      */
    void setResource(Nepomuk::Resource resource);

    /**
      * sets the publication where the new chapter should be aded to
      *
      * @pre valid @c nbib:Publication resource
      */
    void setPublication(Nepomuk::Resource resource);

    /**
      * @return The newly crated @c nbib:Chapter
      */
    Nepomuk::Resource resource();

public slots:
    void accept();

    /**
      * Opens a new dialog to add contacts in more detail
      *
      * @see ContactDialog
      */
    void editContactDialog(Nepomuk::Resource & resource, const QUrl & propertyUrl);

private:
    Ui::AddChapterDialog *ui;

    Nepomuk::Resource m_resource;
    Nepomuk::Resource m_publication;
};

#endif // ADDCHAPTERDIALOG_H
