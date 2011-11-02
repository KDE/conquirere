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

#include "../core/library.h"

#include "../sidebar/sidebarwidget.h"
#include "librarywidget.h"
#include "newprojectdialog.h"
#include "welcomewidget.h"
#include "resourcetablewidget.h"
#include "documentpreview.h"
#include "websearchwidget.h"
#include "bibtexexportdialog.h"
#include "bibteximportwizard.h"

#include <KDE/KApplication>
#include <KDE/KAction>
#include <KDE/KLocale>
#include <KDE/KActionCollection>
#include <KDE/KStandardAction>
#include <KDE/KFileDialog>
#include <KDE/KMessageBox>
#include <KDE/KIO/NetAccess>
#include <KDE/KGlobalSettings>

#include <QtGui/QVBoxLayout>
#include <QtGui/QSplitter>

#include <QtCore/QDebug>


//DEBUG ADD to delete all nepomuk data
#include "nbib.h"
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

MainWindow::MainWindow(QWidget *parent)
    : KParts::MainWindow()
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
    NewProjectDialog npd;

    int ret = npd.exec();

    if(ret == QDialog::Accepted) {
        Library *customLibrary = new Library(Library_Project);
        customLibrary->setName(npd.name());
        customLibrary->setPath(npd.path());
        customLibrary->createLibrary();

        openLibrary(customLibrary);
    }
}

void MainWindow::loadLibrary()
{
    //select name and path of the project
    QString fileNameFromDialog = KFileDialog::getOpenFileName(KGlobalSettings::documentPath(), QLatin1String("*.ini|Conquirere project (*.ini)"));

    if(fileNameFromDialog.isEmpty()) {
        return;
    }

    Library *customLibrary = new Library(Library_Project);
    customLibrary->loadLibrary(fileNameFromDialog);

    openLibrary(customLibrary);
}

void MainWindow::openLibrary(Library *l)
{
    m_libraryWidget->addLibrary(l);

    //connect the fetch indicator to the treewidget
    l->connectFetchIndicator(m_libraryWidget);

    // create a welcome widget for the library
    WelcomeWidget *ww = new WelcomeWidget(l);
    ww->hide();
    m_centerWindow->centralWidget()->layout()->addWidget(ww);
    m_libraryList.insert(l, ww);

    if(m_libraryList.size() > 1) {
        actionCollection()->action(QLatin1String("delete_project"))->setEnabled(true);
        actionCollection()->action(QLatin1String("close_project"))->setEnabled(true);
    }

}

void MainWindow::deleteLibrary()
{
    qDebug() << "TODO delete library";
    //    ProjectWidget *projectWidget = qobject_cast<ProjectWidget *>(centralWidget());

    //    if(projectWidget) {
    //        Project *p = projectWidget->project();
    //        int ret = KMessageBox::warningYesNo(this,
    //                                            QLatin1String("Do you really want to remove the project tag :<br><b>") +
    //                                            p->pimoProject().genericLabel() +
    //                                            QLatin1String("</b><br><br> and delete the folder :<br><b>") +
    //                                            p->path(),
    //                                            QLatin1String("Delete project ") + p->name());

    //        if(ret == KMessageBox::Yes) {
    //            projectWidget->project()->deleteProject();
    //            closeProject();
    //        }
    //    }
}

void MainWindow::closeLibrary()
{
    qDebug() << "TODO close library";
//    m_libraryList.take()
//    Library *l = m_libraries.takeLast();

//    m_libraryWidget->closeLibrary(l);

//    delete l;

//    if(m_libraries.isEmpty()) {
//        actionCollection()->action(QLatin1String("delete_project"))->setEnabled(false);
//        actionCollection()->action(QLatin1String("close_project"))->setEnabled(false);
//    }
}

void MainWindow::exportBibTex()
{
    BibTexExportDialog bed;

    bed.exec();
}

void MainWindow::importBibTex()
{
    BibTeXImportWizard bid;

    bid.exec();
}

void MainWindow::connectKPartGui(KParts::Part * part)
{
    // this cause the screen to flicker as it rebuilds the GUI
    // better yet add all kparts (okular, gwenview, khtml) on program start
    // together with all gui elements
    //createGUI(part);
}

void MainWindow::switchView(ResourceSelection selection, ResourceFilter filter, Library *p)
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
}

void MainWindow::DEBUGDELETEALLDATA()
{
    // fetcha data
    Nepomuk::Query::OrTerm orTerm;

    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Reference() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Publication() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NCO::Contact() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Chapter() ) );
    orTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Series() ) );

    Nepomuk::Query::Query query( orTerm );

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    foreach(const Nepomuk::Query::Result & r, queryResult) {
        r.resource().remove();
    }
}

void MainWindow::setupActions()
{
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

    KAction* exportBibTexAction = new KAction(this);
    exportBibTexAction->setText(i18n("&Export to BibTex"));
    exportBibTexAction->setIcon(KIcon(QLatin1String("document-export")));

    actionCollection()->addAction(QLatin1String("export_bibtex"), exportBibTexAction);
    connect(exportBibTexAction, SIGNAL(triggered(bool)),this, SLOT(exportBibTex()));

    KAction* importBibTexAction = new KAction(this);
    importBibTexAction->setText(i18n("&Import from BibTex"));
    importBibTexAction->setIcon(KIcon(QLatin1String("document-import")));

    actionCollection()->addAction(QLatin1String("import_bibtex"), importBibTexAction);
    connect(importBibTexAction, SIGNAL(triggered(bool)),this, SLOT(importBibTex()));

    KAction* debugDELETE = new KAction(this);
    debugDELETE->setText(i18n("DEBUG DELETE ALL DATA!"));
    debugDELETE->setIcon(KIcon(QLatin1String("document-close")));

    actionCollection()->addAction(QLatin1String("debug_delete"), debugDELETE);
    connect(debugDELETE, SIGNAL(triggered(bool)),this, SLOT(DEBUGDELETEALLDATA()));

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

    m_mainView = new ResourceTableWidget;
    m_mainView->hide();
    mainLayout->addWidget(m_mainView);

    // the left project bar
    m_libraryWidget = new LibraryWidget;
    addDockWidget(Qt::LeftDockWidgetArea, m_libraryWidget);

    //add panel for the document info
    m_sidebarWidget = new SidebarWidget;
    addDockWidget(Qt::RightDockWidgetArea, m_sidebarWidget);

    //add panel for the document preview
    m_documentPreview = new DocumentPreview(this);
    m_centerWindow->addDockWidget(Qt::BottomDockWidgetArea, m_documentPreview);

    m_webSearchWidget = new WebSearchWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, m_webSearchWidget);

    connect(m_libraryWidget, SIGNAL(newSelection(ResourceSelection,ResourceFilter,Library*)),
            m_sidebarWidget, SLOT(newSelection(ResourceSelection,ResourceFilter,Library*)));

    connect(m_mainView, SIGNAL(selectedResource(Nepomuk::Resource&)),
            m_sidebarWidget, SLOT(setResource(Nepomuk::Resource&)));

    connect(m_mainView, SIGNAL(selectedMultipleResources(QList<Nepomuk::Resource>)),
            m_sidebarWidget, SLOT(setMultipleResources(QList<Nepomuk::Resource>)));

    connect(m_mainView, SIGNAL(selectedResource(Nepomuk::Resource&)),
            m_documentPreview, SLOT(setResource(Nepomuk::Resource&)));

    connect(m_documentPreview, SIGNAL(activateKPart(KParts::Part*)),
            this, SLOT(connectKPartGui(KParts::Part*)));

    connect(m_libraryWidget, SIGNAL(newSelection(ResourceSelection,ResourceFilter,Library*)),
            this, SLOT(switchView(ResourceSelection,ResourceFilter,Library*)));

    //create the system library
    Library *l = new Library(Library_System);
    openLibrary(l);

    switchView(Resource_Library, Filter_None, l);
}
