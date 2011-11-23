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
    explicit ListPartsWidget(QWidget *parent = 0);
    virtual ~ListPartsWidget();

    void setPublication(Nepomuk::Resource publication);

    Nepomuk::Resource selectedPart() const;

private slots:
    void editPart();
    void addPart();
    void removePart();

private:
    Ui::ListPartsWidget *ui;

    Nepomuk::Resource m_publication;
};

#endif // LISTPARTSWIDGET_H
