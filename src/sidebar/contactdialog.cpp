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
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

#include <Akonadi/Contact/ContactEditorDialog>
#include <Akonadi/ItemDeleteJob>
#include <Akonadi/Job>
#include <KABC/Addressee>

#include <KIcon>
#include <QInputDialog>

#include <QDebug>

ContactDialog::ContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactDialog)
{
    ui->setupUi(this);

    ui->editButton->setIcon(KIcon("view-pim-contacts"));
    ui->akonadiExport->setIcon(KIcon("akonadi"));
    ui->addContactButton->setIcon(KIcon("contact-new"));
    ui->addResourceButton->setIcon(KIcon("list-add"));
    ui->removeButton->setIcon(KIcon("list-remove"));

    connect(ui->editButton, SIGNAL(clicked()), this, SLOT(editItem()));
    connect(ui->akonadiExport, SIGNAL(clicked()), this, SLOT(pushContactToAkonadi()));
    connect(ui->addContactButton, SIGNAL(clicked()), this, SLOT(addContactItem()));
    connect(ui->addResourceButton, SIGNAL(clicked()), this, SLOT(addResourceItem()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeItem()));
    connect(ui->klistwidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(itemChanged(QListWidgetItem*,QListWidgetItem*)));
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
        i->setData(Qt::UserRole + 1, r.resourceUri());

        QString symbol = r.property(Soprano::Vocabulary::NAO::hasSymbol()).toString();
        if(!symbol.isEmpty()) {
            i->setIcon(KIcon(symbol));
        }
        else if(!i->data(Qt::UserRole).toString().isEmpty()){
            i->setIcon(KIcon("view-pim-contacts"));

        }

        ui->klistwidget->addItem(i);
    }
}

void ContactDialog::editItem()
{
    QListWidgetItem *i = ui->klistwidget->currentItem();

    QString akonadiItemID = i->data(Qt::UserRole).toString();

    if(akonadiItemID.isEmpty()) {
        bool ok;
        QString text = QInputDialog::getText(this, i18n("Create new Nepomuk contact"),
                                             tr("Name:"), QLineEdit::Normal,i->text(), &ok);

        if (ok && !text.isEmpty()) {
            Nepomuk::Resource contact(i->data(Qt::UserRole + 1).toString());
            contact.setProperty(Nepomuk::Vocabulary::NCO::fullname(), text);
            i->setText(text);
        }
    }
    else {
        const Akonadi::Item contact = Akonadi::Item(i->data(Qt::UserRole).toInt());

        Akonadi::ContactEditorDialog *dlg = new Akonadi::ContactEditorDialog( Akonadi::ContactEditorDialog::EditMode, this );
        dlg->setContact( contact );
        dlg->show();
    }
}

void ContactDialog::addContactItem()
{
    Akonadi::ContactEditorDialog *dlg = new Akonadi::ContactEditorDialog( Akonadi::ContactEditorDialog::CreateMode, this );
    connect( dlg, SIGNAL( contactStored( const Akonadi::Item& ) ),this, SLOT( contactStored( const Akonadi::Item& ) ) );
    dlg->show();
}

void ContactDialog::addResourceItem()
{
    bool ok;
    QString text = QInputDialog::getText(this, i18n("Create new Nepomuk contact"),
                                         tr("Contact name:"), QLineEdit::Normal,QString(), &ok);

    if (ok && !text.isEmpty()) {
        Nepomuk::Resource newContact = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::PersonContact());
        newContact.setProperty(Nepomuk::Vocabulary::NCO::fullname(), text);

        // connect new contact to resource
        m_resource.addProperty(m_propertyUrl, newContact);

        // and add it to the listwidget
        QListWidgetItem *i = new QListWidgetItem;
        i->setText(newContact.genericLabel());
        ui->klistwidget->addItem(i);
    }
}

void ContactDialog::contactStored( const Akonadi::Item& item)
{
    Nepomuk::Query::ComparisonTerm akonadiItemId( QUrl("http://akonadi-project.org/ontologies/aneo#akonadiItemId"), Nepomuk::Query::LiteralTerm( item.id() ) );

    Nepomuk::Query::Query query( akonadiItemId );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    qDebug() << "ContactDialog::contactStored found " << queryResult.size() << "created items with akonadiItemId:" << item.id();

    // take first search result or create a new resource
    Nepomuk::Resource newContact;

    QString akonadiResUrl = QLatin1String("akonadi:?item=");
    akonadiResUrl.append(QString::number(item.id()));
    newContact = Nepomuk::Resource(QUrl(akonadiResUrl), Nepomuk::Vocabulary::NCO::PersonContact());
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
    QListWidgetItem *i = ui->klistwidget->currentItem();

    QString akonadiItemID = i->data(Qt::UserRole).toString();
    QUrl resUri = i->data(Qt::UserRole + 1).toUrl();

    qDebug() << "remove item" << akonadiItemID << resUri;
    Nepomuk::Resource contactRes(resUri);

    if(!akonadiItemID.isEmpty()) {
        const Akonadi::Item contact = Akonadi::Item( i->data(Qt::UserRole).toInt() );
        Akonadi::ItemDeleteJob *job = new Akonadi::ItemDeleteJob( contact );
        m_resource.removeProperty(m_propertyUrl, contactRes);
    }
    else {
        // remove only nepomuk resource
        m_resource.removeProperty(m_propertyUrl, contactRes);
        contactRes.remove();
    }
    int row = ui->klistwidget->row(i);
    ui->klistwidget->takeItem(row);
    delete i;
}

void ContactDialog::itemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    if(!current)
        return;

    QString akonadiItemId = current->data(Qt::UserRole).toString();

    if(akonadiItemId.isEmpty()) {
        ui->akonadiExport->setEnabled(true);
    }
    else {
        ui->akonadiExport->setEnabled(false);
    }
}

void ContactDialog::pushContactToAkonadi()
{
    qDebug() << "TODO: push contact to akonadi";
}
