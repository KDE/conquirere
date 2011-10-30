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

#ifndef BIBTEXIMPORTDIALOG_H
#define BIBTEXIMPORTDIALOG_H

#include <QtGui/QDialog>
#include <QtCore/QFutureWatcher>

namespace Ui {
    class BibTexImportDialog;
}

class NBibImporterBibTex;
class KProgressDialog;
class KDialog;

class BibTexImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BibTexImportDialog(QWidget *parent = 0);
    ~BibTexImportDialog();

private slots:
    void accept();
    void importFinished();

private:
    Ui::BibTexImportDialog *ui;

    NBibImporterBibTex *m_nib;
    KProgressDialog *m_progress;
    KDialog *m_importDialog;
    QFutureWatcher<bool> *m_futureWatcher;
};

#endif // BIBTEXIMPORTDIALOG_H
