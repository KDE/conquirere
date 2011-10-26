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

#include "contactdialog.h"
#include "ui_contactdialog.h"

#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

#include <Akonadi/Contact/ContactEditorDialog>
#include <KABC/Addressee>

#include <KIcon>

#include <QDebug>

ContactDialog::ContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactDialog)
{
    ui->setupUi(this);

    ui->editButton->setIcon(KIcon("view-pim-contacts"));
    ui->addButton->setIcon(KIcon("list-add"));
    ui->removeButton->setIcon(KIcon("list-remove"));

    connect(ui->editButton, SIGNAL(clicked()), this, SLOT(editItem()));
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addItem()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeItem()));
}

ContactDialog::~ContactDialog()
{
    delete ui;
}

void ContactDialog::setResource(Nepomuk::Resource & resource, const QUrl & propertyUrl)
{
    m_resource = resource;
    m_propertyUrl = propertyUrl;

    fillWidget();
}

void ContactDialog::fillWidget()
{
    QList<Nepomuk::Resource> resList = m_resource.property(m_propertyUrl).toResourceList();

    foreach(Nepomuk::Resource r, resList) {
        QListWidgetItem *i = new QListWidgetItem;
        i->setText(r.genericLabel());
        i->setData(Qt::UserRole, r.property("http://akonadi-project.org/ontologies/aneo#akonadiItemId").toString());
        ui->klistwidget->addItem(i);
    }
}

void ContactDialog::editItem()
{
    QListWidgetItem *i = ui->klistwidget->currentItem();
    qDebug() << "edit item " << i->text() << "with akonadiID :: " << i->data(Qt::UserRole).toString();

    const Akonadi::Item contact = Akonadi::Item(i->data(Qt::UserRole).toInt());

    Akonadi::ContactEditorDialog *dlg = new Akonadi::ContactEditorDialog( Akonadi::ContactEditorDialog::EditMode, this );
    //connect( dlg, SIGNAL( contactStored( const Akonadi::Item& ) ),this, SLOT( contactStored( const Akonadi::Item& ) ) );
    dlg->setContact( contact );
    dlg->show();
}

void ContactDialog::addItem()
{
    Akonadi::ContactEditorDialog *dlg = new Akonadi::ContactEditorDialog( Akonadi::ContactEditorDialog::CreateMode, this );
    connect( dlg, SIGNAL( contactStored( const Akonadi::Item& ) ),this, SLOT( contactStored( const Akonadi::Item& ) ) );
    dlg->show();
}

void ContactDialog::contactStored( const Akonadi::Item& item)
{
    // at this point a new akonadi resource is created
    // with some luck the akonadi feeder already pushed the resource into nepomuk
    // if not, we create a new resource with the same akonadiItemId.
    // this allows to connect the new contact to the publication and later on we could merge
    // the two resources back together while still allow all necessary edit options

    Nepomuk::Query::ComparisonTerm akonadiItemId( QUrl("http://akonadi-project.org/ontologies/aneo#akonadiItemId"), Nepomuk::Query::LiteralTerm( item.id() ) );

    Nepomuk::Query::Query query( akonadiItemId );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    qDebug() << "ContactDialog::contactStored found " << queryResult.size() << "created items with akonadiItemId:" << item.id();

    // take first search result or create a new resource
    Nepomuk::Resource newContact;
    if(!queryResult.isEmpty()) {
        newContact = queryResult.first().resource();
    }
    else {
        newContact = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::PersonContact());
        newContact.setProperty("http://akonadi-project.org/ontologies/aneo#akonadiItemId", item.id());

        if (item.hasPayload<KABC::Addressee>())
        {
            KABC::Addressee addr = item.payload<KABC::Addressee>();

            QString name = addr.name();
            if(name .isEmpty()) {
                name = addr.formattedName();
            }

            newContact.setProperty(Nepomuk::Vocabulary::NCO::fullname(), name);
        }
    }

    // connect new contact to resource
    m_resource.addProperty(m_propertyUrl, newContact);

    // and add it to the listwidget
    QListWidgetItem *i = new QListWidgetItem;
    i->setText(newContact.genericLabel());
    i->setData(Qt::UserRole, item.id());
    ui->klistwidget->addItem(i);
}

void ContactDialog::removeItem()
{

}
