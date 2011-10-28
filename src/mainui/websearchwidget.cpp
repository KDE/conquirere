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

#include "websearchwidget.h"
#include "ui_websearchwidget.h"

#include "../core/searchresultmodel.h"
#include "../core/htmldelegate.h"

#include <kbibtex/onlinesearchabstract.h>
#include <kbibtex/onlinesearchgeneral.h>
#include <kbibtex/onlinesearchbibsonomy.h>
#include <kbibtex/onlinesearchgooglescholar.h>
#include <kbibtex/onlinesearchpubmed.h>
#include <kbibtex/onlinesearchieeexplore.h>
#include <kbibtex/onlinesearchacmportal.h>
#include <kbibtex/onlinesearchsciencedirect.h>
#include <kbibtex/onlinesearchspringerlink.h>
#include <kbibtex/onlinesearcharxiv.h>
#include <kbibtex/onlinesearchjstor.h>
#include <kbibtex/onlinesearchmathscinet.h>
#include <kbibtex/entry.h>
#include <kbibtex/value.h>
#include <kbibtex/bibtexfilemodel.h>

#include <KAction>
#include <KUrl>
#include <KMessageBox>

#include <QStandardItemModel>
#include <QStackedWidget>
#include <QListWidgetItem>
#include <QDesktopServices>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>

#include <QDebug>

const int HomepageRole = Qt::UserRole + 5;
const int WidgetRole = Qt::UserRole + 6;
const int NameRole = Qt::UserRole + 7;

WebSearchWidget::WebSearchWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::WebSearchWidget)
{
    ui->setupUi(this);

    //share this selection with KBibTeX for the moment
    config = KSharedConfig::openConfig(QLatin1String("kbibtexrc"));
    configGroupName = QLatin1String("Search Engines Docklet");

    setupUi();
}

WebSearchWidget::~WebSearchWidget()
{
    delete ui;
}

void WebSearchWidget::setupUi()
{
    bibtexSortModel = new QSortFilterProxyModel();
    bibtexModel = new SearchResultModel();
    bibtexSortModel->setSourceModel( bibtexModel );

    ui->listView->setModel(bibtexSortModel);
    ui->listView->setWordWrap(true);
    ui->listView->setItemDelegate(new HtmlDelegate());

    ui->searchButton->setIcon(KIcon("media-playback-start"));
    ui->ktabwidget->setTabIcon(0,KIcon("edit-rename"));
    ui->ktabwidget->setTabIcon(1,KIcon("applications-engineering"));
    connect(ui->ktabwidget, SIGNAL(currentChanged(int)), this, SLOT(updateGUI()));

    queryTermsStack = new QStackedWidget();
    ui->queryTab->layout()->addWidget(queryTermsStack);
    QVBoxLayout *vbl = qobject_cast<QVBoxLayout *>(ui->queryTab->layout());
    vbl->addStretch(100);
    generalQueryTermsForm = new OnlineSearchQueryFormGeneral(this);
    connect(generalQueryTermsForm, SIGNAL(returnPressed()), ui->searchButton, SIGNAL(clicked()));
    queryTermsStack->addWidget(generalQueryTermsForm);
    connect(queryTermsStack, SIGNAL(currentChanged(int)), this, SLOT(currentStackWidgetChanged(int)));

    addEngine(new OnlineSearchAcmPortal(this));
    addEngine(new OnlineSearchArXiv(this));
    addEngine(new OnlineSearchBibsonomy(this));
    addEngine(new OnlineSearchGoogleScholar(this));
    addEngine(new OnlineSearchIEEEXplore(this));
    addEngine(new OnlineSearchJStor(this));
    addEngine(new OnlineSearchMathSciNet(this));
    addEngine(new OnlineSearchPubMed(this));
    addEngine(new OnlineSearchScienceDirect(this));
    addEngine(new OnlineSearchSpringerLink(this));

    // List widget for the engines
    connect(ui->engineListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemCheckChanged(QListWidgetItem*)));
    connect(ui->engineListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this,
            SLOT(enginesListCurrentChanged(QListWidgetItem*, QListWidgetItem*)));

    ui->engineListWidget->setSelectionMode(QAbstractItemView::NoSelection);

    actionOpenHomepage = new KAction(KIcon("internet-web-browser"), i18n("Go to Homepage"), this);
    connect(actionOpenHomepage, SIGNAL(triggered()), this, SLOT(openHomepage()));
    ui->engineListWidget->addAction(actionOpenHomepage);
    ui->engineListWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    // label to show all selected engines
    connect(ui->engineSelectionLabel, SIGNAL(linkActivated(QString)), this, SLOT(switchToEngines()));

    switchToSearch();
    updateGUI();
}

void WebSearchWidget::addEngine(OnlineSearchAbstract *engine)
{
    KConfigGroup configGroup(config, configGroupName);

    QListWidgetItem *item = new QListWidgetItem(engine->label(), ui->engineListWidget);
    item->setCheckState(configGroup.readEntry(engine->name(), false) ? Qt::Checked : Qt::Unchecked);
    item->setIcon(engine->icon());
    item->setData(HomepageRole, engine->homepage());
    item->setData(NameRole, engine->name());

    OnlineSearchQueryFormAbstract *widget = engine->customWidget(queryTermsStack);
    item->setData(WidgetRole, QVariant::fromValue<OnlineSearchQueryFormAbstract*>(widget));
    if (widget != NULL)
        queryTermsStack->addWidget(widget);

    itemToOnlineSearch.insert(item, engine);
    connect(engine, SIGNAL(foundEntry(Entry*)), this, SLOT(foundEntry(Entry*)));
    connect(engine, SIGNAL(stoppedSearch(int)), this, SLOT(stoppedSearch(int)));
    connect(engine, SIGNAL(progress(int, int)), this, SLOT(updateProgress(int, int)));
}

void WebSearchWidget::switchToEngines()
{
    ui->ktabwidget->setCurrentWidget(ui->enginesTab);
}

void WebSearchWidget::itemCheckChanged(QListWidgetItem *item)
{
    int numCheckedEngines = 0;
    for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = itemToOnlineSearch.constBegin(); it != itemToOnlineSearch.constEnd(); ++it) {
        if (it.key()->checkState() == Qt::Checked) {
            ++numCheckedEngines;
        }
    }

    ui->searchButton->setEnabled(numCheckedEngines > 0);

    if (item != NULL) {
        KConfigGroup configGroup(config, configGroupName);
        QString name = item->data(NameRole).toString();
        configGroup.writeEntry(name, item->checkState() == Qt::Checked);
        config->sync();
    }
}

void WebSearchWidget::openHomepage()
{
    QListWidgetItem *item = ui->engineListWidget->currentItem();
    if (item != NULL) {
        KUrl url = item->data(HomepageRole).value<KUrl>();
        QDesktopServices::openUrl(url);
    }
}

void WebSearchWidget::enginesListCurrentChanged(QListWidgetItem *current)
{
    actionOpenHomepage->setEnabled(current != NULL);
}

void WebSearchWidget::enginesListCurrentChanged(QListWidgetItem *current, QListWidgetItem * previous)
{
    enginesListCurrentChanged(current);
}

void WebSearchWidget::currentStackWidgetChanged(int index)
{
    for (int i = queryTermsStack->count() - 1; i >= 0; --i) {
        OnlineSearchQueryFormAbstract *wsqfa = static_cast<OnlineSearchQueryFormAbstract*>(queryTermsStack->widget(i));
        if (i == index) {
            connect(wsqfa, SIGNAL(returnPressed()), ui->searchButton, SLOT(click()));
        }
        else {
            disconnect(wsqfa, SIGNAL(returnPressed()), ui->searchButton, SLOT(click()));
        }
    }
}

void WebSearchWidget::updateProgress(int cur, int total)
{
    OnlineSearchAbstract *ws = static_cast<OnlineSearchAbstract*>(sender());
    progressMap[ws] = total > 0 ? cur * 1000 / total : 0;

    int progress = 0, count = 0;
    for (QMap<OnlineSearchAbstract*, int>::ConstIterator it = progressMap.constBegin(); it != progressMap.constEnd(); ++it, ++count)
        progress += it.value();

    ui->progressBar->setValue(count >= 1 ? progress / count : 0);
}

void WebSearchWidget::startSearch()
{
    OnlineSearchQueryFormAbstract *currentForm = currentQueryForm();
    if (!currentForm->readyToStart()) {
        KMessageBox::sorry(this, i18n("Could not start searching the Internet:\nThe search terms are not complete or invalid."), i18n("Searching the Internet"));
        return;
    }

    runningSearches.clear();
    bibtexModel->clear();
    ui->progressBar->setValue(0);
    progressMap.clear();

    if (currentForm == generalQueryTermsForm) {
        /// start search using the general-purpose form's values

        QMap<QString, QString> queryTerms = generalQueryTermsForm->getQueryTerms();
        int numResults = generalQueryTermsForm->getNumResults();
        for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = itemToOnlineSearch.constBegin(); it != itemToOnlineSearch.constEnd(); ++it) {
            if (it.key()->checkState() == Qt::Checked) {
                it.value()->startSearch(queryTerms, numResults);
                runningSearches.insert(it.value());
            }
        }
        if (runningSearches.isEmpty()) {
            /// if no search engine has been checked (selected), something went wrong
            return;
        }
    }
    else {
        /// use the single selected search engine's specific form

        for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = itemToOnlineSearch.constBegin(); it != itemToOnlineSearch.constEnd(); ++it) {
            if (it.key()->checkState() == Qt::Checked) {
                it.value()->startSearch();
                runningSearches.insert(it.value());
            }
        }
        if (runningSearches.isEmpty()) {
            /// if no search engine has been checked (selected), something went wrong
            return;
        }
    }

    switchToCancel();
}

OnlineSearchQueryFormAbstract *WebSearchWidget::currentQueryForm()
{
    return static_cast<OnlineSearchQueryFormAbstract*>(queryTermsStack->currentWidget());
}

void WebSearchWidget::switchToSearch()
{
    for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = itemToOnlineSearch.constBegin(); it != itemToOnlineSearch.constEnd(); ++it)
        disconnect(ui->searchButton, SIGNAL(clicked()), it.value(), SLOT(cancel()));

    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(startSearch()));
    ui->searchButton->setText(i18n("Search"));
    ui->searchButton->setIcon(KIcon("media-playback-start"));
    ui->ktabwidget->setEnabled(true);
    ui->ktabwidget->unsetCursor();
}

void WebSearchWidget::switchToCancel()
{
    disconnect(ui->searchButton, SIGNAL(clicked()), this, SLOT(startSearch()));

    for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = itemToOnlineSearch.constBegin(); it != itemToOnlineSearch.constEnd(); ++it)
        connect(ui->searchButton, SIGNAL(clicked()), it.value(), SLOT(cancel()));
    ui->searchButton->setText(i18n("Cancel"));
    ui->searchButton->setIcon(KIcon("media-playback-stop"));
    ui->ktabwidget->setEnabled(false);
    ui->ktabwidget->setCursor(Qt::WaitCursor);
}

void WebSearchWidget::stoppedSearch(int resultCode)
{
    OnlineSearchAbstract *engine = static_cast<OnlineSearchAbstract *>(sender());
    if (runningSearches.remove(engine)) {
        //qDebug() << "Search from engine" << engine->label() << "stopped with code" << resultCode  << (resultCode == 0 ? "(OK)" : "(Error)");
        if (runningSearches.isEmpty()) {
            /// last search engine stopped
            switchToSearch();
            emit doneSearching();
            ui->progressBar->setValue(100);
        } else {
            QStringList remainingEngines;
            foreach(OnlineSearchAbstract *running, runningSearches) {
                remainingEngines.append(running->label());
            }
            if (!remainingEngines.isEmpty()) {
                //qDebug() << "Remaining running engines:" << remainingEngines.join(", ");
            }
        }
    }
}

void WebSearchWidget::updateGUI()
{
    if (ui->engineSelectionLabel == NULL) return;

    QStringList checkedEngines;
    QListWidgetItem *cursor = NULL;
    for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = itemToOnlineSearch.constBegin(); it != itemToOnlineSearch.constEnd(); ++it)
        if (it.key()->checkState() == Qt::Checked) {
            checkedEngines << it.key()->text();
            cursor = it.key();
        }

    switch (checkedEngines.size()) {
    case 0: ui->engineSelectionLabel->setText(i18n("No search engine selected. <a href=\"changeEngine\">Change</a>"));break;
    case 1: ui->engineSelectionLabel->setText(i18n("Search engine <b>%1</b> is selected. <a href=\"changeEngine\">Change</a>", checkedEngines.first()));break;
    case 2: ui->engineSelectionLabel->setText(i18n("Search engines <b>%1</b> and <b>%2</b> are selected. <a href=\"changeEngine\">Change</a>", checkedEngines.first(), checkedEngines.at(1)));break;
    case 3: ui->engineSelectionLabel->setText(i18n("Search engines <b>%1</b>, <b>%2</b>, and <b>%3</b> are selected. <a href=\"changeEngine\">Change</a>", checkedEngines.first(), checkedEngines.at(1), checkedEngines.at(2)));break;
    default: ui->engineSelectionLabel->setText(i18n("Search engines <b>%1</b>, <b>%2</b>, and more are selected. <a href=\"changeEngine\">Change</a>", checkedEngines.first(), checkedEngines.at(1)));break;
    }

    OnlineSearchQueryFormAbstract *currentQueryWidget = NULL;
    if (checkedEngines.size() == 1)
        currentQueryWidget = cursor->data(WidgetRole).value<OnlineSearchQueryFormAbstract*>();
    if (currentQueryWidget == NULL)
        currentQueryWidget = generalQueryTermsForm;
    queryTermsStack->setCurrentWidget(currentQueryWidget);
}

void WebSearchWidget::foundEntry(Entry*entry)
{
    bibtexModel->addEntry(entry);
}
