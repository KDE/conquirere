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

#include <nepomuk-webminer/extractorfactory.h>

#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include <KDE/KConfigGroup>
#include <KDE/KConfig>
#include <KDE/KAction>
#include <KDE/KUrl>
#include <KDE/KDebug>

#include <QtCore/QtConcurrentRun>

#include <QtGui/QListWidgetItem>
#include <QtGui/QDesktopServices>
#include <QtGui/QMenu>

const int HomepageRole = Qt::UserRole + 5;
const int WidgetRole = Qt::UserRole + 6;
const int IdentifierRole = Qt::UserRole + 7;
const int DescriptionRole = Qt::UserRole + 7;

enum SearchSource {
    Search_Everywhere,
    Search_Nepomuk,
    Search_Web
};

SearchWidget::SearchWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchWidget)
    , m_actionOpenHomepage(0)
    , m_searchResultModel(0)
    , m_ef(0)
    , m_currentWebExtractor(0)
    , m_futureWatcherNepomuk(0)
{
    ui->setupUi(this);
    setupUi();

    //TODO: why has is the widget owner of the SearchResultModel?
    m_searchResultModel = new SearchResultModel();
    connect(this, SIGNAL(newSearchStarted()), m_searchResultModel, SLOT(clearData()));
    connect(this, SIGNAL(searchResult(QVariantList)), m_searchResultModel, SLOT(addSearchResult(QVariantList)) );

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
    delete m_actionOpenHomepage;
    delete m_searchResultModel;
    delete m_ef;
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
        //TODO: use resourcewatcher for project fetching
        if(!ui->labelLibrary->isEnabled()) {
            fetchProjects();
        }

        ui->labelLibrary->setEnabled(true);
        ui->editLibrary->setEnabled(true);
    }
}

void SearchWidget::fetchProjects()
{
    ui->editLibrary->clear();

    ui->editLibrary->addItem(i18n("Full Library"));

    QString query = "select DISTINCT ?resource ?name where { "
                     "?resource a pimo:Project ."
                     "?resource nao:prefLabel ?name ."
                     "}";

    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    while( it.next() ) {
        Soprano::BindingSet p = it.current();

        ui->editLibrary->addItem(p.value("name").toString(),
                                 p.value("resource").toString());
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
    if(!item)
        return;

    QString identifier = item->data(IdentifierRole).toString();

    if(item->checkState() == Qt::Checked) {
        m_pluginList << identifier;
    }
    else {
        m_pluginList.removeAll( identifier );
    }

    // do not allow to start a search without any checked search engine
    SearchSource source = SearchSource(ui->selectSource->currentIndex());
    if(source == Search_Web) {
        ui->buttonSearch->setEnabled(m_pluginList.size() > 0);
    }

    // save user selection to config
    KConfig config;
    KConfigGroup searchSettingsGroup( &config, QLatin1String("SearchSettings") );
    searchSettingsGroup.writeEntry(identifier, item->checkState() == Qt::Checked);
    searchSettingsGroup.config()->sync();
}

void SearchWidget::startSearch()
{
    emit newSearchStarted();

    switchToCancel();
    m_maxProgress = 0;
    ui->progressBar->setValue(0);
    int numResults = ui->editNumberOfResults->value();
    if(numResults < 1) numResults = 9999;

    SearchSource source = SearchSource(ui->selectSource->currentIndex());

    // start the nepomuk search
    if(source == Search_Everywhere || source == Search_Nepomuk) {
        m_maxProgress++;
        bool orQuery = true;

        //limit all results by its rdf:type if one is selected
        QStringList rdfTypes;
        if(ui->cbDocument->isChecked()) {
            rdfTypes.append(QLatin1String("nfo:Document"));
            rdfTypes.append(QLatin1String("nfo:PaginatedTextDocument"));
            rdfTypes.append(QLatin1String("nfo:PlainTextDocument"));
        }
        if(ui->cbAudio->isChecked()) {
            rdfTypes.append(QLatin1String("nfo:Audio"));
        }
        if(ui->cbVideo->isChecked()) {
            rdfTypes.append(QLatin1String("nfo:Video"));
        }
        if(ui->cbImage->isChecked()) {
            rdfTypes.append(QLatin1String("nfo:Image"));
        }
        if(ui->cbEmail->isChecked()) {
            rdfTypes.append(QLatin1String("nmo:Email"));
        }
        if(ui->cbNote->isChecked()) {
            rdfTypes.append(QLatin1String("pimo:Note"));
        }
        if(ui->cbNote->isChecked()) {
            rdfTypes.append(QLatin1String("pimo:Note"));
        }
        if(ui->cbPublication->isChecked()) {
            rdfTypes.append(QLatin1String("nbib:Publication"));
            rdfTypes.append(QLatin1String("nbib:Series"));
        }
        if(ui->cbWebpage->isChecked()) {
            rdfTypes.append(QLatin1String("nfo:Website"));
            rdfTypes.append(QLatin1String("nfo:Webpage"));
            rdfTypes.append(QLatin1String("nfo:Bookmark"));
        }

        QString limit_by_rdfTypes;
        if(!rdfTypes.isEmpty()) {
            limit_by_rdfTypes = QString::fromLatin1("?resource a ?type . FILTER (?type in ("+ rdfTypes.join(",").toLatin1() +") ).");
        }

        QString limit_by_content;
        if(!ui->editContent->text().isEmpty()) {
            limit_by_content = QString::fromLatin1("?resource nie:plainTextContent ?plainText ."
                                                   "FILTER(bif:contains(?plainText, \"'"+ ui->editContent->text().toLatin1() +"'\")) ."
                                                   "OPTIONAL { ?resource nie:title ?title . }"
                                                   "OPTIONAL { ?resource ?v ?creator . FILTER (?v in (nco:creator,nco:publisher,nbib:editor) ). ?creator nco:fullname ?name .}");
        }

        QString limit_by_title;
        if(!ui->editTitle->text().isEmpty()) {
            limit_by_title = QString::fromLatin1("?resource nie:title ?title ."
                                                 "FILTER(bif:contains(?title, \"'"+ ui->editTitle->text().toLatin1() +"'\")) ."
                                                 "OPTIONAL { ?resource nie:title ?title . }"
                                                 "OPTIONAL { ?resource ?v ?creator . FILTER (?v in (nco:creator,nco:publisher,nbib:editor) ). ?creator nco:fullname ?name .}");
        }

        QString limit_by_author;
        if(!ui->editAuthor->text().isEmpty()) {
            limit_by_author = QString::fromLatin1("?r ?v ?creator ."
                                                  "FILTER (?v in (nco:creator,nco:publisher,nbib:editor) )."
                                                  "?creator nco:fullname ?name ."
                                                  "FILTER(bif:contains(?name, \"'" + ui->editAuthor->text().toLatin1() + "'\")) ."
                                                  "OPTIONAL  { ?resource nie:label ?title . }");
        }

        QString limit_by_project;
        if(ui->editLibrary->currentIndex() != 0) {
            limit_by_project = QString("?resource nao:isRelated  <%1> .").arg(ui->editLibrary->itemData(ui->editLibrary->currentIndex()).toString());
        }

        QString orQueryString;
        if(orQuery) {
            orQueryString = QLatin1String("UNION");
        }

        QString query = QString::fromLatin1("select distinct ?resource ?title ?name ?type ?plaintext ?star ?url where {"
                                            + limit_by_rdfTypes.toLatin1()
                                            + limit_by_project.toLatin1() +
                                            "{" + limit_by_content.toLatin1() + "} " + orQueryString.toLatin1() +
                                            "{" + limit_by_title.toLatin1() + "} " + orQueryString.toLatin1() +
                                            "{" + limit_by_author.toLatin1() + "}"
                                            "OPTIONAL{?resource nie:url ?url . }"
                                            "OPTIONAL{?resource nao:numericRating ?star . }"
                                            "} LIMIT " + QString("%1").arg(numResults).toLatin1()
                                            );

        kDebug() << query;

        QFuture<QVariantList > future = QtConcurrent::run(this, &SearchWidget::queryNepomuk, query);
        m_futureWatcherNepomuk = new QFutureWatcher<QVariantList >();

        m_futureWatcherNepomuk->setFuture(future);
        connect(m_futureWatcherNepomuk, SIGNAL(finished()),this, SLOT(finishedNepomukQuery()));
    }

    // start the websearch
    if(source == Search_Everywhere ||
       source == Search_Web) {
        m_maxProgress += m_pluginList.size();

        m_currentExtractor = 0;
        queryWebExtractor(m_currentExtractor);
    }

}

void SearchWidget::finishedNepomukQuery()
{
    QVariantList resultList = m_futureWatcherNepomuk->future().result();
    // now we have all nepomuk resources
    emit searchResult(resultList);

    updateProgress();
}

void SearchWidget::finishedWebextractorQuery(const QVariantList &searchResults)
{
    kDebug() << "finished websearch";
    QVariantList resultList;

    // now add engine details to each result, so we can add more info in teh searchresultmodel
    foreach(const QVariant &v, searchResults) {
        QVariantMap vm = v.toMap();
        vm.insert(QLatin1String("engine-type"),QLatin1String("web"));
        vm.insert(QLatin1String("engine-name"), m_currentWebExtractor->info().name);
        vm.insert(QLatin1String("engine-id"), m_currentWebExtractor->info().identifier);
        vm.insert(QLatin1String("engine-icon"), m_currentWebExtractor->info().icon);
        vm.insert(QLatin1String("engine-script"), m_currentWebExtractor->info().file);
        resultList.append(vm);
    }

    updateProgress();
    emit searchResult(resultList);

    m_currentExtractor++;
    if(m_currentExtractor < m_pluginList.size()) {
        queryWebExtractor(m_currentExtractor);
    }
}

void SearchWidget::updateProgress()
{
    qreal progressPerItem = 100/m_maxProgress;
    ui->progressBar->setValue( ui->progressBar->value() + progressPerItem);

    if( ui->progressBar->value() > 98) { // might get stuck at 99 due to rounding
        ui->progressBar->setValue( 100 );
        switchToSearch();
    }
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

    KConfig config;
    KConfigGroup searchSettingsGroup( &config, QLatin1String("SearchSettings") );

    m_ef = new NepomukWebMiner::Extractor::ExtractorFactory;
    QList<NepomukWebMiner::Extractor::WebExtractor::Info> plugins = m_ef->listAvailablePlugins(QLatin1String("publication"));

    foreach(const NepomukWebMiner::Extractor::WebExtractor::Info & info, plugins) {
        QListWidgetItem *item = new QListWidgetItem(info.name, ui->listWebEngines);

        item->setCheckState(searchSettingsGroup.readEntry(info.identifier, false) ? Qt::Checked : Qt::Unchecked);
        item->setIcon( KIcon( info.icon ));
        item->setData(IdentifierRole, info.identifier);
        item->setData(Qt::ToolTipRole, info.description);

        if(item->checkState() == Qt::Checked) {
            m_pluginList << info.identifier;
        }
    }

    connect(ui->listWebEngines, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(itemCheckChanged(QListWidgetItem*)));

    m_actionOpenHomepage = new KAction(KIcon(QLatin1String("internet-web-browser")), i18n("Go to Homepage"), this);
    connect(m_actionOpenHomepage, SIGNAL(triggered()), this, SLOT(openHomepage()));
    ui->listWebEngines->addAction(m_actionOpenHomepage);
    ui->listWebEngines->setContextMenuPolicy(Qt::ActionsContextMenu);

    switchToSearch();
}

void SearchWidget::switchToSearch()
{
    connect(ui->buttonSearch, SIGNAL(clicked()), this, SLOT(startSearch()));
    ui->buttonSearch->setText(i18nc("Button that starts the search","Search"));
    ui->buttonSearch->setIcon(KIcon(QLatin1String("media-playback-start")));
    ui->optionsTab->setEnabled(true);
    ui->enginesTab->setEnabled(true);
}

void SearchWidget::switchToCancel()
{
    disconnect(ui->buttonSearch, SIGNAL(clicked()), this, SLOT(startSearch()));

    ui->buttonSearch->setText(i18n("Cancel"));
    ui->buttonSearch->setIcon(KIcon(QLatin1String("media-playback-stop")));
    ui->optionsTab->setEnabled(false);
    ui->enginesTab->setEnabled(false);
}

QVariantList SearchWidget::queryNepomuk(const QString &query)
{
    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    // combine all search results again, as we get lots of double resources for the different persons and so on
    QMap<QString, QStringList> resultList;
    int count = 0;
    while( it.next() ) {
        Soprano::BindingSet p = it.current();

        // get either a new entry or what we have inserted beforehand
        QStringList curEntry = resultList.value( p.value("resource").toString(), QStringList());

        // now set / add each queried value into the StringList
        if (curEntry.isEmpty() ) {
            curEntry << p.value("title").toString() << p.value("name").toString()
                     << p.value("type").toString() << p.value("plaintext").toString()
                     << p.value("star").toString() << p.value("url").toString();
        }
        else {
            // add additional info to existing resource (as a list split by ;)
            QString type = p.value("type").toString();
            if(!type.isEmpty() && !curEntry.at(3).contains(type))
                curEntry.replace(3, QString("%1;%2").arg(curEntry.at(3)).arg(type) );

            QString name = p.value("name").toString();
            if(!name.isEmpty() && !curEntry.at(2).contains(name))
                curEntry.replace(3, QString("%1;%2").arg(curEntry.at(2)).arg(name) );
        }

        // and save the result back into the map
        resultList.insert(p.value("resource").toString(), curEntry);
        count++;
    }

    // now that we have combined all search results properly.
    // press the min the same form as we get the web results, so we just need 1 way to add the dta to teh searchresult model

    QVariantList finalResultList;
    QMapIterator<QString, QStringList> i(resultList);
    while(i.hasNext()) {
        i.next();

        QVariantMap vm;
        vm.insert(QLatin1String("engine-type"), QLatin1String("nepomuk"));
        vm.insert(QLatin1String("engine-name"), i18n("Nepomuk"));
        vm.insert(QLatin1String("engine-icon"), QLatin1String("nepomuk"));
        vm.insert(QLatin1String("nepomuk-uri"), i.key());

        vm.insert(QLatin1String("title"), i.value().at(0));
        vm.insert(QLatin1String("authors"), i.value().at(1));
        vm.insert(QLatin1String("publicationtype"), i.value().at(2));
        //vm.insert(QLatin1String("details"), i.value().at(3)); //TODO: plaintext excerpt handling
        vm.insert(QLatin1String("date"), QLatin1String("")); //TODO: fetch date from nepomuk
        vm.insert(QLatin1String("star"), i.value().at(4));
        vm.insert(QLatin1String("fileurl"), i.value().at(5));

        finalResultList.append(vm);
    }

    return finalResultList;
}

void SearchWidget::queryWebExtractor(int nextExtractor)
{
    if(m_currentWebExtractor)
        disconnect(m_currentWebExtractor, SIGNAL(searchResults(QVariantList)), this, SLOT(finishedWebextractorQuery(QVariantList)));
    m_currentWebExtractor = m_ef->getExtractor(m_pluginList.at(nextExtractor));
    connect(m_currentWebExtractor, SIGNAL(searchResults(QVariantList)), this, SLOT(finishedWebextractorQuery(QVariantList)));

    QVariantMap searchParameters;
    searchParameters.insert("title", ui->editTitle->text());
    searchParameters.insert("alttitle", ui->editContent->text()); //TODO: enable content based websearch(abstract?)
    searchParameters.insert("author", ui->editAuthor->text());
    searchParameters.insert("yearMin", QString(""));
    searchParameters.insert("yearMax", QString(""));
    searchParameters.insert("journal", QString(""));

    m_currentWebExtractor->search( QLatin1String("publication"), searchParameters );
}
