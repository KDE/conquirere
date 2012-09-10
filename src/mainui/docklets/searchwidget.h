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

#include "core/models/searchresultmodel.h"

#include <QtCore/QFutureWatcher>

#include <QtGui/QWidget>
#include <QtCore/QMap>

namespace Ui {
    class SearchWidget;
}

namespace NepomukMetaDataExtractor {
namespace Extractor {
class ExtractorFactory;
class WebExtractor;
}
}
class KAction;
class QListWidgetItem;

/**
  * @brief Dockwidget to set some search options and allow the user to search Nepomuk and KBibTeX online engines
  *
  * @todo TODO: ad ResourceWatcher to check for created/changed/deleted projects
  */
class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWidget(QWidget *parent = 0);
    ~SearchWidget();

    SearchResultModel* searchResultModel();

signals:
    void newSearchStarted();
    void searchResult(QMap<QString, QStringList> nepomukResults);
    void searchResult(const QVariantList &searchResult);

private slots:
    void sourceChanged(int selection);
    void fetchProjects();

    void openHomepage();
    void enginesListCurrentChanged(QListWidgetItem *current);
    void itemCheckChanged(QListWidgetItem* item);

    void startSearch();

    void finishedNepomukQuery();
    void finishedWebextractorQuery(const QVariantList &searchResults);

    void updateProgress();

    void saveSettings();
    void loadSettings();

private:
    void setupUi();
    void switchToSearch();
    void switchToCancel();

    QVariantList queryNepomuk(const QString &query);
    void queryWebExtractor(int nextExtractor);

    Ui::SearchWidget *ui;
    KAction *m_actionOpenHomepage;
    SearchResultModel *m_searchResultModel;

    NepomukMetaDataExtractor::Extractor::ExtractorFactory *m_ef;
    NepomukMetaDataExtractor::Extractor::WebExtractor *m_currentWebExtractor;
    QStringList m_pluginList;
    int m_currentExtractor;
    int m_maxProgress;

    QFutureWatcher<QVariantList > *m_futureWatcherNepomuk;
};

#endif // SEARCHWIDGET_H
