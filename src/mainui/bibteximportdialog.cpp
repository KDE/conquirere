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

#include "../nbibio/nbibimporterbibtex.h"
#include "../nbibio/conflictmanager.h"

#include <KDE/KDialog>
#include <KDE/KProgressDialog>

#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>
#include <QtCore/QStringList>
#include <QtCore/QThread>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QFile>

#include <QtCore/QDebug>

bool concurrentImport(NBibImporterBibTex *nib, const QString &fileName)
{
    return nib->fromFile(fileName);
}

BibTexImportDialog::BibTexImportDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BibTexImportDialog)
    , m_nib(0)
    , m_progress(0)
    , m_futureWatcher(0)
{
    ui->setupUi(this);
}

BibTexImportDialog::~BibTexImportDialog()
{
    delete m_importDialog;
    delete m_nib;
    delete m_progress;
    delete m_futureWatcher;
    delete ui;
}

void BibTexImportDialog::accept()
{
    if(ui->bibFile->text().isEmpty())
        return;

    m_progress = new KProgressDialog(0, i18n("Import publications"), QLatin1String("Abort import"));
    m_progress->setWindowModality(Qt::WindowModal);
    m_progress->show();
    m_progress->setFocus();

    m_nib = new NBibImporterBibTex();

    connect(m_nib, SIGNAL(progress(int)), m_progress, SLOT(setValue(int)));
    connect(m_progress, SIGNAL(canceled()), m_nib, SLOT(cancel()));

    QFuture<bool> future = QtConcurrent::run(concurrentImport, m_nib, ui->bibFile->text());

    m_futureWatcher = new QFutureWatcher<bool>();
    m_futureWatcher->setFuture(future);
    connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(importFinished()));

}

void BibTexImportDialog::importFinished()
{
    m_progress->close();

    m_importDialog = new KDialog( this );
    m_importDialog->setCaption( i18n("Import finished") );
    m_importDialog->setButtons( KDialog::Ok | KDialog::User1 );
    m_importDialog->setButtonText(KDialog::User1, i18n("Conflict Manager"));
    m_importDialog->enableButton(KDialog::User1, false);

    QWidget *w = new QWidget(m_importDialog);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *hlayout = new QHBoxLayout();
    mainLayout->addLayout(hlayout);

    QVBoxLayout *pubLayout = new QVBoxLayout();
    QLabel pubLabel;
    pubLabel.setText(i18n("Publication Import:"));
    pubLayout->addWidget(&pubLabel);

    QFormLayout *pubSubLayout = new QFormLayout;
    pubSubLayout->addRow(tr("duplicates:"), new QLabel(QString::number(m_nib->publicationDuplicates())));
    pubSubLayout->addRow(tr("new entries:"), new QLabel(QString::number(m_nib->publicationEntries())));
    pubSubLayout->addRow(tr("conflicts:"), new QLabel(QString::number(m_nib->conflictManager()->publicationConflicts().size())));
    pubLayout->addLayout(pubSubLayout);

    hlayout->addLayout(pubLayout);

    QVBoxLayout *refLayout = new QVBoxLayout();
    QLabel refLabel;
    refLabel.setText(i18n("References Import:"));
    refLayout->addWidget(&refLabel);

    QFormLayout *refSubLayout = new QFormLayout;
    refSubLayout->addRow(tr("duplicates:"), new QLabel(QString::number(m_nib->referenceDuplicates())));
    refSubLayout->addRow(tr("new entries:"), new QLabel(QString::number(m_nib->referenceEntries())));
    refSubLayout->addRow(tr("conflicts:"), new QLabel(QString::number(m_nib->conflictManager()->referenceConflicts().size())));
    refLayout->addLayout(refSubLayout);
    hlayout->addLayout(refLayout);

    QLabel conflictLabel;
    if(!m_nib->conflictManager()->publicationConflicts().isEmpty() ||
       !m_nib->conflictManager()->referenceConflicts().isEmpty()) {
        conflictLabel.setText(i18n("Conflict might be duplicate entries too.\nYou can open the conflict manager to solve them manually or click ok to leave them as new entries."));
        conflictLabel.setWordWrap(true);
        mainLayout->addWidget(&conflictLabel);
        m_importDialog->enableButton(KDialog::User1, true);
    }

    mainLayout->addStretch();

    w->setLayout(mainLayout);
    m_importDialog->setMainWidget( w );

    int ret = m_importDialog->exec();

    if(ret == KDialog::Accepted) {
        close();
    }
    else if(ret == KDialog::User1) {
        qDebug() << "conflict manager not implemented yet :(";
        close();
    }
}
