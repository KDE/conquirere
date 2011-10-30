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

#include "bibtexexportdialog.h"
#include "ui_bibtexexportdialog.h"

#include "../nbibio/nbibexporterbibtex.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

#include <KDE/KProgressDialog>

#include <QtCore/QThread>
#include <QtCore/QtConcurrentRun>

#include <QtCore/QDebug>

bool concurrentExport(NBibExporterBibTex *exporter, const QString &fileName, Nepomuk::Query::Query query)
{
    QList<Nepomuk::Resource> resources;

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    foreach(const Nepomuk::Query::Result & r, queryResult) {
        resources.append(r.resource());
    }

    return exporter->toFile(fileName, resources);
}

BibTexExportDialog::BibTexExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BibTexExportDialog)
{
    ui->setupUi(this);
}

BibTexExportDialog::~BibTexExportDialog()
{
    delete ui;

    delete m_exporter;
    delete m_progress;
    delete m_futureWatcher;
}

void BibTexExportDialog::accept()
{
    //create file to export
    if(ui->folder->text().isEmpty()) {
        return;
    }

    QString filename = ui->folder->text();
    //filename.append(QLatin1String("/bibtex.bib"));

    m_progress = new KProgressDialog(i18n("Export BibTeX"), QLatin1String("Abort import"), 0, 100);
    m_progress->setWindowModality(Qt::WindowModal);
    m_progress->show();
    m_progress->setFocus();

    m_exporter = new NBibExporterBibTex();

    connect(m_exporter, SIGNAL(progress(int)), m_progress, SLOT(setValue(int)));
    connect(m_progress, SIGNAL(canceled()), m_exporter, SLOT(cancel()));

    // create query to fetch all used resources for the export
    Nepomuk::Query::AndTerm andTerm;
    andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Reference() ) );

    if(!ui->onlyReferences->isChecked())
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Publication() ) );

    Nepomuk::Query::Query query( andTerm );


    QFuture<bool> future = QtConcurrent::run(concurrentExport, m_exporter, filename, query);

    m_futureWatcher = new QFutureWatcher<bool>();
    m_futureWatcher->setFuture(future);
    connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(exportFinished()));

    close();
}

void BibTexExportDialog::exportFinished()
{
    qDebug() << "export finished!";
}
