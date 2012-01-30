/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "dbcheckdialog.h"
#include "ui_dbcheckdialog.h"

#include "nbib.h"
#include "sync.h"
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>
#include <Nepomuk/Tag>

#include <Nepomuk/Variant>

#include <QDebug>

using namespace Nepomuk::Vocabulary;

DbCheckDialog::DbCheckDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbCheckDialog)
{
    ui->setupUi(this);

    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(addToList(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(queryFinished()));

    connect(ui->checkAll, SIGNAL(clicked()), this, SLOT(checkAll()));
    connect(ui->checkAuthor, SIGNAL(clicked()), this, SLOT(checkAuthor()));
    connect(ui->checkCollection, SIGNAL(clicked()), this, SLOT(checkCollection()));
    connect(ui->checkDocumentPart, SIGNAL(clicked()), this, SLOT(checkDocumentPart()));
    connect(ui->checkReference, SIGNAL(clicked()), this, SLOT(checkReference()));
    connect(ui->checkSeries, SIGNAL(clicked()), this, SLOT(checkSeries()));

    connect(ui->removeData, SIGNAL(clicked()), this, SLOT(removeData()));

    ui->checkAuthor->setEnabled(false);
}

DbCheckDialog::~DbCheckDialog()
{
    delete ui;
}

void DbCheckDialog::checkCollection()
{
    m_toBeDeleted.clear();
    ui->listWidget->clear();
    ui->checkAll->setEnabled(false);
    ui->checkAuthor->setEnabled(false);
    ui->checkCollection->setEnabled(false);
    ui->checkDocumentPart->setEnabled(false);
    ui->checkReference->setEnabled(false);
    ui->checkSeries->setEnabled(false);

    QString query = "select DISTINCT ?r where { "
    "?r a sync:ServerSyncData ."
    "}";

    m_queryClient->sparqlQuery( query );

    ui->infoLabel->setText(i18n("processing query"));
}

void DbCheckDialog::checkSeries()
{
    m_toBeDeleted.clear();
    ui->listWidget->clear();
    ui->checkAll->setEnabled(false);
    ui->checkAuthor->setEnabled(false);
    ui->checkCollection->setEnabled(false);
    ui->checkDocumentPart->setEnabled(false);
    ui->checkReference->setEnabled(false);
    ui->checkSeries->setEnabled(false);

    QString query = "select DISTINCT ?r where { "
                     "?r a nbib:Series ."
                     "OPTIONAL { ?r nbib:seriesOf ?seriesOf } ."
                     "FILTER (!bound(?seriesOf) )"
                     "}";

    Nepomuk::Resource myTag = Nepomuk::Resource(QString("nepomuk:/res/dd299c50-d0d8-4c09-9a3b-58d761bae1e3"));
    Nepomuk::Query::ComparisonTerm term( Soprano::Vocabulary::NAO::isRelated(), Nepomuk::Query::ResourceTerm(myTag) );

    Nepomuk::Query::Query queryTEST(term);
    qDebug() << myTag.resourceUri().toString();

    Nepomuk::Resource syncTest = Nepomuk::Resource(QUrl());
    syncTest.addType(Nepomuk::Vocabulary::SYNC::ServerSyncData());
    QUrl x = Nepomuk::Vocabulary::SYNC::Attachment();
    syncTest.setProperty(Nepomuk::Vocabulary::SYNC::syncDataType(), x);
    syncTest.setProperty(Nepomuk::Vocabulary::SYNC::id(), "yay");

    Nepomuk::Resource syncTest2 = Nepomuk::Resource(QUrl());
    syncTest2.addType(Nepomuk::Vocabulary::SYNC::ServerSyncData());
    QUrl y = Nepomuk::Vocabulary::SYNC::Note();
    syncTest2.setProperty(Nepomuk::Vocabulary::SYNC::syncDataType(), y);
    syncTest2.setProperty(Nepomuk::Vocabulary::SYNC::id(), "yay1234");

     m_queryClient->sparqlQuery( query );
     ui->infoLabel->setText(i18n("processing query"));
}

void DbCheckDialog::checkAuthor()
{
    m_toBeDeleted.clear();
    ui->listWidget->clear();
    ui->checkAll->setEnabled(false);
    ui->checkAuthor->setEnabled(false);
    ui->checkCollection->setEnabled(false);
    ui->checkDocumentPart->setEnabled(false);
    ui->checkReference->setEnabled(false);
    ui->checkSeries->setEnabled(false);
}

void DbCheckDialog::checkReference()
{
    m_toBeDeleted.clear();
    ui->listWidget->clear();
    ui->checkAll->setEnabled(false);
    ui->checkAuthor->setEnabled(false);
    ui->checkCollection->setEnabled(false);
    ui->checkDocumentPart->setEnabled(false);
    ui->checkReference->setEnabled(false);
    ui->checkSeries->setEnabled(false);

    QString query = "select DISTINCT ?r where { "
                     "?r a nbib:Reference ."
                     "OPTIONAL { ?r nbib:publication ?publication } ."
                     "FILTER (!bound(?publication) )"
                     "}";

     m_queryClient->sparqlQuery( query );
     ui->infoLabel->setText(i18n("processing query"));
}

void DbCheckDialog::checkDocumentPart()
{
    m_toBeDeleted.clear();
    ui->listWidget->clear();
    ui->checkAll->setEnabled(false);
    ui->checkAuthor->setEnabled(false);
    ui->checkCollection->setEnabled(false);
    ui->checkDocumentPart->setEnabled(false);
    ui->checkReference->setEnabled(false);
    ui->checkSeries->setEnabled(false);

    QString query = "select DISTINCT ?r where { "
                     "?r a nbib:DocumentPart . "
                     "OPTIONAL { ?r nbib:documentPartOf ?documentPartOf } ."
                     "FILTER (!bound(?documentPartOf) )"
                     "}";

     m_queryClient->sparqlQuery( query );
     ui->infoLabel->setText(i18n("processing query"));
}

void DbCheckDialog::checkAll()
{
    m_toBeDeleted.clear();
    ui->listWidget->clear();
    ui->checkAll->setEnabled(false);
    ui->checkAuthor->setEnabled(false);
    ui->checkCollection->setEnabled(false);
    ui->checkDocumentPart->setEnabled(false);
    ui->checkReference->setEnabled(false);
    ui->checkSeries->setEnabled(false);

    QString query = "select DISTINCT ?r where { "
                     "?r a ?v2 ."
                     "FILTER(?v2 in (nco:Contact,nbib:Reference,nbib:Publication,nbib:Series,nbib:DocumentPart,pimo:Event,pimo:Project,sync:ServerSyncData)) ."
                     "OPTIONAL { ?r aneo:akonadiItemId ?fromakonadi } ."
                     "FILTER (!bound(?fromakonadi) )"
                     "}";

     m_queryClient->sparqlQuery( query );
     ui->infoLabel->setText(i18n("processing query"));
}

void DbCheckDialog::addToList( const QList< Nepomuk::Query::Result > &entries )
{
    foreach(const Nepomuk::Query::Result &result, entries) {
        ui->listWidget->addItem(result.resource().genericLabel());
        m_toBeDeleted.append(result.resource());
    }
}

void DbCheckDialog::queryFinished()
{
    m_queryClient->close();

    ui->checkAll->setEnabled(true);
//    ui->checkAuthor->setEnabled(true);
    ui->checkCollection->setEnabled(true);
    ui->checkDocumentPart->setEnabled(true);
    ui->checkReference->setEnabled(true);
    ui->checkSeries->setEnabled(true);

    ui->infoLabel->setText(i18n("Found %1 entries", m_toBeDeleted.size()));
}

void DbCheckDialog::removeData()
{
    foreach(Nepomuk::Resource r, m_toBeDeleted) {
        r.remove();
    }

    ui->listWidget->clear();
}
