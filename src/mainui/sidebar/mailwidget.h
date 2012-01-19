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

#ifndef MAILWIDGET_H
#define MAILWIDGET_H

#include "sidebarcomponent.h"

#include <Nepomuk/Resource>

namespace Ui {
    class MailWidget;
}

class MailWidget : public SidebarComponent
{
    Q_OBJECT

public:
    explicit MailWidget(QWidget *parent = 0);
    ~MailWidget();

    void setResource(Nepomuk::Resource & resource);
    void newButtonClicked();
    void deleteButtonClicked();

private:
    Ui::MailWidget *ui;
};

#endif // MAILWIDGET_H
