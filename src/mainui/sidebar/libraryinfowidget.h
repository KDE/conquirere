/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef LIBRARYINFOWIDGET_H
#define LIBRARYINFOWIDGET_H

#include "sidebarcomponent.h"

#include <Nepomuk2/Resource>

namespace Ui {
    class LibraryInfoWidget;
}

class KProgressDialog;

/**
  * @brief SidebarWidget to show some Library details and some interaction buttons
  *
  * Allows to interact with a Library via t heLibraryManager
  */
class LibraryInfoWidget : public SidebarComponent
{
    Q_OBJECT

public:
    explicit LibraryInfoWidget(QWidget *parent = 0);
    ~LibraryInfoWidget();

    Nepomuk2::Resource resource();

    void setLibrary(Library *p);

    void setResource(Nepomuk2::Resource & resource);
    void newButtonClicked();
    void deleteButtonClicked();

private slots:
    void importData();
    void exportData();
    void openSettings();
    void syncData();
    void closeLibrary();
    void deleteLibrary();

    void setSyncProgress(int value);
    void setSyncStatus(const QString &status);
    void syncFinished();

private:
    void setupUI();
    Ui::LibraryInfoWidget *ui;
    Library * m_curLibrary;
    KProgressDialog *m_kpd;
};

#endif // LIBRARYINFOWIDGET_H
