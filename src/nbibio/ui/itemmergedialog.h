/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#ifndef ITEMMERGEDIALOG_H
#define ITEMMERGEDIALOG_H

#include <QtGui/QDialog>

#include "nbibio/nepomuksyncclient.h"
#include "nbibio/storageglobals.h"
#include "core/library.h"

namespace Ui {
    class ItemMergeDialog;
}

class QScrollArea;
class Entry;

class ItemMergeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemMergeDialog(QWidget *parent = 0);
    ~ItemMergeDialog();

    void setItemsToMerge(const QList<SyncMergeDetails> &items);
    void setProviderDetails(ProviderSyncDetails psd);
    void setLibraryToSyncWith(Library *l);


private slots:
    void showNext();
    void showPrevious();
    void finish();

    void replaceSelection();

private:
    struct MergedResults {
        Nepomuk2::Resource localSyncResource;
        QSharedPointer<Entry> localEntry;
        Entry * originalServerEntry;
        Entry * serverChanges;
        Entry * mergedChanges;
    };

    void showItem(int index);

    QMap<QString, QString> m_keyTranslate;
    Ui::ItemMergeDialog *ui;
    ProviderSyncDetails m_psd;
    Library *m_libraryToSyncWith;

    QList<MergedResults> m_mergeResults;
    int m_currentItem;

    QScrollArea *m_serverScrollArea;
    QScrollArea *m_localScrollArea;
};

#endif // ITEMMERGEDIALOG_H
