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

#ifndef DOCUMENTWIDGET_H
#define DOCUMENTWIDGET_H

#include "sidebarcomponent.h"
#include <Nepomuk/Resource>

#include <QtGui/QWidget>

namespace Ui {
    class DocumentWidget;
}

class KFileMetaDataWidget;

class DocumentWidget : public SidebarComponent
{
    Q_OBJECT

public:
    explicit DocumentWidget(QWidget *parent = 0);
    ~DocumentWidget();

public slots:
    virtual void setResource(Nepomuk::Resource & resource);
    virtual void deleteButtonClicked();

private slots:
    void addPublication();
    void removePublication();

private:
    Nepomuk::Resource m_document;

    Ui::DocumentWidget *ui;
};

#endif // DOCUMENTWIDGET_H
