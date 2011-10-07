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

#include "bibteximportdialog.h"
#include "ui_bibteximportdialog.h"

#include "../libnbibio/nbibimporterbibtex.h"

#include <QStringList>
#include <QProgressDialog>
#include <QThread>
#include <qtconcurrentrun.h>
#include <QFutureWatcher>
#include <QFile>
#include <QDebug>

bool concurrentImport(NBibImporterBibTex *nib, const QString &fileName)
{
    return nib->fromFile(fileName);
}

BibTexImportDialog::BibTexImportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BibTexImportDialog)
{
    ui->setupUi(this);
}

BibTexImportDialog::~BibTexImportDialog()
{
    delete ui;
}

void BibTexImportDialog::accept()
{
    if(ui->bibFile->text().isEmpty())
        return;

    m_progress = new QProgressDialog(i18n("Import publications"), "Abort import", 0, 100);
    m_progress->setWindowModality(Qt::WindowModal);
    m_progress->show();
    m_progress->setFocus();

    NBibImporterBibTex *nib = new NBibImporterBibTex();

    connect(nib, SIGNAL(progress(int)), m_progress, SLOT(setValue(int)));
    connect(m_progress, SIGNAL(canceled()), nib, SLOT(cancel()));

    QFuture<bool> future = QtConcurrent::run(concurrentImport, nib, ui->bibFile->text());

    m_futureWatcher = new QFutureWatcher<bool>();
    m_futureWatcher->setFuture(future);
    connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(importfinished()));

}

void BibTexImportDialog::importfinished()
{
    qDebug() << "import returned false ... solve conflicts";

    delete m_progress;
    delete m_futureWatcher;
    close();
}
