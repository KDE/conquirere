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
#include "core/projectsettings.h"

#include <Nepomuk2/Variant>
#include <Nepomuk2/Resource>
#include <Nepomuk2/DataManagement>
#include <Nepomuk2/SimpleResource>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/StoreResourcesJob>
#include <KDE/KJob>

#include <Nepomuk2/ResourceManager>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include <Nepomuk2/Vocabulary/NIE>
#include "ontology/nbib.h"
#include "sro/nbib/series.h"
#include "sro/nbib/article.h"
#include "sro/nbib/proceedings.h"
#include "sro/nbib/reference.h"

#include <QtTest>
#include <QtDebug>

#include <kdebug.h>
#include <qtest_kde.h>

/**
 * @file coreproject.cpp
 *
 * @test UnitTest for the Library, Project and its Project settings
 *       checks: creation/change/removal and deletion of the library and some resoruces
 */
class CoreProject: public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();

    void createProjectTest();
    void loadProjectTest();
    void addResourceProjectTest();
    void removeResourceProjectTest();
    void changeProjectTest();
    void deleteProjectTest();

    void cleanupTestCase();

private:
    Nepomuk2::Resource pimoProject;
    QString testName;
    QString testDescription;
    Library *l;
    ProjectSettings *ps;

    QUrl articleUri;
    QUrl proceedingsUri;
    QUrl seriesUri;
    QUrl referenceUri;
};

QTEST_KDEMAIN_CORE(CoreProject)

void CoreProject::initTestCase()
{
    l= 0;
    ps = 0;

    testName = QString::fromUtf8("UNITTEST-Project");
    testDescription = QString::fromUtf8("UNITTEST Description üäö+.-!§$%");

    //insert some test publication
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Series series;
    series.setTitle(QLatin1String("UNITTEST-Series-Title"));

    Nepomuk2::NBIB::Proceedings proceedings;
    proceedings.setTitle(QLatin1String("UNITTEST-Proceedings-Title"));
    proceedings.addInSeries(series.uri());
    series.addSeriesOf(proceedings.uri());

    Nepomuk2::NBIB::Article article;
    article.setTitle(QLatin1String("UNITTEST-Article-Title"));
    article.setCollection(proceedings.uri());

    Nepomuk2::NBIB::Reference reference;
    reference.setCiteKey(QLatin1String("UNITTEST-Citekey"));
    reference.setPublication(article.uri());
    article.addReference(reference.uri());

    graph << article << reference << proceedings << series;

    Nepomuk2::StoreResourcesJob* srj = Nepomuk2::storeResources(graph,Nepomuk2::IdentifyNone);

    if(!srj->exec()) {
        qDebug() << srj->errorString();
        QFAIL("Could not insert test publication into Nepomuk");
    }

    // save the real nepomuk uris for later use
    articleUri = srj->mappings().value(article.uri());
    proceedingsUri = srj->mappings().value(proceedings.uri());
    seriesUri = srj->mappings().value(series.uri());
    referenceUri = srj->mappings().value(reference.uri());
}

void CoreProject::createProjectTest() {

    //########################################################
    //# Create a new library

    pimoProject = Library::createLibrary(testName, testDescription, QString(""));

    QVERIFY2( pimoProject.isValid(), "pimoProject for the library is NOT Valid" );
    QVERIFY2( pimoProject.exists(), "pimoProject for the library does NOT exist" );

    if( !pimoProject.exists() )
        QFAIL("Without working project all other tests will fail too.");
}

void CoreProject::loadProjectTest() {

    //########################################################
    //# Load library based on pimoProject resource again

    l = new Library();
    l->loadLibrary( pimoProject );

    ps = l->settings();

    QCOMPARE(ps->name(), testName);
    QCOMPARE(ps->description(), testDescription);

    QVERIFY2( ps->projectThing().isValid(), "pimoProject for the loaded library is NOT Valid" );
    QVERIFY2( ps->projectTag().isValid(), "tag for the loaded library is NOT Valid" );
    QVERIFY2( ps->projectTag().exists(), "tag for the loaded library does NOT exist" );
}

void CoreProject::addResourceProjectTest()
{
    Nepomuk2::Resource articleCheck(articleUri);
    l->addResource(articleCheck);

    // wait untill the asynchronous addProperty finished
    QTest::qWait(5000);

    // now check if the resource and all its connected components are really added to the
    Nepomuk2::Resource proceedingsCheck(proceedingsUri);
    Nepomuk2::Resource seriesCheck(seriesUri);
    Nepomuk2::Resource referenceCheck(referenceUri);

    bool articleAdded = articleCheck.isRelateds().contains(pimoProject);
    QVERIFY2( articleAdded, "Article not added to the project");

    /*
     //TODO: instead of adding collection/Sereis to the project, the model should list them automatically when the article is in the project
    // will not be added automatically
    bool proceedingsAdded = proceedingsCheck.isRelateds().contains(pimoProject);
    QVERIFY2( proceedingsAdded, "Proceedings not added to the project");

    bool seriesAdded = seriesCheck.isRelateds().contains(pimoProject);
    QVERIFY2( seriesAdded, "Series not added to the project");
    */

    //QEXPECT_FAIL("", "res.types not loaded correctly, should be fixed in 4.9.1", Continue);
    bool referenceAdded = referenceCheck.isRelateds().contains(pimoProject);
    QVERIFY2( referenceAdded, "Reference not added to the project");
}

void CoreProject::removeResourceProjectTest()
{
    Nepomuk2::Resource articleCheck(articleUri);
    l->removeResource(articleCheck);

    // wait until the asynchronous removeProperty finished
    QTest::qWait(5000);

    // now check if the resource and all its connected components are really added to the
    Nepomuk2::Resource referenceCheck(referenceUri);

    bool articleAdded = articleCheck.isRelateds().contains(pimoProject);
    QVERIFY2( !articleAdded, "Project relation was not removed from Article");

    bool referenceAdded = referenceCheck.isRelateds().contains(pimoProject);
    QVERIFY2( !referenceAdded, "Project relation was not removed from Reference");
}

void CoreProject::changeProjectTest()
{
    // change the name of the project
    QString  newName = QLatin1String("UNITTEST Changed Name");
    ps->setName(newName);

    // check if old tag was deleted
    QString query = QString::fromLatin1("select distinct ?r ?label where {"
                                        "?r a nao:Tag ."
                                        "?r nao:prefLabel ?label ."
                                        "FILTER regex(?label, '" + testName.toLatin1() + "') ."
                                        "FILTER NOT EXISTS { ?r a pimo:Topic . }"
                                        "}");
    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    int oldLabelCount = 0;
    while( it.next() ) { oldLabelCount++;}

    QVERIFY2( oldLabelCount == 0, "Old project tag was not deleted");

    // check if new tag was created
    QString query2 = QString::fromLatin1("select distinct ?r ?label where {"
                                        "?r a nao:Tag ."
                                        "?r nao:prefLabel ?label ."
                                        "FILTER regex(?label, '^" + newName.toLatin1() + "') ."
                                        "FILTER NOT EXISTS { ?r a pimo:Topic . }"
                                        "}");
    Soprano::QueryResultIterator it2 = model->executeQuery( query2, Soprano::Query::QueryLanguageSparql );

    int newLabelCount = 0;
    while( it2.next() ) { newLabelCount++;}

    QVERIFY2( newLabelCount == 1, "New project tag was not created correctly");
}

void CoreProject::deleteProjectTest()
{
    l->deleteLibrary();

    // wait until the asynchronous removeResource finished
    QTest::qWait(5000);

    QVERIFY2( !pimoProject.exists(), "Project was not deleted correctly");
}

void CoreProject::cleanupTestCase()
{
    // remove all data created by this unittest from the nepomuk database again
    KJob *job = Nepomuk2::removeDataByApplication();
    if(!job->exec()) {
        qWarning() << job->errorString();
        QFAIL("Cleanup did not work");
    }

    delete l;
}


#include "coreproject.moc"
