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

#ifndef PROJECTTREEWIDGET_H
#define PROJECTTREEWIDGET_H

#include <QWidget>

#include "../../globals.h"

namespace Ui {
    class ProjectTreeWidget;
}

class ProjectTreeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectTreeWidget(QWidget *parent = 0);
    ~ProjectTreeWidget();

signals:
    void newSelection(ResourceSelection selection);

private slots:
    void selectionchanged();

private:
    void setupWidget();

    Ui::ProjectTreeWidget *ui;
};

#endif // PROJECTTREEWIDGET_H
