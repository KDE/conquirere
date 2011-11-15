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

#include "../core/models/searchresultmodel.h"
#include "../core/delegates/htmldelegate.h"
#include "../nbibio/pipe/bibtextoclipboardpipe.h"
#include "../nbibio/pipe/bibtextonepomukpipe.h"

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

#include <KDE/KAction>
#include <KDE/KUrl>
#include <KDE/KMessageBox>

#include <QtGui/QStandardItemModel>
#include <QtGui/QStackedWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QDesktopServices>
#include <QtGui/QVBoxLayout>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QMenu>

#include <QtCore/QDebug>

const int HomepageRole = Qt::UserRole + 5;
const int WidgetRole = Qt::UserRole + 6;
const int NameRole = Qt::UserRole + 7;

WebSearchWidget::WebSearchWidget(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::WebSearchWidget)
    , m_generalQueryTermsForm(0)
{
    ui->setupUi(this);

    //share this selection with KBibTeX for the moment
    m_config = KSharedConfig::openConfig(QLatin1String("kbibtexrc"));
    m_configGroupName = QLatin1String("Search Engines Docklet");

    setupUi();
}

WebSearchWidget::~WebSearchWidget()
{
    delete ui;
    delete m_queryTermsStack;
    delete m_actionOpenHomepage;
    //qDeleteAll(m_progressMap);
    qDeleteAll(m_runningSearches);
    //delete m_generalQueryTermsForm;

    delete m_importSearchResult;
    delete m_exportBibTexReference;
    delete m_exportCiteKey;
    delete m_bibtexSortModel;
    delete m_bibtexModel;
}

void WebSearchWidget::setupUi()
{
    m_bibtexSortModel = new QSortFilterProxyModel();
    m_bibtexModel = new SearchResultModel();
    m_bibtexSortModel->setSourceModel( m_bibtexModel );

    ui->listView->setModel(m_bibtexSortModel);
    ui->listView->setWordWrap(true);
    ui->listView->setItemDelegate(new HtmlDelegate());
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(resultContextMenu(const QPoint &)));

    m_importSearchResult = new KAction(this);
    m_importSearchResult->setText(i18n("Import Entry"));
    m_importSearchResult->setIcon(KIcon(QLatin1String("document-import")));
    connect(m_importSearchResult, SIGNAL(triggered()), this, SLOT(importSearchResult()));

    m_exportBibTexReference = new KAction(this);
    m_exportBibTexReference->setText(i18n("Export BibTex to Clipboard"));
    m_exportBibTexReference->setIcon(KIcon(QLatin1String("document-export")));
    connect(m_exportBibTexReference, SIGNAL(triggered()), this, SLOT(exportBibTexReference()));

    m_exportCiteKey = new KAction(this);
    m_exportCiteKey->setText(i18n("Export Citekey to Clipboard"));
    m_exportCiteKey->setIcon(KIcon(QLatin1String("document-export")));
    connect(m_exportCiteKey, SIGNAL(triggered()), this, SLOT(exportCiteKey()));

    ui->searchButton->setIcon(KIcon(QLatin1String("media-playback-start")));
    ui->ktabwidget->setTabIcon(0,KIcon(QLatin1String("edit-rename")));
    ui->ktabwidget->setTabIcon(1,KIcon(QLatin1String("applications-engineering")));
    connect(ui->ktabwidget, SIGNAL(currentChanged(int)), this, SLOT(updateGUI()));

    m_queryTermsStack = new QStackedWidget();
    ui->queryTab->layout()->addWidget(m_queryTermsStack);
    QVBoxLayout *vbl = qobject_cast<QVBoxLayout *>(ui->queryTab->layout());
    vbl->addStretch(100);
    m_generalQueryTermsForm = new OnlineSearchQueryFormGeneral(this);
    connect(m_generalQueryTermsForm, SIGNAL(returnPressed()), ui->searchButton, SIGNAL(clicked()));
    m_queryTermsStack->addWidget(m_generalQueryTermsForm);
    connect(m_queryTermsStack, SIGNAL(currentChanged(int)), this, SLOT(currentStackWidgetChanged(int)));

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

    m_actionOpenHomepage = new KAction(KIcon(QLatin1String("internet-web-browser")), i18n("Go to Homepage"), this);
    connect(m_actionOpenHomepage, SIGNAL(triggered()), this, SLOT(openHomepage()));
    ui->engineListWidget->addAction(m_actionOpenHomepage);
    ui->engineListWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    // label to show all selected engines
    connect(ui->engineSelectionLabel, SIGNAL(linkActivated(QString)), this, SLOT(switchToEngines()));

    switchToSearch();
    updateGUI();
}

void WebSearchWidget::addEngine(OnlineSearchAbstract *engine)
{
    KConfigGroup configGroup(m_config, m_configGroupName);

    QListWidgetItem *item = new QListWidgetItem(engine->label(), ui->engineListWidget);
    item->setCheckState(configGroup.readEntry(engine->name(), false) ? Qt::Checked : Qt::Unchecked);
    item->setIcon(engine->icon());
    item->setData(HomepageRole, engine->homepage());
    item->setData(NameRole, engine->name());

    OnlineSearchQueryFormAbstract *widget = engine->customWidget(m_queryTermsStack);
    item->setData(WidgetRole, QVariant::fromValue<OnlineSearchQueryFormAbstract*>(widget));
    if (widget != NULL)
        m_queryTermsStack->addWidget(widget);

    m_itemToOnlineSearch.insert(item, engine);
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
    for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = m_itemToOnlineSearch.constBegin(); it != m_itemToOnlineSearch.constEnd(); ++it) {
        if (it.key()->checkState() == Qt::Checked) {
            ++numCheckedEngines;
        }
    }

    ui->searchButton->setEnabled(numCheckedEngines > 0);

    if (item != NULL) {
        KConfigGroup configGroup(m_config, m_configGroupName);
        QString name = item->data(NameRole).toString();
        configGroup.writeEntry(name, item->checkState() == Qt::Checked);
        m_config->sync();
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
    m_actionOpenHomepage->setEnabled(current != NULL);
}

void WebSearchWidget::enginesListCurrentChanged(QListWidgetItem *current, QListWidgetItem * previous)
{
    enginesListCurrentChanged(current);
}

void WebSearchWidget::currentStackWidgetChanged(int index)
{
    for (int i = m_queryTermsStack->count() - 1; i >= 0; --i) {
        OnlineSearchQueryFormAbstract *wsqfa = static_cast<OnlineSearchQueryFormAbstract*>(m_queryTermsStack->widget(i));
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
    m_progressMap[ws] = total > 0 ? cur * 1000 / total : 0;

    int progress = 0, count = 0;
    for (QMap<OnlineSearchAbstract*, int>::ConstIterator it = m_progressMap.constBegin(); it != m_progressMap.constEnd(); ++it, ++count)
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

    m_runningSearches.clear();
    m_bibtexModel->clear();
    ui->progressBar->setValue(0);
    m_progressMap.clear();

    if (currentForm == m_generalQueryTermsForm) {
        /// start search using the general-purpose form's values

        QMap<QString, QString> queryTerms = m_generalQueryTermsForm->getQueryTerms();
        int numResults = m_generalQueryTermsForm->getNumResults();
        for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = m_itemToOnlineSearch.constBegin(); it != m_itemToOnlineSearch.constEnd(); ++it) {
            if (it.key()->checkState() == Qt::Checked) {
                it.value()->startSearch(queryTerms, numResults);
                m_runningSearches.insert(it.value());
            }
        }
        if (m_runningSearches.isEmpty()) {
            /// if no search engine has been checked (selected), something went wrong
            return;
        }
    }
    else {
        /// use the single selected search engine's specific form

        for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = m_itemToOnlineSearch.constBegin(); it != m_itemToOnlineSearch.constEnd(); ++it) {
            if (it.key()->checkState() == Qt::Checked) {
                it.value()->startSearch();
                m_runningSearches.insert(it.value());
            }
        }
        if (m_runningSearches.isEmpty()) {
            /// if no search engine has been checked (selected), something went wrong
            return;
        }
    }

    switchToCancel();
}

OnlineSearchQueryFormAbstract *WebSearchWidget::currentQueryForm()
{
    return static_cast<OnlineSearchQueryFormAbstract*>(m_queryTermsStack->currentWidget());
}

void WebSearchWidget::switchToSearch()
{
    for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = m_itemToOnlineSearch.constBegin(); it != m_itemToOnlineSearch.constEnd(); ++it)
        disconnect(ui->searchButton, SIGNAL(clicked()), it.value(), SLOT(cancel()));

    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(startSearch()));
    ui->searchButton->setText(i18n("Search"));
    ui->searchButton->setIcon(KIcon(QLatin1String("media-playback-start")));
    ui->queryTab->setEnabled(true);
    ui->enginesTab->setEnabled(true);
    ui->ktabwidget->unsetCursor();
}

void WebSearchWidget::switchToCancel()
{
    disconnect(ui->searchButton, SIGNAL(clicked()), this, SLOT(startSearch()));

    for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = m_itemToOnlineSearch.constBegin(); it != m_itemToOnlineSearch.constEnd(); ++it)
        connect(ui->searchButton, SIGNAL(clicked()), it.value(), SLOT(cancel()));
    ui->searchButton->setText(i18n("Cancel"));
    ui->searchButton->setIcon(KIcon(QLatin1String("media-playback-stop")));
    ui->queryTab->setEnabled(false);
    ui->enginesTab->setEnabled(false);
    ui->ktabwidget->setCurrentWidget(ui->searchResults);
    ui->ktabwidget->setCursor(Qt::WaitCursor);
}

void WebSearchWidget::stoppedSearch(int resultCode)
{
    OnlineSearchAbstract *engine = static_cast<OnlineSearchAbstract *>(sender());
    if (m_runningSearches.remove(engine)) {
        //qDebug() << "Search from engine" << engine->label() << "stopped with code" << resultCode  << (resultCode == 0 ? "(OK)" : "(Error)");
        if (m_runningSearches.isEmpty()) {
            /// last search engine stopped
            switchToSearch();
            emit doneSearching();
            ui->progressBar->setValue(100);
        } else {
            QStringList remainingEngines;
            foreach(OnlineSearchAbstract *running, m_runningSearches) {
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
    for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = m_itemToOnlineSearch.constBegin(); it != m_itemToOnlineSearch.constEnd(); ++it)
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
        currentQueryWidget = m_generalQueryTermsForm;
    m_queryTermsStack->setCurrentWidget(currentQueryWidget);
}

void WebSearchWidget::resultContextMenu(const QPoint & pos)
{
    QModelIndex mi = ui->listView->indexAt(pos);
    if(mi.isValid()) {
        QMenu menu(this);
        menu.addAction(m_importSearchResult);
        menu.addSeparator();
        menu.addAction(m_exportBibTexReference);
        menu.addAction(m_exportCiteKey);
        menu.exec(QCursor::pos());
    }
}

void WebSearchWidget::importSearchResult()
{
    QModelIndexList mil = ui->listView->selectionModel()->selectedIndexes();

    File f;
    foreach(const QModelIndex & mi, mil) {
        Entry *e = m_bibtexModel->entryAt(mi.row());
        f.append(e);
    }

    BibTexToNepomukPipe btnp;
    btnp.pipeExport(f);
}

void WebSearchWidget::exportBibTexReference()
{
    QModelIndexList mil = ui->listView->selectionModel()->selectedIndexes();

    File f;
    foreach(const QModelIndex & mi, mil) {
        Entry *e = m_bibtexModel->entryAt(mi.row());
        f.append(e);
    }

    BibTexToClipboardPipe btcp;
    btcp.setExportType(BibTexToClipboardPipe::Export_SOURCE);
    btcp.pipeExport(f);
}

void WebSearchWidget::exportCiteKey()
{
    QModelIndexList mil = ui->listView->selectionModel()->selectedIndexes();

    File f;
    foreach(const QModelIndex & mi, mil) {
        Entry *e = m_bibtexModel->entryAt(mi.row());
        f.append(e);
    }

    BibTexToClipboardPipe btcp;
    btcp.setExportType(BibTexToClipboardPipe::Export_CITEKEY);
    btcp.pipeExport(f);
}

void WebSearchWidget::foundEntry(Entry*entry)
{
    m_bibtexModel->addEntry(entry);
}
