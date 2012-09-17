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

#include "providerselectionwidget.h"
#include "ui_providerselectionwidget.h"

#include "core/projectsettings.h"

#include <KDE/KGlobalSettings>
#include <KDE/KMessageBox>

const int PROVIDER_UUID = Qt::UserRole + 10;

ProviderSelectionWidget::ProviderSelectionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProviderSelectionWidget)
{
    ui->setupUi(this);

    ui->editButton->setIcon(KIcon("document-edit"));
    connect(ui->editButton, SIGNAL(clicked()), this, SLOT(editProvider()));
    ui->addButton->setIcon(KIcon("list-add"));
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addProvider()));
    ui->removeButton->setIcon(KIcon("list-remove"));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeProvider()));

//    fetchAkonadiCollection();
}

ProviderSelectionWidget::~ProviderSelectionWidget()
{
    delete ui;
}

void ProviderSelectionWidget::setProjectSettings(ProjectSettings *ps)
{
    m_settings = ps;

    //resetSettings(); called by overlying widgets
}

void ProviderSelectionWidget::resetSettings()
{
    /*
    ui->listProvider->clear();

    QList<ProviderSyncDetails> syncList = m_settings->allProviderSyncDetails();

    foreach(const ProviderSyncDetails & psd, syncList) {
        QListWidgetItem *qlwi = new QListWidgetItem(psd.providerInfo->providerIcon(), psd.providerInfo->providerName());
        qlwi->setData(PROVIDER_UUID, psd.uuid);
        ui->listProvider->addItem(qlwi);
    }
    */
}

void ProviderSelectionWidget::applySettings()
{
    // sync provider settings are saved automatically
}

void ProviderSelectionWidget::editProvider()
{
    /*
    QListWidgetItem *qlwi = ui->listProvider->currentItem();

    if(!qlwi)
        return;

    QString uuid = qlwi->data(PROVIDER_UUID).toString();

    ProviderSyncDetails oldPsd = m_settings->providerSyncDetails(uuid);

    KDialog dlg;
    ProviderSettings ps(&dlg, true);
    ps.setProviderSettingsDetails(oldPsd);
    dlg.setMainWidget(&ps);

    int ret = dlg.exec();

    if(ret == KDialog::Accepted) {
        ProviderSyncDetails newPsd = ps.providerSettingsDetails();

        m_settings->setProviderSyncDetails(newPsd, uuid);

        QString itemName = newPsd.providerInfo->providerName();
        qlwi->setText(itemName);

        ps.savePasswordInKWallet();
    }
    */
}

void ProviderSelectionWidget::addProvider()
{
    /*
    KDialog dlg;

    ProviderSettings ps(&dlg, true);
    dlg.setMainWidget(&ps);

    ps.setAkonadiContactDetails(m_contactList);
    ps.setAkonadiEventDetails(m_eventList);

    connect(this, SIGNAL(addContactCollection(QList<ProviderSettings::AkonadiDetails>)), &ps, SLOT(setAkonadiContactDetails(QList<ProviderSettings::AkonadiDetails>)));
    connect(this, SIGNAL(addEventCollection(QList<ProviderSettings::AkonadiDetails>)), &ps, SLOT(setAkonadiEventDetails(QList<ProviderSettings::AkonadiDetails>)));

    int ret = dlg.exec();

    if(ret == KDialog::Accepted) {
        ProviderSyncDetails newPsd = ps.providerSettingsDetails();
        QString uuid = m_settings->setProviderSyncDetails(newPsd, QString());
        QListWidgetItem *qlwi = new QListWidgetItem(newPsd.providerInfo->providerIcon(), newPsd.providerInfo->providerName());
        qlwi->setData(PROVIDER_UUID, uuid);
        ui->listProvider->addItem(qlwi);
        ps.savePasswordInKWallet();
    }
    */
}

void ProviderSelectionWidget::removeProvider()
{
    /*
    QListWidgetItem *qlwi = ui->listProvider->currentItem();

    if(!qlwi)
        return;

    QString uuid = qlwi->data(PROVIDER_UUID).toString();

    ui->listProvider->removeItemWidget(qlwi);
    delete qlwi;

    m_settings->removeProviderSyncDetails(uuid);
    */
}

