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

#ifndef LISTPUBLICATIONSDIALOG_H
#define LISTPUBLICATIONSDIALOG_H

#include <Nepomuk/Resource>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>

#include <QtGui/QDialog>

namespace Ui {
    class ListPublicationsDialog;
}

class Library;

class ListPublicationsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ListPublicationsDialog(QWidget *parent = 0);
    ~ListPublicationsDialog();

    void setLibrary(Library *p);

    Nepomuk::Resource selectedPublication();

    void keyPressEvent(QKeyEvent * e);

private slots:
    void applyFilter();
    void headerContextMenu(const QPoint &pos);
    void changeHeaderSectionVisibility();

private:
    Ui::ListPublicationsDialog *ui;

    Library *m_library;
};

#endif // LISTPUBLICATIONSDIALOG_H
