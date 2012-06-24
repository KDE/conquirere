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

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include "sro/nco/personcontact.h"
#include "sro/aneo/akonadidataobject.h"

#include <Nepomuk2/Variant>
#include <Nepomuk2/Vocabulary/NCO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Query/QueryServiceClient>
#include <Nepomuk2/Query/ComparisonTerm>
#include <Nepomuk2/Query/LiteralTerm>
#include <Nepomuk2/Query/Result>
#include <Nepomuk2/Query/QueryParser>

#include <Akonadi/Contact/ContactEditorDialog>
#include <Akonadi/CollectionDialog>
#include <Akonadi/ItemDeleteJob>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/Job>

#include <KABC/Addressee>
#include <KDE/KIcon>
#include <KDE/KInputDialog>

#include <QtCore/QPointer>

using namespace Nepomuk2::Vocabulary;

const int AKONADI_URL = Qt::UserRole + 100;
const int NEPOMUK_RESOURCE = Qt::UserRole + 101;

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
    connect(ui->klistwidget, SIGNAL(itemSelectionChanged()),this, SLOT(itemSelectionChanged()));
}

ContactDialog::~ContactDialog()
{
    delete ui;
}

void ContactDialog::setResource(Nepomuk2::Resource & resource, const QUrl & propertyUrl)
{
    m_resource = resource;
    m_propertyUrl = propertyUrl;

    fillWidget();
}

void ContactDialog::fillWidget()
{
    QList<Nepomuk2::Resource> resList = m_resource.property(m_propertyUrl).toResourceList();

    foreach(const Nepomuk2::Resource & r, resList) {
        QListWidgetItem *i = new QListWidgetItem;
        i->setText(r.genericLabel());
        i->setData(AKONADI_URL, r.property(Nepomuk2::Vocabulary::NIE::url()).toString());
        i->setData(NEPOMUK_RESOURCE, r.resourceUri());

        QString symbol = r.property(Soprano::Vocabulary::NAO::hasSymbol()).toString();
        if(!symbol.isEmpty()) {
            i->setIcon(KIcon(symbol));
        }
        else if(!i->data(AKONADI_URL).toString().isEmpty()){
            i->setIcon(KIcon("view-pim-contacts"));

        }

        ui->klistwidget->addItem(i);
    }

    if(resList.isEmpty()) {
        ui->editButton->setEnabled(false);
        ui->akonadiExport->setEnabled(false);
        ui->removeButton->setEnabled(false);
    }
}

void ContactDialog::editItem()
{
    QListWidgetItem *i = ui->klistwidget->currentItem();

    QString akonadiItemID = i->data(AKONADI_URL).toString();

    if(akonadiItemID.isEmpty()) {
        bool ok;
        QString text = KInputDialog::getText(i18n("Edit Nepomuk contact"),
                                             i18nc("The name of the person", "Name:"),i->text(), &ok, this);

        if (ok && !text.isEmpty()) {
            // change name via DMS

            QList<QUrl> resourceUris; resourceUris << i->data(NEPOMUK_RESOURCE).toString();
            QVariantList value; value << text;
            Nepomuk2::setProperty(resourceUris, NCO::fullname(), value);

            i->setText(text);
        }
    }
    else {
        QString strippedId = akonadiItemID.remove(QLatin1String("akonadi:?item="));
        const Akonadi::Item contact = Akonadi::Item(strippedId.toInt());

        Akonadi::ContactEditorDialog *dlg = new Akonadi::ContactEditorDialog( Akonadi::ContactEditorDialog::EditMode, this );
        connect( dlg, SIGNAL(contactStored(Akonadi::Item)),this, SLOT(contactUpdated(Akonadi::Item)) );
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

        //create new contact via Nepomuk DMS
        Nepomuk2::SimpleResourceGraph graph;
        Nepomuk2::NCO::PersonContact contact;

        contact.setFullname( text );
        contact.setProperty(Soprano::Vocabulary::NAO::prefLabel(), text);

        graph << contact;

        //blocking graph save
        Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew );
        if( !srj->exec() ) {
            kWarning() << "could not create new contact" << srj->errorString();
            return;
        }

        Nepomuk2::Resource contactResource = Nepomuk2::Resource::fromResourceUri( srj->mappings().value( contact.uri() ) );

        // because we could not create the links via the SimpleResource method, we add 2 additional calls to do them now
        QList<QUrl> resUri; resUri << m_resource.resourceUri();
        QVariantList value; value << contactResource.resourceUri();
        Nepomuk2::addProperty(resUri, m_propertyUrl, value);

        kDebug() << "add resource" << contactResource << contactResource.resourceUri() << "to publication";

        // and add it to the listwidget
        QListWidgetItem *i = new QListWidgetItem;
        i->setText( text );
        i->setData(NEPOMUK_RESOURCE, contactResource.resourceUri());
        ui->klistwidget->addItem(i);

        ui->editButton->setEnabled(true);
        ui->removeButton->setEnabled(true);

        ui->klistwidget->setCurrentRow(0);
    }
}

void ContactDialog::contactStored( const Akonadi::Item& item)
{
    // ok Akonadi saved its data and is about to stream it into nepomuk
    // we create a new Contact resource here that contains the aknadi specific url
    // the Nepomuk DMS system will merge them correctly

    QString contactFullname;
    if (item.hasPayload<KABC::Addressee>())
    {
        KABC::Addressee addr = item.payload<KABC::Addressee>();

        contactFullname = addr.name();
        if(contactFullname.isEmpty()) {
            contactFullname = addr.formattedName();
        }
    }

    //create new contact via Nepomuk DMS
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NCO::Contact contact;
    contact.addType(NIE::InformationElement());
    contact.addType(NIE::DataObject());

    contact.setProperty(NIE::url(), item.url());

    graph << contact;

    //blocking graph save
    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew );
    if( !srj->exec() ) {
        kWarning() << "could not create new akonadi contact" << srj->errorString();
        return;
    }
    Nepomuk2::Resource contactResource = Nepomuk2::Resource::fromResourceUri( srj->mappings().value( contact.uri() ) );

    // because we could not create the links via the SimpleResource method, we add 2 additional calls to do them now
    QList<QUrl> resUri; resUri << m_resource.resourceUri();
    QVariantList value; value << contactResource.resourceUri();
    Nepomuk2::addProperty(resUri, m_propertyUrl, value);

    // and add it to the listwidget
    QListWidgetItem *i = new QListWidgetItem;
    i->setText( contactFullname );
    i->setData(AKONADI_URL, item.url().toEncoded());
    i->setData(NEPOMUK_RESOURCE, contactResource.resourceUri());
    i->setIcon(KIcon("view-pim-contacts"));
    ui->klistwidget->addItem(i);

    ui->editButton->setEnabled(true);
    ui->removeButton->setEnabled(true);

    ui->klistwidget->setCurrentRow(0);
}

void ContactDialog::contactUpdated( const Akonadi::Item& item)
{
    QString contactFullname;
    if (item.hasPayload<KABC::Addressee>())
    {
        KABC::Addressee addr = item.payload<KABC::Addressee>();

        contactFullname = addr.name();
        if(contactFullname.isEmpty()) {
            contactFullname = addr.formattedName();
        }
    }

    // fake update of the listwidget. Whe nwe close the dialog the nepomuk resource is reloaded and
    // the real data as pushed from akinadi is shown instead
    QListWidgetItem *i = ui->klistwidget->currentItem();
    i->setText( contactFullname );
}

void ContactDialog::removeItem()
{
    QListWidgetItem *i = ui->klistwidget->currentItem();

    QUrl contactUri = i->data(NEPOMUK_RESOURCE).toUrl();

    QList<QUrl> resourceUris; resourceUris << m_resource.resourceUri();
    QVariantList value; value << contactUri;
    Nepomuk2::removeProperty(resourceUris, m_propertyUrl, value);

    int row = ui->klistwidget->row(i);
    ui->klistwidget->takeItem(row);
    delete i;

    if(ui->klistwidget->count() == 0) {
        ui->editButton->setEnabled(false);
        ui->akonadiExport->setEnabled(false);
        ui->removeButton->setEnabled(false);
    }
}

void ContactDialog::itemSelectionChanged()
{
    if( !ui->klistwidget->currentItem() )
        return;

    QString akonadiItemId = ui->klistwidget->currentItem()->data(AKONADI_URL).toString();

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

        Nepomuk2::Resource contactRes = Nepomuk2::Resource::fromResourceUri(i->data(NEPOMUK_RESOURCE).toUrl());

        QString fullname = contactRes.property(Nepomuk2::Vocabulary::NCO::fullname()).toString();
        QString name;
        QString familyname;
        QStringList splittedName = fullname.split(QLatin1String(" "));
        if(splittedName.size() >= 2) {
            name = splittedName.first();
            familyname = splittedName.last();
        }
        else {
            familyname = fullname;
        }

        // clean the resource
//        QList<QUrl> conatctUri; conatctUri << i->data(NEPOMUK_RESOURCE).toUrl();
//        Nepomuk2::removeDataByApplication( conatctUri );

        KABC::Addressee addr;
        addr.setGivenName( name );
        addr.setFamilyName( familyname );
        addr.setFormattedName( fullname );

        Akonadi::Item item;
        item.setMimeType( KABC::Addressee::mimeType() );
        item.setPayload<KABC::Addressee>( addr );

        Akonadi::ItemCreateJob *job = new Akonadi::ItemCreateJob( item, collection );

        if ( !job->exec() ) {
            qDebug() << "Error:" << job->errorString();
        }

        QList<QUrl> resUri; resUri << contactRes.resourceUri();
        QVariantList value; value << job->item().url();
        Nepomuk2::setProperty(resUri, NIE::url(), value);

        i->setData(AKONADI_URL, job->item().url().toEncoded());
        i->setIcon(KIcon("view-pim-contacts"));
    }

    delete dlg;
}


