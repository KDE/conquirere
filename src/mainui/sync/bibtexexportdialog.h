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

#ifndef BIBTEXEXPORTDIALOG_H
#define BIBTEXEXPORTDIALOG_H

#include <QtGui/QDialog>
#include <QtCore/QFutureWatcher>
#include <Nepomuk2/Query/Result>
#include <Nepomuk2/Query/QueryServiceClient>

#include "nbibio/bibtex/bibtexexporter.h"

namespace Ui {
    class BibTexExportDialog;
}

class LibraryManager;
class Library;
class KProgressDialog;

/**
  * @todo create QWizard from it .. same look and feel like the import wizard
  */
class BibTexExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BibTexExportDialog(QWidget *parent = 0);
    virtual ~BibTexExportDialog();

    void setInitialFileType(BibTexExporter::FileType selectedFileType);

    void setLibraryManager(LibraryManager *lm);
    void setExportLibrary(Library* l);
    void setResourceList(QList<Nepomuk2::Resource> exportList);

private slots:
    void accept();
    void exportFinished();
    void addToExportList( const QList< Nepomuk2::Query::Result > &entries );
    void queryFinished();

private:
    Ui::BibTexExportDialog *ui;

    BibTexExporter::FileType m_selectedFileType;
    LibraryManager *m_libraryManager;
    Library* m_importLibrary;
    QList<Nepomuk2::Resource> m_exportList;
    BibTexExporter *m_exporter;
    KProgressDialog *m_progress;
    Nepomuk2::Query::QueryServiceClient *m_queryClient;
    QFutureWatcher<QStringList> *m_futureWatcher;
};

#endif // BIBTEXEXPORTDIALOG_H
