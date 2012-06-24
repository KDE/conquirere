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

#ifndef SEARCHRESULTINFOWIDGET_H
#define SEARCHRESULTINFOWIDGET_H

#include "sidebarcomponent.h"

#include <Nepomuk2/Resource>

namespace Ui {
    class SearchResultInfoWidget;
}

class SearchResultInfoWidget : public SidebarComponent
{
    Q_OBJECT

public:
    explicit SearchResultInfoWidget(QWidget *parent = 0);
    ~SearchResultInfoWidget();

    Nepomuk2::Resource resource();

    void setResource(Nepomuk2::Resource & resource);
    void newButtonClicked();
    void deleteButtonClicked();

private:
    Ui::SearchResultInfoWidget *ui;
};

#endif // SEARCHRESULTINFOWIDGET_H
