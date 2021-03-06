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

#include "listpartswidget.h"
#include "ui_listpartswidget.h"

#include "addchapterdialog.h"
#include "publicationwidget.h"
#include "listpublicationsdialog.h"
#include "core/librarymanager.h"
#include "core/library.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include "sro/nbib/publication.h"
#include "sro/nbib/article.h"
#include "sro/nbib/chapter.h"

#include "nbib.h"
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Vocabulary/NCAL>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Variant>

#include <KDE/KDialog>
#include <KDE/KIcon>
#include <KDE/KDebug>

#include <QtGui/QListWidgetItem>
#include <QtCore/QPointer>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

ListPartsWidget::ListPartsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListPartsWidget)
{
    ui->setupUi(this);

    ui->editPart->setIcon(KIcon("document-edit"));
    ui->addPart->setIcon(KIcon("list-add"));
    ui->removePart->setIcon(KIcon("list-remove"));
    ui->deletePart->setIcon(KIcon("edit-delete"));

    connect(ui->editPart, SIGNAL(clicked()), this, SLOT(editPart()));
    connect(ui->addPart, SIGNAL(clicked()), this, SLOT(addPart()));
    connect(ui->removePart, SIGNAL(clicked()), this, SLOT(removePart()));
    connect(ui->deletePart, SIGNAL(clicked()), this, SLOT(deletePart()));
}

ListPartsWidget::~ListPartsWidget()
{
    delete ui;
}

void ListPartsWidget::setLibraryManager(LibraryManager *lm)
{
    m_libraryManager = lm;
}

void ListPartsWidget::setResource(Nepomuk2::Resource resource)
{
    m_resource = resource;

    ui->listWidget->clear();

    // When the Publication is a nbib:collection or one of its subclasses list all the articles in it
    // if the resource is a nbib:Series or its subtype list all publications in it
    // otherwise list all chapters

    // get the resource list
    QList<Nepomuk2::Resource> resourceList;
    if(m_resource.hasType(NBIB::Collection())) {
        m_partType = Collection;
        ui->labelTitel->setText(i18n("List of articles:"));
        resourceList = m_resource.property(NBIB::article()).toResourceList();
        ui->removePart->setVisible(true);
    }
    else if(m_resource.hasType(NBIB::Series())) {
        m_partType = Series;
        ui->labelTitel->setText(i18n("List of Publications:"));
        resourceList = m_resource.property(NBIB::seriesOf()).toResourceList();
        ui->removePart->setVisible(true);
    }
    else if(m_resource.hasType(PIMO::Event()) ) {
        m_partType = Publication;
        ui->labelTitel->setText(i18n("List of Publications:"));
        resourceList = m_resource.property(NBIB::eventPublication()).toResourceList();
        ui->removePart->setVisible(true);
    }
    else {
        m_partType = Chapter;
        ui->labelTitel->setText(i18n("List of chapters:"));
        resourceList = m_resource.property(NBIB::documentPart()).toResourceList();
        ui->removePart->setVisible(false);
    }

    // fill the list widget
    foreach(const Nepomuk2::Resource & r, resourceList) {
        QListWidgetItem *i = new QListWidgetItem();

        QString showString;
        switch(m_partType) {
        case Chapter:
            showString = showChapterString(r);
            break;
        case Series:
            showString = showSeriesOfString(r);
            break;
        case Collection:
            showString = showArticleString(r);
            break;
        case Publication:
            showString = showArticleString(r);
            break;
        }

        if(m_partType != Chapter) {
            BibGlobals::BibEntryType bet = BibGlobals::BibEntryTypeFromUrl(r);
            i->setIcon( KIcon(BibGlobals::BibEntryTypeIcon(bet)) );
        }

        i->setText(showString);
        i->setData(Qt::UserRole, r.uri());
        ui->listWidget->addItem(i);
    }

    if(ui->listWidget->count() == 0) {
        ui->editPart->setEnabled(false);
        ui->removePart->setEnabled(false);
        ui->deletePart->setEnabled(false);
    }
    else {
        ui->editPart->setEnabled(true);
        ui->removePart->setEnabled(true);
        ui->deletePart->setEnabled(false);
    }
}

QString ListPartsWidget::showChapterString(Nepomuk2::Resource publication)
{
    // creates a string in the form
    // "1. Introduction" or just "Introduction"

    QString showString;
    QString title = publication.property(NIE::title()).toString();
    QString number = publication.property(NBIB::chapterNumber()).toString();

    showString.append(title);
    if(!number.isEmpty()) {
        showString.prepend(number + QLatin1String(". "));
    }

    return showString;
}

QString ListPartsWidget::showSeriesOfString(Nepomuk2::Resource publication)
{
    QString showString;
    QString title = publication.property(NIE::title()).toString();
    QString number = publication.property(NBIB::number()).toString();
    QString volume = publication.property(NBIB::volume()).toString();

    showString.append(title);
    if(!volume.isEmpty()) {
        showString.append( ' ' + i18n("Volume %1", volume));
    }
    if(!number.isEmpty()) {
        showString.append( ' ' + i18n("Issue %1", number));
    }

    return showString;
}

QString ListPartsWidget::showArticleString(Nepomuk2::Resource publication)
{
    QString title = publication.property(NIE::title()).toString();

    return title;
}

Nepomuk2::Resource ListPartsWidget::selectedPart() const
{
    QListWidgetItem *i = ui->listWidget->currentItem();

    Nepomuk2::Resource nepomukPart(i->data(Qt::UserRole).toString());

    return nepomukPart;
}

void ListPartsWidget::editPart()
{
    //get selected chapter
    QListWidgetItem *i = ui->listWidget->currentItem();
    if(!i)  { return; }

    Nepomuk2::Resource resourceToEdit = Nepomuk2::Resource::fromResourceUri(i->data(Qt::UserRole).toString());
    QString showNewString;

    switch(m_partType) {
    case Chapter:
        editChapter(resourceToEdit);
        showNewString = showChapterString(resourceToEdit);
        break;
    case Series:
        editFromSeries(resourceToEdit);
        showNewString = showSeriesOfString(resourceToEdit);
        break;
    case Collection:
        editFromCollection(resourceToEdit);
        showNewString = showArticleString(resourceToEdit);
        break;
    case Publication:
        editFromEvent(resourceToEdit);
        showNewString = showArticleString(resourceToEdit);
        break;
    }

    if(m_partType != Chapter) {
        BibGlobals::BibEntryType bet = BibGlobals::BibEntryTypeFromUrl(resourceToEdit);
        i->setIcon( KIcon(BibGlobals::BibEntryTypeIcon(bet)) );
    }

    i->setText(showNewString);
    emit resourceCacheNeedsUpdate(m_resource);
}

void ListPartsWidget::addPart()
{
    switch(m_partType) {
    case Chapter:
        addChapter();
        break;
    case Series:
        addToSeries();
        break;
    case Collection:
        addToCollection();
        break;
    case Publication:
        addToEvent();
        break;
    }

    if(ui->listWidget->count() == 0) {
        ui->editPart->setEnabled(false);
        ui->removePart->setEnabled(false);
    }
    else {
        ui->editPart->setEnabled(true);
        ui->removePart->setEnabled(true);
    }
    emit resourceCacheNeedsUpdate(m_resource);
}

void ListPartsWidget::removePart()
{
    QListWidgetItem *i = ui->listWidget->currentItem();
    if(!i) { return; }

    Nepomuk2::Resource resource(i->data(Qt::UserRole).toUrl());
    ui->listWidget->removeItemWidget(i);
    delete i;

    switch(m_partType) {
    case Chapter:
        removeChapter(resource);
        break;
    case Series:
        removeFromSeries(resource);
        break;
    case Collection:
        removeFromCollection(resource);
        break;
    case Publication:
        removeFromEvent(resource);
        break;
    }
    ui->listWidget->setCurrentRow(0);

    emit resourceCacheNeedsUpdate(m_resource);

    if(ui->listWidget->count() == 0) {
        ui->editPart->setEnabled(false);
        ui->removePart->setEnabled(false);
    }
    else {
        ui->editPart->setEnabled(true);
        ui->removePart->setEnabled(true);
        ui->listWidget->setCurrentRow(0);
    }
}

void ListPartsWidget::deletePart()
{
    QListWidgetItem *i = ui->listWidget->currentItem();
    if(!i) { return; }

    Nepomuk2::Resource resourceToBeDeleted(i->data(Qt::UserRole).toUrl());

    removePart();

    switch(m_partType) {
    case Publication:
        deleteFromEvent(resourceToBeDeleted);
        break;
    case Chapter:
        deleteChapter(resourceToBeDeleted);
        break;
    case Series:
        deleteFromSeries(resourceToBeDeleted);
        break;
    case Collection:
        deleteFromCollection(resourceToBeDeleted);
        break;
    }
}

void ListPartsWidget::editChapter(Nepomuk2::Resource editResource)
{
    QPointer<AddChapterDialog> acd = new AddChapterDialog(this);

    acd->setResource(editResource);
    acd->exec();

    delete acd;
}

void ListPartsWidget::addChapter()
{
    // create new temp Chapter via DMS
    // if the user clicks cancel in the next dialog, the resource will be deleted again
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Chapter tempChapter;

    tempChapter.setProperty(NIE::title(), i18n("New Chapter"));
    tempChapter.setProperty(NBIB::chapterNumber(), i18n("1"));

    graph << tempChapter;

    //blocking graph save
    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNone );
    if( !srj->exec() ) {
        kWarning() << "could not create temporay chapter" << srj->errorString();
        return;
    }

    Nepomuk2::Resource tempResource = Nepomuk2::Resource::fromResourceUri( srj->mappings().value( tempChapter.uri() ) );

    // because we could not create the links via the SimpleResource method, we add 2 additional calls to do them now
    QList<QUrl> resUri; resUri << m_resource.uri();
    QVariantList value; value << tempResource.uri();
    Nepomuk2::addProperty(resUri, NBIB::documentPart(), value);

    resUri.clear(); resUri << tempResource.uri();
    value.clear(); value << m_resource.uri();
    Nepomuk2::setProperty(resUri, NBIB::documentPartOf(), value);

    resUri.clear(); resUri << m_resource.uri();
    value.clear(); value << tempResource.uri();
    Nepomuk2::setProperty(resUri, NAO::hasSubResource(), value); // delete chapter when publication is deleted


    QPointer<AddChapterDialog> acd = new AddChapterDialog(this);

    acd->setResource(tempResource);

    int ret = acd->exec();

    if(ret == QDialog::Accepted) {
        QListWidgetItem *i = new QListWidgetItem();
        QString showString = showChapterString(tempResource);

        i->setText(showString);
        i->setData(Qt::UserRole, tempResource.uri());
        ui->listWidget->addItem(i);
        ui->listWidget->setCurrentItem(i);

        emit resourceCacheNeedsUpdate(m_resource);
    }
    else {
        m_libraryManager->systemLibrary()->deleteResource( tempResource );
    }

    delete acd;
}

void ListPartsWidget::removeChapter(Nepomuk2::Resource chapter)
{
    if(m_resource.hasType(NBIB::Publication())) {
        deleteChapter(chapter); // otherwise we get a loose chapter that does not belong anywhere, removing only works for references
        return;
    }

    QList<QUrl> resourceUris;
    resourceUris << m_resource.uri();
    QVariantList value;
    value << chapter.uri();

    Nepomuk2::setProperty(resourceUris, NBIB::documentPart(), value);
}

void ListPartsWidget::deleteChapter(Nepomuk2::Resource chapter)
{
    QList<QUrl> resourceUris;
    resourceUris << chapter.uri();

    Nepomuk2::removeResources(resourceUris);
}

void ListPartsWidget::addToSeries()
{
    // determine if we need to create a special collection type
    // if the series is aJ ournal, the publications added to it must be JournalIssue and so on
    QUrl type;
    if(m_resource.hasType(NBIB::Journal())) {
        type = NBIB::JournalIssue();
    }
    else if(m_resource.hasType(NBIB::Newspaper())) {
        type = NBIB::NewspaperIssue();
    }
    else if(m_resource.hasType(NBIB::Magazin())) {
        type = NBIB::MagazinIssue();
    }
    else if(m_resource.hasType(NBIB::BookSeries())) {
        type = NBIB::Book();
    }
    else {
        type = NBIB::Collection(); //TODO default not Collection but plainPublication?
    }

    // create temp Resource via DMS
    // if the user clicks cancel in the next dialog, the resource will be deleted again
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Publication tempPublication;

    tempPublication.addType( type );
    tempPublication.setProperty(NIE::title(), i18n("New Publication"));

    graph << tempPublication;

    //blocking graph save
    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNone );
    if( !srj->exec() ) {
        kWarning() << "could not create temporay publication" << srj->errorString();
        return;
    }

    Nepomuk2::Resource tempResource = Nepomuk2::Resource::fromResourceUri( srj->mappings().value( tempPublication.uri() ) );

    // because we could not create the links via the SimpleResource method, we add 2 additional calls to do them now
    QList<QUrl> resUri; resUri << m_resource.uri();
    QVariantList value; value << tempResource.uri();
    Nepomuk2::addProperty(resUri, NBIB::seriesOf(), value);

    resUri.clear(); resUri << tempResource.uri();
    value.clear(); value << m_resource.uri();
    KJob *job2 = Nepomuk2::addProperty(resUri, NBIB::inSeries(), value);
    job2->exec(); // wait until this is created, otherwise the parent Series won't show up in the PublicationWidget

    // open the publication Widget to allow the change of the resource
    QPointer<KDialog> addIssueWidget = new KDialog(this);
    PublicationWidget *pw = new PublicationWidget(addIssueWidget);
    pw->setResource(tempResource);
    pw->setLibraryManager(m_libraryManager);

    addIssueWidget->setMainWidget(pw);
    addIssueWidget->setInitialSize(QSize(400,300));

    int ret = addIssueWidget->exec();

    if(ret == KDialog::Accepted) {
        // the user wants to keep this resource
        QListWidgetItem *i = new QListWidgetItem();
        QString showString = showSeriesOfString(tempResource);

        if(m_partType != Chapter) {
            BibGlobals::BibEntryType bet = BibGlobals::BibEntryTypeFromUrl(tempResource);
            i->setIcon( KIcon(BibGlobals::BibEntryTypeIcon(bet)) );
        }

        i->setText(showString);
        i->setData(Qt::UserRole, tempResource.uri());
        ui->listWidget->addItem(i);
        emit resourceCacheNeedsUpdate(m_resource);
    }
    else {
        // the user cancel the creation, so we delete it again
        m_libraryManager->systemLibrary()->deleteResource( tempResource );
    }

    delete addIssueWidget;
}

void ListPartsWidget::editFromSeries(Nepomuk2::Resource editResource)
{
    QPointer<KDialog> addIssueWidget = new KDialog(this);

    PublicationWidget *pw = new PublicationWidget(addIssueWidget);
    pw->setResource(editResource);
    pw->setLibraryManager(m_libraryManager);

    addIssueWidget->setMainWidget(pw);
    addIssueWidget->setInitialSize(QSize(400,300));

    addIssueWidget->exec();

    delete addIssueWidget;
}

void ListPartsWidget::removeFromSeries(Nepomuk2::Resource  publication)
{
    QList<QUrl> resourceUris; resourceUris << m_resource.uri();
    QVariantList value; value << publication.uri();
    Nepomuk2::removeProperty(resourceUris, NBIB::seriesOf(), value);

    resourceUris.clear(); resourceUris << publication.uri();
    value.clear(); value << m_resource.uri();
    Nepomuk2::removeProperty(resourceUris, NBIB::inSeries(), value);
}

void ListPartsWidget::deleteFromSeries(Nepomuk2::Resource  publication)
{
    m_libraryManager->systemLibrary()->deleteResource( publication );
}

void ListPartsWidget::addToCollection()
{
    // create temp Resource via DMS
    // if teh user clikcs cancel in the next dialog, the resource will be deleted again
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Article tempArticle;

    tempArticle.setProperty(NIE::title(), i18n("New Article"));

    graph << tempArticle;

    //blocking graph save
    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNone );
    if( !srj->exec() ) {
        kWarning() << "could not create temporay article" << srj->errorString();
        return;
    }

    Nepomuk2::Resource tempResource = Nepomuk2::Resource::fromResourceUri( srj->mappings().value( tempArticle.uri() ) );

    // because we could not create the links via the SimpleResource method, we add 2 additional calls to do them now
    QList<QUrl> resUri; resUri << m_resource.uri();
    QVariantList value; value << tempResource.uri();
    Nepomuk2::addProperty(resUri, NBIB::article(), value);

    resUri.clear(); resUri << tempResource.uri();
    value.clear(); value << m_resource.uri();
    KJob *job2 = Nepomuk2::addProperty(resUri, NBIB::collection(), value);
    job2->exec(); // wait until this is created, otherwise the parent Series won't show up in the PublicationWidget

    QPointer<KDialog> addIssueWidget = new KDialog(this);
    PublicationWidget *pw = new PublicationWidget(addIssueWidget);
    pw->setResource(tempResource);
    pw->setLibraryManager(m_libraryManager);

    addIssueWidget->setMainWidget(pw);
    addIssueWidget->setInitialSize(QSize(400,300));

    int ret = addIssueWidget->exec();

    if(ret == KDialog::Accepted) {
        // the user wants to keep this resource
        QListWidgetItem *i = new QListWidgetItem();
        QString showString = showArticleString(tempResource);

        if(m_partType != Chapter) {
            BibGlobals::BibEntryType bet = BibGlobals::BibEntryTypeFromUrl(tempResource);
            i->setIcon( KIcon(BibGlobals::BibEntryTypeIcon(bet)) );
        }

        i->setText(showString);
        i->setData(Qt::UserRole, tempResource.uri());
        ui->listWidget->addItem(i);
        emit resourceCacheNeedsUpdate(m_resource);
    }
    else {
        // the user cancel the creation, so we delete it again
        m_libraryManager->systemLibrary()->deleteResource( tempResource );
    }

    delete addIssueWidget;
}

void ListPartsWidget::editFromCollection(Nepomuk2::Resource editResource)
{
    QPointer<KDialog> addIssueWidget = new KDialog(this);

    PublicationWidget *pw = new PublicationWidget(addIssueWidget);
    pw->setResource(editResource);
    pw->setLibraryManager(m_libraryManager);

    addIssueWidget->setMainWidget(pw);
    addIssueWidget->setInitialSize(QSize(400,300));

    addIssueWidget->exec();

    delete addIssueWidget;
}

void ListPartsWidget::removeFromCollection(Nepomuk2::Resource  article)
{
    QList<QUrl> resourceUris; resourceUris << m_resource.uri();
    QVariantList value; value << article.uri();

    Nepomuk2::setProperty(resourceUris, NBIB::article(), value);

    resourceUris.clear(); resourceUris << article.uri();
    value.clear(); value << m_resource.uri();

    Nepomuk2::setProperty(resourceUris, NBIB::collection(), value);
}

void ListPartsWidget::deleteFromCollection(Nepomuk2::Resource  article)
{
    QList<QUrl> resourceUris;
    resourceUris << article.uri();

    Nepomuk2::removeResources(resourceUris);
}

void ListPartsWidget::addToEvent()
{
    QPointer<ListPublicationsDialog> lpd = new ListPublicationsDialog(this);
    lpd->setLibraryManager(m_libraryManager);

    int ret = lpd->exec();

    if(ret == KDialog::Accepted) {
        Nepomuk2::Resource publication = lpd->selectedPublication();

        // do the crosslinking via DMS
        QList<QUrl> resUri; resUri << m_resource.uri();
        QVariantList value; value << publication.uri();
        Nepomuk2::addProperty(resUri, NBIB::eventPublication(), value);

        resUri.clear(); resUri << publication.uri();
        value.clear(); value << m_resource.uri();
        Nepomuk2::addProperty(resUri, NBIB::event(), value);

        QListWidgetItem *i = new QListWidgetItem();

        if(m_partType != Chapter) {
            BibGlobals::BibEntryType bet = BibGlobals::BibEntryTypeFromUrl(publication);
            i->setIcon( KIcon(BibGlobals::BibEntryTypeIcon(bet)) );
        }

        i->setText(publication.property(NIE::title()).toString());
        i->setData(Qt::UserRole, publication.uri());
        ui->listWidget->addItem(i);

        emit resourceCacheNeedsUpdate(m_resource);
    }

    delete lpd;
}

void ListPartsWidget::editFromEvent(Nepomuk2::Resource editResource)
{
    QPointer<KDialog> addPublicationWidget = new KDialog(this);

    PublicationWidget *pw = new PublicationWidget(addPublicationWidget);
    pw->setResource(editResource);
    pw->setLibraryManager(m_libraryManager);

    addPublicationWidget->setMainWidget(pw);
    addPublicationWidget->setInitialSize(QSize(400,300));

    addPublicationWidget->exec();

    delete addPublicationWidget;
}

void ListPartsWidget::removeFromEvent(Nepomuk2::Resource  publication)
{
    QList<QUrl> resourceUris; resourceUris << m_resource.uri();
    QVariantList value; value << publication.uri();
    Nepomuk2::setProperty(resourceUris, NBIB::eventPublication(), value);

    resourceUris.clear(); resourceUris << publication.uri();
    value.clear(); value << m_resource.uri();
    Nepomuk2::setProperty(resourceUris, NBIB::event(), value);
}

void ListPartsWidget::deleteFromEvent(Nepomuk2::Resource  publication)
{
    QList<QUrl> resourceUris;
    resourceUris << publication.uri();

    Nepomuk2::removeResources(resourceUris);
}
