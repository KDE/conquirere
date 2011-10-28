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

#ifndef WEBSEARCHWIDGET_H
#define WEBSEARCHWIDGET_H

#include <QDockWidget>
#include <QMap>
#include <QSet>
#include <KConfigGroup>
#include <KConfig>
#include <KSharedConfigPtr>

namespace Ui {
    class WebSearchWidget;
}

class OnlineSearchAbstract;
class OnlineSearchQueryFormAbstract;
class OnlineSearchQueryFormGeneral;
class QListWidgetItem;
class QStackedWidget;
class KAction;
class Entry;
class QSortFilterProxyModel;
class SearchResultModel;

class WebSearchWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit WebSearchWidget(QWidget *parent = 0);
    ~WebSearchWidget();

signals:
    void doneSearching();

private slots:
    void switchToEngines();
    void startSearch();
    void foundEntry(Entry *entry);
    void stoppedSearch(int resultCode);
    void itemCheckChanged(QListWidgetItem* item);
    void openHomepage();
    void enginesListCurrentChanged(QListWidgetItem *current);
    void enginesListCurrentChanged(QListWidgetItem*current, QListWidgetItem* previous);
    void currentStackWidgetChanged(int);
    void updateProgress(int, int);
    void updateGUI();

private:
    void setupUi();
    void addEngine(OnlineSearchAbstract *engine);
    OnlineSearchQueryFormAbstract *currentQueryForm();
    void switchToSearch();
    void switchToCancel();

    Ui::WebSearchWidget *ui;
    QMap<QListWidgetItem*, OnlineSearchAbstract*> itemToOnlineSearch;
    QStackedWidget *queryTermsStack;
    KSharedConfigPtr config;
    QString configGroupName;
    KAction *actionOpenHomepage;
    QMap<OnlineSearchAbstract*, int> progressMap;
    QSet<OnlineSearchAbstract*> runningSearches;
    OnlineSearchQueryFormGeneral *generalQueryTermsForm;

    QSortFilterProxyModel *bibtexSortModel;
    SearchResultModel *bibtexModel;
};

#endif // WEBSEARCHWIDGET_H
