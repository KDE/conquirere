/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

#include <kglobalsettings.h>
#include <KFileDialog>

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);

    ui->path_lineEdit->setText(KGlobalSettings::documentPath());
    ui->path_lineEdit->setEnabled(false);
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

QString NewProjectDialog::name() const
{
    return ui->name_lineEdit->text();
}

QString NewProjectDialog::path() const
{
    return ui->path_lineEdit->text() + QLatin1String("/") + name();
}

void NewProjectDialog::selectFolder()
{
    QString selectedFolder = KFileDialog::getExistingDirectory(KGlobalSettings::documentPath());

    if(!selectedFolder.isEmpty()) {
        ui->path_lineEdit->setText(selectedFolder);
    }
}
