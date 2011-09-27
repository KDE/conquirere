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
#include "core/project.h"
#include "core/bibtexexporter.h"

#include "sidebar/sidebarwidget.h"
#include "mainui/projecttreewidget.h"
#include "mainui/newprojectdialog.h"
#include "mainui/welcomewidget.h"
#include "mainui/projectwidget.h"

#include <KDE/KApplication>
#include <KDE/KAction>
#include <KDE/KLocale>
#include <KDE/KActionCollection>
#include <KDE/KStandardAction>
#include <KFileDialog>
#include <KMessageBox>
#include <KIO/NetAccess>
#include <kglobalsettings.h>

#include <QSplitter>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
{
    setupActions();
    setupMainWindow();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createProject()
{
    NewProjectDialog npd;

    int ret = npd.exec();

    if(ret == QDialog::Accepted) {
        Project *project = new Project();
        project->setName(npd.name());
        project->setPath(npd.path());
        project->createProject();

        openProject(project);
    }
}

void MainWindow::loadProject()
{
    //select name and path of the project
    QString fileNameFromDialog = KFileDialog::getOpenFileName(KGlobalSettings::documentPath(), QLatin1String("*.ini|Conquirere project (*.ini)"));

    if(fileNameFromDialog.isEmpty()) {
        return;
    }

    Project *project = new Project();
    project->loadProject(fileNameFromDialog);

    openProject(project);
}

void MainWindow::openProject(Project *p)
{
    m_projectTree->addProject(p);
    m_projects.append(p);

    //connect the fetch indicator to the treewidget

    actionCollection()->action(QLatin1String("delete_project"))->setEnabled(true);
    actionCollection()->action(QLatin1String("close_project"))->setEnabled(true);
    actionCollection()->action(QLatin1String("export_bibtex"))->setEnabled(true);
}

void MainWindow::deleteProject()
{
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

void MainWindow::closeProject()
{
    m_projectTree->closeProject(m_projects.first());

    if(m_projects.isEmpty()) {
        actionCollection()->action(QLatin1String("delete_project"))->setEnabled(false);
        actionCollection()->action(QLatin1String("close_project"))->setEnabled(false);
        actionCollection()->action(QLatin1String("export_bibtex"))->setEnabled(false);
    }
}

void MainWindow::exportBibTex()
{
    // get all documents in the project
//    ProjectWidget *projectWidget = qobject_cast<ProjectWidget *>(centralWidget());

//    if(projectWidget) {
//        Project *p = projectWidget->project();

//        BibTexExporter expBibTex;
//        expBibTex.setIsRelatedTo(p->pimoProject());

//        expBibTex.exportReferences(p->path() + QLatin1String("bibtex.bib"));
//    }
}

void MainWindow::setupActions()
{
    KAction* newProjectAction = new KAction(this);
    newProjectAction->setText(i18n("&Create Project"));
    newProjectAction->setIcon(KIcon(QLatin1String("document-new")));

    actionCollection()->addAction(QLatin1String("new_project"), newProjectAction);
    connect(newProjectAction, SIGNAL(triggered(bool)),this, SLOT(createProject()));

    KAction* loadProjectAction = new KAction(this);
    loadProjectAction->setText(i18n("&Load Project"));
    loadProjectAction->setIcon(KIcon(QLatin1String("document-open")));

    actionCollection()->addAction(QLatin1String("load_project"), loadProjectAction);
    connect(loadProjectAction, SIGNAL(triggered(bool)),this, SLOT(loadProject()));

    KAction* removeProjectAction = new KAction(this);
    removeProjectAction->setText(i18n("&Delete Project"));
    removeProjectAction->setIcon(KIcon(QLatin1String("document-close")));
    removeProjectAction->setEnabled(false);

    actionCollection()->addAction(QLatin1String("delete_project"), removeProjectAction);
    connect(removeProjectAction, SIGNAL(triggered(bool)),this, SLOT(deleteProject()));

    KAction* closeProjectAction = new KAction(this);
    closeProjectAction->setText(i18n("&Close Project"));
    closeProjectAction->setIcon(KIcon(QLatin1String("document-close")));
    closeProjectAction->setEnabled(false);

    actionCollection()->addAction(QLatin1String("close_project"), closeProjectAction);
    connect(closeProjectAction, SIGNAL(triggered(bool)),this, SLOT(closeProject()));

    KAction* exportBibTexAction = new KAction(this);
    exportBibTexAction->setText(i18n("&Export to BibTex"));
    exportBibTexAction->setIcon(KIcon(QLatin1String("document-export")));
    exportBibTexAction->setEnabled(false);

    actionCollection()->addAction(QLatin1String("export_bibtex"), exportBibTexAction);
    connect(exportBibTexAction, SIGNAL(triggered(bool)),this, SLOT(exportBibTex()));

    KStandardAction::quit(kapp, SLOT(quit()),actionCollection());

    setupGUI();
}

void MainWindow::setupMainWindow()
{
    QSplitter *splitter = new QSplitter(this);
    // the left project bar
    m_projectTree = new ProjectTreeWidget;

    m_mainView = new ProjectWidget;

    //add panel for the document info
    m_sidebarWidget = new SidebarWidget;

    connect(m_projectTree, SIGNAL(newSelection(LibraryType,ResourceSelection,Project*)),
            m_sidebarWidget, SLOT(newSelection(LibraryType,ResourceSelection,Project*)));

    connect(m_mainView, SIGNAL(selectedResource(Nepomuk::Resource&)),
            m_sidebarWidget, SLOT(setResource(Nepomuk::Resource&)));

    connect(m_projectTree, SIGNAL(newSelection(LibraryType,ResourceSelection,Project*)),
            m_mainView, SLOT(switchView(LibraryType,ResourceSelection,Project*)));

    splitter->addWidget(m_projectTree);
    splitter->addWidget(m_mainView);
    splitter->addWidget(m_sidebarWidget);

    QList<int> sizes;
    sizes << 150 << 500 << 200;
    splitter->setSizes(sizes);

    setCentralWidget(splitter);
}
