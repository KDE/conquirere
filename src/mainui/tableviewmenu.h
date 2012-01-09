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

#ifndef TABLEVIEWMENU_H
#define TABLEVIEWMENU_H

#include <QObject>

#include <Nepomuk/Resource>

#include <QtCore/QSharedPointer>

class Entry;
class MainWindow;

class TableViewMenu : public QObject
{
    Q_OBJECT
public:
    explicit TableViewMenu(QObject *parent = 0);

    void showNepomukEntryMenu(Nepomuk::Resource resource);
    void showBibTeXEntryMenu(QSharedPointer<Entry> entry);
    void setMainWindow(MainWindow *mw);

public slots:
    void addSelectedToProject();
    void removeSelectedFromProject();
    void removeSelectedFromSystem();
    void openSelected();

    void importSearchResult();
    void exportBibTexReference();
    void exportCiteKey();

private:
    MainWindow *m_parent;
    Nepomuk::Resource m_nepomukResource;
    QSharedPointer<Entry> m_bibtexEntry;
};

#endif // TABLEVIEWMENU_H
