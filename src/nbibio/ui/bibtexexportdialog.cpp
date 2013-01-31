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

#include "core/librarymanager.h"
#include "core/library.h"
#include "core/projectsettings.h"

#include "nbib.h"
#include <Nepomuk2/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Query/Term>
#include <Nepomuk2/Query/ResourceTerm>
#include <Nepomuk2/Query/ResourceTypeTerm>
#include <Nepomuk2/Query/ComparisonTerm>
#include <Nepomuk2/Query/AndTerm>
#include <Nepomuk2/Query/QueryServiceClient>
#include <Nepomuk2/Query/Result>
#include <Nepomuk2/Query/QueryParser>

#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>

#include <KDE/KProgressDialog>

#include <QtCore/QThread>
#include <QtCore/QtConcurrentRun>

#include <KDE/KDebug>

QStringList concurrentExport(BibTexExporter *exporter, const QString &fileName, QList<Nepomuk2::Resource> resources)
{
    QStringList errorLog;
    exporter->toFile(fileName, resources, &errorLog);

    return errorLog;
}

BibTexExportDialog::BibTexExportDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BibTexExportDialog)
    , m_selectedFileType(BibTexExporter::EXPORT_BIBTEX)
    , m_libraryManager(0)
    , m_importLibrary(0)
    , m_exporter(0)
    , m_progress(0)
    , m_futureWatcher(0)
{
    ui->setupUi(this);

    ui->dataSelection->addItem( i18n("System Library") );

    m_queryClient = new Nepomuk2::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk2::Query::Result>)), this, SLOT(addToExportList(QList<Nepomuk2::Query::Result>)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(queryFinished()));
}

void BibTexExportDialog::setInitialFileType(BibTexExporter::FileType selectedFileType)
{
    m_selectedFileType = selectedFileType;

    ui->selectedFileType->setCurrentIndex(m_selectedFileType);
}

void BibTexExportDialog::setLibraryManager(LibraryManager *lm)
{
    m_libraryManager = lm;

    int i=1;
    int selectedIndex=0;
    foreach(Library *l, m_libraryManager->openProjects()) {
        ui->dataSelection->addItem(l->settings()->name(), l->settings()->projectThing().uri());

        if(l == m_importLibrary) { selectedIndex=i; }

        i++;
    }

    ui->dataSelection->setCurrentIndex(selectedIndex);
}

void BibTexExportDialog::setExportLibrary(Library* l)
{
    m_importLibrary = l;

    int selectLib = ui->dataSelection->findData( l->settings()->projectThing().uri() );
    ui->dataSelection->setCurrentIndex(selectLib);
}

void BibTexExportDialog::setResourceList(QList<Nepomuk2::Resource> exportList)
{
        m_exportList = exportList;
        ui->groupBox_2->setVisible(false);
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

    m_progress = new KProgressDialog(this, i18n("Export BibTeX"), QLatin1String("Fetch data"));
    m_progress->progressBar()->setRange(0,100);
    m_progress->setWindowModality(Qt::WindowModal);
    m_progress->show();
    m_progress->setFocus();

    m_selectedFileType = BibTexExporter::FileType(ui->selectedFileType->currentIndex());
    m_exporter = new BibTexExporter;

    m_exporter->setFileType(m_selectedFileType);
    connect(m_exporter, SIGNAL(progress(int)), m_progress->progressBar(), SLOT(setValue(int)));

    // create query to fetch all used resources for the export
    if(m_exportList.isEmpty()) {
        Nepomuk2::Query::AndTerm andTerm;
        andTerm.addSubTerm( Nepomuk2::Query::ResourceTypeTerm( Nepomuk2::Vocabulary::NBIB::Reference() ) );

        if(!ui->onlyReferences->isChecked()) {
            andTerm.addSubTerm( Nepomuk2::Query::ResourceTypeTerm( Nepomuk2::Vocabulary::NBIB::Publication() ) );
        }

        int curIndex = ui->dataSelection->currentIndex();
        if(curIndex > 0) {
            QUrl projectToExport = ui->dataSelection->itemData(curIndex).toUrl();
            Nepomuk2::Resource projectThing = Nepomuk2::Resource(projectToExport);

            andTerm.addSubTerm( Nepomuk2::Query::ComparisonTerm( Soprano::Vocabulary::NAO::isRelated(),
                                                                Nepomuk2::Query::ResourceTerm( projectThing )));
        }

        Nepomuk2::Query::Query query( andTerm );
        m_queryClient->query(query);
    }
    else {
        queryFinished();
    }
}

void BibTexExportDialog::exportFinished()
{
    close();
}

void BibTexExportDialog::addToExportList( const QList< Nepomuk2::Query::Result > &entries )
{
    kDebug() << "addToExportList";
    foreach(const Nepomuk2::Query::Result &r, entries) {
        m_exportList << r.resource();
    }
}

void BibTexExportDialog::queryFinished()
{
    kDebug() << "query finished";
    m_queryClient->close();
    m_progress->progressBar()->setValue(50);
    m_progress->setLabelText(i18n("export data"));

    QString filename = ui->folder->text();
    QFuture<QStringList> future = QtConcurrent::run(concurrentExport, m_exporter, filename, m_exportList);

    m_futureWatcher = new QFutureWatcher<QStringList>();
    m_futureWatcher->setFuture(future);
    connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(exportFinished()));
}
