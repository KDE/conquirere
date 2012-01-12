
#include <QtTest>
#include <QtCore>
#include <QtGui>

class BibtexNepomukTest: public QObject
{
    Q_OBJECT
private slots:
    //void initTestCase();
    void myFirstTest();
    void mySecondTest();
    //void cleanupTestCase();
};

QTEST_MAIN(BibtexNepomukTest)

void BibtexNepomukTest::myFirstTest()
{
    QCOMPARE( 1, 1 );
}
void BibtexNepomukTest::mySecondTest()
{
    QCOMPARE( 1, 2 );
}

#include "bibtexnepomuktest.moc"
