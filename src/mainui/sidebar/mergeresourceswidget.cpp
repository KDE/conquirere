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

#include "mergeresourceswidget.h"
#include "ui_mergeresourceswidget.h"

#include "core/library.h"
#include "mainui/librarymanager.h"
#include "core/projectsettings.h"

#include "nbibio/bibtex/bibtexvariant.h"
#include "nbibio/pipe/clipboardpipe.h"

#include "nbibio/pipe/kilelyxpipe.h"
#include "sync/bibtexexportdialog.h"

#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>

#include <Nepomuk2/DataManagement>
#include <KDE/KJob>

#include <KDE/KMessageBox>
#include <KDE/KInputDialog>
#include <KDE/KIcon>
#include <KDE/KDebug>

#include <QDBusInterface>
#include <QtGui/QMenu>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

MergeResourcesWidget::MergeResourcesWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::MergeResourcesWidget)
{
    ui->setupUi(this);

    // Resource management
    ui->buttonAddToProject->setIcon(KIcon("insert-link"));
    connect(ui->buttonAddToProject, SIGNAL(clicked()), this, SLOT(addToProject()));

    ui->buttonRemoveFromProject->setIcon(KIcon("remove-link"));
    connect(ui->buttonRemoveFromProject, SIGNAL(clicked()), this, SLOT(removeFromProject()));

    ui->buttonMerge->setIcon(KIcon("merge"));
    connect(ui->buttonMerge, SIGNAL(clicked()), this, SLOT(merge()));

    ui->buttonRemoveFromSystem->setIcon(KIcon("edit-delete-shred"));
    connect(ui->buttonRemoveFromSystem, SIGNAL(clicked()), this, SLOT(removeFromSystem()));

    // export management
    ui->buttonExportBibToClipboard->setIcon(KIcon("document-export"));
    connect(ui->buttonExportBibToClipboard, SIGNAL(clicked()), this, SLOT(bibtexToClipboard()));

    ui->buttonExportKeyToClipboard->setIcon(KIcon("document-export"));
    connect(ui->buttonExportKeyToClipboard, SIGNAL(clicked()), this, SLOT(citekeyToClipboard()));

    ui->buttonSendToLyX->setIcon(KIcon("document-export"));
    connect(ui->buttonSendToLyX, SIGNAL(clicked()), this, SLOT(sendToLyXKile()));

    ui->buttonExportToFile->setIcon(KIcon("document-export"));
    connect(ui->buttonExportToFile, SIGNAL(clicked()), this, SLOT(exportToFile()));

    // metadata management
    ui->buttonReindex->setIcon(KIcon("document-edit-verify"));
    connect(ui->buttonReindex, SIGNAL(clicked()), this, SLOT(reindexFiles()));

    ui->buttonFetchMetaData->setIcon(KIcon("download"));
    connect(ui->buttonFetchMetaData, SIGNAL(clicked()), this, SLOT(fetchMetaData()));
    ui->buttonFetchMetaData->setEnabled(false);

    m_nepomukDBus = new QDBusInterface( "org.kde.nepomuk.services.nepomukfileindexer", "/nepomukfileindexer" );
}

MergeResourcesWidget::~MergeResourcesWidget()
{
    delete ui;
}

void MergeResourcesWidget::setResources(QList<Nepomuk2::Resource> resourcelist)
{
    ui->mergeInfoLabel->setText(i18n("You have selected %1 different resources.", resourcelist.size()));
    m_resourceList = resourcelist;
}

Nepomuk2::Resource MergeResourcesWidget::resource()
{
    Nepomuk2::Resource invalid;
    return invalid;
}

void MergeResourcesWidget::setResource(Nepomuk2::Resource & resource)
{
    Q_UNUSED(resource);
    //not used
}

void MergeResourcesWidget::newButtonClicked()
{
    //not used
}

void MergeResourcesWidget::deleteButtonClicked()
{
    //not used
}

void MergeResourcesWidget::addToProject()
{
    QList<QAction*> actionCollection;
    QMenu addToProjects;

    if(libraryManager()->openProjects().isEmpty()) {
        addToProjects.addAction(i18n("No open projects"));
    }
    else {
        foreach(Library *l, libraryManager()->openProjects()) {
            QAction *a = new QAction(l->settings()->name(), this);
            a->setData(l->settings()->projectThing().uri());
            connect(a, SIGNAL(triggered(bool)),this, SLOT(addToSelectedProject()));
            addToProjects.addAction(a);
            actionCollection.append(a);
        }
    }

    addToProjects.exec(QCursor::pos());

    qDeleteAll(actionCollection);
}

void MergeResourcesWidget::addToSelectedProject()
{
    QAction *a = qobject_cast<QAction *>(sender());

    if(!a) { return; }

    QUrl pimoThing = a->data().toUrl();

    Q_ASSERT( pimoThing.isValid() );

    Library *selectedLib = libraryManager()->libFromResourceUri(pimoThing);

    foreach(const Nepomuk2::Resource &r, m_resourceList) {
        selectedLib->addResource(r);
    }
}

void MergeResourcesWidget::removeFromProject()
{
    QList<QAction*> actionCollection;
    QMenu addToProjects;

    // get the list of projects
    QSet<Nepomuk2::Resource> projectList;
    foreach(const Nepomuk2::Resource &r, m_resourceList) {
        QList<Nepomuk2::Resource> relatedList = r.property(NAO::isRelated()).toResourceList();

        foreach(const Nepomuk2::Resource &project, relatedList) {
            if(project.hasType(PIMO::Project())) {
                projectList << project;
            }
        }
    }

    if(projectList.isEmpty()) {
        addToProjects.addAction(i18n("not related to any project"));
    }
    else {
        foreach(const Nepomuk2::Resource &r, projectList) {
            QAction *a = new QAction(r.genericLabel(), this);
            a->setData(r.uri());
            connect(a, SIGNAL(triggered(bool)),this, SLOT(removeFromSelectedProject()));
            addToProjects.addAction(a);
            actionCollection.append(a);
        }
    }

    addToProjects.exec(QCursor::pos());

    qDeleteAll(actionCollection);
}

void MergeResourcesWidget::removeFromSelectedProject()
{
    QAction *a = qobject_cast<QAction *>(sender());

    if(!a) { return; }

    Nepomuk2::Resource pimoProject = Nepomuk2::Resource(a->data().toString());

    Library *l = libraryManager()->libFromResourceUri(pimoProject.uri());

    foreach(const Nepomuk2::Resource &r, m_resourceList) {
        l->removeResource(r);
    }
}

void MergeResourcesWidget::merge()
{
    QStringList itemList;
    foreach(const Nepomuk2::Resource &r, m_resourceList) {
        itemList << r.genericLabel();
    }

    QString selectedMainResource = KInputDialog::getItem(i18n("Merge Resources"), i18n("Select sthe reource where all others should be merged into"), itemList);

    if(selectedMainResource.isEmpty()) {
        return;
    }

    // get item from selectedname
    Nepomuk2::Resource mainResource;
    foreach(const Nepomuk2::Resource &r, m_resourceList) {
        if(r.genericLabel() == selectedMainResource)
        {
            mainResource = r;
        }
    }

    int i=0;
    foreach(const Nepomuk2::Resource &r, m_resourceList) {
        if(i==0) {
            mainResource = r;
            i++;
        }
        else {
            Nepomuk2::mergeResources(mainResource.uri(), r.uri());
        }
    }
}

void MergeResourcesWidget::removeFromSystem()
{
    QStringList itemList;
    foreach(const Nepomuk2::Resource &r, m_resourceList) {
        itemList << r.genericLabel();
    }

    int ret = KMessageBox::warningYesNoList(this,i18n("Are you sure you want to delete the %1 resources?", m_resourceList.size()), itemList);

    if(ret != KMessageBox::Yes) {
        return;
    }

    foreach(const Nepomuk2::Resource &r, m_resourceList) {
        libraryManager()->systemLibrary()->deleteResource(r);
    }
}

void MergeResourcesWidget::bibtexToClipboard()
{
    ClipboardPipe btcp;
    btcp.setCiteCommand(ConqSettings::referenceCommand());
    btcp.setExportType(ClipboardPipe::Export_SOURCE);
    btcp.pipeExport(m_resourceList);
}

void MergeResourcesWidget::citekeyToClipboard()
{
    ClipboardPipe btcp;
    btcp.setCiteCommand(ConqSettings::referenceCommand());
    btcp.setExportType(ClipboardPipe::Export_CITEKEY);
    btcp.pipeExport(m_resourceList);
}

void MergeResourcesWidget::sendToLyXKile()
{
    QStringList *error = new QStringList();
    KileLyxPipe klp;
    klp.setErrorLog(error);
    klp.pipeExport(m_resourceList);

    if(!error->isEmpty()) {
        KMessageBox::errorList(this,i18n("Could not send the references to the Kile/LyX pipe"), *error);
    }
}

void MergeResourcesWidget::exportToFile()
{
    BibTexExportDialog bed;
    bed.setInitialFileType(BibTexExporter::EXPORT_BIBTEX);
    bed.setResourceList(m_resourceList);

    bed.exec();
}

void MergeResourcesWidget::reindexFiles()
{
    foreach(const Nepomuk2::Resource &r, m_resourceList) {
        QString filePath = r.property(NIE::url()).toString().remove(QLatin1String("file://"));

        if(!filePath.isEmpty())
            m_nepomukDBus->call("org.kde.nepomuk.FileIndexer.indexFile", filePath);
    }
}

void MergeResourcesWidget::fetchMetaData()
{
    //TODOD implement metda data fetching from the web
}

