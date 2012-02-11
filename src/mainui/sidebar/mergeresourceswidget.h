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

#ifndef MERGERESOURCESWIDGET_H
#define MERGERESOURCESWIDGET_H

#include "sidebarcomponent.h"

#include <Nepomuk/Resource>

#include <QtGui/QWidget>
#include <QtCore/QList>

class QDBusInterface;
namespace Ui {
    class MergeResourcesWidget;
}

class MergeResourcesWidget : public SidebarComponent
{
    Q_OBJECT

public:
    explicit MergeResourcesWidget(QWidget *parent = 0);
    virtual ~MergeResourcesWidget();

    /**
      * Set all selected respources
      */
    void setResources(QList<Nepomuk::Resource> resourcelist);

    // not used, but required for the SidebarComponent
    Nepomuk::Resource resource();

public slots:
    // not used, but required for the SidebarComponent
    void setResource(Nepomuk::Resource & resource);
    void newButtonClicked();
    void deleteButtonClicked();

private slots:
    // Resource management
    void addToProject();
    void addToSelectedProject();
    void removeFromProject();
    void removeFromSelectedProject();
    void merge();
    void removeFromSystem();
    // export management
    void bibtexToClipboard();
    void citekeyToClipboard();
    void sendToLyXKile();
    void exportToFile();
    // metadata management
    void reindexFiles();
    void fetchMetaData();

private:
    Ui::MergeResourcesWidget *ui;
    QList<Nepomuk::Resource> m_resourceList;
    QDBusInterface *m_nepomukDBus;
};

#endif // MERGERESOURCESWIDGET_H
