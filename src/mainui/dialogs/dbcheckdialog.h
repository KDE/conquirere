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

#ifndef DBCHECKDIALOG_H
#define DBCHECKDIALOG_H

#include <QtGui/QDialog>

#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>

namespace Ui {
    class DbCheckDialog;
}

/**
  * @brief debugging dialog to remove some nepomuk data
  *
  * This will be removed once the program will not create data it can't remove anymore or create othewise data
  * that will unecessary after some other data is removed.
  *
  * @li Like keeping a reference to a deleted publication
  * @li Keeping the Chapter(DocumentPart) after the publication is deleted
  * @li fill the database with Authornames that wont be used anymore
  *
  * and so on.
  */
class DbCheckDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DbCheckDialog(QWidget *parent = 0);
    ~DbCheckDialog();

private slots:
    void checkCollection();
    void checkSeries();
    void checkAuthor();
    void checkReference();
    void checkDocumentPart();

    /**
      * @todo should be changed to "delete all data created by this application"
      */
    void checkAll();

    void addToList( const QList< Nepomuk::Query::Result > &entries );
    void queryFinished();
    void removeData();

private:
    Ui::DbCheckDialog *ui;

    Nepomuk::Query::QueryServiceClient *m_queryClient;
    QList<Nepomuk::Resource> m_toBeDeleted;
};

#endif // DBCHECKDIALOG_H
