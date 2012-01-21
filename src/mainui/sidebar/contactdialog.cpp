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
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

#include <Akonadi/Contact/ContactEditorDialog>
#include <Akonadi/CollectionDialog>
#include <Akonadi/ItemDeleteJob>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/Job>

#include <KABC/Addressee>
#include <KDE/KIcon>
#include <KDE/KInputDialog>

#include <QtCore/QPointer>

ContactDialog::ContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactDialog)
{
    ui->setupUi(this);

    ui->editButton->setIcon(KIcon("document-edit"));
    ui->akonadiExport->setIcon(KIcon("akonadi"));
    ui->addContactButton->setIcon(KIcon("contact-new"));
    ui->addResourceButton->setIcon(KIcon("list-add"));
    ui->removeButton->setIcon(KIcon("list-remove"));

    connect(ui->editButton, SIGNAL(clicked()), this, SLOT(editItem()));
    connect(ui->akonadiExport, SIGNAL(clicked()), this, SLOT(pushContactToAkonadi()));
    connect(ui->addContactButton, SIGNAL(clicked()), this, SLOT(addAkonadiContact()));
    connect(ui->addResourceButton, SIGNAL(clicked()), this, SLOT(addNepomukContact()));
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

    foreach(const Nepomuk::Resource & r, resList) {
        QListWidgetItem *i = new QListWidgetItem;
        i->setText(r.genericLabel());
        i->setData(Qt::UserRole, r.property(Nepomuk::Vocabulary::NIE::url()).toString());
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
        QString text = KInputDialog::getText(i18n("Edit Nepomuk contact"),
                                             i18nc("The name of the person", "Name:"),i->text(), &ok, this);

        if (ok && !text.isEmpty()) {
            Nepomuk::Resource contact(i->data(Qt::UserRole + 1).toString());
            contact.setProperty(Nepomuk::Vocabulary::NCO::fullname(), text);
            i->setText(text);
        }
    }
    else {
        QString strippedId = akonadiItemID.remove(QLatin1String("akonadi:?item="));
        const Akonadi::Item contact = Akonadi::Item(strippedId.toInt());

        Akonadi::ContactEditorDialog *dlg = new Akonadi::ContactEditorDialog( Akonadi::ContactEditorDialog::EditMode, this );
        dlg->setContact( contact );
        dlg->show();
    }
}

void ContactDialog::addAkonadiContact()
{
    Akonadi::ContactEditorDialog *dlg = new Akonadi::ContactEditorDialog( Akonadi::ContactEditorDialog::CreateMode, this );
    connect( dlg, SIGNAL(contactStored(Akonadi::Item)),this, SLOT(contactStored(Akonadi::Item)) );
    dlg->show();
}

void ContactDialog::addNepomukContact()
{
    bool ok;
    QString text = KInputDialog::getText(i18n("Create new Nepomuk contact"),
                                         i18n("Contact name:"), QString(), &ok, this);

    if (ok && !text.isEmpty()) {
        Nepomuk::Resource newContact = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NCO::PersonContact());
        newContact.setProperty(Nepomuk::Vocabulary::NCO::fullname(), text);

        // connect new contact to resource
        m_resource.addProperty(m_propertyUrl, newContact);

        // and add it to the listwidget
        QListWidgetItem *i = new QListWidgetItem;
        i->setText(newContact.genericLabel());
        i->setData(Qt::UserRole + 1, newContact.resourceUri());
        ui->klistwidget->addItem(i);
    }
}

void ContactDialog::contactStored( const Akonadi::Item& item)
{
    Nepomuk::Query::ComparisonTerm akonadiItemId( Nepomuk::Vocabulary::NIE::url(), Nepomuk::Query::LiteralTerm( item.url().toEncoded() ) );

    Nepomuk::Query::Query query( akonadiItemId );
    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    kDebug() << "found " << queryResult.size() << "created items with akonadiItemId:" << item.id();

    // take first search result or create a new resource
    Nepomuk::Resource newContact;

    QString akonadiResUrl = QLatin1String("akonadi:?item=");
    akonadiResUrl.append(QString::number(item.id()));
    newContact = Nepomuk::Resource(item.url(), Nepomuk::Vocabulary::NCO::PersonContact());

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
    i->setData(Qt::UserRole, item.url());
    i->setData(Qt::UserRole +1, newContact.uri());
    i->setIcon(KIcon("view-pim-contacts"));
    ui->klistwidget->addItem(i);
}

void ContactDialog::removeItem()
{
    QListWidgetItem *i = ui->klistwidget->currentItem();

    QUrl resUri = i->data(Qt::UserRole + 1).toUrl();
    Nepomuk::Resource contactRes(resUri);
    m_resource.removeProperty(m_propertyUrl, contactRes);

    int row = ui->klistwidget->row(i);
    ui->klistwidget->takeItem(row);
    delete i;
}

void ContactDialog::itemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    Q_UNUSED(previous)

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
    // Show the user a dialog to select a writable collection of contacts
    QPointer<Akonadi::CollectionDialog> dlg = new Akonadi::CollectionDialog( Akonadi::CollectionDialog::AllowToCreateNewChildCollection, 0, this );
    dlg->setMimeTypeFilter( QStringList() << KABC::Addressee::mimeType() );
    dlg->setAccessRightsFilter( Akonadi::Collection::CanCreateItem);
    dlg->setDescription( i18n( "Select an address book:" ) );

    int ret = dlg->exec();
    if ( ret == KDialog::Accepted ) {
        const Akonadi::Collection collection = dlg->selectedCollection();

        QListWidgetItem *i = ui->klistwidget->currentItem();

        QUrl resUri = i->data(Qt::UserRole + 1).toUrl();
        Nepomuk::Resource contactRes(resUri);

        KABC::Addressee addr;
        addr.setName( contactRes.property(Nepomuk::Vocabulary::NCO::fullname()).toString() );
        addr.setFamilyName( contactRes.property(Nepomuk::Vocabulary::NCO::fullname()).toString() );
        addr.setFormattedName( contactRes.property(Nepomuk::Vocabulary::NCO::fullname()).toString() );

        Akonadi::Item item;
        item.setMimeType( KABC::Addressee::mimeType() );
        item.setPayload<KABC::Addressee>( addr );

        Akonadi::ItemCreateJob *job = new Akonadi::ItemCreateJob( item, collection );

        if ( !job->exec() ) {
            qDebug() << "Error:" << job->errorString();
        }

        contactRes.setProperty(Nepomuk::Vocabulary::NIE::url(), QUrl(job->item().url()));
        //contactRes.addType( QUrl("http://akonadi-project.org/ontologies/aneo#AkonadiDataObject"));
        //contactRes.setProperty("http://akonadi-project.org/ontologies/aneo#akonadiItemId",job->item().id() );

        i->setData(Qt::UserRole, job->item().url().toEncoded());
        i->setIcon(KIcon("view-pim-contacts"));
    }

    delete dlg;
}
