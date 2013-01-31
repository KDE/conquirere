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

#ifndef ITEMDELETEDIALOG_H
#define ITEMDELETEDIALOG_H

#include <QDialog>

#include <Nepomuk2/Resource>

namespace Ui {
    class ItemDeleteDialog;
}

class ItemDeleteDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        LocalDelete,
        ServerDelete,
        ServerGroupRemoval
    };

    explicit ItemDeleteDialog(Mode mode, QWidget *parent = 0);
    ~ItemDeleteDialog();

    void setItems(const QList<Nepomuk2::Resource> & items);
    void setItems(const QVariantList &items);

private slots:
    void showDetails();

private:
    void localDeleteDetails(const QList<Nepomuk2::Resource> &items);
    void serverDeleteDetails(const QVariantList &items);


    Ui::ItemDeleteDialog *ui;
    Mode m_mode;
};

#endif // ITEMDELETEDIALOG_H
