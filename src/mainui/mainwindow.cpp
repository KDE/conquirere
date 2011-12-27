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
#include "core/models/nepomukmodel.h"

#include "welcomewidget.h"
#include "resourcetablewidget.h"

#include "sidebar/sidebarwidget.h"
#include "dialogs/newprojectwizard.h"
#include "dialogs/loadproject.h"
#include "docklets/librarywidget.h"
#include "docklets/documentpreview.h"
#include "docklets/websearchwidget.h"

#include "sync/bibtexexportdialog.h"
#include "sync/bibteximportwizard.h"
#include "sync/synczoterodialog.h"
#include "sync/syncbutton.h"

#include <KDE/KApplication>
#include <KDE/KAction>
#include <KDE/KLocale>
#include <KDE/KActionCollection>
#include <KDE/KStandardAction>
#include <KDE/KFileDialog>
#include <KDE/KMessageBox>
#include <KDE/KIO/NetAccess>
#include <KDE/KGlobalSettings>

#include <Nepomuk/Resource>
#include <Nepomuk/Variant>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/PIMO>

#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QSortFilterProxyModel>

#include <QtCore/QDebug>


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

//DEBUG online storage sync

#include "onlinestorage/zotero/readfromzotero.h"
#include "onlinestorage/zotero/writetozotero.h"
#include "onlinestorage/zotero/synczotero.h"


#include "nbibio/pipe/nepomuktobibtexpipe.h"
#include "nbibio/nbibexporterfile.h"
#include <kbibtex/fileexporterbibtex.h>
#include <kbibtex/fileimporterbibtex.h>
#include "onlinestorage/syncstorageui.h"

MainWindow::MainWindow(QWidget *parent)
    : KParts::MainWindow()
    , m_curLibrary(0)
{
    setupMainWindow();
    setupActions();
}

MainWindow::~MainWindow()
{
    //createGUI(0); // disconnects KPart gui elements again

    delete m_libraryWidget;
    delete m_mainView;
    delete m_sidebarWidget;
    delete m_documentPreview;
    delete m_webSearchWidget;

    qDeleteAll(m_libraryList);
}

void MainWindow::createLibrary()
{
    NewProjectWizard npw;

    int ret = npw.exec();

    if(ret == QDialog::Accepted) {
        openLibrary(npw.newLibrary());
    }
}

void MainWindow::loadLibrary()
{
    LoadProject lp;

    int ret = lp.exec();

    if(ret == QDialog::Accepted) {
        openLibrary(lp.loadedLibrary());
    }
}

void MainWindow::openLibrary(Library *l)
{
    m_libraryWidget->addLibrary(l);

    // create a welcome widget for the library
    WelcomeWidget *ww = new WelcomeWidget(l);
    ww->hide();
    m_centerWindow->centralWidget()->layout()->addWidget(ww);
    m_libraryList.insert(l, ww);

    if(m_libraryList.size() > 1) {
        actionCollection()->action(QLatin1String("delete_project"))->setEnabled(true);
        actionCollection()->action(QLatin1String("close_project"))->setEnabled(true);
    }

    foreach (QSortFilterProxyModel *model, l->viewModels()) {
        NepomukModel *m = qobject_cast<NepomukModel *>(model->sourceModel());

        m->startFetchData();
    }

    switchView(Resource_Library, Max_BibTypes, l);
}

QList<Library *> MainWindow::openLibraries()
{
    QList<Library *> openLibraryList;

    QMapIterator<Library *, QWidget *> i(m_libraryList);
    while (i.hasNext()) {
        i.next();
        if(i.key()->libraryType() == Library_Project) {
            openLibraryList.append(i.key());
        }
    }

     return openLibraryList;
}

void MainWindow::deleteLibrary()
{
    int ret = KMessageBox::warningYesNo(this,
                                        QLatin1String("Do you really want to delete the project:<br><b>") +
                                        m_curLibrary->name());

    if(ret == KMessageBox::Yes) {
        m_curLibrary->deleteLibrary();
        closeLibrary();
    }
}

void MainWindow::closeLibrary()
{
    if(m_curLibrary->libraryType() == Library_System) {
        m_curLibrary = openLibraries().first();
        if(!m_curLibrary)
            return;
    }

    m_libraryWidget->closeLibrary(m_curLibrary);
    QWidget *w = m_libraryList.take(m_curLibrary);
    w->hide();
    w->deleteLater();

    delete m_curLibrary;

    switchView(Resource_Library, Max_BibTypes, m_systemLibrary);
}

void MainWindow::importBibTex()
{
    BibTeXImportWizard bid;
    bid.setSystemLibrary(m_systemLibrary);

    bid.exec();
}

void MainWindow::importZotero()
{
    SyncZoteroDialog szd;

    szd.exec();
}

void MainWindow::exportBibTex()
{
    BibTexExportDialog bed;
    bed.setInitialFileType(NBibExporterFile::EXPORT_BIBTEX);

    bed.exec();
}

void MainWindow::exportZotero()
{
    SyncZoteroDialog szd;

    szd.exec();
}

void MainWindow::exportPdf()
{
    BibTexExportDialog bed;
    bed.setInitialFileType(NBibExporterFile::EXPORT_PDF);

    bed.exec();
}

void MainWindow::exportOtherFile()
{
    BibTexExportDialog bed;
    bed.setInitialFileType(NBibExporterFile::EXPORT_HTML);

    bed.exec();
}

void MainWindow::syncZotero()
{
//    SyncZoteroDialog szd;

//    szd.exec();

    // test cases
    // load bibfile from hard drive
    QFile loadFile(QString("/home/joerg/kbibtex_zotero.bib"));
    if (!loadFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "could not open file";
        return;
    }

    FileImporterBibTeX fibt;
    m_bibFile = fibt.load(&loadFile);

    qDebug() << "imported bibfile" << m_bibFile->size();

    // sync with zotero
    SyncZotero *sz = new SyncZotero();
    connect(sz, SIGNAL(syncInProgress(bool)), this, SLOT(syncZoteroRDY(bool)));

    ProviderSyncDetails psd;
    psd.userName = QString("795913");
    psd.pwd = QString("TBydrlOdZo05mmzMhO8PlWCv");
    psd.url = QString("users");
    psd.askBeforeDeletion = false;
    psd.collection = QString("");
    psd.syncMode = Full_Sync;
    psd.mergeMode = Manual;

    sz->setProviderSettings(psd);
    sz->syncWithStorage(m_bibFile);
}

void MainWindow::syncZoteroRDY(bool inprogress)
{
    if(!inprogress) {
        // save back to disk
        QFile saveFile(QString("/home/joerg/kbibtex_zotero_afterDownload.bib"));
        if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "could not open file";
            return;
        }

        FileExporterBibTeX febt;
        febt.save(&saveFile, m_bibFile);
    }
}

void MainWindow::dbCheck()
{
    qDebug() << "MainWindow::dbCheck()";
}

void MainWindow::dbBackup()
{
    qDebug() << "MainWindow::dbBackup()";
}

void MainWindow::connectKPartGui(KParts::Part * part)
{
    // this cause the screen to flicker as it rebuilds the GUI
    // better yet add all kparts (okular, gwenview, khtml) on program start
    // together with all gui elements
    //createGUI(part);
}

void MainWindow::switchView(ResourceSelection selection, BibEntryType filter, Library *p)
{
    if(selection == Resource_Library) {
        m_mainView->hide();

        //hide all welcome widgets in case we switch from one library to another
        foreach (QWidget *w, m_libraryList)
             w->hide();

        //show welcome page for the current library
        QWidget *ww = m_libraryList.value(p);
        ww->show();

        m_sidebarWidget->clear();
        m_documentPreview->clear();
    }
    else {
        m_mainView->show();

        //hide all welcome widgets
        foreach (QWidget *w, m_libraryList)
             w->hide();

        m_mainView->switchView(selection, filter, p);
    }

    m_curLibrary = p;

    if(m_curLibrary->libraryType() == Library_System) {
        QAction *a = actionCollection()->action(QLatin1String("delete_project"));
        if(a)
            a->setEnabled(false);
        QAction *b = actionCollection()->action(QLatin1String("close_project"));
        if(b)
            b->setEnabled(false);
    }
    else {
        QAction *a = actionCollection()->action(QLatin1String("delete_project"));
        if(a)
            a->setEnabled(true);
        QAction *b = actionCollection()->action(QLatin1String("close_project"));
        if(b)
            b->setEnabled(true);
    }
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

    Nepomuk::Query::Query query( orTerm );

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    foreach(const Nepomuk::Query::Result & r, queryResult) {
        r.resource().remove();
    }
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
    connect(removeProjectAction, SIGNAL(triggered(bool)),this, SLOT(deleteLibrary()));

    KAction* closeProjectAction = new KAction(this);
    closeProjectAction->setText(i18n("&Close Project"));
    closeProjectAction->setIcon(KIcon(QLatin1String("document-close")));
    closeProjectAction->setEnabled(false);
    actionCollection()->addAction(QLatin1String("close_project"), closeProjectAction);
    connect(closeProjectAction, SIGNAL(triggered(bool)),this, SLOT(closeLibrary()));

    // Database menu
    // import section
    KAction* importBibTexAction = new KAction(this);
    importBibTexAction->setText(i18n("BibTeX"));
    importBibTexAction->setIcon(KIcon(QLatin1String("kbibtex")));
    actionCollection()->addAction(QLatin1String("db_import_bibtex"), importBibTexAction);
    connect(importBibTexAction, SIGNAL(triggered(bool)),this, SLOT(importBibTex()));

    KAction* importZoteroAction = new KAction(this);
    importZoteroAction->setText(i18n("Zotero"));
    importZoteroAction->setIcon(KIcon(QLatin1String("storage-zotero")));
    actionCollection()->addAction(QLatin1String("db_import_zotero"), importZoteroAction);
    connect(importZoteroAction, SIGNAL(triggered(bool)),this, SLOT(importZotero()));

    // export section
    KAction* exportBibTexAction = new KAction(this);
    exportBibTexAction->setText(i18n("BibTeX"));
    exportBibTexAction->setIcon(KIcon(QLatin1String("kbibtex")));
    actionCollection()->addAction(QLatin1String("db_export_bibtex"), exportBibTexAction);
    connect(exportBibTexAction, SIGNAL(triggered(bool)),this, SLOT(exportBibTex()));

    KAction* exportZoteroAction = new KAction(this);
    exportZoteroAction->setText(i18n("Zotero"));
    exportZoteroAction->setIcon(KIcon(QLatin1String("storage-zotero")));
    actionCollection()->addAction(QLatin1String("db_export_zotero"), exportZoteroAction);
    connect(exportZoteroAction, SIGNAL(triggered(bool)),this, SLOT(exportZotero()));

    KAction* exportPdfAction = new KAction(this);
    exportPdfAction->setText(i18n("Pdf"));
    exportPdfAction->setIcon(KIcon(QLatin1String("application-pdf")));
    actionCollection()->addAction(QLatin1String("db_export_pdf"), exportPdfAction);
    connect(exportPdfAction, SIGNAL(triggered(bool)),this, SLOT(exportPdf()));

    KAction* exportFileAction = new KAction(this);
    exportFileAction->setText(i18n("other File"));
    exportFileAction->setIcon(KIcon(QLatin1String("application-rtf")));
    actionCollection()->addAction(QLatin1String("db_export_file"), exportFileAction);
    connect(exportFileAction, SIGNAL(triggered(bool)),this, SLOT(exportOtherFile()));

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
    actionCollection()->addAction(QLatin1String("toggle_websearch"), m_webSearchWidget->toggleViewAction());

    KStandardAction::quit(kapp, SLOT(quit()),actionCollection());

    setupGUI();
}

void MainWindow::setupMainWindow()
{
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

    m_mainView = new ResourceTableWidget();
    m_mainView->hide();
    m_mainView->setMainWindow(this);
    mainLayout->addWidget(m_mainView);

    // the left project bar
    m_libraryWidget = new LibraryWidget;
    addDockWidget(Qt::LeftDockWidgetArea, m_libraryWidget);

    //add panel for the document info
    m_sidebarWidget = new SidebarWidget;
    m_sidebarWidget->setMainWindow(this);
    addDockWidget(Qt::RightDockWidgetArea, m_sidebarWidget);

    //add panel for the document preview
    m_documentPreview = new DocumentPreview(this);
    m_centerWindow->addDockWidget(Qt::BottomDockWidgetArea, m_documentPreview);

    m_webSearchWidget = new WebSearchWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, m_webSearchWidget);

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

    //create the system library
    m_systemLibrary = new Library(Library_System);
    m_systemLibrary->setupModels();
    openLibrary(m_systemLibrary);

    switchView(Resource_Library, Max_BibTypes, m_systemLibrary);

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
