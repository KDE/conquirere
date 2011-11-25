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

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <KDE/KDialog>

#include <QtGui/QListWidgetItem>
#include <QtCore/QDebug>

ListPartsWidget::ListPartsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListPartsWidget)
{
    ui->setupUi(this);

    connect(ui->editPart, SIGNAL(clicked()), this, SLOT(editPart()));
    connect(ui->addPart, SIGNAL(clicked()), this, SLOT(addPart()));
    connect(ui->removePart, SIGNAL(clicked()), this, SLOT(removePart()));
}

ListPartsWidget::~ListPartsWidget()
{
    delete ui;
}

void ListPartsWidget::setResource(Nepomuk::Resource resource)
{
    m_resource = resource;

    ui->listWidget->clear();

    // When the Publication is a nbib:collection or one of its subclasses list all the articles in it
    // if the resource is a nbib:Series or its subtype list all publications in it
    // otherwise list all chapters

    // get the resource list
    QList<Nepomuk::Resource> resourceList;
    if(m_resource.hasType(Nepomuk::Vocabulary::NBIB::Collection())) {
        m_partType = Collection;
        resourceList = m_resource.property(Nepomuk::Vocabulary::NBIB::article()).toResourceList();
    }
    else if(m_resource.hasType(Nepomuk::Vocabulary::NBIB::Series())) {
        m_partType = Series;
        resourceList = m_resource.property(Nepomuk::Vocabulary::NBIB::seriesOf()).toResourceList();
    }
    else {
        m_partType = Chapter;
        resourceList = m_resource.property(Nepomuk::Vocabulary::NBIB::documentPart()).toResourceList();
    }

    // fill the list widget
    foreach(const Nepomuk::Resource & r, resourceList) {
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
        }
        i->setText(showString);
        i->setData(Qt::UserRole, r.resourceUri());
        ui->listWidget->addItem(i);
    }

    if(ui->listWidget->count() == 0) {
        ui->editPart->setEnabled(false);
        ui->removePart->setEnabled(false);
    }
    else {
        ui->editPart->setEnabled(true);
        ui->removePart->setEnabled(true);
    }
}

QString ListPartsWidget::showChapterString(Nepomuk::Resource publication)
{
    QString title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
    QString number = publication.property(Nepomuk::Vocabulary::NBIB::chapterNumber()).toString();
    QString showString = QString ("%1 : %2").arg(number).arg(title);

    return showString;
}

QString ListPartsWidget::showSeriesOfString(Nepomuk::Resource publication)
{
    QString title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
    QString number = publication.property(Nepomuk::Vocabulary::NBIB::number()).toString();
    QString volume = publication.property(Nepomuk::Vocabulary::NBIB::volume()).toString();
    QString showString = QString("Volume %1 / Issue %2 :: %3").arg(volume).arg(number).arg(title);

    return showString;
}

QString ListPartsWidget::showArticleString(Nepomuk::Resource publication)
{
    QString title = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();

    return title;
}

Nepomuk::Resource ListPartsWidget::selectedPart() const
{
    QListWidgetItem *i = ui->listWidget->currentItem();

    Nepomuk::Resource nepomukPart(i->data(Qt::UserRole).toString());

    return nepomukPart;
}

void ListPartsWidget::editPart()
{
    //get selected chapter
    QListWidgetItem *i = ui->listWidget->currentItem();
    if(!i)  { return; }

    Nepomuk::Resource tmpChapter(i->data(Qt::UserRole).toString());
    QString showNewString;

    switch(m_partType) {
    case Chapter:
        editChapter(tmpChapter);
        showNewString = showChapterString(tmpChapter);
        break;
    case Series:
        editSeries(tmpChapter);
        showNewString = showSeriesOfString(tmpChapter);
        break;
    case Collection:
        editCollection(tmpChapter);
        showNewString = showArticleString(tmpChapter);
        break;
    }

    i->setText(showNewString);
    emit resourceUpdated(m_resource);
}

void ListPartsWidget::addPart()
{
    switch(m_partType) {
    case Chapter:
        addChapter();
        break;
    case Series:
        addSeries();
        break;
    case Collection:
        addCollection();
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
}

void ListPartsWidget::removePart()
{
    QListWidgetItem *i = ui->listWidget->currentItem();
    if(!i) { return; }

    Nepomuk::Resource resource(i->data(Qt::UserRole).toUrl());
    ui->listWidget->removeItemWidget(i);
    delete i;

    switch(m_partType) {
    case Chapter:
        removeChapter(resource);
        break;
    case Series:
        removeSeries(resource);
        break;
    case Collection:
        removeCollection(resource);
        break;
    }
    ui->listWidget->setCurrentRow(0);

    emit resourceUpdated(m_resource);

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

void ListPartsWidget::editChapter(Nepomuk::Resource editResource)
{
    AddChapterDialog acd;
    acd.setResource(editResource);
    acd.exec();
}

void ListPartsWidget::addChapter()
{

    qDebug() << "##########################################################";
    qDebug() << "all existing chapters before the add";

    QList<Nepomuk::Resource> tocResources = m_resource.property(Nepomuk::Vocabulary::NBIB::documentPart()).toResourceList();

    foreach(const Nepomuk::Resource & r, tocResources) {
        QString title = r.property(Nepomuk::Vocabulary::NIE::title()).toString();
        QString number = r.property(Nepomuk::Vocabulary::NBIB::chapterNumber()).toString();
        QString listEntry = QString ("%1 : %2").arg(number).arg(title);
        qDebug() << listEntry << r.resourceUri();
    }
    qDebug() << "##########################################################";

    AddChapterDialog acd;
    acd.setPublication(m_resource);

    int ret = acd.exec();
    Nepomuk::Resource chapter = acd.resource();

    if(ret == QDialog::Accepted) {
        QListWidgetItem *i = new QListWidgetItem();
        QString showString = showChapterString(chapter);
        i->setText(showString);
        i->setData(Qt::UserRole, chapter.resourceUri());
        ui->listWidget->addItem(i);
        ui->listWidget->setCurrentItem(i);
        emit resourceUpdated(m_resource);
    }
    else {
        chapter.remove();
    }


    qDebug() << "##########################################################";
    qDebug() << "all existing chapters after the add";

    QList<Nepomuk::Resource> tocResources2 = m_resource.property(Nepomuk::Vocabulary::NBIB::documentPart()).toResourceList();

    foreach(const Nepomuk::Resource & r, tocResources2) {
        QString title = r.property(Nepomuk::Vocabulary::NIE::title()).toString();
        QString number = r.property(Nepomuk::Vocabulary::NBIB::chapterNumber()).toString();
        QString listEntry = QString ("%1 : %2").arg(number).arg(title);
        qDebug() << listEntry << r.resourceUri();
    }
    qDebug() << "##########################################################";
}

void ListPartsWidget::removeChapter(Nepomuk::Resource  chapter)
{
    m_resource.removeProperty(Nepomuk::Vocabulary::NBIB::documentPart(), chapter);
    chapter.remove();
}

void ListPartsWidget::addSeries()
{
    KDialog addIssueWidget;

    QString seriesTitle = m_resource.property(Nepomuk::Vocabulary::NIE::title()).toString();

    QUrl type;
    if(m_resource.hasType(Nepomuk::Vocabulary::NBIB::Journal())) {
        type = Nepomuk::Vocabulary::NBIB::JournalIssue();
    }
    else if(m_resource.hasType(Nepomuk::Vocabulary::NBIB::Newspaper())) {
        type = Nepomuk::Vocabulary::NBIB::NewspaperIssue();
    }
    else if(m_resource.hasType(Nepomuk::Vocabulary::NBIB::Magazin())) {
        type = Nepomuk::Vocabulary::NBIB::MagazinIssue();
    }
    else if(m_resource.hasType(Nepomuk::Vocabulary::NBIB::BookSeries())) {
        type = Nepomuk::Vocabulary::NBIB::Book();
    }
    else {
        type = Nepomuk::Vocabulary::NBIB::Collection();
    }

    Nepomuk::Resource tempIssue(QUrl(), type);
    tempIssue.setProperty(Nepomuk::Vocabulary::NIE::title(), seriesTitle);
    tempIssue.setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), m_resource);
    m_resource.addProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), tempIssue);

    PublicationWidget *pw = new PublicationWidget();
    pw->setResource(tempIssue);
    //pw->setLibrary(library());

    addIssueWidget.setMainWidget(pw);
    addIssueWidget.setInitialSize(QSize(400,300));

    int ret = addIssueWidget.exec();

    if(ret == KDialog::Accepted) {
        QListWidgetItem *i = new QListWidgetItem();
        QString showString = showSeriesOfString(tempIssue);
        i->setText(showString);
        i->setData(Qt::UserRole, tempIssue.resourceUri());
        ui->listWidget->addItem(i);
        emit resourceUpdated(m_resource);
    }
    else {
        // remove temp issue again
        m_resource.removeProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), tempIssue);
        tempIssue.remove();
    }
}

void ListPartsWidget::editSeries(Nepomuk::Resource editResource)
{
    KDialog addIssueWidget;

    PublicationWidget *pw = new PublicationWidget();
    pw->setResource(editResource);
    //pw->setLibrary(library());

    addIssueWidget.setMainWidget(pw);
    addIssueWidget.setInitialSize(QSize(400,300));

    addIssueWidget.exec();
}

void ListPartsWidget::removeSeries(Nepomuk::Resource  publication)
{
    m_resource.removeProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), publication);
    publication.remove();
}

void ListPartsWidget::addCollection()
{
    KDialog addIssueWidget;

    Nepomuk::Resource tempArticle(QUrl(), Nepomuk::Vocabulary::NBIB::Article());
    tempArticle.setProperty(Nepomuk::Vocabulary::NBIB::collection(), m_resource);
    m_resource.setProperty(Nepomuk::Vocabulary::NBIB::article(), tempArticle);

    PublicationWidget *pw = new PublicationWidget();
    pw->setResource(tempArticle);
    //pw->setLibrary(library());

    addIssueWidget.setMainWidget(pw);
    addIssueWidget.setInitialSize(QSize(400,300));

    int ret = addIssueWidget.exec();

    if(ret == KDialog::Accepted) {
        QListWidgetItem *i = new QListWidgetItem();
        QString showString = showArticleString(tempArticle);
        i->setText(showString);
        i->setData(Qt::UserRole, tempArticle.resourceUri());
        ui->listWidget->addItem(i);
        emit resourceUpdated(m_resource);
    }
    else {
        // remove temp issue again
        m_resource.removeProperty(Nepomuk::Vocabulary::NBIB::article(), tempArticle);
        tempArticle.remove();
    }
}

void ListPartsWidget::editCollection(Nepomuk::Resource editResource)
{
    KDialog addIssueWidget;

    PublicationWidget *pw = new PublicationWidget();
    pw->setResource(editResource);
    //pw->setLibrary(library());

    addIssueWidget.setMainWidget(pw);
    addIssueWidget.setInitialSize(QSize(400,300));

    addIssueWidget.exec();
}

void ListPartsWidget::removeCollection(Nepomuk::Resource  article)
{
    m_resource.removeProperty(Nepomuk::Vocabulary::NBIB::article(), article);
    article.remove();
}
