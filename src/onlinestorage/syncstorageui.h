/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef SYNCSTORAGEUI_H
#define SYNCSTORAGEUI_H

#include <QDialog>

#include <kbibtex/file.h>

namespace Ui {
    class SyncStorageUi;
}

class SyncStorage;

/**
  * @brief Simple GUI to sync a bibtex file with an online storage
  *
  * @author Jörg Ehrichs <joerg.ehrichs@gmx.de>
  */
class SyncStorageUi : public QDialog
{
    Q_OBJECT

public:
    explicit SyncStorageUi(QWidget *parent = 0);
    ~SyncStorageUi();

    void setBibTeXFile(File *fileToSync);

private slots:
    void switchProvider();
    void startSync();
    void syncStatus(bool inProgress);
    void cancelSync();
    void cancelClose();

private:
    Ui::SyncStorageUi *ui;

    File *m_fileToSync;
    SyncStorage *m_syncStorage;
    bool m_syncInProgress;
};

#endif // SYNCSTORAGEUI_H
