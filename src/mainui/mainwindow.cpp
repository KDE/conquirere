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
#include "splashscreen.h"

#include "settings/conquireresettingsdialog.h"

#include "sidebar/sidebarwidget.h"

#include "dialogs/newprojectwizard.h"
#include "dialogs/loadproject.h"
#include "dialogs/selectopenproject.h"
#include "dialogs/dbcheckdialog.h"

#include "docklets/librarywidget.h"
#include "docklets/documentpreview.h"
#include "docklets/searchwidget.h"

#include "sync/backgroundsync.h"
#include "sync/storagesyncwizard.h"

#include "nbibio/conquirere.h"

#include <KDE/KApplication>
#include <KDE/KAction>
#include <KDE/KLocale>
#include <KDE/KActionCollection>
#include <KDE/KStandardAction>
#include <KDE/KStandardDirs>
#include <KDE/KMessageBox>
#include <KDE/KProgressDialog>

#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/PIMO>
#include <KDE/KStatusBar>
#include <KDE/KDebug>

#include <QtGui/QLayout>
#include <QtGui/QSplitter>


MainWindow::MainWindow(QWidget *parent)
    : KParts::MainWindow()
{
    Q_UNUSED(parent);

    SplashScreen *splash = new SplashScreen;

    if(ConqSettings::splashScreen()) {
        splash->show();
    }

    splash->message(QString("load mainwindow .."));
    setupMainWindow();
    splash->message(QString("load actions ..."));
    setupActions();

    setupLibrary( splash );

    splash->hide();
    delete splash;
}

MainWindow::~MainWindow()
{
//    delete m_libraryWidget;
//    delete m_tableWidget;
//    delete m_sidebarWidget;
//    delete m_searchWidget;
    //delete m_documentPreview // done in queryExit()

//    qDeleteAll(m_libraryList); not working ...
    QMapIterator<QUrl, QWidget *> i(m_libraryList);
     while (i.hasNext()) {
         i.next();
         delete i.value();
     }

     delete m_libraryManager;
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

void MainWindow::syncStorage()
{
    StorageSyncWizard ssw;

    ssw.setLibraryManager(m_libraryManager);

    ssw.exec();
}

void MainWindow::dbCheck()
{
    DbCheckDialog dbcd;

    dbcd.exec();
}

void MainWindow::dbBackup()
{
    kDebug() << "not implemented yet ...";
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
        m_tableWidget->hide();

        //hide all welcome widgets in case we switch from one library to another
        foreach (QWidget *w, m_libraryList)
             w->hide();

        //show welcome page for the current library
        QWidget *ww = m_libraryList.value(selectedLibrary->settings()->projectThing().uri());
        ww->show();

        // reset the sidebarwidget and documentpreview
        // if there was some Nepomuk2::Resouce selected beforehand
        m_sidebarWidget->clear();
        m_documentPreview->clear();
    }
    // This shows the normal table view with all the data
    // the contend is based on teh users selection from te hgiven values of this function
    else {
        m_tableWidget->show();

        //hide all welcome widgets
        foreach (QWidget *w, m_libraryList)
             w->hide();

        m_tableWidget->switchView(selection, filter, selectedLibrary);
    }

    m_libraryManager->setCurrentUsedLibrary(selectedLibrary);
}

void MainWindow::showSearchResults()
{
    m_tableWidget->show();

    //hide all welcome widgets
    foreach (QWidget *w, m_libraryList)
         w->hide();

    m_tableWidget->showSearchResult();
    m_sidebarWidget->showSearchResults();

    m_libraryManager->setCurrentUsedLibrary(m_libraryManager->systemLibrary());
}

void MainWindow::viewFullModeCache()
{
    QList<int> fullMain; fullMain << 10 << 5000 << 10;
    m_mainSplitter->setSizes(fullMain);

    QList<int> fullMiddle; fullMiddle << 100 << 30;
    m_middleSplitter->setSizes(fullMiddle);
}

void MainWindow::viewDocumentModeCache()
{
    QList<int> fullMain; fullMain << 0 << 5000 << 10;
    m_mainSplitter->setSizes(fullMain);

    QList<int> fullMiddle; fullMiddle << 0 << 10;
    m_middleSplitter->setSizes(fullMiddle);
}

void MainWindow::viewProjectModeCache()
{
    QList<int> fullMain; fullMain << 10 << 5000 << 10;
    m_mainSplitter->setSizes(fullMain);

    QList<int> fullMiddle; fullMiddle << 100 << 0;
    m_middleSplitter->setSizes(fullMiddle);
}

void MainWindow::openLibrary(Library *l)
{
    // create a welcome widget for the library
    WelcomeWidget *ww = new WelcomeWidget(l);
    ww->hide();
    m_centralLayout->addWidget(ww);
    m_libraryList.insert(l->settings()->projectThing().uri(), ww);

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

void MainWindow::startFullSync()
{
    m_kpd = new KProgressDialog;
    m_kpd->setMinimumWidth(400);
    BackgroundSync *backgroundSyncManager = new BackgroundSync;
    backgroundSyncManager->setLibraryManager(m_libraryManager);

    connect(backgroundSyncManager, SIGNAL(progress(int)), this, SLOT(setSyncProgress(int)));
    connect(backgroundSyncManager, SIGNAL(progressStatus(QString)), this, SLOT(setSyncStatus(QString)));
    connect(backgroundSyncManager, SIGNAL(allSyncTargetsFinished()), this, SLOT(syncFinished()));
    connect(m_kpd, SIGNAL(cancelClicked()), backgroundSyncManager, SLOT(cancelSync()));

    backgroundSyncManager->startSync();

    m_kpd->exec();
}

void MainWindow::setSyncProgress(int value)
{
    if(m_kpd)
        m_kpd->progressBar()->setValue(value);
}

void MainWindow::setSyncStatus(const QString &status)
{
    if(m_kpd)
        m_kpd->setLabelText(status);
}

void MainWindow::syncFinished()
{
    delete m_kpd;
    m_kpd = 0;
    kDebug() << "cleanup again";

    BackgroundSync *bs = qobject_cast<BackgroundSync *>(sender());
    if(bs)
        delete bs;
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
    removeProjectAction->setIcon(KIcon(QLatin1String("edit-delete-shred")));
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
    importBibTexAction->setText(i18n("Import from File"));
    importBibTexAction->setIcon(KIcon(QLatin1String("document-import")));
    actionCollection()->addAction(QLatin1String("db_import_file"), importBibTexAction);
    connect(importBibTexAction, SIGNAL(triggered(bool)),m_libraryManager, SLOT(importData()));

    // export section
    KAction* exportBibTexAction = new KAction(this);
    exportBibTexAction->setText(i18n("Export to File"));
    exportBibTexAction->setIcon(KIcon(QLatin1String("document-export")));
    actionCollection()->addAction(QLatin1String("db_export_file"), exportBibTexAction);
    connect(exportBibTexAction, SIGNAL(triggered(bool)),m_libraryManager, SLOT(exportData()));

    // sync actions
    KAction* syncZoteroAction = new KAction(this);
    syncZoteroAction->setText(i18n("External Storage Sync"));
    syncZoteroAction->setIcon(KIcon(QLatin1String("svn-update")));
    actionCollection()->addAction(QLatin1String("db_sync_storage"), syncZoteroAction);
    connect(syncZoteroAction, SIGNAL(triggered(bool)),this, SLOT(syncStorage()));

    KAction* triggerBackgroundSyncAction = new KAction(this);
    triggerBackgroundSyncAction->setText(i18n("Synchronize Collection"));
    triggerBackgroundSyncAction->setIcon(KIcon(QLatin1String("view-refresh")));
    actionCollection()->addAction(QLatin1String("db_background_sync"), triggerBackgroundSyncAction);
    connect(triggerBackgroundSyncAction, SIGNAL(triggered(bool)), this, SLOT(startFullSync()));

    // other database actions
    KAction* dbCheckAction = new KAction(this);
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
    KAction* viewFullModeCache = new KAction(this);
    viewFullModeCache->setText(i18n("View Full Mode"));
    viewFullModeCache->setIcon(KIcon(QLatin1String("view-choose")));
    actionCollection()->addAction(QLatin1String("view_full_mode"), viewFullModeCache);
    connect(viewFullModeCache, SIGNAL(triggered(bool)),this, SLOT(viewFullModeCache()));

    KAction* viewDocumentModeCache = new KAction(this);
    viewDocumentModeCache->setText(i18n("View Document Mode"));
    viewDocumentModeCache->setIcon(KIcon(QLatin1String("view-table-of-contents-ltr")));
    actionCollection()->addAction(QLatin1String("view_document_mode"), viewDocumentModeCache);
    connect(viewDocumentModeCache, SIGNAL(triggered(bool)),this, SLOT(viewDocumentModeCache()));

    KAction* viewProjectModeCache = new KAction(this);
    viewProjectModeCache->setText(i18n("View Project Mode"));
    viewProjectModeCache->setIcon(KIcon(QLatin1String("view-media-playlist")));
    actionCollection()->addAction(QLatin1String("view_project_mode"), viewProjectModeCache);
    connect(viewProjectModeCache, SIGNAL(triggered(bool)),this, SLOT(viewProjectModeCache()));

    // settings action
    KAction* openSettings = new KAction(this);
    openSettings->setText(i18n("Configure Conquirere"));
    openSettings->setIcon(KIcon(QLatin1String("configure")));
    actionCollection()->addAction(QLatin1String("open_settings"), openSettings);
    connect(openSettings, SIGNAL(triggered(bool)),this, SLOT(showConqSettings()));

    KStandardAction::quit(kapp, SLOT(quit()),actionCollection());

    setupGUI(Default, KStandardDirs::locate("appdata",QLatin1String("conquirereui.rc")));

    statusBar()->hide();
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

    // create widget centralWidget
    QWidget *nw = new QWidget(this);
    nw->setLayout(new QVBoxLayout(nw));
    setCentralWidget(nw);

    m_mainSplitter = new QSplitter(nw);
    nw->layout()->addWidget(m_mainSplitter);

    // the left project bar (left side)
    m_libraryWidget = new LibraryWidget;
    m_libraryWidget->setLibraryManager(m_libraryManager);
    connect(m_libraryWidget, SIGNAL(showSearchResults()), this, SLOT(showSearchResults()));
    m_mainSplitter->addWidget( m_libraryWidget );

    // create widget for the middle section
    QWidget *middleSection = new QWidget();
    middleSection->setLayout(new QVBoxLayout(middleSection));
    m_mainSplitter->addWidget( middleSection );

    //add panel for the document info (right side)
    m_sidebarWidget = new SidebarWidget;
    m_sidebarWidget->setLibraryManager(m_libraryManager);
    m_mainSplitter->addWidget( m_sidebarWidget );

    // now add another  splitter for the middle section
    // top row a widget that changes between welcome widget and table vie
    // below the document view

    m_middleSplitter = new QSplitter(middleSection);
    m_middleSplitter->setOrientation(Qt::Vertical);
    middleSection->layout()->addWidget( m_middleSplitter );

    // the widget for the table/welcome widget change
    QWidget *central = new QWidget();
    m_centralLayout = new QVBoxLayout(central);
    central->setLayout(m_centralLayout);
    m_middleSplitter->addWidget( central );

    // the main table view
    m_tableWidget = new ResourceTableWidget();
    m_tableWidget->hide();
    m_tableWidget->setLibraryManager(m_libraryManager);
    m_tableWidget->setSearchResultModel(m_libraryWidget->searchResultModel());
    m_centralLayout->addWidget(m_tableWidget);

    m_documentPreview = new DocumentPreview();
    m_middleSplitter->addWidget(m_documentPreview);

    // set some default sizes so the resourcetable/welcome widget is maximised
    m_mainSplitter->setStretchFactor(0,1);
    m_mainSplitter->setStretchFactor(1,100);
    m_mainSplitter->setStretchFactor(2,1);
    m_middleSplitter->setStretchFactor(0,100);
    m_middleSplitter->setStretchFactor(1,20);
    QList<int> hideDocPreviewSizes; hideDocPreviewSizes << 100 << 0;
    m_middleSplitter->setSizes(hideDocPreviewSizes);


    connect(m_libraryWidget, SIGNAL(newSelection(ResourceSelection,BibEntryType,Library*)),
            m_sidebarWidget, SLOT(newSelection(ResourceSelection,BibEntryType,Library*)));

    connect(m_tableWidget, SIGNAL(selectedResource(Nepomuk2::Resource&, bool)),
            m_sidebarWidget, SLOT(setResource(Nepomuk2::Resource&)));

    connect(m_tableWidget, SIGNAL(selectedMultipleResources(QList<Nepomuk2::Resource>)),
            m_sidebarWidget, SLOT(setMultipleResources(QList<Nepomuk2::Resource>)));

    connect(m_tableWidget, SIGNAL(selectedResource(Nepomuk2::Resource&, bool)),
            m_documentPreview, SLOT(setResource(Nepomuk2::Resource&, bool)));

    connect(m_sidebarWidget, SIGNAL(openDocument(Nepomuk2::Resource&,bool)),
            m_documentPreview, SLOT(setResource(Nepomuk2::Resource&, bool)));

    connect(m_documentPreview, SIGNAL(activeDocumentChanged(Nepomuk2::Resource&)),
            m_sidebarWidget, SLOT(setResource(Nepomuk2::Resource&)));

    connect(m_documentPreview, SIGNAL(activateKPart(KParts::Part*)),
            this, SLOT(connectKPartGui(KParts::Part*)));

    connect(m_libraryWidget, SIGNAL(newSelection(ResourceSelection,BibEntryType,Library*)),
            this, SLOT(switchView(ResourceSelection,BibEntryType,Library*)));
}

void MainWindow::setupLibrary( SplashScreen *splash )
{
    //create the system library
    Library *l = new Library();

    connect(l, SIGNAL(statusMessage(QString)), splash, SLOT(message(QString)));

    l->loadSystemLibrary( );
    m_libraryManager->addSystemLibrary(l);

    m_sidebarWidget->newSelection(Resource_Library, Max_BibTypes, l);
}
