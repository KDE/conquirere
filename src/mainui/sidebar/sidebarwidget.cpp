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

#include "sidebarwidget.h"
#include "ui_sidebarwidget.h"

#include "publicationwidget.h"
#include "sidebarcomponent.h"
#include "referencewidget.h"
#include "documentwidget.h"
#include "notewidget.h"
#include "serieswidget.h"
#include "eventwidget.h"
#include "libraryinfowidget.h"
#include "searchresultinfowidget.h"
#include "mailwidget.h"
#include "mergeresourceswidget.h"

#include "mainui/librarymanager.h"
#include "mainui/resourcetablewidget.h"

#include "core/library.h"
#include "core/projectsettings.h"

#include "nbibio/pipe/nepomuktobibtexpipe.h"

#include <kbibtex/findpdfui.h>

#include <Nepomuk/Resource>
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NMO>

#include <KDE/KGlobalSettings>
#include <KDE/KDebug>

#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedLayout>
#include <QtGui/QMenu>

SidebarWidget::SidebarWidget(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::DockWidget)
    , m_searchResultVisible(false)
    , m_currentWidget(0)
{
    ui->setupUi(this);

    setupUi();
}

SidebarWidget::~SidebarWidget()
{
    delete ui;
}

void SidebarWidget::setResource(Nepomuk::Resource & resource)
{
    Q_ASSERT(m_currentWidget != 0);

    // if we selected several entries beforehand and now just 1
    // go back to the original widget that displays the info
    if(m_stackedLayout->currentWidget() == m_mergeWidget) {
        m_stackedLayout->setCurrentWidget(m_currentWidget);
    }

    // check with widget mus tbe shown based on the resource type
    if(m_searchResultVisible) {
        findResourceSelection(resource);
    }

    m_curResource = resource;
    m_currentWidget->setResource(resource);

    ui->newButton->setEnabled(true);

    if(resource.isValid()) {
        ui->deleteButton->setEnabled(true);
        ui->linkAddButton->setEnabled(true);
        ui->linkRemoveButton->setEnabled(true);
        ui->findPdf->setEnabled(true);
    }
    else {
        ui->deleteButton->setEnabled(false);
        ui->linkAddButton->setEnabled(false);
        ui->linkRemoveButton->setEnabled(false);
    }
}

void SidebarWidget::setMultipleResources(QList<Nepomuk::Resource> resourcelist)
{
    m_stackedLayout->setCurrentWidget(m_mergeWidget);
    m_mergeWidget->setResources(resourcelist);
}

void SidebarWidget::newButtonClicked()
{
    Q_ASSERT(m_currentWidget != 0);

    m_currentWidget->newButtonClicked();
}

void SidebarWidget::deleteButtonClicked()
{
    Q_ASSERT(m_currentWidget != 0);

    m_currentWidget->deleteButtonClicked();
}

void SidebarWidget::addToProject()
{
    QList<QAction*> actionCollection;
    QMenu addToProjects;

    if(m_libraryManager->openProjects().isEmpty()) {
        addToProjects.addAction(i18n("No open projects"));
    }
    else {
        foreach(Library *l, m_libraryManager->openProjects()) {
            QAction *a = new QAction(l->settings()->name(), this);
            a->setData(l->settings()->projectThing().resourceUri());
            connect(a, SIGNAL(triggered(bool)),this, SLOT(addToSelectedProject()));
            addToProjects.addAction(a);
            actionCollection.append(a);
        }
    }

    addToProjects.exec(QCursor::pos());

    qDeleteAll(actionCollection);
}

void SidebarWidget::addToSelectedProject()
{
    QAction *a = qobject_cast<QAction *>(sender());

    if(!a)
        return;

    QUrl pimoThing = a->data().toUrl();

    Q_ASSERT( pimoThing.isValid() );

    Library *selectedLib = m_libraryManager->libFromResourceUri(pimoThing);

    if(m_curResource.isValid()) {
        selectedLib->addResource(m_curResource);
    }
}

void SidebarWidget::removeFromProject()
{
    QList<QAction*> actionCollection;
    QMenu addToProjects;

    QList<Nepomuk::Resource> relatedList = m_curResource.property(Soprano::Vocabulary::NAO::isRelated()).toResourceList();

    if(relatedList.isEmpty()) {
        addToProjects.addAction(i18n("not related to any project"));
    }
    else {
        foreach(const Nepomuk::Resource &r, relatedList) {
            QAction *a = new QAction(r.genericLabel(), this);
            a->setData(r.resourceUri());
            connect(a, SIGNAL(triggered(bool)),this, SLOT(removeFromSelectedProject()));
            addToProjects.addAction(a);
            actionCollection.append(a);
        }
    }

    addToProjects.exec(QCursor::pos());

    qDeleteAll(actionCollection);
}

void SidebarWidget::removeFromSelectedProject()
{
    QAction *a = qobject_cast<QAction *>(sender());

    if(!a)
        return;

    Nepomuk::Resource pimoProject = Nepomuk::Resource(a->data().toString());

    if(m_curResource.isValid()) {
        m_curResource.removeProperty(Soprano::Vocabulary::NAO::isRelated(), pimoProject);
        pimoProject.removeProperty(Soprano::Vocabulary::NAO::isRelated(), m_curResource);
    }
}

void SidebarWidget::findPdf()
{
    NepomukToBibTexPipe bibtexPipe;
    QList<Nepomuk::Resource> exportList;
    exportList.append(m_curResource);
    bibtexPipe.pipeExport(exportList);
    File bibFile = *bibtexPipe.bibtexFile();

    Entry *e = dynamic_cast<Entry *>(bibFile.first().data());

    // cache previous file/url values
    QString localFile = PlainTextValue::text(e->value(Entry::ftLocalFile));
    int nrLF = 0;
    if(localFile.isEmpty()) {
        nrLF = 0;
    }
    else {
        nrLF = 2;
    }

    QString urlFile = PlainTextValue::text(e->value(Entry::ftUrl));
    int nrUF = 0;
    if(urlFile.isEmpty()) {
        nrUF = 0;
    }
    else {
        nrUF = 2;
    }

    FindPDFUI::interactiveFindPDF(*e, bibFile, this);

    // find newly added localfileXX entries
    bool haveLocalfile = true;
    while(haveLocalfile) {
        QString newLocalFile;
        if(nrLF>0) {
            newLocalFile = PlainTextValue::text(e->value(QString("localfile%1").arg(nrLF)));
            nrLF++;
        }
        else {
            newLocalFile = PlainTextValue::text(e->value(QString("localfile")));
            nrLF = 2;
        }

        if(newLocalFile.isEmpty()) {
            haveLocalfile = false;
        }
        else {
            Nepomuk::Resource dataObject(QUrl(), Nepomuk::Vocabulary::NFO::FileDataObject());
            dataObject.setProperty(Nepomuk::Vocabulary::NIE::url(), newLocalFile);
            // connect new dataobject to resource
            m_curResource.addProperty( Nepomuk::Vocabulary::NBIB::isPublicationOf(), dataObject);
            //and the backreference
            dataObject.setProperty(Nepomuk::Vocabulary::NBIB::publishedAs(), m_curResource);
        }
    }

    // find newly added urlXX entries
    bool haveUrlfile = true;
    while(haveUrlfile) {
        QString newurlFile;
        if(nrUF>0) {
            newurlFile = PlainTextValue::text(e->value(QString("url%1").arg(nrUF)));
            nrUF++;
        }
        else {
            newurlFile = PlainTextValue::text(e->value(QString("url")));
            nrUF = 2;
        }

        if(newurlFile.isEmpty()) {
            haveUrlfile = false;
        }
        else {
            Nepomuk::Resource dataObject(QUrl(), Nepomuk::Vocabulary::NFO::RemoteDataObject());
            dataObject.setProperty(Nepomuk::Vocabulary::NIE::url(), newurlFile);
            // connect new dataobject to resource
            m_curResource.addProperty( Nepomuk::Vocabulary::NBIB::isPublicationOf(), dataObject);
            //and the backreference
            dataObject.setProperty(Nepomuk::Vocabulary::NBIB::publishedAs(), m_curResource);
        }
    }
}

void SidebarWidget::setLibraryManager(LibraryManager* lm)
{
    m_libraryManager = lm;

    m_libraryInfoWidget->setLibraryManager(m_libraryManager);
    m_searchResultInfoWidget->setLibraryManager(m_libraryManager);
    m_documentWidget->setLibraryManager(m_libraryManager);
    m_publicationWidget->setLibraryManager(m_libraryManager);
    m_referenceWidget->setLibraryManager(m_libraryManager);
    m_noteWidget->setLibraryManager(m_libraryManager);
    m_eventWidget->setLibraryManager(m_libraryManager);
    m_seriesWidget->setLibraryManager(m_libraryManager);
    m_mailWidget->setLibraryManager(m_libraryManager);

    connect(this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), m_libraryManager, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
}

void SidebarWidget::hasPublication(bool publication)
{
    ui->addPublication->setEnabled(!publication);
    ui->removePublication->setEnabled(publication);
}

void SidebarWidget::hasReference(bool reference)
{
    ui->removeReference->setEnabled(reference);
}

void SidebarWidget::newSelection(ResourceSelection selection, BibEntryType filter, Library *selectedLibrary)
{
    Q_UNUSED(filter);

    //TODO hacky solution disables searchview when sender is not the tablewidget
    ResourceTableWidget *rtw = qobject_cast<ResourceTableWidget *>(sender());
    if(!rtw) {
        m_searchResultVisible = false;
    }

    ui->addPublication->setVisible(false);
    ui->removePublication->setVisible(false);
    ui->addReference->setVisible(false);
    ui->removeReference->setVisible(false);
    ui->findPdf->setVisible(false);
    ui->lineFindPdf->setVisible(false);
    ui->newButton->setToolTip(QString());
    ui->deleteButton->setToolTip(QString());

    disconnect(ui->addPublication, SIGNAL(clicked()) );
    disconnect(ui->removePublication, SIGNAL(clicked()) );
    disconnect(ui->addReference, SIGNAL(clicked()) );
    disconnect(ui->removeReference, SIGNAL(clicked()) );

    switch(selection) {
    case Resource_Library:
    {
        m_stackedLayout->setCurrentWidget(m_libraryInfoWidget);
        m_libraryInfoWidget->setLibrary(selectedLibrary);
        m_currentWidget = m_libraryInfoWidget;
        ui->titleLabel->setText(i18nc("Header for the library details","Library"));
        break;
    }
    case Resource_Document:
    {
        m_stackedLayout->setCurrentWidget(m_documentWidget);
        m_currentWidget = m_documentWidget;
        ui->titleLabel->setText(i18nc("Header for the document details","Document"));
        ui->addPublication->setVisible(true);
        ui->removePublication->setVisible(true);
        ui->newButton->setToolTip(i18n("New document details"));
        ui->deleteButton->setToolTip(i18n("Delete document"));

        connect(ui->addPublication, SIGNAL(clicked()), m_documentWidget, SLOT(setPublication()));
        connect(ui->removePublication, SIGNAL(clicked()), m_documentWidget, SLOT(removePublication()));
        break;
    }
    case Resource_Mail:
    {
        m_stackedLayout->setCurrentWidget(m_mailWidget);
        m_currentWidget = m_mailWidget;
        ui->titleLabel->setText(i18nc("Header for the mail details","Mail"));
        break;
    }
    case Resource_Reference:
    {
        m_stackedLayout->setCurrentWidget(m_referenceWidget);
        m_currentWidget = m_referenceWidget;
        ui->titleLabel->setText(i18nc("Header for the reference details","Reference"));
        ui->newButton->setToolTip(i18n("New reference"));
        ui->deleteButton->setToolTip(i18n("Delete reference"));
        ui->findPdf->setVisible(true);
        ui->lineFindPdf->setVisible(true);
        break;
    }
    case Resource_Note:
    {
        m_stackedLayout->setCurrentWidget(m_noteWidget);
        m_currentWidget = m_noteWidget;
        ui->titleLabel->setText(i18nc("Header for the note details","Note"));
        ui->newButton->setToolTip(i18n("New note"));
        ui->deleteButton->setToolTip(i18n("Delete note"));
        break;
    }
    case Resource_Event:
    {
        m_stackedLayout->setCurrentWidget(m_eventWidget);
        m_currentWidget = m_eventWidget;
        ui->titleLabel->setText(i18nc("Header for the event details","Event"));
        ui->newButton->setToolTip(i18n("New event"));
        ui->deleteButton->setToolTip(i18n("Delete event"));
        break;
    }
    case Resource_Publication:
    {
        m_stackedLayout->setCurrentWidget(m_publicationWidget);
        m_currentWidget = m_publicationWidget;
        ui->titleLabel->setText(i18nc("Header for the publications details","Publication"));
        ui->addReference->setVisible(true);
        ui->removeReference->setVisible(true);
        ui->newButton->setToolTip(i18n("New publication"));
        ui->deleteButton->setToolTip(i18n("Delete publication"));
        ui->findPdf->setVisible(true);
        ui->lineFindPdf->setVisible(true);

        connect(ui->addReference, SIGNAL(clicked()), m_publicationWidget, SLOT(addReference()));
        connect(ui->removeReference, SIGNAL(clicked()), m_publicationWidget, SLOT(removeReference()));
        break;
    }
    case Resource_Series:
    {
        m_stackedLayout->setCurrentWidget(m_seriesWidget);
        m_currentWidget = m_seriesWidget;
        ui->titleLabel->setText(i18nc("Header for the series details","Series"));
        ui->newButton->setToolTip(i18n("New series"));
        ui->deleteButton->setToolTip(i18n("Delete series"));
        break;
    }
    case Resource_SearchResults:
    {
        m_stackedLayout->setCurrentWidget(m_searchResultInfoWidget);
        m_currentWidget = m_searchResultInfoWidget;
        ui->titleLabel->setText(i18nc("Header for the search result details","Search Results"));
        break;
    }
    case Resource_Website:
    case Resource_Media:
        kDebug() << "todo not implemented yet";
        break;
    case Max_ResourceTypes:
        qFatal("Max resourcetypes should never be used here");
    }
}

void SidebarWidget::showSearchResults()
{
    newSelection(Resource_SearchResults, Max_BibTypes, m_libraryManager->systemLibrary());
    m_searchResultVisible = true;
}

void SidebarWidget::findResourceSelection(Nepomuk::Resource & resource)
{
    ResourceSelection selection;
    BibEntryType filter = Max_BibTypes;

    if(!resource.isValid()) {
        selection = Resource_SearchResults;
    }
    else if(resource.hasType(Nepomuk::Vocabulary::NBIB::Publication())) {
       selection = Resource_Publication;
    }
    else if(resource.hasType(Nepomuk::Vocabulary::NBIB::Series())) {
        selection = Resource_Series;
    }
    else if(resource.hasType(Nepomuk::Vocabulary::NBIB::Reference())) {
        selection = Resource_Reference;
    }
    else if(resource.hasType(Nepomuk::Vocabulary::PIMO::Note())) {
        selection = Resource_Note;
    }
    else if(resource.hasType(Nepomuk::Vocabulary::NMO::Email())) {
        selection = Resource_Mail;
    }
    else if(resource.hasType(Nepomuk::Vocabulary::NFO::Website())) {
        selection = Resource_Website;
    }
    else if(resource.hasType(Nepomuk::Vocabulary::NFO::Bookmark())) {
        selection = Resource_Website;
    }
    else if(resource.hasType(Nepomuk::Vocabulary::PIMO::Event())) {
        selection = Resource_Event;
    }
    else {
        selection = Resource_Document;
    }

    newSelection(selection, filter, m_libraryManager->systemLibrary());
}

void SidebarWidget::clear()
{
    Nepomuk::Resource empty;
    setResource(empty);
}

void SidebarWidget::setupUi()
{
    setFont(KGlobalSettings::smallestReadableFont());

    // set button icons
    ui->newButton->setIcon(KIcon(QLatin1String("document-new")));
    ui->deleteButton->setIcon(KIcon(QLatin1String("document-close")));

    ui->newButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);

    ui->linkAddButton->setIcon(KIcon(QLatin1String("insert-link")));
    ui->linkRemoveButton->setIcon(KIcon(QLatin1String("remove-link")));
    ui->linkAddButton->setEnabled(false);
    ui->linkRemoveButton->setEnabled(false);

    ui->addPublication->setIcon(KIcon(QLatin1String("news-subscribe")));
    ui->removePublication->setIcon(KIcon(QLatin1String("news-unsubscribe")));
    ui->addPublication->setVisible(false);
    ui->removePublication->setVisible(false);
    ui->addPublication->setEnabled(false);
    ui->removePublication->setEnabled(false);

    ui->addReference->setIcon(KIcon(QLatin1String("format-indent-more")));
    ui->removeReference->setIcon(KIcon(QLatin1String("format-indent-less")));
    ui->addReference->setEnabled(true);
    ui->removeReference->setEnabled(false);
    ui->addReference->setVisible(false);
    ui->removeReference->setVisible(false);

    ui->findPdf->setIcon(KIcon(QLatin1String("application-pdf")));
    ui->findPdf->setEnabled(false);
    ui->findPdf->setVisible(false);
    ui->lineFindPdf->setVisible(false);

    // add stacked layout and all useable widget
    m_stackedLayout = new QStackedLayout;
    ui->contentWidget->setLayout(m_stackedLayout);

    m_mergeWidget = new MergeResourcesWidget(this);
    m_stackedLayout->addWidget(m_mergeWidget);

    m_libraryInfoWidget = new LibraryInfoWidget(this);
    m_stackedLayout->addWidget(m_libraryInfoWidget);

    m_searchResultInfoWidget = new SearchResultInfoWidget(this);
    m_stackedLayout->addWidget(m_searchResultInfoWidget);

    m_documentWidget = new DocumentWidget(this);
    m_stackedLayout->addWidget(m_documentWidget);
    connect(m_documentWidget, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    m_publicationWidget = new PublicationWidget(this);
    m_stackedLayout->addWidget(m_publicationWidget);
    connect(m_publicationWidget, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    m_referenceWidget = new ReferenceWidget(this);
    m_stackedLayout->addWidget(m_referenceWidget);
    connect(m_referenceWidget, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    m_noteWidget = new NoteWidget(this);
    m_stackedLayout->addWidget(m_noteWidget);
    connect(m_noteWidget, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    m_eventWidget = new EventWidget(this);
    m_stackedLayout->addWidget(m_eventWidget);
    connect(m_eventWidget, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    m_seriesWidget = new SeriesWidget(this);
    m_stackedLayout->addWidget(m_seriesWidget);
    connect(m_seriesWidget, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    m_mailWidget = new MailWidget(this);
    m_stackedLayout->addWidget(m_mailWidget);
    connect(m_mailWidget, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));

    m_currentWidget = m_libraryInfoWidget;
    m_stackedLayout->setCurrentWidget(m_libraryInfoWidget);
}
