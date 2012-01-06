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

#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <KDE/KConfigGroup>
#include <KDE/KConfig>
#include <KDE/KSharedConfigPtr>

#include <Nepomuk/Query/Result>

#include <QtGui/QDockWidget>
#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QMetaType>

namespace Ui {
    class SearchWidget;
}

namespace Nepomuk {
    namespace Query {
        class QueryServiceClient;
    }
}

class Entry;
class OnlineSearchAbstract;

class KAction;
class QListWidgetItem;

struct SearchResultEntry {
    Nepomuk::Query::Result nepomukResult;
    OnlineSearchAbstract *webEngine;
    Entry *webResult;
};

Q_DECLARE_METATYPE(SearchResultEntry)

class SearchWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(QWidget *parent = 0);
    ~SearchWidget();

signals:
    void newSearchStarted();
    void searchResult(SearchResultEntry newEntry);

private slots:
    void openHomepage();
    void enginesListCurrentChanged(QListWidgetItem *current);
    void itemCheckChanged(QListWidgetItem* item);

    void startSearch();
    void foundOnlineEntry(Entry *newEntry);
    void foundNepomukEntry(QList<Nepomuk::Query::Result> newEntry);

    void nepomukResultCount(int results);
    void nepomukQueryFinished();
    void updateProgress(int, int);

private:
    void setupUi();
    void addEngine(OnlineSearchAbstract *engine);

    Ui::SearchWidget *ui;
    KAction *m_actionOpenHomepage;
    KSharedConfigPtr m_config;
    QString m_configGroupName;

    Nepomuk::Query::QueryServiceClient *m_queryClient;
    QMap<QListWidgetItem*, OnlineSearchAbstract*> m_itemToOnlineSearch;
    QSet<OnlineSearchAbstract*> m_runningWebSearches;
    QMap<OnlineSearchAbstract*, int> m_websearchProgressMap;
    bool m_nepomukSearchInProgress;
};

#endif // SEARCHWIDGET_H
