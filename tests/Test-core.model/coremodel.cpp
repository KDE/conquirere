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

#include <QtGui/QSortFilterProxyModel>

#include <QtTest>
#include <QtDebug>

/**
 * @brief UnitTest for the Library Project and its Project settings
 *
 * checks: creation/change/removala and deletion of the library and some resoruces
 */
class CoreModel: public QObject
{
    Q_OBJECT

private slots:

    void benchmarkSystemModelTest();

private:
    bool waitForSignal(QObject *sender, const char *signal, int timeout = 1000);

};

QTEST_MAIN(CoreModel)

void CoreModel::benchmarkSystemModelTest() {

    //########################################################
    //# Load library based on pimoProject resource again

    Library *l = new Library();
    l->loadSystemLibrary();

    NepomukModel *documentModel = qobject_cast<NepomukModel *>( l->viewModels().value(Resource_Document)->sourceModel() );
    NepomukModel *referenceModel = qobject_cast<NepomukModel *>( l->viewModels().value(Resource_Reference)->sourceModel() );
    NepomukModel *publicationModel = qobject_cast<NepomukModel *>( l->viewModels().value(Resource_Publication)->sourceModel() );
    NepomukModel *seriesModel = qobject_cast<NepomukModel *>( l->viewModels().value(Resource_Series)->sourceModel() );
    NepomukModel *noteModel = qobject_cast<NepomukModel *>( l->viewModels().value(Resource_Note)->sourceModel() );
    NepomukModel *eventModel = qobject_cast<NepomukModel *>( l->viewModels().value(Resource_Event)->sourceModel() );

    QBENCHMARK {
        documentModel->startFetchData();
        QVERIFY(waitForSignal(documentModel, SIGNAL(queryFinished()), 10000));

        referenceModel->startFetchData();
        QVERIFY(waitForSignal(referenceModel, SIGNAL(queryFinished()), 10000));

        publicationModel->startFetchData();
        QVERIFY(waitForSignal(publicationModel, SIGNAL(queryFinished()), 10000));

        seriesModel->startFetchData();
        QVERIFY(waitForSignal(seriesModel, SIGNAL(queryFinished()), 10000));

        noteModel->startFetchData();
        QVERIFY(waitForSignal(noteModel, SIGNAL(queryFinished()), 10000));

        eventModel->startFetchData();
        QVERIFY(waitForSignal(eventModel, SIGNAL(queryFinished()), 10000));
    }

    qDebug() << "#################################################";
    qDebug() << "Number of documents :: " << documentModel->rowCount();
    qDebug() << "Number of references :: " << referenceModel->rowCount();
    qDebug() << "Number of publications :: " << publicationModel->rowCount();
    qDebug() << "Number of Series :: " << seriesModel->rowCount();
    qDebug() << "Number of Notes :: " << noteModel->rowCount();
    qDebug() << "Number of events :: " << eventModel->rowCount();
    qDebug() << "#################################################";

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

#include "coreproject.moc"
