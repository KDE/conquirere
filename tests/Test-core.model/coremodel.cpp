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

#include "core/library.h"
#include "core/models/nepomukmodel.h"

#include <Nepomuk2/Variant>
#include <Nepomuk2/Resource>
#include <Nepomuk2/DataManagement>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/StoreResourcesJob>
#include <KDE/KJob>

#include <Nepomuk2/Vocabulary/NIE>
#include "nbib.h"
#include "sro/nbib/book.h"
#include "sro/nbib/publication.h"
#include "sro/nbib/journal.h"
#include "sro/nco/contact.h"
#include "sro/nco/organizationcontact.h"
#include "sro/nbib/reference.h"

#include "core/queryclients/publicationquery.h"
#include "core/queryclients/seriesquery.h"

#include <QtGui/QSortFilterProxyModel>

#include <QtTest>
#include <QtDebug>
#include <KDE/KDebug>

#include <qtest_kde.h>

/**
 * @file coremodel.cpp
 *
 * @test UnitTest for the Library table models.
 *       checks: benchmark loading and ResourceWatcher adding/changing of entries
 */
class CoreModel: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void addPublicationTest();
    void changePublicationTest();
    void addSeriesTest();
    void changeSeriesTest();

    void cleanupTestCase();


private:
    bool waitForSignal(QObject *sender, const char *signal, int timeout = 1000);

    Library *l;
};

QTEST_KDEMAIN_CORE(CoreModel)

void CoreModel::initTestCase()
{
    l = new Library();
    l->loadSystemLibrary();
    l->modelStartFetching();

    // we wait here to ensure the models are loaded correctly
    QEventLoop loop;
    QTimer::singleShot( 500, &loop, SLOT(quit()) );
    loop.exec();
}

void CoreModel::addPublicationTest()
{
    kDebug() << "######################################################################################################";

    NepomukModel *publicationModel = qobject_cast<NepomukModel *>( l->viewModel(BibGlobals::Resource_Publication)->sourceModel() );
    QSignalSpy dataSizeChangedSignal(publicationModel, SIGNAL(dataSizeChaged(int)));

    // lets add a very simple book
    Nepomuk2::SimpleResourceGraph graph;

    Nepomuk2::NCO::Contact editor;
    editor.setFullname(QLatin1String("UNITTEST-Editor"));

    Nepomuk2::NCO::Contact author;
    author.setFullname(QLatin1String("UNITTEST-Author"));

    Nepomuk2::NCO::OrganizationContact publisher;
    publisher.setFullname(QLatin1String("UNITTEST-Publisher"));

    QDateTime publicationDate = QDateTime::fromString("1986-04-03T12:12:12Z", Qt::ISODate);

    Nepomuk2::NBIB::Book book;
    book.setTitle(QLatin1String("UNITTEST-book"));
    book.addCreator(author.uri());
    book.addEditor(editor.uri());
    book.addPublisher( publisher.uri() );
    book.setPublicationDate( publicationDate );

    Nepomuk2::NBIB::Reference bookReference;
    bookReference.setCiteKey(QLatin1String("UNITTEST-bookCiteKey"));
    bookReference.setPublication( book.uri() );
    book.addReference(bookReference.uri());

    graph << book << bookReference << editor << author << publisher;

    Nepomuk2::StoreResourcesJob* srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNone);

    if(!srj->exec()) {
        qDebug() << srj->errorString();
        QFAIL("Could not insert test publication into Nepomuk");
    }

    // we wait here so the last element in the model is really the newly added publication
    QEventLoop loop;
    QTimer::singleShot( 500, &loop, SLOT(quit()) );
    loop.exec();

    //check that the signal was emitted
    QCOMPARE(dataSizeChangedSignal.count(), 1);

    // now check if the data was correctly added to the model
    int lastEntry = publicationModel->rowCount()-1;
    QString addresourceType = publicationModel->data( publicationModel->index(lastEntry,PublicationQuery::Column_ResourceType), Qt::DisplayRole).toString();
    QString addtitle = publicationModel->data( publicationModel->index(lastEntry,PublicationQuery::Column_Title), Qt::DisplayRole).toString();
    QString addauthor = publicationModel->data( publicationModel->index(lastEntry,PublicationQuery::Column_Author), Qt::DisplayRole).toString();
    QString addpublisher = publicationModel->data( publicationModel->index(lastEntry,PublicationQuery::Column_Publisher), Qt::DisplayRole).toString();
    QString adddate = publicationModel->data( publicationModel->index(lastEntry,PublicationQuery::Column_Date), Qt::DisplayRole).toString();
    QString addcitekey = publicationModel->data( publicationModel->index(lastEntry,PublicationQuery::Column_CiteKey), Qt::DisplayRole).toString();
    QString addeditor = publicationModel->data( publicationModel->index(lastEntry,PublicationQuery::Column_Editor), Qt::DisplayRole).toString();

    QCOMPARE(addresourceType, QLatin1String("Book"));
    QCOMPARE(addtitle, QLatin1String("UNITTEST-book"));
    QCOMPARE(addauthor, QLatin1String("UNITTEST-Author"));
    QCOMPARE(addpublisher, QLatin1String("UNITTEST-Publisher"));
    QCOMPARE(adddate, QLatin1String("03.04.1986"));
    QCOMPARE(addcitekey, QLatin1String("UNITTEST-bookCiteKey"));
    QCOMPARE(addeditor, QLatin1String("UNITTEST-Editor"));
}

void CoreModel::changePublicationTest()
{
    kDebug() << "######################################################################################################";

    NepomukModel *publicationModel = qobject_cast<NepomukModel *>( l->viewModel(BibGlobals::Resource_Publication)->sourceModel() );
    qRegisterMetaType<QModelIndex>("QModelIndex");
    QSignalSpy dataChangedSignal(publicationModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

    QVERIFY2( publicationModel->rowCount() != 0, "No resources in the model that can be changed");

    // get the last added resource
    int lastEntry = publicationModel->rowCount()-1;
    Nepomuk2::Resource entry = publicationModel->documentResource(publicationModel->index(lastEntry,PublicationQuery::Column_ResourceType));

    QVERIFY2(entry.isValid() && entry.exists(), "No valid publication was retrieved from the model");

    KJob* job = Nepomuk2::setProperty(QList<QUrl>() << entry.uri(), Nepomuk2::Vocabulary::NIE::title(),QVariantList() << QLatin1String("UNITTEST-Changed-Name"));

    if(!job->exec()) {
        qDebug() << job->errorString();
        QFAIL("Could not change the name of the publication");
    }

    // @see BUG: https://bugs.kde.org/show_bug.cgi?id=306108
    // The reason this is required, is cause the Resource class is also updated via
    // dbus, and we have no way of controlling which slot would be called first.
    QEventLoop loop;
    QTimer::singleShot( 500, &loop, SLOT(quit()) );
    loop.exec();

    //check that the signal was emitted
    // as we do wait 500ms this signal was definitly emitted inbetween too
    QVERIFY(dataChangedSignal.count() >= 1);

    QString changedTitle = publicationModel->data( publicationModel->index(lastEntry,PublicationQuery::Column_Title), Qt::DisplayRole).toString();

    QCOMPARE(changedTitle, QLatin1String("UNITTEST-Changed-Name"));
}

void CoreModel::addSeriesTest()
{
    kDebug() << "######################################################################################################";

    NepomukModel *seriesModel = qobject_cast<NepomukModel *>( l->viewModel(BibGlobals::Resource_Series)->sourceModel() );
    QSignalSpy dataSizeChangedSignal(seriesModel, SIGNAL(dataSizeChaged(int)));

    // lets add a very simple book
    Nepomuk2::SimpleResourceGraph graph;

    Nepomuk2::NBIB::Journal journal;
    journal.setTitle(QLatin1String("UNITTEST-journal"));

    graph << journal;

    Nepomuk2::StoreResourcesJob* srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNone);

    if(!srj->exec()) {
        qDebug() << srj->errorString();
        QFAIL("Could not insert test journal into Nepomuk");
    }

    // we wait here so the last element in the model is really the newly added publication
    QEventLoop loop;
    QTimer::singleShot( 1000, &loop, SLOT(quit()) );
    loop.exec();

    //check that the signal was emitted
    QCOMPARE(dataSizeChangedSignal.count(), 1);

    // now check if the data was correctly added t othe model
    int lastEntry = seriesModel->rowCount()-1;
    QString addresourceType = seriesModel->data( seriesModel->index(lastEntry,SeriesQuery::Column_ResourceType), Qt::DisplayRole).toString();
    QString addtitle = seriesModel->data( seriesModel->index(lastEntry,SeriesQuery::Column_Title), Qt::DisplayRole).toString();

    QCOMPARE(addresourceType, QLatin1String("Journal"));
    QCOMPARE(addtitle, QLatin1String("UNITTEST-journal"));
}

void CoreModel::changeSeriesTest()
{
    kDebug() << "######################################################################################################";

    NepomukModel *seriesModel = qobject_cast<NepomukModel *>( l->viewModel(BibGlobals::Resource_Series)->sourceModel() );
    qRegisterMetaType<QModelIndex>("QModelIndex");
    QSignalSpy dataChangedSignal(seriesModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)));

    QVERIFY2( seriesModel->rowCount() != 0, "No resources in the model that can be changed");

    // get the last added resource
    int lastEntry = seriesModel->rowCount()-1;
    Nepomuk2::Resource entry = seriesModel->documentResource(seriesModel->index(lastEntry,SeriesQuery::Column_ResourceType));

    QVERIFY2(entry.isValid() && entry.exists(), "No valid series was retrieved from the model");

    KJob *job = Nepomuk2::setProperty(QList<QUrl>() << entry.uri(), Nepomuk2::Vocabulary::NIE::title(),QVariantList() << QLatin1String("UNITTEST-Changed-Name"));

    if(!job->exec()) {
        qDebug() << job->errorString();
        QFAIL("Could not change the name of the journal");
    }

    // @see BUG: https://bugs.kde.org/show_bug.cgi?id=306108
    // The reason this is required, is cause the Resource class is also updated via
    // dbus, and we have no way of controlling which slot would be called first.
    QEventLoop loop;
    QTimer::singleShot( 1000, &loop, SLOT(quit()) );
    loop.exec();

    //check that the signal was emitted
    // as we do wait 500ms this signal was definitly emitted inbetween too
    QVERIFY(dataChangedSignal.count() >= 1);

    // get the changed resource and check the new name
    QString changedTitle = seriesModel->data( seriesModel->index(lastEntry,SeriesQuery::Column_Title), Qt::DisplayRole).toString();
    QCOMPARE(changedTitle, QLatin1String("UNITTEST-Changed-Name"));
}

void CoreModel::cleanupTestCase()
{
    // remove all data created by this unittest from the nepomuk database again
    KJob *job = Nepomuk2::removeDataByApplication();
    if(!job->exec()) {
        qWarning() << job->errorString();
        QFAIL("Cleanup did not work");
    }

    delete l;
}

bool CoreModel::waitForSignal(QObject *sender, const char *signal, int timeout) {
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(timeout);
    timer.setSingleShot(true);

    loop.connect(sender, signal, SLOT(quit()));
    loop.connect(&timer, SIGNAL(timeout()), SLOT(quit()));
    timer.start();
    loop.exec();

    return timer.isActive();
}

#include "coremodel.moc"
