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

#include "serieswidget.h"
#include "ui_serieswidget.h"
#include "../core/library.h"

#include "propertywidgets/stringedit.h"
#include "publicationwidget.h"

#include "nbib.h"
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>

#include <KDE/KDialog>

#include <QtCore/QDebug>

SeriesWidget::SeriesWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::SeriesWidget)
{
    ui->setupUi(this);

    setupWidget();
}

SeriesWidget::~SeriesWidget()
{
    delete ui;
}

void SeriesWidget::setLibrary(Library *p)
{
    SidebarComponent::setLibrary(p);

    //TODO remove and use ResourceWatcher later on
    connect(ui->editIssn, SIGNAL(resourceUpdated(Nepomuk::Resource)), p, SIGNAL(resourceUpdated(Nepomuk::Resource)));
    connect(ui->editTitle, SIGNAL(resourceUpdated(Nepomuk::Resource)), p, SIGNAL(resourceUpdated(Nepomuk::Resource)));

    connect(this, SIGNAL(resourceUpdated(Nepomuk::Resource)), p, SIGNAL(resourceUpdated(Nepomuk::Resource)));
}

void SeriesWidget::setResource(Nepomuk::Resource & resource)
{
    m_series = resource;

    //check if the resource is a valid series
    if(!m_series.isValid()) {
        ui->editIssn->setEnabled(false);
        ui->editTitle->setEnabled(false);
        ui->editType->setEnabled(false);
        ui->issuesList->setEnabled(false);
        ui->addIssue->setEnabled(false);
        ui->editIssue->setEnabled(false);
        ui->removeIssue->setEnabled(false);
    }
    else {
        ui->editIssn->setEnabled(true);
        ui->editTitle->setEnabled(true);
        ui->editType->setEnabled(true);
        ui->issuesList->setEnabled(true);
        ui->addIssue->setEnabled(true);
        ui->editIssue->setEnabled(true);
        ui->removeIssue->setEnabled(true);
    }

    emit resourceChanged(m_series);

    SeriesType seriesType = SeriesTypeFromUrl(m_series);

    int index = ui->editType->findData(seriesType);
    ui->editType->setCurrentIndex(index);

    // fill list issues
    ui->issuesList->clear();
    QList<Nepomuk::Resource> tocResources = m_series.property(Nepomuk::Vocabulary::NBIB::seriesOf()).toResourceList();

    foreach(const Nepomuk::Resource & r, tocResources) {
        QListWidgetItem *i = new QListWidgetItem();
        QString title = r.property(Nepomuk::Vocabulary::NIE::title()).toString();
        QString number = r.property(Nepomuk::Vocabulary::NBIB::number()).toString();
        QString volume = r.property(Nepomuk::Vocabulary::NBIB::volume()).toString();
        QString showString = QString("Volume %1 / Issue %2 :: %3").arg(volume).arg(number).arg(title);
        i->setText(showString);
        i->setData(Qt::UserRole, r.resourceUri());
        ui->issuesList->addItem(i);
    }
}

void SeriesWidget::newSeriesTypeSelected(int index)
{
    qDebug() << "SeriesWidget::newSeriesTypeSelected";
}

void SeriesWidget::newButtonClicked()
{
    //create a new resource
    Nepomuk::Resource nb = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::NBIB::Publication());
    QList<QUrl> types;
    types.append(Nepomuk::Vocabulary::NBIB::Publication());
    nb.setTypes(types);

    setResource(nb);
}

void SeriesWidget::deleteButtonClicked()
{
    qDebug() << "remove all links fro mthe deleted series";

    m_series.remove();

    setResource(m_series);
}

void SeriesWidget::addIssue()
{
    KDialog addIssueWidget;

    QString seriesTitle = m_series.property(Nepomuk::Vocabulary::NIE::title()).toString();

    QUrl type;
    if(m_series.hasType(Nepomuk::Vocabulary::NBIB::Journal())) {
        type = Nepomuk::Vocabulary::NBIB::JournalIssue();
    }
    else if(m_series.hasType(Nepomuk::Vocabulary::NBIB::Newspaper())) {
        type = Nepomuk::Vocabulary::NBIB::NewspaperIssue();
    }
    else if(m_series.hasType(Nepomuk::Vocabulary::NBIB::Magazin())) {
        type = Nepomuk::Vocabulary::NBIB::MagazinIssue();
    }
    else if(m_series.hasType(Nepomuk::Vocabulary::NBIB::BookSeries())) {
        type = Nepomuk::Vocabulary::NBIB::Book();
    }
    else {
        type = Nepomuk::Vocabulary::NBIB::Collection();
    }

    Nepomuk::Resource tempIssue(QUrl(), type);
    tempIssue.setProperty(Nepomuk::Vocabulary::NIE::title(), seriesTitle);
    tempIssue.setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), m_series);
    m_series.setProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), tempIssue);

    PublicationWidget *pw = new PublicationWidget();
    pw->setResource(tempIssue);
    pw->setDialogMode(true);
    pw->setLibrary(library());

    addIssueWidget.setMainWidget(pw);
    addIssueWidget.setInitialSize(QSize(400,300));

    int ret = addIssueWidget.exec();

    if(ret == KDialog::Accepted) {
        QListWidgetItem *i = new QListWidgetItem();
        QString title = tempIssue.property(Nepomuk::Vocabulary::NIE::title()).toString();
        QString number = tempIssue.property(Nepomuk::Vocabulary::NBIB::number()).toString();
        QString volume = tempIssue.property(Nepomuk::Vocabulary::NBIB::volume()).toString();
        QString showString = QString("Volume %1 / Issue %2 :: %3").arg(volume).arg(number).arg(title);
        i->setText(showString);
        i->setData(Qt::UserRole, tempIssue.resourceUri());
        ui->issuesList->addItem(i);
        emit resourceUpdated(m_series);
    }
    else {
        // remove temp issue again
        m_series.removeProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), tempIssue);
        tempIssue.remove();
    }
}

void SeriesWidget::editIssue()
{
    QListWidgetItem *i = ui->issuesList->currentItem();

    Nepomuk::Resource selectedIssue(i->data(Qt::UserRole).toString());
    emit resourceUpdated(m_series);

    KDialog addIssueWidget;

    PublicationWidget *pw = new PublicationWidget();
    pw->setResource(selectedIssue);
    pw->setDialogMode(true);
    pw->setLibrary(library());

    addIssueWidget.setMainWidget(pw);
    addIssueWidget.setInitialSize(QSize(400,300));

    addIssueWidget.exec();

    QString title = selectedIssue.property(Nepomuk::Vocabulary::NIE::title()).toString();
    QString number = selectedIssue.property(Nepomuk::Vocabulary::NBIB::number()).toString();
    QString volume = selectedIssue.property(Nepomuk::Vocabulary::NBIB::volume()).toString();
    QString showString = QString("Volume %1 / Issue %2 :: %3").arg(volume).arg(number).arg(title);
    i->setText(showString);
}

void SeriesWidget::removeIssue()
{
    QListWidgetItem *i = ui->issuesList->currentItem();

    Nepomuk::Resource selectedIssue(i->data(Qt::UserRole).toString());
    m_series.removeProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), selectedIssue);

    selectedIssue.remove();

    ui->issuesList->removeItemWidget(i);
    delete i;
    emit resourceUpdated(m_series);
}

void SeriesWidget::changeRating(int newRating)
{
    m_series.setRating(newRating);

    emit resourceUpdated(m_series);
}

void SeriesWidget::setupWidget()
{
    int i=0;
    foreach(const QString &s, SeriesTypeTranslation) {
        ui->editType->addItem(s,(SeriesType)i);
        i++;
    }

    connect(ui->editType, SIGNAL(currentIndexChanged(int)), this, SLOT(newSeriesTypeSelected(int)));

    ui->editIssn->setPropertyUrl( Nepomuk::Vocabulary::NBIB::issn() );
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editIssn, SLOT(setResource(Nepomuk::Resource&)));
    ui->editTitle->setPropertyUrl( Nepomuk::Vocabulary::NIE::title() );
    connect(this, SIGNAL(resourceChanged(Nepomuk::Resource&)), ui->editTitle, SLOT(setResource(Nepomuk::Resource&)));

    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(changeRating(int)));
}
