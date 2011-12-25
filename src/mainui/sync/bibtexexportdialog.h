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

#include "../../nbibio/nbibexporterfile.h"

namespace Ui {
    class BibTexExportDialog;
}

class KProgressDialog;

class BibTexExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BibTexExportDialog(QWidget *parent = 0);
    virtual ~BibTexExportDialog();

    void setInitialFileType(NBibExporterFile::FileType selectedFileType);

private slots:
    void accept();
    void exportFinished();

private:
    Ui::BibTexExportDialog *ui;

    NBibExporterFile::FileType m_selectedFileType;
    NBibExporterFile *m_exporter;
    KProgressDialog *m_progress;
    QFutureWatcher<QStringList> *m_futureWatcher;
};

#endif // BIBTEXEXPORTDIALOG_H
