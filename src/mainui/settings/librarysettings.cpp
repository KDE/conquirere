/*
 * Copyright 2013 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "librarysettings.h"
#include "ui_librarysettings.h"

#include "config/conquirere.h"

#include <KDE/KFileDialog>

LibrarySettings::LibrarySettings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LibrarySettings)
{
    ui->setupUi(this);

    setupGui();
    resetSettings();
}

LibrarySettings::~LibrarySettings()
{
    delete ui;
}

void LibrarySettings::resetSettings()
{
    //General
    //ui->loadSystemLibraryOnStartUp->setChecked( ConqSettings::showSystemLibraryOnStartUp() );

    //Documents
    ui->cbShowDocumentsType->setChecked( ConqSettings::showDocumentsType() );
    ui->cbShowSpreadSheetsType->setChecked( ConqSettings::showSpreadsheetType() );
    ui->cbShowPaginatedTextDocument->setChecked( ConqSettings::showPaginatedTextDocumentType() );
    ui->cbShowMindMaps->setChecked( ConqSettings::showMindMapType() );
    ui->cbShowPlainTextDocumentsType->setChecked( ConqSettings::showPlainTextDocumentsType() );

    ui->lwSystemWhiteList->addItems( ConqSettings::systemLibraryWhiteList() );

    //Notes

    disableDocumentMimeType();
}

void LibrarySettings::applySettings()
{
    //General
    //ConqSettings::setShowSystemLibraryOnStartUp( ui->loadSystemLibraryOnStartUp->isChecked() );

    //Documents
    ConqSettings::setShowDocumentsType( ui->cbShowDocumentsType->isChecked() );
    ConqSettings::setShowSpreadsheetType( ui->cbShowSpreadSheetsType->isChecked() );
    ConqSettings::setShowPaginatedTextDocumentType( ui->cbShowPaginatedTextDocument->isChecked() );
    ConqSettings::setShowMindMapType( ui->cbShowMindMaps->isChecked() );
    ConqSettings::setShowPlainTextDocumentsType( ui->cbShowPlainTextDocumentsType->isChecked() );

    QStringList whitepathList;
    for( int i=0; i < ui->lwSystemWhiteList->count(); i++) {
        whitepathList << ui->lwSystemWhiteList->item(i)->text();
    }

    ConqSettings::setSystemLibraryWhiteList( whitepathList );
    ConqSettings::self()->writeConfig();
}

void LibrarySettings::addWhiteListFolder()
{
    KUrl url = KFileDialog::getExistingDirectoryUrl(KUrl(QLatin1String("~/")),this,QLatin1String("Select the Folder that contains your documents"));

    if( !url.isEmpty() ) {
         ui->lwSystemWhiteList->addItem( url.url() );
    }

    emit contentChanged();
}

void LibrarySettings::removeCurrentWhiteListFolder()
{
    int currentItem = ui->lwSystemWhiteList->currentRow();

    if( currentItem < ui->lwSystemWhiteList->count() ) {
        QListWidgetItem *lwi = ui->lwSystemWhiteList->takeItem(currentItem);
        delete lwi;
    }

    emit contentChanged();
}

void LibrarySettings::disableDocumentMimeType()
{
    if( ui->cbShowMindMaps->isChecked() ||
        ui->cbShowSpreadSheetsType->isChecked() ||
        ui->cbShowPaginatedTextDocument->isChecked() ||
        ui->cbShowPlainTextDocumentsType->isChecked() ) {

        ui->cbShowDocumentsType->setChecked(false);
        ui->cbShowDocumentsType->setEnabled(false);
    }
    else {
        ui->cbShowDocumentsType->setChecked(true);
        ui->cbShowDocumentsType->setEnabled(true);
    }
}

void LibrarySettings::forceTypesOnDocumentDisable()
{
    if( !ui->cbShowDocumentsType->isChecked() &&
        !ui->cbShowMindMaps->isChecked() &&
        !ui->cbShowSpreadSheetsType->isChecked() &&
        !ui->cbShowPaginatedTextDocument->isChecked() &&
        !ui->cbShowPlainTextDocumentsType->isChecked() ) {

        ui->cbShowMindMaps->setChecked(true);
        ui->cbShowSpreadSheetsType->setChecked(true);
        ui->cbShowPaginatedTextDocument->setChecked(true);

        ui->cbShowDocumentsType->setChecked(false);
        ui->cbShowDocumentsType->setEnabled(false);
    }
}

void LibrarySettings::setupGui()
{
    ui->pbAddSystemFolder->setIcon(KIcon("list-add"));
    connect(ui->pbAddSystemFolder, SIGNAL(clicked()), this, SLOT(addWhiteListFolder()));

    ui->pbRemoveSystemFolder->setIcon(KIcon("list-remove"));
    connect(ui->pbRemoveSystemFolder, SIGNAL(clicked()), this, SLOT(removeCurrentWhiteListFolder()));

    connect(ui->cbShowDocumentsType, SIGNAL(clicked()), this, SLOT(forceTypesOnDocumentDisable()));

    connect(ui->cbShowMindMaps, SIGNAL(clicked()), this, SLOT(disableDocumentMimeType()));
    connect(ui->cbShowSpreadSheetsType, SIGNAL(clicked()), this, SLOT(disableDocumentMimeType()));
    connect(ui->cbShowPaginatedTextDocument, SIGNAL(clicked()), this, SLOT(disableDocumentMimeType()));
    connect(ui->cbShowPlainTextDocumentsType, SIGNAL(clicked()), this, SLOT(disableDocumentMimeType()));
}
