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
#include "../libnbibio/conflictmanager.h"

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

BibTexImportDialog::BibTexImportDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BibTexImportDialog)
    , m_nib(0)
    ,m_progress(0)
    ,m_futureWatcher(0)
{
    ui->setupUi(this);
}

BibTexImportDialog::~BibTexImportDialog()
{
    delete m_nib;
    delete m_progress;
    delete m_futureWatcher;
    delete ui;
}

void BibTexImportDialog::accept()
{
    if(ui->bibFile->text().isEmpty())
        return;

    m_progress = new QProgressDialog(i18n("Import publications"), QLatin1String("Abort import"), 0, 100);
    m_progress->setWindowModality(Qt::WindowModal);
    m_progress->show();
    m_progress->setFocus();

    m_nib = new NBibImporterBibTex();

    connect(m_nib, SIGNAL(progress(int)), m_progress, SLOT(setValue(int)));
    connect(m_progress, SIGNAL(canceled()), m_nib, SLOT(cancel()));

    QFuture<bool> future = QtConcurrent::run(concurrentImport, m_nib, ui->bibFile->text());

    m_futureWatcher = new QFutureWatcher<bool>();
    m_futureWatcher->setFuture(future);
    connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(importfinished()));

}

void BibTexImportDialog::importfinished()
{
    m_progress->close();

    if(m_nib->conflictManager()->hasConflicts()) {
        qDebug() << "import finished with conflicts :: open solvemanager";
        //qDebug() << "detected problems :: " << m_nib->conflictManager()->entries().size();
        qDebug() << "New Entries >" << m_nib->newEntries() << " :: Duplicates detected >" << m_nib->duplicates();
    }
    else {
        qDebug() << "import finished without conflicts";
        qDebug() << "New Entries >" << m_nib->newEntries() << " :: Duplicates detected >" << m_nib->duplicates();
        close();
    }
}
