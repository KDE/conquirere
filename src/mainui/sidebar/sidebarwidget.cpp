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

#include "nbibio/pipe/nepomuktovariantpipe.h"
#include "nbibio/bibtex/bibtexvariant.h"

#include <kbibtex/findpdfui.h>
#include <nepomukmetadataextractor/fetcherdialog.h>

#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/NFO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/NMO>

#include <KDE/KGlobalSettings>
#include <KDE/KDebug>

#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedLayout>
#include <QtGui/QMenu>
#include <QtCore/QPointer>

using namespace Soprano::Vocabulary;
using namespace Nepomuk2::Vocabulary;
using namespace NepomukMetaDataExtractor::UI;

SidebarWidget::SidebarWidget(QWidget *parent)
    : QWidget(parent)
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

void SidebarWidget::setResource(Nepomuk2::Resource & resource)
{
    Q_ASSERT(m_currentWidget != 0);

    // if we selected several entries beforehand and now just 1
    // go back to the original widget that displays the info
    if(m_currentWidget == m_mergeWidget) {
        findResourceSelection(resource);
    }

    // check with widget must be shown based on the resource type
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

void SidebarWidget::setResource(SearchResultModel::SRCachedRowEntry webResult)
{
    if(m_currentWidget == m_searchResultInfoWidget) {
        m_searchResultInfoWidget->setResource(webResult);
    }
}

void SidebarWidget::setMultipleResources(QList<Nepomuk2::Resource> resourcelist)
{
    m_stackedLayout->setCurrentWidget(m_mergeWidget);
    m_mergeWidget->setResources(resourcelist);
    m_currentWidget = m_mergeWidget;

    ui->titleLabel->setText(i18nc("Header for the multiple resource selection widget","Resource Management"));

    ui->addPublication->setVisible(false);
    ui->removePublication->setVisible(false);
    ui->addReference->setVisible(false);
    ui->removeReference->setVisible(false);
    ui->findPdf->setVisible(false);
    ui->lineFindPdf->setVisible(false);
    ui->newButton->setVisible(false);
    ui->deleteButton->setVisible(false);
    ui->linkAddButton->setVisible(false);
    ui->linkRemoveButton->setVisible(false);

    ui->line_2->setVisible(false);
    ui->linePublication->setVisible(false);
    ui->lineFindPdf->setVisible(false);
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
            a->setData(l->settings()->projectThing().uri());
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

    if(!a) { return; }

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

    QList<Nepomuk2::Resource> relatedList = m_curResource.property(NAO::isRelated()).toResourceList();

    if(relatedList.isEmpty()) {
        addToProjects.addAction(i18n("not related to any project"));
    }
    else {
        foreach(const Nepomuk2::Resource &r, relatedList) {
            if(!r.hasType(PIMO::Project())) { continue; }

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

void SidebarWidget::removeFromSelectedProject()
{
    QAction *a = qobject_cast<QAction *>(sender());

    if(!a) { return; }

    Nepomuk2::Resource pimoProject = Nepomuk2::Resource(a->data().toString());

    if(m_curResource.isValid() && pimoProject.exists()) {
        Library *l = m_libraryManager->libFromResourceUri(pimoProject.uri());
        l->removeResource(m_curResource);
    }
}

void SidebarWidget::findPdf()
{
    NepomukToVariantPipe ntvp;
    ntvp.pipeExport( QList<Nepomuk2::Resource>() << m_curResource);
    QVariantList list = ntvp.variantList();

    File *bibFile = BibTexVariant::fromVariant(list);

    Entry *e = dynamic_cast<Entry *>(bibFile->first().data());

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

    FindPDFUI::interactiveFindPDF(*e, *bibFile, this);

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
            Nepomuk2::Resource dataObject(QUrl(), NFO::FileDataObject());
            dataObject.setProperty(NIE::url(), newLocalFile);
            // connect new dataobject to resource
            m_curResource.addProperty( NBIB::isPublicationOf(), dataObject);
            //and the backreference
            dataObject.setProperty(NBIB::publishedAs(), m_curResource);
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
            Nepomuk2::Resource dataObject(QUrl(), NFO::RemoteDataObject());
            dataObject.setProperty(NIE::url(), newurlFile);
            // connect new dataobject to resource
            m_curResource.addProperty( NBIB::isPublicationOf(), dataObject);
            //and the backreference
            dataObject.setProperty(NBIB::publishedAs(), m_curResource);
        }
    }

    delete bibFile;
}


void SidebarWidget::fetchMetaData()
{
    if(!m_curResource.exists())
        return;

    QPointer<FetcherDialog> fd = new FetcherDialog;

    fd->setForceUpdate(true);

    if(m_currentWidget == m_documentWidget) {
        KUrl url( m_curResource.property(NIE::url()).toString() );

        kDebug() << "find meta data for file" << url;

        fd->addFetcherPath( url );
        fd->show();
    }
    else {
        kDebug() << "muh...";
    }

    fd->exec();

    delete fd;
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
    m_mergeWidget->setLibraryManager(m_libraryManager);
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
    ui->fetchMetaData->setVisible(false);
    ui->newButton->setToolTip(QString());
    ui->deleteButton->setToolTip(QString());
    ui->newButton->setVisible(true);
    ui->deleteButton->setVisible(true);
    ui->linkAddButton->setVisible(true);
    ui->linkRemoveButton->setVisible(true);
    ui->line_2->setVisible(true);
    ui->linePublication->setVisible(true);
    ui->lineFindPdf->setVisible(false);

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
        ui->lineFindPdf->setVisible(true);
        ui->fetchMetaData->setVisible(true);

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
        ui->fetchMetaData->setVisible(true);

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

void SidebarWidget::findResourceSelection(Nepomuk2::Resource & resource)
{
    ResourceSelection selection;
    BibEntryType filter = Max_BibTypes;

    if(!resource.isValid()) {
        selection = Resource_SearchResults;
    }
    else if(resource.hasType(NBIB::Publication())) {
       selection = Resource_Publication;
    }
    else if(resource.hasType(NBIB::Series())) {
        selection = Resource_Series;
    }
    else if(resource.hasType(NBIB::Reference())) {
        selection = Resource_Reference;
    }
    else if(resource.hasType(PIMO::Note())) {
        selection = Resource_Note;
    }
    else if(resource.hasType(NMO::Email())) {
        selection = Resource_Mail;
    }
    else if(resource.hasType(NFO::Website())) {
        selection = Resource_Website;
    }
    else if(resource.hasType(NFO::Bookmark())) {
        selection = Resource_Website;
    }
    else if(resource.hasType(PIMO::Event())) {
        selection = Resource_Event;
    }
    else {
        selection = Resource_Document;
    }

    newSelection(selection, filter, m_libraryManager->systemLibrary());
}

void SidebarWidget::clear()
{
    Nepomuk2::Resource empty;
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
    ui->findPdf->setEnabled(true);
    ui->findPdf->setVisible(false);

    ui->fetchMetaData->setIcon(KIcon(QLatin1String("nepomuk")));
    ui->fetchMetaData->setEnabled(true);
    ui->fetchMetaData->setVisible(false);
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

    PublicationWidget * pw = new PublicationWidget(this);
    m_stackedLayout->addWidget(pw);
    connect(pw, SIGNAL(openDocument(Nepomuk2::Resource&,bool)), this, SIGNAL(openDocument(Nepomuk2::Resource&,bool)));
    m_publicationWidget = pw;

    m_referenceWidget = new ReferenceWidget(this);
    m_stackedLayout->addWidget(m_referenceWidget);

    m_noteWidget = new NoteWidget(this);
    m_stackedLayout->addWidget(m_noteWidget);

    m_eventWidget = new EventWidget(this);
    m_stackedLayout->addWidget(m_eventWidget);

    m_seriesWidget = new SeriesWidget(this);
    m_stackedLayout->addWidget(m_seriesWidget);

    m_mailWidget = new MailWidget(this);
    m_stackedLayout->addWidget(m_mailWidget);

    m_currentWidget = m_libraryInfoWidget;
    m_stackedLayout->setCurrentWidget(m_libraryInfoWidget);
}
