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

#include "searchwidget.h"
#include "ui_searchwidget.h"

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
#include <kbibtex/onlinesearchingentaconnect.h>

#include <kbibtex/entry.h>

#include "nbib.h"
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NMO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Resource>
#include <Nepomuk/Variant>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>

#include <KDE/KConfigGroup>
#include <KDE/KConfig>
#include <KDE/KAction>
#include <KDE/KUrl>
#include <KDE/KDebug>

#include <QtGui/QListWidgetItem>
#include <QtGui/QDesktopServices>
#include <QtGui/QMenu>

const int HomepageRole = Qt::UserRole + 5;
const int WidgetRole = Qt::UserRole + 6;
const int NameRole = Qt::UserRole + 7;

enum SearchSource {
    Search_Everywhere,
    Search_Nepomuk,
    Search_Web
};

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchWidget)
    , m_actionOpenHomepage(0)
    , m_queryClient(0)
    , m_searchResultModel(0)
{
    ui->setupUi(this);
    setupUi();

    m_searchResultModel = new SearchResultModel();
    connect(this, SIGNAL(newSearchStarted()), m_searchResultModel, SLOT(clearData()));
    connect(this, SIGNAL(searchResult(SearchResultEntry)), m_searchResultModel, SLOT(addSearchResult(SearchResultEntry)));

    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(foundNepomukEntry(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(nepomukQueryFinished()));

    m_projectQueryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_projectQueryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(fillProjectList(QList<Nepomuk::Query::Result>)));

    loadSettings();

    connect(ui->editAuthor, SIGNAL(returnPressed()), this, SLOT(startSearch()));
    connect(ui->editContent, SIGNAL(returnPressed()), this, SLOT(startSearch()));
    connect(ui->editTitle, SIGNAL(returnPressed()), this, SLOT(startSearch()));

    connect(ui->selectSource, SIGNAL(currentIndexChanged(int)), SLOT(sourceChanged(int)));

    fetchProjects();
}

SearchWidget::~SearchWidget()
{
    saveSettings();

    delete ui;
    m_queryClient->close();
    delete m_queryClient;
    m_projectQueryClient->close();
    delete m_projectQueryClient;
    delete m_actionOpenHomepage;

    qDeleteAll(m_itemToOnlineSearch);
    delete m_searchResultModel;
}

SearchResultModel* SearchWidget::searchResultModel()
{
    return m_searchResultModel;
}


void SearchWidget::sourceChanged(int selection)
{
    if( selection == 2) { // web only
        ui->labelLibrary->setEnabled(false);
        ui->editLibrary->setEnabled(false);
    }
    else {
         // only fetch new projects when we di not have a list already
        // but fetch them always if we select aeverywhere or nepomuk only
        // so we can update when new projects are created in between
        // basically a Nepomuk Resource watcher would be cleaner, but maybe over the top
        if(!ui->labelLibrary->isEnabled()) {
            fetchProjects();
        }

        ui->labelLibrary->setEnabled(true);
        ui->editLibrary->setEnabled(true);
    }
}

void SearchWidget::fetchProjects()
{
    m_projectQueryClient->close();
    ui->editLibrary->clear();

    ui->editLibrary->addItem(i18n("Full Library"));

    QString query = "select DISTINCT ?r where { "
                     "?r a pimo:Project ."
                     "}";

     m_projectQueryClient->sparqlQuery( query );
}

void SearchWidget::fillProjectList( const QList< Nepomuk::Query::Result > &entries )
{
    foreach(const Nepomuk::Query::Result &r, entries) {
        ui->editLibrary->addItem(r.resource().property(Soprano::Vocabulary::NAO::prefLabel()).toString(),
                                 r.resource().resourceUri());
    }
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

    // do not allow to start a search without any checked search engine
    SearchSource source = SearchSource(ui->selectSource->currentIndex());
    if(source == Search_Web) {
        ui->buttonSearch->setEnabled(numCheckedEngines > 0);
    }

    if (item != NULL) {
        KConfig config;
        KConfigGroup searchSettingsGroup( &config, QLatin1String("SearchSettings") );
        QString name = item->data(NameRole).toString();
        searchSettingsGroup.writeEntry(name, item->checkState() == Qt::Checked);
        searchSettingsGroup.config()->sync();
    }
}

void SearchWidget::startSearch()
{
    switchToCancel();
    ui->progressBar->setValue(0);
    int numResults = ui->editNumberOfResults->value();
    if(numResults < 1) numResults = 9999;

    SearchSource source = SearchSource(ui->selectSource->currentIndex());

    // start the nepomuk search
    if(source == Search_Everywhere ||
       source == Search_Nepomuk) {
        m_nepomukSearchInProgress = true;

        Nepomuk::Query::AndTerm andTerm;

        QString content = ui->editContent->text();
        QString title =  ui->editTitle->text();
        QString author =  ui->editAuthor->text();

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

        if(!author.isEmpty()) {
            Nepomuk::Query::OrTerm orTerm;

            Nepomuk::Query::ComparisonTerm naoSearch( Soprano::Vocabulary::NAO::creator(), Nepomuk::Query::LiteralTerm(author) );
            naoSearch.setComparator(Nepomuk::Query::ComparisonTerm::Contains);
            orTerm.addSubTerm(naoSearch);

            Nepomuk::Query::ComparisonTerm ncoSearch( Nepomuk::Vocabulary::NCO::creator(), Nepomuk::Query::LiteralTerm(author) );
            ncoSearch.setComparator(Nepomuk::Query::ComparisonTerm::Contains);
            orTerm.addSubTerm(ncoSearch);

            andTerm.addSubTerm( orTerm );
        }

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
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NMO::Email() ) );
        if(ui->cbNote->isChecked())
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::PIMO::Note() ) );
        if(ui->cbPublication->isChecked()) {
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Publication() ) );
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Series() ) );
        }
        if(ui->cbWebpage->isChecked()) {
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Website() ) );
            orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Bookmark() ) );
        }

        if(ui->editLibrary->currentIndex() != 0) {
            Nepomuk::Resource project = Nepomuk::Resource::fromResourceUri( ui->editLibrary->itemData(ui->editLibrary->currentIndex()).toString());
            Nepomuk::Query::OrTerm orTerm;
            orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Soprano::Vocabulary::NAO::hasTag(),
                                                               Nepomuk::Query::ResourceTerm( project )));
            orTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Soprano::Vocabulary::NAO::isRelated(),
                                                               Nepomuk::Query::ResourceTerm( project )));
            andTerm.addSubTerm(orTerm);
        }

        andTerm.addSubTerm(orTerm);

        // build the query
        Nepomuk::Query::Query query( andTerm );

        query.setLimit(numResults);

        m_queryClient->query(query);
    }

    // start the websearch
    if(source == Search_Everywhere ||
       source == Search_Web) {

        m_runningWebSearches.clear();
        m_websearchProgressMap.clear();

        /// start search using the general-purpose form's values
        QMap<QString, QString> queryTerms;
        queryTerms.insert("title", ui->editTitle->text());
        queryTerms.insert("free", ui->editContent->text());
        queryTerms.insert("author", ui->editAuthor->text());

        for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = m_itemToOnlineSearch.constBegin(); it != m_itemToOnlineSearch.constEnd(); ++it) {
            if (it.key()->checkState() == Qt::Checked) {
                it.value()->startSearch(queryTerms, numResults);
                m_runningWebSearches.insert(it.value());
            }
        }
    }

    emit newSearchStarted();
}

void SearchWidget::foundOnlineEntry(QSharedPointer<Entry> newEntry)
{
    OnlineSearchAbstract *engine = qobject_cast<OnlineSearchAbstract *>(sender());

    SearchResultEntry sre;
    sre.webEngine = engine;
    sre.webResult = newEntry;

    emit searchResult(sre);
}

void SearchWidget::foundNepomukEntry(QList<Nepomuk::Query::Result> newEntry)
{
    foreach(const Nepomuk::Query::Result &r, newEntry) {
        SearchResultEntry sre;
        sre.nepomukResult = r;
        //sre.webResult = 0;
        sre.webEngine = 0;

        emit searchResult(sre);
    }
}

void SearchWidget::nepomukQueryFinished()
{
    m_nepomukSearchInProgress = false;
    updateProgress(1,1);

    SearchSource source = SearchSource(ui->selectSource->currentIndex());
    if(source == Search_Nepomuk) {
        switchToSearch();
    }
    if(source == Search_Everywhere && m_runningWebSearches.isEmpty()) {
        switchToSearch();
    }
}

void SearchWidget::websearchStopped(int resultCode)
{
    Q_UNUSED(resultCode);

    OnlineSearchAbstract *engine = static_cast<OnlineSearchAbstract *>(sender());
    if (m_runningWebSearches.remove(engine)) {
        /// last search engine stopped
        if (m_runningWebSearches.isEmpty()) {
            SearchSource source = SearchSource(ui->selectSource->currentIndex());
            if(source == Search_Everywhere && !m_nepomukSearchInProgress) {
                switchToSearch();
            }
            if(source == Search_Web) {
                switchToSearch();
            }

            ui->progressBar->setValue(100);
        }
//        else {
//            QStringList remainingEngines;
//            foreach(OnlineSearchAbstract *running, m_runningSearches) {
//                remainingEngines.append(running->label());
//            }
//        }
    }
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

void SearchWidget::saveSettings()
{
    KConfig config;
    KConfigGroup searchSettingsGroup( &config, QLatin1String("SearchSettings") );
    searchSettingsGroup.writeEntry( QLatin1String("numOfResults"), ui->editNumberOfResults->value() );
    searchSettingsGroup.writeEntry( QLatin1String("searchMode"), ui->selectSource->currentIndex() );
    searchSettingsGroup.writeEntry( QLatin1String("findAudio"), ui->cbAudio->isChecked() );
    searchSettingsGroup.writeEntry( QLatin1String("findDocument"), ui->cbDocument->isChecked() );
    searchSettingsGroup.writeEntry( QLatin1String("findEmail"), ui->cbEmail->isChecked() );
    searchSettingsGroup.writeEntry( QLatin1String("findImage"), ui->cbImage->isChecked() );
    searchSettingsGroup.writeEntry( QLatin1String("findNote"), ui->cbNote->isChecked() );
    searchSettingsGroup.writeEntry( QLatin1String("findPublication"), ui->cbPublication->isChecked() );
    searchSettingsGroup.writeEntry( QLatin1String("findVideo"), ui->cbVideo->isChecked() );
    searchSettingsGroup.writeEntry( QLatin1String("findWebpage"), ui->cbWebpage->isChecked() );
    searchSettingsGroup.config()->sync();
}

void SearchWidget::loadSettings()
{
    KConfig config;
    KConfigGroup searchSettingsGroup( &config, QLatin1String("SearchSettings") );

    int numOfResults = searchSettingsGroup.readEntry( QLatin1String("numOfResults"), 50 );
    ui->editNumberOfResults->setValue(numOfResults);

    int searchMode = searchSettingsGroup.readEntry( QLatin1String("searchMode"), 0 );
    ui->selectSource->setCurrentIndex(searchMode);
    sourceChanged(searchMode);

    bool audioChecked = searchSettingsGroup.readEntry( QLatin1String("findAudio"), true );
    ui->cbAudio->setChecked(audioChecked);
    bool audioDocument = searchSettingsGroup.readEntry( QLatin1String("findDocument"), true );
    ui->cbDocument->setChecked(audioDocument);
    bool audioEmail = searchSettingsGroup.readEntry( QLatin1String("findEmail"), true );
    ui->cbEmail->setChecked(audioEmail);
    bool audioImage = searchSettingsGroup.readEntry( QLatin1String("findImage"), true );
    ui->cbImage->setChecked(audioImage);
    bool audioNote = searchSettingsGroup.readEntry( QLatin1String("findNote"), true );
    ui->cbNote->setChecked(audioNote);
    bool audioPublication = searchSettingsGroup.readEntry( QLatin1String("findPublication"), true );
    ui->cbPublication->setChecked(audioPublication);
    bool audioVideo = searchSettingsGroup.readEntry( QLatin1String("findVideo"), true );
    ui->cbVideo->setChecked(audioVideo);
    bool audioWebpage = searchSettingsGroup.readEntry( QLatin1String("findWebpage"), true );
    ui->cbWebpage->setChecked(audioWebpage);
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
    addEngine(new OnlineSearchIngentaConnect(this));

    connect(ui->listWebEngines, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemCheckChanged(QListWidgetItem*)));

    m_actionOpenHomepage = new KAction(KIcon(QLatin1String("internet-web-browser")), i18n("Go to Homepage"), this);
    connect(m_actionOpenHomepage, SIGNAL(triggered()), this, SLOT(openHomepage()));
    ui->listWebEngines->addAction(m_actionOpenHomepage);
    ui->listWebEngines->setContextMenuPolicy(Qt::ActionsContextMenu);

    switchToSearch();
}

void SearchWidget::addEngine(OnlineSearchAbstract *engine)
{
    KConfig config;
    KConfigGroup searchSettingsGroup( &config, QLatin1String("SearchSettings") );

    QListWidgetItem *item = new QListWidgetItem(engine->label(), ui->listWebEngines);
    item->setCheckState(searchSettingsGroup.readEntry(engine->name(), false) ? Qt::Checked : Qt::Unchecked);
    item->setIcon(engine->icon());
    item->setData(HomepageRole, engine->homepage());
    item->setData(NameRole, engine->name());

    m_itemToOnlineSearch.insert(item, engine);
    connect(engine, SIGNAL(foundEntry(QSharedPointer<Entry>)), this, SLOT(foundOnlineEntry(QSharedPointer<Entry>)));
    connect(engine, SIGNAL(stoppedSearch(int)), this, SLOT(websearchStopped(int)));
    connect(engine, SIGNAL(progress(int,int)), this, SLOT(updateProgress(int,int)));
}

void SearchWidget::switchToSearch()
{
    for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = m_itemToOnlineSearch.constBegin(); it != m_itemToOnlineSearch.constEnd(); ++it)
        disconnect(ui->buttonSearch, SIGNAL(clicked()), it.value(), SLOT(cancel()));

    connect(ui->buttonSearch, SIGNAL(clicked()), this, SLOT(startSearch()));
    ui->buttonSearch->setText(i18nc("Button that starts the search","Search"));
    ui->buttonSearch->setIcon(KIcon(QLatin1String("media-playback-start")));
    ui->optionsTab->setEnabled(true);
    ui->enginesTab->setEnabled(true);
}

void SearchWidget::switchToCancel()
{
    disconnect(ui->buttonSearch, SIGNAL(clicked()), this, SLOT(startSearch()));

    for (QMap<QListWidgetItem*, OnlineSearchAbstract*>::ConstIterator it = m_itemToOnlineSearch.constBegin(); it != m_itemToOnlineSearch.constEnd(); ++it)
        connect(ui->buttonSearch, SIGNAL(clicked()), it.value(), SLOT(cancel()));

    ui->buttonSearch->setText(i18n("Cancel"));
    ui->buttonSearch->setIcon(KIcon(QLatin1String("media-playback-stop")));
    ui->optionsTab->setEnabled(false);
    ui->enginesTab->setEnabled(false);
}
