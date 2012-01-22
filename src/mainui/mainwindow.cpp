/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "mainwindow.h"

#include "core/library.h"
#include "core/projectsettings.h"

#include "librarymanager.h"
#include "welcomewidget.h"
#include "resourcetablewidget.h"

#include "settings/conquireresettingsdialog.h"

#include "sidebar/sidebarwidget.h"

#include "dialogs/newprojectwizard.h"
#include "dialogs/loadproject.h"
#include "dialogs/selectopenproject.h"

#include "docklets/librarywidget.h"
#include "docklets/documentpreview.h"
#include "docklets/searchwidget.h"

#include "sync/syncbutton.h"

#include "nbibio/conquirere.h"

#include <KDE/KApplication>
#include <KDE/KAction>
#include <KDE/KLocale>
#include <KDE/KActionCollection>
#include <KDE/KStandardAction>
#include <KDE/KStandardDirs>
#include <KDE/KMessageBox>

#include <Nepomuk/Resource>
#include <Nepomuk/Variant>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/PIMO>

#include <QtGui/QLayout>


//DEBUG ADD to delete all nepomuk data
#include "nbib.h"
#include "sync.h"
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>


MainWindow::MainWindow(QWidget *parent)
    : KParts::MainWindow()
{
    Q_UNUSED(parent);

    setupMainWindow();
    setupActions();

    //create the system library
    Library *l = new Library();
    l->loadSystemLibrary();
    m_libraryManager->addSystemLibrary(l);

    m_sidebarWidget->newSelection(Resource_Library, Max_BibTypes, l);
}

MainWindow::~MainWindow()
{
    delete m_libraryWidget;
    delete m_mainView;
    delete m_sidebarWidget;
    delete m_searchWidget;
    //delete m_documentPreview // done in queryExit()

//    qDeleteAll(m_libraryList); not working ...
    QMapIterator<QUrl, QWidget *> i(m_libraryList);
     while (i.hasNext()) {
         i.next();
         delete i.value();
     }

     delete m_libraryManager;
     delete m_centerWindow;
}

void MainWindow::createLibrary()
{
    NewProjectWizard npw;

    int ret = npw.exec();

    if(ret == QDialog::Accepted) {
        m_libraryManager->addLibrary( npw.newLibrary() );
    }
}

void MainWindow::loadLibrary()
{
    LoadProject lp;

    int ret = lp.exec();

    if(ret == QDialog::Accepted) {
        m_libraryManager->addLibrary( lp.loadedLibrary() );
    }
}

void MainWindow::deleteLibrarySelection()
{
    QList<Library *> libList = m_libraryManager->openProjects();

    if(libList.isEmpty())
        return;

    Library *selectedLib = 0;

    if(libList.size() > 1) {
        SelectOpenProject sop;
        sop.setActionText(i18n("Which project should be deleted?"));
        sop.setProjectList(libList);

        int ret = sop.exec();

        if(ret != QDialog::Accepted) {
            return;
        }

        selectedLib = sop.getSelected();
    }
    else {
        selectedLib = libList.first();
    }

    int ret2 = KMessageBox::warningYesNo(this,
                                         QLatin1String("Do you really want to delete the project:<br><b>") +
                                         selectedLib->settings()->name());

    if(ret2 == KMessageBox::Yes) {
        m_libraryManager->deleteLibrary(selectedLib);
    }
}

void MainWindow::closeLibrarySelection()
{
    QList<Library *> libList = m_libraryManager->openProjects();

    if(libList.isEmpty())
        return;

    Library *selectedLib = 0;

    if(libList.size() > 1) {
        SelectOpenProject sop;
        sop.setActionText(i18n("Which project should be closed?"));
        sop.setProjectList(libList);

        int ret = sop.exec();

        if(ret != QDialog::Accepted) {
            return;
        }

        selectedLib = sop.getSelected();
    }
    else {
        selectedLib = libList.first();
    }

    m_libraryManager->closeLibrary(selectedLib);
}

void MainWindow::importZotero()
{
    m_libraryManager->importData( LibraryManager::Zotero_Sync);
}

void MainWindow::exportZotero()
{
    m_libraryManager->exportData(LibraryManager::Zotero_Sync);
}

void MainWindow::syncZotero()
{
    m_libraryManager->syncData(LibraryManager::Zotero_Sync);
}

void MainWindow::dbCheck()
{
    qDebug() << "MainWindow::dbCheck()";
}

void MainWindow::dbBackup()
{
    qDebug() << "MainWindow::dbBackup()";
}

void MainWindow::showConqSettings()
{
    ConquirereSettingsDialog csd;
    csd.setProjectSettings(m_libraryManager->systemLibrary()->settings());

    csd.exec();
}

void MainWindow::connectKPartGui(KParts::Part * part)
{
    // this cause the screen to flicker as it rebuilds the GUI
    // better yet add all kparts (okular, gwenview, khtml) on program start
    // together with all gui elements

    createGUI(part);
}

void MainWindow::switchView(ResourceSelection selection, BibEntryType filter, Library *selectedLibrary)
{
    // This case means we show the Welcome widget for the selected library
    if(selection == Resource_Library) {
        m_mainView->hide();

        //hide all welcome widgets in case we switch from one library to another
        foreach (QWidget *w, m_libraryList)
             w->hide();

        //show welcome page for the current library
        QWidget *ww = m_libraryList.value(selectedLibrary->settings()->projectThing().resourceUri());
        ww->show();

        // reset the sidebarwidget and documentpreview
        // if there was some Nepomuk::Resouce selected beforehand
        m_sidebarWidget->clear();
        m_documentPreview->clear();
    }
    // This shows the normal table view with all the data
    // the contend is based on teh users selection from te hgiven values of this function
    else {
        m_mainView->show();

        //hide all welcome widgets
        foreach (QWidget *w, m_libraryList)
             w->hide();

        m_mainView->switchView(selection, filter, selectedLibrary);
    }

    m_libraryManager->setCurrentUsedLibrary(selectedLibrary);
}

void MainWindow::showSearchResults()
{
    m_mainView->show();

    //hide all welcome widgets
    foreach (QWidget *w, m_libraryList)
         w->hide();

    m_mainView->showSearchResult();
    m_sidebarWidget->showSearchResults();

    m_libraryManager->setCurrentUsedLibrary(m_libraryManager->systemLibrary());
}

void MainWindow::DEBUGDELETEALLDATA()
{
    // fetcha data
    Nepomuk::Query::OrTerm orTerm;

    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Reference() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Publication() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NCO::Contact() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::DocumentPart() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Series() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Journal() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::SYNC::ServerSyncData() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::PIMO::Event() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::PIMO::Project() ) );

    Nepomuk::Query::Query query( orTerm );

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    foreach(const Nepomuk::Query::Result & r, queryResult) {
        r.resource().remove();
    }
}















void MainWindow::openLibrary(Library *l)
{
    // create a welcome widget for the library
    WelcomeWidget *ww = new WelcomeWidget(l);
    ww->hide();
    m_centerWindow->centralWidget()->layout()->addWidget(ww);
    m_libraryList.insert(l->settings()->projectThing().resourceUri(), ww);

    if(!m_libraryManager->openProjects().isEmpty()) {
        actionCollection()->action(QLatin1String("delete_project"))->setEnabled(true);
        actionCollection()->action(QLatin1String("close_project"))->setEnabled(true);
    }

    switchView(Resource_Library, Max_BibTypes, l);
}

void MainWindow::closeLibrary(const QUrl &projectThingUrl)
{
    if(m_libraryManager->openProjects().isEmpty()) {
        actionCollection()->action(QLatin1String("delete_project"))->setEnabled(false);
        actionCollection()->action(QLatin1String("close_project"))->setEnabled(false);
    }

    QWidget *w = m_libraryList.take(projectThingUrl);
    if(w) {
        w->hide();
        w->deleteLater();
    }

    switchView(Resource_Library, Max_BibTypes, m_libraryManager->systemLibrary());
}

bool MainWindow::queryExit()
{
    // this here is necessary ... otherwise we crash on close because of the hiding event ...
    createGUI(0);
    delete m_documentPreview;
    m_documentPreview = 0;
    return true;
}

void MainWindow::setupActions()
{
    //File action menu
    KAction* newProjectAction = new KAction(this);
    newProjectAction->setText(i18n("&Create Project"));
    newProjectAction->setIcon(KIcon(QLatin1String("document-new")));
    actionCollection()->addAction(QLatin1String("new_project"), newProjectAction);
    connect(newProjectAction, SIGNAL(triggered(bool)),this, SLOT(createLibrary()));

    KAction* loadProjectAction = new KAction(this);
    loadProjectAction->setText(i18n("&Load Project"));
    loadProjectAction->setIcon(KIcon(QLatin1String("document-open")));
    actionCollection()->addAction(QLatin1String("load_project"), loadProjectAction);
    connect(loadProjectAction, SIGNAL(triggered(bool)),this, SLOT(loadLibrary()));

    KAction* removeProjectAction = new KAction(this);
    removeProjectAction->setText(i18n("&Delete Project"));
    removeProjectAction->setIcon(KIcon(QLatin1String("document-close")));
    removeProjectAction->setEnabled(false);
    actionCollection()->addAction(QLatin1String("delete_project"), removeProjectAction);
    connect(removeProjectAction, SIGNAL(triggered(bool)),this, SLOT(deleteLibrarySelection()));

    KAction* closeProjectAction = new KAction(this);
    closeProjectAction->setText(i18n("&Close Project"));
    closeProjectAction->setIcon(KIcon(QLatin1String("document-close")));
    closeProjectAction->setEnabled(false);
    actionCollection()->addAction(QLatin1String("close_project"), closeProjectAction);
    connect(closeProjectAction, SIGNAL(triggered(bool)),this, SLOT(closeLibrarySelection()));

    // Database menu
    // import section
    KAction* importBibTexAction = new KAction(this);
    importBibTexAction->setText(i18n("File"));
    importBibTexAction->setIcon(KIcon(QLatin1String("application-rtf")));
    actionCollection()->addAction(QLatin1String("db_import_file"), importBibTexAction);
    connect(importBibTexAction, SIGNAL(triggered(bool)),m_libraryManager, SLOT(importData()));

    KAction* importZoteroAction = new KAction(this);
    importZoteroAction->setText(i18n("Zotero"));
    importZoteroAction->setIcon(KIcon(QLatin1String("storage-zotero")));
    actionCollection()->addAction(QLatin1String("db_import_zotero"), importZoteroAction);
    connect(importZoteroAction, SIGNAL(triggered(bool)),this, SLOT(importZotero()));

    // export section
    KAction* exportBibTexAction = new KAction(this);
    exportBibTexAction->setText(i18n("File"));
    exportBibTexAction->setIcon(KIcon(QLatin1String("application-rtf")));
    actionCollection()->addAction(QLatin1String("db_export_file"), exportBibTexAction);
    connect(exportBibTexAction, SIGNAL(triggered(bool)),m_libraryManager, SLOT(exportData()));

    KAction* exportZoteroAction = new KAction(this);
    exportZoteroAction->setText(i18n("Zotero"));
    exportZoteroAction->setIcon(KIcon(QLatin1String("storage-zotero")));
    actionCollection()->addAction(QLatin1String("db_export_zotero"), exportZoteroAction);
    connect(exportZoteroAction, SIGNAL(triggered(bool)),this, SLOT(exportZotero()));

    // sync actions
    KAction* syncZoteroAction = new KAction(this);
    syncZoteroAction->setText(i18n("Zotero"));
    syncZoteroAction->setIcon(KIcon(QLatin1String("storage-zotero")));
    actionCollection()->addAction(QLatin1String("db_sync_zotero"), syncZoteroAction);
    connect(syncZoteroAction, SIGNAL(triggered(bool)),this, SLOT(syncZotero()));

    KAction* triggerBackgroundSyncAction = new KAction(this);
    triggerBackgroundSyncAction->setText(i18n("Synchronize Collection"));
    triggerBackgroundSyncAction->setIcon(KIcon(QLatin1String("view-refresh")));
    actionCollection()->addAction(QLatin1String("db_background_sync"), triggerBackgroundSyncAction);
    connect(triggerBackgroundSyncAction, SIGNAL(triggered(bool)),m_syncButton, SLOT(startSync()));

    // other database actions
    KAction* dbCheckAction = new KAction(this);
    dbCheckAction->setEnabled(false);
    dbCheckAction->setText(i18n("Check Database"));
    dbCheckAction->setIcon(KIcon(QLatin1String("document-preview-archive")));
    actionCollection()->addAction(QLatin1String("db_check"), dbCheckAction);
    connect(dbCheckAction, SIGNAL(triggered(bool)),this, SLOT(dbCheck()));

    KAction* dbBackupAction = new KAction(this);
    dbBackupAction->setEnabled(false);
    dbBackupAction->setText(i18n("Backup Database"));
    dbBackupAction->setIcon(KIcon(QLatin1String("svn-update")));
    actionCollection()->addAction(QLatin1String("db_backup"), dbBackupAction);
    connect(dbBackupAction, SIGNAL(triggered(bool)),this, SLOT(dbBackup()));

    //View menu
    KAction* updateListCache = new KAction(this);
    updateListCache->setText(i18n("Update List Cache"));
    updateListCache->setIcon(KIcon(QLatin1String("view-refresh")));
    actionCollection()->addAction(QLatin1String("update_list_cache"), updateListCache);
    connect(updateListCache, SIGNAL(triggered(bool)),m_libraryManager, SLOT(updateListCache()));

    // settings action
    KAction* openSettings = new KAction(this);
    openSettings->setText(i18n("Configure Conquirere"));
    openSettings->setIcon(KIcon(QLatin1String("configure")));
    actionCollection()->addAction(QLatin1String("open_settings"), openSettings);
    connect(openSettings, SIGNAL(triggered(bool)),this, SLOT(showConqSettings()));


    // ##############################################
    // Debug data
    KAction* debugDELETE = new KAction(this);
    debugDELETE->setText(i18n("DEBUG DELETE ALL DATA!"));
    debugDELETE->setIcon(KIcon(QLatin1String("document-close")));
    actionCollection()->addAction(QLatin1String("debug_delete"), debugDELETE);
    connect(debugDELETE, SIGNAL(triggered(bool)),this, SLOT(DEBUGDELETEALLDATA()));

    // ##############################################

    actionCollection()->addAction(QLatin1String("toggle_library"), m_libraryWidget->toggleViewAction());
    actionCollection()->addAction(QLatin1String("toggle_sidebar"), m_sidebarWidget->toggleViewAction());
    actionCollection()->addAction(QLatin1String("toggle_docpreview"), m_documentPreview->toggleViewAction());
    actionCollection()->addAction(QLatin1String("toggle_search"), m_searchWidget->toggleViewAction());

    KStandardAction::quit(kapp, SLOT(quit()),actionCollection());

    setupGUI(Default, KStandardDirs::locate("appdata",QLatin1String("conquirereui.rc")));
}

void MainWindow::setupMainWindow()
{
    m_libraryManager = new LibraryManager;
    connect(m_libraryManager, SIGNAL(libraryAdded(Library*)), this, SLOT(openLibrary(Library*)));
    connect(m_libraryManager, SIGNAL(libraryRemoved(QUrl)), this, SLOT(closeLibrary(QUrl)));

    // we start by applying all hidden sections from the "OnRestart" key to the real hidden part
    // without ths difference, we could end up badly when the user chanegs the hiddens election and
    // does not restart the program immediately

    ConqSettings::setHiddenNbibPublications( ConqSettings::hiddenNbibPublicationsOnRestart() );
    ConqSettings::setHiddenNbibSeries( ConqSettings::hiddenNbibSeriesOnRestart() );
    ConqSettings::self()->writeConfig();

    if(ConqSettings::documentPosition() == ConqSettings::EnumDocumentPosition::middle) {
        // create second mainwindow that holds the document preview
        // this way the bottum doc widget will not take up all the space but is
        // limited by the left/right doc
        m_centerWindow = new QMainWindow(this);
        m_centerWindow->setWindowFlags(Qt::Widget);
        QWidget *nw = new QWidget();
        QVBoxLayout *mainLayout = new QVBoxLayout();
        nw->setLayout(mainLayout);
        m_centerWindow->setCentralWidget(nw);
        setCentralWidget(m_centerWindow);

        // the main table view
        m_mainView = new ResourceTableWidget();
        m_mainView->hide();
        m_mainView->setLibraryManager(m_libraryManager);
        mainLayout->addWidget(m_mainView);

        //add panel for the document preview
        m_documentPreview = new DocumentPreview();
        m_centerWindow->addDockWidget(Qt::BottomDockWidgetArea, m_documentPreview);

    }
    else if(ConqSettings::documentPosition() == ConqSettings::EnumDocumentPosition::independant) {
        m_centerWindow = this;
        QWidget *nw = new QWidget();
        QVBoxLayout *mainLayout = new QVBoxLayout();
        nw->setLayout(mainLayout);
        m_centerWindow->setCentralWidget(nw);

        // the main table view
        m_mainView = new ResourceTableWidget();
        m_mainView->hide();
        m_mainView->setLibraryManager(m_libraryManager);
        mainLayout->addWidget(m_mainView);

        //add panel for the document preview
        m_documentPreview = new DocumentPreview();
        m_centerWindow->addDockWidget(Qt::RightDockWidgetArea, m_documentPreview);
    }

    // the left project bar
    m_libraryWidget = new LibraryWidget;
    m_libraryWidget->setLibraryManager(m_libraryManager);
    addDockWidget(Qt::LeftDockWidgetArea, m_libraryWidget);
    connect(m_libraryWidget, SIGNAL(showSearchResults()), this, SLOT(showSearchResults()));

    //add panel for the document info
    m_sidebarWidget = new SidebarWidget;
    m_sidebarWidget->setLibraryManager(m_libraryManager);
    addDockWidget(Qt::RightDockWidgetArea, m_sidebarWidget);

    // the search widget
    m_searchWidget = new SearchWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, m_searchWidget);
    m_mainView->setSearchResultModel(m_searchWidget->searchResultModel());
    connect(m_searchWidget, SIGNAL(newSearchStarted()), this, SLOT(showSearchResults()));

    connect(m_libraryWidget, SIGNAL(newSelection(ResourceSelection,BibEntryType,Library*)),
            m_sidebarWidget, SLOT(newSelection(ResourceSelection,BibEntryType,Library*)));

    connect(m_mainView, SIGNAL(selectedResource(Nepomuk::Resource&)),
            m_sidebarWidget, SLOT(setResource(Nepomuk::Resource&)));

    connect(m_mainView, SIGNAL(selectedMultipleResources(QList<Nepomuk::Resource>)),
            m_sidebarWidget, SLOT(setMultipleResources(QList<Nepomuk::Resource>)));

    connect(m_mainView, SIGNAL(selectedResource(Nepomuk::Resource&)),
            m_documentPreview, SLOT(setResource(Nepomuk::Resource&)));

    connect(m_documentPreview, SIGNAL(activateKPart(KParts::Part*)),
            this, SLOT(connectKPartGui(KParts::Part*)));

    connect(m_libraryWidget, SIGNAL(newSelection(ResourceSelection,BibEntryType,Library*)),
            this, SLOT(switchView(ResourceSelection,BibEntryType,Library*)));

    loadConfig();

    m_syncButton = new SyncButton();
    m_syncButton->setMainWindow(this);
}

void MainWindow::loadConfig()
{
    KSharedConfigPtr config = KSharedConfig::openConfig("conquirererc");

    KConfigGroup generalGroup = config->group("General");
    QString NepomukCollection = generalGroup.readEntry( "NepomukCollection", QString() );

    if(NepomukCollection.isEmpty()) {
        qDebug() << "create nepomuk collection";
        Nepomuk::Resource collection(QUrl(), Nepomuk::Vocabulary::PIMO::Collection());
        collection.setProperty(Soprano::Vocabulary::NAO::identifier(), QString("Conquirere Collection"));
        generalGroup.writeEntry("NepomukCollection", collection.resourceUri().toString());
        config->sync();
    }
}
