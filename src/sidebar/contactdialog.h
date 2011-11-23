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

#include <QtGui/QDialog>
#include <QtCore/QUrl>

namespace Ui {
    class ContactDialog;
}

class QListWidgetItem;

/**
  * @brief Shows a new dialog to edit contacts in deeper detail
  *
  * Allows to create ne contacts eitehr in Nepomuk or as Akonadi::Item
  * can push already existing nepomuk contacts to an akonadi adressbook collection
  *
  */
class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactDialog(QWidget *parent = 0);
    virtual ~ContactDialog();

    /**
      * Sets the resource which will be editied
      *
      * @p resource the resource which need additional contact details
      * @p propertyUrl the property that should be changed for example nco:creator or nco:editor
      */
    void setResource(Nepomuk::Resource & resource, const QUrl & propertyUrl);

private slots:
    /**
      * edits a contact either via a simple fullname string for nepomuk only contacts or via Akonadi::ContactEditorDialog
      */
    void editItem();

    /**
      * Asks for the fullname of the contact via KInputDialog::getText() and creates a @c nco:PersonContact from it
      *
      * The inserted name corresponds to the nco:fullname property
      */
    void addNepomukContact();

    /**
      * opens a Akonadi::ContactEditorDialog to create a new contact.
      *
      * calls contactStored() when the dialog is closed
      */
    void addAkonadiContact();

    /**
      * create nepomuk resource from the akonadi item and connects it to the publication
      *
      * @todo fix resource duplication when a new akonadi item is created
      */
    void contactStored( const Akonadi::Item& item);

    /**
      * removes the contact from the publication again
      *
      * does not delete the resource
      */
    void removeItem();

    /**
      * enables / disables the akonadi export button when the selection is changed
      *
      * only enables akonadi export if the item is not available in akonadi already
      */
    void itemChanged(QListWidgetItem* current, QListWidgetItem* previous);

    /**
      * creates a akonadi contact item from the current nepomuk resource
      *
      * uses Akonadi::CollectionDialog to show the collection selection and creates a new
      * akonadi item in there
      *
      * @todo fix resource duplication when a new akonadi item is created
      */
    void pushContactToAkonadi();

private:
    void fillWidget();

    Ui::ContactDialog *ui;
    Nepomuk::Resource m_resource;
    QUrl m_propertyUrl;
};

#endif // CONTACTDIALOG_H
