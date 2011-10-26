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

#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <Nepomuk/Resource>
#include <Akonadi/Item>

#include <QDialog>
#include <QUrl>

namespace Ui {
    class ContactDialog;
}

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactDialog(QWidget *parent = 0);
    ~ContactDialog();

    void setResource(Nepomuk::Resource & resource, const QUrl & propertyUrl);

private slots:
    void editItem();
    void addItem();
    void contactStored( const Akonadi::Item& item);
    void removeItem();

private:
    void fillWidget();

    Ui::ContactDialog *ui;
    Nepomuk::Resource m_resource;
    QUrl m_propertyUrl;
};

#endif // CONTACTDIALOG_H
