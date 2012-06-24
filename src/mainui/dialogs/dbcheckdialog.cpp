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
#include <Nepomuk2/Vocabulary/NCO>
#include <Nepomuk2/Vocabulary/NIE>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Soprano/Vocabulary/NAO>

#include <Nepomuk2/Query/Term>
#include <Nepomuk2/Query/ResourceTerm>
#include <Nepomuk2/Query/ResourceTypeTerm>
#include <Nepomuk2/Query/ComparisonTerm>
#include <Nepomuk2/Query/AndTerm>
#include <Nepomuk2/Query/OrTerm>
#include <Nepomuk2/Query/QueryServiceClient>
#include <Nepomuk2/Query/Result>
#include <Nepomuk2/Query/QueryParser>
#include <Nepomuk2/Tag>

#include <Nepomuk2/Variant>

#include <Nepomuk2/DataManagement>

#include <QDebug>

using namespace Nepomuk2::Vocabulary;

DbCheckDialog::DbCheckDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbCheckDialog)
{
    ui->setupUi(this);

    m_queryClient = new Nepomuk2::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk2::Query::Result>)), this, SLOT(addToList(QList<Nepomuk2::Query::Result>)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(queryFinished()));

    connect(ui->checkAll, SIGNAL(clicked()), this, SLOT(checkAll()));
    connect(ui->checkAuthor, SIGNAL(clicked()), this, SLOT(checkAuthor()));
    connect(ui->checkCollection, SIGNAL(clicked()), this, SLOT(checkCollection()));
    connect(ui->checkDocumentPart, SIGNAL(clicked()), this, SLOT(checkDocumentPart()));
    connect(ui->checkReference, SIGNAL(clicked()), this, SLOT(checkReference()));
    connect(ui->checkSeries, SIGNAL(clicked()), this, SLOT(checkSeries()));
    connect(ui->checkTags, SIGNAL(clicked()), this, SLOT(checkTags()));

    connect(ui->removeData, SIGNAL(clicked()), this, SLOT(removeData()));
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
    ui->checkTags->setEnabled(false);

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
    ui->checkTags->setEnabled(false);

    QString query = "select DISTINCT ?r where { "
                     "?r a nbib:Series ."
                     "OPTIONAL { ?r nbib:seriesOf ?seriesOf } ."
                     "FILTER (!bound(?seriesOf) )"
                     "}";

    Nepomuk2::Resource myTag = Nepomuk2::Resource(QString("nepomuk:/res/dd299c50-d0d8-4c09-9a3b-58d761bae1e3"));
    Nepomuk2::Query::ComparisonTerm term( Soprano::Vocabulary::NAO::isRelated(), Nepomuk2::Query::ResourceTerm(myTag) );

    Nepomuk2::Query::Query queryTEST(term);
    qDebug() << myTag.resourceUri().toString();

    Nepomuk2::Resource syncTest = Nepomuk2::Resource(QUrl());
    syncTest.addType(Nepomuk2::Vocabulary::SYNC::ServerSyncData());
    QUrl x = Nepomuk2::Vocabulary::SYNC::Attachment();
    syncTest.setProperty(Nepomuk2::Vocabulary::SYNC::syncDataType(), x);
    syncTest.setProperty(Nepomuk2::Vocabulary::SYNC::id(), "yay");

    Nepomuk2::Resource syncTest2 = Nepomuk2::Resource(QUrl());
    syncTest2.addType(Nepomuk2::Vocabulary::SYNC::ServerSyncData());
    QUrl y = Nepomuk2::Vocabulary::SYNC::Note();
    syncTest2.setProperty(Nepomuk2::Vocabulary::SYNC::syncDataType(), y);
    syncTest2.setProperty(Nepomuk2::Vocabulary::SYNC::id(), "yay1234");

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
    ui->checkTags->setEnabled(false);

    QString query = "select DISTINCT ?r where { "
    "?r a nco:Contact ."
    "}";

    m_queryClient->sparqlQuery( query );

    ui->infoLabel->setText(i18n("processing query"));
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
    ui->checkTags->setEnabled(false);

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
    ui->checkTags->setEnabled(false);

    QString query = "select DISTINCT ?r where { "
                     "?r a nbib:DocumentPart . "
                     "OPTIONAL { ?r nbib:documentPartOf ?documentPartOf } ."
                     "FILTER (!bound(?documentPartOf) )"
                     "}";

     m_queryClient->sparqlQuery( query );
     ui->infoLabel->setText(i18n("processing query"));
}

void DbCheckDialog::checkTags()
{
    m_toBeDeleted.clear();
    ui->listWidget->clear();
    ui->checkAll->setEnabled(false);
    ui->checkAuthor->setEnabled(false);
    ui->checkCollection->setEnabled(false);
    ui->checkDocumentPart->setEnabled(false);
    ui->checkReference->setEnabled(false);
    ui->checkSeries->setEnabled(false);
    ui->checkTags->setEnabled(false);

    QString query = "select DISTINCT ?r where { "
                     "?r a nao:Tag . "
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
    ui->checkTags->setEnabled(false);

    QString query = "select DISTINCT ?r where { "
                     "?r a ?v2 ."
                     "FILTER(?v2 in (nco:Contact,nbib:Reference,pimo:Note,nbib:Publication,nbib:Series,nbib:DocumentPart,pimo:Project,sync:ServerSyncData)) ."
                     "OPTIONAL { ?r aneo:akonadiItemId ?fromakonadi } ."
                     "FILTER (!bound(?fromakonadi) )"
                     "}";

     m_queryClient->sparqlQuery( query );
     ui->infoLabel->setText(i18n("processing query"));
}

void DbCheckDialog::addToList( const QList< Nepomuk2::Query::Result > &entries )
{
    foreach(const Nepomuk2::Query::Result &result, entries) {


//        QList<QUrl> resourceUris;
//        resourceUris << result.resource().uri();
//        QVariantList value;
//        value <<  QString("DMS TEST");
//        Nepomuk2::setProperty(resourceUris, NIE::title(), value);


        ui->listWidget->addItem(result.resource().genericLabel());
        m_toBeDeleted.append(result.resource());
    }
}

void DbCheckDialog::queryFinished()
{
    m_queryClient->close();

    ui->checkAll->setEnabled(true);
    ui->checkAuthor->setEnabled(true);
    ui->checkCollection->setEnabled(true);
    ui->checkDocumentPart->setEnabled(true);
    ui->checkReference->setEnabled(true);
    ui->checkSeries->setEnabled(true);
    ui->checkTags->setEnabled(true);

    ui->infoLabel->setText(i18n("Found %1 entries", m_toBeDeleted.size()));
}

void DbCheckDialog::removeData()
{
    foreach(Nepomuk2::Resource r, m_toBeDeleted) {
        r.remove();
    }

    ui->listWidget->clear();
}
