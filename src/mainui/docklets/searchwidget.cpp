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

#include "searchwidget.h"
#include "../../build/src/mainui/ui_searchwidget.h"

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

#include "nbib.h"
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NMO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Resource>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/QueryServiceClient>

#include <KDE/KAction>
#include <KDE/KUrl>

#include <QtGui/QListWidgetItem>
#include <QtGui/QDesktopServices>
#include <QtGui/QMenu>

#include <QtCore/QDebug>

const int HomepageRole = Qt::UserRole + 5;
const int WidgetRole = Qt::UserRole + 6;
const int NameRole = Qt::UserRole + 7;

SearchWidget::SearchWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::SearchWidget)
{
    qRegisterMetaType<SearchResultEntry>("SearchResultEntry");

    ui->setupUi(this);

    //share this selection with KBibTeX for the moment
    m_config = KSharedConfig::openConfig(QLatin1String("kbibtexrc"));
    m_configGroupName = QLatin1String("Search Engines Docklet");

    setupUi();

    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(foundNepomukEntry(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(nepomukQueryFinished()));
    connect(m_queryClient, SIGNAL(resultCount(int)), this, SLOT(nepomukResultCount(int)));
}

SearchWidget::~SearchWidget()
{
    delete ui;
    m_queryClient->close();
    delete m_queryClient;

    qDeleteAll(m_itemToOnlineSearch);
}

void SearchWidget::openHomepage()
{
    QListWidgetItem *item = ui->listWebEngines->currentItem();
    if (item != NULL) {
        KUrl url = item->data(HomepageRole).value<KUrl>();
        QDesktopServices::openUrl(url);
    }
}

void SearchWidget::enginesListCurrentChanged(QListWidgetItem *current)
{
    m_actionOpenHomepage->setEnabled(current != NULL);
}

void SearchWidget::itemCheckChanged(QListWidgetItem* item)
{
    int numCheckedEngines = 0;
    for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = m_itemToOnlineSearch.constBegin(); it != m_itemToOnlineSearch.constEnd(); ++it) {
        if (it.key()->checkState() == Qt::Checked) {
            ++numCheckedEngines;
        }
    }

    // no not allow to start a search without any checked search engine
    if(ui->rbSearchWebOnly->isChecked()) {
        ui->buttonSearch->setEnabled(numCheckedEngines > 0);
    }

    if (item != NULL) {
        KConfigGroup configGroup(m_config, m_configGroupName);
        QString name = item->data(NameRole).toString();
        configGroup.writeEntry(name, item->checkState() == Qt::Checked);
        m_config->sync();
    }
}

void SearchWidget::startSearch()
{
    ui->progressBar->setValue(0);
    int numResults = ui->editNumberOfResults->value();
    if(numResults < 1) numResults = 9999;

    // start the nepomuk search
    if(ui->rbSearchBoth->isChecked() ||
       ui->rbSearchNepomukOnly->isChecked()) {
        m_nepomukSearchInProgress = true;

        Nepomuk::Query::AndTerm andTerm;

        QString content = ui->editContent->text();
        QString title =  ui->editTitle->text();

        if(!content.isEmpty()) {
            Nepomuk::Query::OrTerm orTerm;
            orTerm.addSubTerm(Nepomuk::Query::LiteralTerm(content));

            Nepomuk::Query::ComparisonTerm fullTextSearch( Nepomuk::Vocabulary::NIE::plainTextContent(), Nepomuk::Query::LiteralTerm(content) );
            fullTextSearch.setComparator(Nepomuk::Query::ComparisonTerm::Contains);
            orTerm.addSubTerm(fullTextSearch);

            andTerm.addSubTerm(orTerm);
        }

        if(!title.isEmpty())
             andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::NIE::title(),
                                                            Nepomuk::Query::LiteralTerm(content) ));

        Nepomuk::Query::OrTerm orTerm;
        if(ui->cbDocument->isChecked())
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Document() ) );
        if(ui->cbAudio->isChecked())
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Audio() ) );
        if(ui->cbVideo->isChecked())
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Video() ) );
        if(ui->cbImage->isChecked())
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Image() ) );
        if(ui->cbEmail->isChecked())
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NMO::Message() ) );
        if(ui->cbNote->isChecked())
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::PIMO::Note() ) );
        if(ui->cbPublication->isChecked()) {
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Publication() ) );
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Series() ) );
        }
        if(ui->cbWebpage->isChecked())
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Website() ) );

        andTerm.addSubTerm(orTerm);

        // build the query
        Nepomuk::Query::Query query( andTerm );

        //query.setLimit(numResults);

        m_queryClient->query(query);
    }

    // start the websearch
    if(ui->rbSearchBoth->isChecked() ||
       ui->rbSearchWebOnly->isChecked()) {

        m_runningWebSearches.clear();
        m_websearchProgressMap.clear();

        /// start search using the general-purpose form's values

        QMap<QString, QString> queryTerms;// = m_generalQueryTermsForm->getQueryTerms();
        queryTerms.insert("title", ui->editTitle->text());
        queryTerms.insert("free", ui->editContent->text());
        //queryTerms.insert("author", ui->editAuthor->text());

        for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = m_itemToOnlineSearch.constBegin(); it != m_itemToOnlineSearch.constEnd(); ++it) {
            if (it.key()->checkState() == Qt::Checked) {
                it.value()->startSearch(queryTerms, numResults);
                m_runningWebSearches.insert(it.value());
            }
        }
    }
}

void SearchWidget::foundOnlineEntry(Entry *newEntry)
{
    OnlineSearchAbstract *engine = qobject_cast<OnlineSearchAbstract *>(sender());
    qDebug() << "SearchWidget::foundOnlineEntry from " << engine->name();

    SearchResultEntry sre;
    sre.webEngine = engine;
    sre.webResult = newEntry;

    emit searchResult(sre);
}

void SearchWidget::foundNepomukEntry(QList<Nepomuk::Query::Result> newEntry)
{
    qDebug() << "SearchWidget::foundNepomukEntry";

    if(!newEntry.first().excerpt().isEmpty()) {
       qDebug() <<  newEntry.first().excerpt();
    }

    SearchResultEntry sre;
    sre.nepomukResult = newEntry.first();

    emit searchResult(sre);
}

void SearchWidget::nepomukResultCount(int results)
{
    qDebug() << "SearchWidget::nepomukResultCount()" << results;
}

void SearchWidget::nepomukQueryFinished()
{
    m_nepomukSearchInProgress = false;
    qDebug() << "SearchWidget::nepomukQueryFinished()";
    updateProgress(1,1);
}

void SearchWidget::updateProgress(int cur, int total)
{
    OnlineSearchAbstract *ws = static_cast<OnlineSearchAbstract*>(sender());
    if(ws)
        m_websearchProgressMap[ws] = total > 0 ? cur * 1000 / total : 0;

    int progress = 0, count = 0;
    for (QMap<OnlineSearchAbstract*, int>::ConstIterator it = m_websearchProgressMap.constBegin(); it != m_websearchProgressMap.constEnd(); ++it, ++count)
        progress += it.value();

    if(!m_nepomukSearchInProgress)
        progress += 100/(m_websearchProgressMap.size()+1);

    ui->progressBar->setValue(count >= 1 ? progress / count : 0);
}

void SearchWidget::setupUi()
{
    ui->buttonSearch->setIcon(KIcon(QLatin1String("media-playback-start")));

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

    connect(ui->listWebEngines, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemCheckChanged(QListWidgetItem*)));

    m_actionOpenHomepage = new KAction(KIcon(QLatin1String("internet-web-browser")), i18n("Go to Homepage"), this);
    connect(m_actionOpenHomepage, SIGNAL(triggered()), this, SLOT(openHomepage()));
    ui->listWebEngines->addAction(m_actionOpenHomepage);
    ui->listWebEngines->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(ui->buttonSearch, SIGNAL(clicked()), this, SLOT(startSearch()));
}

void SearchWidget::addEngine(OnlineSearchAbstract *engine)
{
    KConfigGroup configGroup(m_config, m_configGroupName);

    QListWidgetItem *item = new QListWidgetItem(engine->label(), ui->listWebEngines);
    item->setCheckState(configGroup.readEntry(engine->name(), false) ? Qt::Checked : Qt::Unchecked);
    item->setIcon(engine->icon());
    item->setData(HomepageRole, engine->homepage());
    item->setData(NameRole, engine->name());

//    OnlineSearchQueryFormAbstract *widget = engine->customWidget(m_queryTermsStack);
//    item->setData(WidgetRole, QVariant::fromValue<OnlineSearchQueryFormAbstract*>(widget));
//    if (widget != NULL)
//        m_queryTermsStack->addWidget(widget);

    m_itemToOnlineSearch.insert(item, engine);
    connect(engine, SIGNAL(foundEntry(Entry*)), this, SLOT(foundOnlineEntry(Entry*)));
//    connect(engine, SIGNAL(stoppedSearch(int)), this, SLOT(stoppedSearch(int)));
    connect(engine, SIGNAL(progress(int,int)), this, SLOT(updateProgress(int,int)));
}
