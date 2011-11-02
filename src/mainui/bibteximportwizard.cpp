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

#include "bibteximportwizard.h"
#include "ui_bibteximportwizard.h"

#include "../nbibio/nbibimporterbibtex.h"

#include <kbibtex/fileimporterbibtex.h>
#include <kbibtex/file.h>

#include <KDE/KUrlRequester>
#include <KDE/KComboBox>

//#include <Akonadi/Item>
//#include <KABC/Addressee>
//#include <Akonadi/ItemFetchJob>
//#include <Akonadi/ItemCreateJob>
//#include <Akonadi/CollectionCreateJob>
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QCheckBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtCore/QThread>
#include <QtCore/QtConcurrentRun>

#include <QDebug>

BibTeXImportWizard::BibTeXImportWizard(QWidget *parent)
    : QWizard(parent)
    , ui(new Ui::BibTeXImportWizard)
{
    addPage(new IntroPage);
    addPage(new ParseFile);
    addPage(new NepomukImport);

    ui->setupUi(this);
}

BibTeXImportWizard::~BibTeXImportWizard()
{
    delete ui;
}

/*
 * 1. Wizard page
 *
 * Selection of the file and options
 */
IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{

    // fetching all collections containing emails recursively, starting at the root collection
    Akonadi::CollectionFetchJob *job = new Akonadi::CollectionFetchJob( Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this );
    job->fetchScope().setContentMimeTypes( QStringList() << "application/x-vnd.kde.contactgroup" );
    connect( job, SIGNAL( collectionsReceived( const Akonadi::Collection::List& ) ),
             this, SLOT( collectionsReceived( const Akonadi::Collection::List& ) ) );

    setTitle(i18n("BibTeX Importer"));
    setSubTitle(i18n("This wizard will guide you through the import process."));
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark1.png"));

    //file selection
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *fileLayout = new QHBoxLayout();
    fileLayout->setSpacing(20);
    QLabel *fileNameLabel = new QLabel(i18n("File"));
    fileName = new KUrlRequester();
    registerField("fileName", fileNameLabel);
    connect(fileName, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));

    fileLayout->addWidget(fileNameLabel);
    fileLayout->addWidget(fileName);
    mainLayout->addLayout(fileLayout);

    // options
    QGroupBox *groupBox = new QGroupBox(i18n("Collection"));
    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBox->setLayout(groupBoxLayout);

    QCheckBox *fdBox = new QCheckBox(i18n("find duplicates"));
    registerField("duplicates", fdBox);
    fdBox->setChecked(true);
    groupBoxLayout->addWidget(fdBox);
    QCheckBox *cb2 = new QCheckBox(i18n("Add contacts to Akonadi"));
    registerField("akonadiContact", cb2);
    groupBoxLayout->addWidget(cb2);

    QHBoxLayout *comboLayout = new QHBoxLayout();
    addressComboBox = new KComboBox();
    addressComboBox->setEnabled(false);
    addressComboBox->setMaximumSize(200,50);
    comboLayout->addSpacing(20);
    comboLayout->addWidget(addressComboBox);
    groupBoxLayout->addLayout(comboLayout);
    connect(cb2, SIGNAL(clicked(bool)), addressComboBox, SLOT(setEnabled(bool)));

    mainLayout->addWidget(groupBox);

    setLayout(mainLayout);
}

bool IntroPage::isComplete() const
{
    if(fileName->text().isEmpty())
        return false;
    else
        return true;
}

void IntroPage::collectionsReceived( const Akonadi::Collection::List& list)
{
    foreach(const Akonadi::Collection & c, list) {
        addressComboBox->addItem(c.name(), c.id());
    }
}

/*
 * 2. Wizard page
 *
 * KBibTeX import of the bib file
 */
/*
File* concurrentBibImport(FileImporterBibTeX *importer, const QString &fileName, bool findDuplicates)
{
    QFile bibFile(fileName);
    if (!bibFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "can't open file " << fileName;
        return 0;
    }

    File *f = importer->load(&bibFile);

    if(findDuplicates) {
        qDebug() << "find duplicates";
    }

    return f;
}
*/
bool concurrentBibImport(NBibImporterBibTex *importer, const QString &fileName)
{
    return importer->readBibFile(fileName);
}

ParseFile::ParseFile(QWidget *parent)
    : QWizardPage(parent)
{
    futureWatcher = 0;
    importer = 0;
    setTitle(i18n("Reading BibTeX File"));
    setSubTitle(i18n("Please wait until all entries have been processed."));
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark1.png"));

    //file selection
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *processingLayout = new QHBoxLayout();
    processingLayout->setSpacing(20);
    QLabel *processingLabel = new QLabel(i18n("Processing:"));
    progressBar = new QProgressBar;
    registerField("progressBar*", progressBar);
    connect(progressBar, SIGNAL(valueChanged(int)), this, SIGNAL(completeChanged()));

    processingLayout->addWidget(processingLabel);
    processingLayout->addWidget(progressBar);
    mainLayout->addLayout(processingLayout);

    QVBoxLayout *gridCenterLayout = new QVBoxLayout();
    gridCenterLayout->addStretch();
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setSpacing(10);
    gridLayout->setMargin(0);
    QLabel *le = new QLabel(i18n("Entries:"));
    QLabel *la = new QLabel(i18n("Authors:"));
    QLabel *ld = new QLabel(i18n("Duplicates:"));
    entryNumber = new QLabel(QString::number(0));
    authorNumber = new QLabel(QString::number(0));
    duplicateNumber = new QLabel(QString::number(0));
    QFrame *hline = new QFrame();
    hline->setFrameStyle(QFrame::HLine);
    mergeButton = new QPushButton(i18n("Merge"));
    connect(mergeButton, SIGNAL(clicked()), this, SLOT(showMergeDialog()));

    gridLayout->addWidget(le,0,0);
    gridLayout->addWidget(entryNumber,0,1);
    gridLayout->addWidget(la,1,0);
    gridLayout->addWidget(authorNumber,1,1);
    gridLayout->addWidget(hline,2,0);
    gridLayout->setColumnStretch(2, 3);
    gridLayout->addWidget(ld,3,0);
    gridLayout->addWidget(duplicateNumber,3,1);
    gridLayout->addWidget(mergeButton,3,2);
    gridCenterLayout->addLayout(gridLayout);
    gridCenterLayout->addStretch();

    mainLayout->addLayout(gridCenterLayout);
    mainLayout->setSpacing(20);

    QLabel *text = new QLabel(i18n("Please choose if you want to merge the duplicated entries or leave them as they are."
                                   "Afterwards continue to import all entries into the Nepomuk/Akonadi storage."));
    text->setWordWrap(true);

    mainLayout->addWidget(text);

    setLayout(mainLayout);
}

void ParseFile::initializePage()
{
    IntroPage *ip = dynamic_cast<IntroPage *>(wizard()->page(0));

    delete futureWatcher;
    delete importer;
    importer = new NBibImporterBibTex;

    importer->setFindDuplicates(field(QLatin1String("duplicates")).toBool());
    bool importContactToAkonadi = field(QLatin1String("akonadiContact")).toBool();
    importer->setImportContactToAkonadi(importContactToAkonadi);
    if(importContactToAkonadi) {
        int curAddressBook = ip->addressComboBox->currentIndex();
        Akonadi::Collection c(ip->addressComboBox->itemData(curAddressBook).toInt());
        importer->setAkonadiAddressbook(c);
    }
    connect(importer, SIGNAL(progress(int)), progressBar, SLOT(setValue(int)) );

    // start background thread with the import
    QFuture<bool> future = QtConcurrent::run(concurrentBibImport, importer, ip->fileName->text());

    futureWatcher = new QFutureWatcher<bool>();
    futureWatcher->setFuture(future);
    connect(futureWatcher, SIGNAL(finished()),this, SLOT(importFinished()));
}

bool ParseFile::isComplete() const
{
    if(progressBar->value() == 100)
        return true;
    else
        return false;
}

void ParseFile::importFinished()
{
    bool result = futureWatcher->future().result();
    if(!result) {
        qDebug() << "error during import";
    }
    else {
        File * importedFile = importer->bibFile();

        if(importedFile) {
            qDebug() << "finished import with " << importedFile->size();
            entryNumber->setText(QString::number(importedFile->size()));
            int authors = importedFile->uniqueEntryValuesList(QLatin1String("author")).size();
            authorNumber->setText(QString::number(authors));
            duplicateNumber->setText(QString::number(0));
        }
    }
}

void ParseFile::cleanupPage()
{
    entryNumber->setText(QString::number(0));
    authorNumber->setText(QString::number(0));
    duplicateNumber->setText(QString::number(0));
}

void ParseFile::showMergeDialog()
{
    qDebug() << "open kbibtex merge window";
}

/*
 * 3. Wizard page
 *
 * pipe the bibtex File from the previous page to the nepomuk storage
 */
void concurrentNepomukImport(NBibImporterBibTex *importer)
{
    importer->pipeToNepomuk();
}

NepomukImport::NepomukImport(QWidget *parent)
    : QWizardPage(parent)
{
    btnp = 0;
    m_futureWatcher = 0;
    setTitle(i18n("Importing to Nepomuk / Akonadi"));
    setSubTitle(i18n("This might take a while until all entries are processed"));
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark1.png"));

    //file selection
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *processingLayout = new QHBoxLayout();
    processingLayout->setSpacing(20);
    QLabel *processingLabel = new QLabel(i18n("Processing:"));
    progressBar = new QProgressBar;
    registerField("progressBarNepomuk", progressBar);
    connect(progressBar, SIGNAL(valueChanged(int)), this, SIGNAL(completeChanged()));

    processingLayout->addWidget(processingLabel);
    processingLayout->addWidget(progressBar);
    mainLayout->addLayout(processingLayout);

    setLayout(mainLayout);
}

void NepomukImport::initializePage()
{
    ParseFile *pf = dynamic_cast<ParseFile *>(wizard()->page(1));

    connect(pf->importer, SIGNAL(progress(int)), progressBar, SLOT(setValue(int)) );

    delete m_futureWatcher;
    // start background thread with the import
    QFuture<void> future = QtConcurrent::run(concurrentNepomukImport, pf->importer);

    m_futureWatcher = new QFutureWatcher<void>();
    m_futureWatcher->setFuture(future);
    connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(importFinished()));
}

bool NepomukImport::isComplete() const
{
    if(progressBar->value() == 100)
        return true;
    else
        return false;
}

void NepomukImport::importFinished()
{
    qDebug() << "finished import";
}
