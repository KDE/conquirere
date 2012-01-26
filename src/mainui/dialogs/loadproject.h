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

#ifndef LOADPROJECT_H
#define LOADPROJECT_H

#include <QtGui/QDialog>

#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>

namespace Ui {
    class LoadProject;
}

class Library;

/**
  * @brief Dialog to open a previous Conquirere project @c Library
  *
  * The list of available projects is retrieved by getting all @c pimo:isRelated @c pimo:Projects
  * for the @c pimo:Collection Conquirere program.
  */
class LoadProject : public QDialog
{
    Q_OBJECT

public:
    explicit LoadProject(QWidget *parent = 0);
    ~LoadProject();

    void fetchProjects();
    Library *loadedLibrary() const;

private slots:
    void queryFinished();
    void fillProjectList( const QList< Nepomuk::Query::Result > &entries );

    void showCollection(int currentRow);

    void accept();

private:
    Ui::LoadProject *ui;
    Nepomuk::Query::QueryServiceClient *m_queryClient;
    Library *m_loadLibrary;
};

#endif // LOADPROJECT_H
