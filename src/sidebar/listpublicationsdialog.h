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

#ifndef LISTPUBLICATIONSDIALOG_H
#define LISTPUBLICATIONSDIALOG_H

#include <Nepomuk/Resource>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>

#include <QtGui/QDialog>

namespace Ui {
    class ListPublicationsDialog;
}

class Library;
class QListWidgetItem;

class ListPublicationsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ListPublicationsDialog(QWidget *parent = 0);
    ~ListPublicationsDialog();

    void setLibrary(Library *p);

    Nepomuk::Resource selectedPublication();

private slots:
    void fetchData();

    void addPublicationData(const QList< Nepomuk::Query::Result > &entries);

    // called when all values have been announced in addCompletionData
    // quits the queryservice and calls createCompletionModel()
    void queryFinished();

    void showPublication( QListWidgetItem * item );

private:
    Ui::ListPublicationsDialog *ui;

    Library *m_library;

    Nepomuk::Query::QueryServiceClient *m_queryClient;
    QHash<QString, QUrl> m_listCache;
};

#endif // LISTPUBLICATIONSDIALOG_H
