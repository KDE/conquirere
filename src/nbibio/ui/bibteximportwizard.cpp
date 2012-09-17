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

#include "core/library.h"
#include "core/tagcloud.h"
#include "core/projectsettings.h"
#include "core/librarymanager.h"
#include "nbibio/bibtex/bibteximporter.h"

#include <kbibtex/fileimporterbibtex.h>
#include <kbibtex/file.h>
#include <kbibtex/findduplicatesui.h>
#include <kbibtex/findduplicates.h>

#include <Nepomuk2/Variant>

#include <KDE/KUrlRequester>
#include <KDE/KComboBox>
#include <KDE/KDialog>
#include <KDE/KService>
#include <KDE/KParts/Part>
#include <KDE/KStandardDirs>

#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionFetchScope>

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QCheckBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>
#include <QtCore/QThread>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QPointer>

#include <QDebug>

BibTeXImportWizard::BibTeXImportWizard(QWidget *parent)
    : QWizard(parent)
    , ui(new Ui::BibTeXImportWizard)
{

    ui->setupUi(this);
}

BibTeXImportWizard::~BibTeXImportWizard()
{
    delete ui;
}

void BibTeXImportWizard::setLibraryManager(LibraryManager *lm)
{
    m_libraryManager = lm;
}

LibraryManager *BibTeXImportWizard::libraryManager()
{
    return m_libraryManager;
}

void BibTeXImportWizard::setImportLibrary(Library *l)
{
    m_importToLibrary = l;
}

Library *BibTeXImportWizard::importLibrary()
{
    return m_importToLibrary;
}

void BibTeXImportWizard::setupUi()
{
    IntroPage *ip = new IntroPage;
    addPage(ip);
    ip->setupUi();
    addPage(new ParseFile);
    addPage(new NepomukImport);
}

/*
 * 1. Wizard page
 *
 * Selection of the file and options
 */
IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
    , addressComboBox(0)
    , fileType(0)
    , fileName(0)
    , projectImport(0)
{
}

void IntroPage::setupUi()
{
    // fetching all collections containing emails recursively, starting at the root collection
    Akonadi::CollectionFetchJob *job = new Akonadi::CollectionFetchJob( Akonadi::Collection::root(), Akonadi::CollectionFetchJob::Recursive, this );
    job->fetchScope().setContentMimeTypes( QStringList() << "application/x-vnd.kde.contactgroup" );
    connect( job, SIGNAL(collectionsReceived(Akonadi::Collection::List)),
             this, SLOT(collectionsReceived(Akonadi::Collection::List)) );

    setTitle(i18n("BibTeX Importer"));
    setSubTitle(i18n("This wizard will guide you through the import process."));
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark1.png"));

    //###################################################################
    //# file selection
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QFormLayout *fileLayout = new QFormLayout();
    fileType = new KComboBox();
    fileType->addItem(i18n("BibTeX"));
    fileType->addItem(i18n("PDF"));
    fileType->addItem(i18n("Ris"));
    fileType->setCurrentIndex(0);
    registerField("fileType", fileType);
    fileName = new KUrlRequester();
    registerField("fileName", fileName);
    connect(fileName, SIGNAL(textChanged(QString)), this, SIGNAL(completeChanged()));

    //fileLayout->addRow(i18n("File type:"), fileType);
    fileLayout->addRow(i18n("File:"), fileName);
    mainLayout->addLayout(fileLayout);

    //###################################################################
    //# options
    QGroupBox *groupBox = new QGroupBox(i18n("Options"));
    QVBoxLayout *groupBoxLayout = new QVBoxLayout();
    groupBox->setLayout(groupBoxLayout);

    QCheckBox *fdBox = new QCheckBox(i18n("find duplicates"));
    registerField("duplicates", fdBox);
    fdBox->setChecked(true);
    groupBoxLayout->addWidget(fdBox);

    //##################################################################
    //# Import Contacts to Akonadi
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

    //##################################################################
    //# Import to Project
    QCheckBox *cb3 = new QCheckBox(i18n("Import to Project"));
    registerField("projectImport", cb3);
    groupBoxLayout->addWidget(cb3);
    QHBoxLayout *comboLayout2 = new QHBoxLayout();
    projectImport = new KComboBox();
    projectImport->addItem( i18n("No Project") );

    BibTeXImportWizard *biw = static_cast<BibTeXImportWizard *>(wizard());

    int i=1;
    int selectedIndex=0;
    foreach(Library *l, biw->libraryManager()->openProjects()) {
        projectImport->addItem(l->settings()->name(), l->settings()->projectThing().uri());

        if(l == biw->importLibrary()) { selectedIndex=i; }

        i++;
    }
    projectImport->setCurrentIndex(selectedIndex);

    if(selectedIndex != 0) {
        cb3->setChecked(true);
        projectImport->setEnabled(true);
    }
    else {
        cb3->setChecked(false);
        projectImport->setEnabled(false);
    }

    comboLayout2->addSpacing(20);
    comboLayout2->addWidget(projectImport);
    groupBoxLayout->addLayout(comboLayout2);
    connect(cb3, SIGNAL(clicked(bool)), projectImport, SLOT(setEnabled(bool)));

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
bool concurrentBibImport(BibTexImporter *importer, const QString &fileName)
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

    //##################################################################
    //# file selection
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
    importer = new BibTexImporter;

    int fileType = field(QLatin1String("fileType")).toInt();
    fileType = 0; //disable other fileimporters for now, don't seem to work
    importer->setFileType( BibTexImporter::FileType(fileType) );
    importer->setFindDuplicates(field(QLatin1String("duplicates")).toBool());

    bool importContactToAkonadi = field(QLatin1String("akonadiContact")).toBool();
    if(importContactToAkonadi) {
        int curAddressBook = ip->addressComboBox->currentIndex();
        Akonadi::Collection c(ip->addressComboBox->itemData(curAddressBook).toInt());
        importer->setAkonadiAddressbook(c);
    }

    bool importToProject = field(QLatin1String("projectImport")).toBool();
    if(importToProject) {
        int curProject = ip->projectImport->currentIndex();
        Nepomuk2::Resource pimoThing = Nepomuk2::Resource (ip->projectImport->itemData(curProject).toUrl());
        importer->setProjectPimoThing(pimoThing);
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
            entryNumber->setText(QString::number(importedFile->size()));
            int authors = importedFile->uniqueEntryValuesList(QLatin1String("author")).size();
            authorNumber->setText(QString::number(authors));
            duplicateNumber->setText(QString::number(importer->duplicates().size()));
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
    QPointer<KDialog> dlg = new KDialog();
    File * importedFile = importer->bibFile();
    QList<EntryClique*> cliques = importer->duplicates();

    if(!cliques.isEmpty()) {
        MergeWidget *mw = new MergeWidget(importedFile, cliques, dlg);
        dlg->setMainWidget(mw);
        int ret = dlg->exec();

        if (ret == QDialog::Accepted) {
            MergeDuplicates md(dlg);
            md.mergeDuplicateEntries(cliques, importedFile);
            importer->findDuplicates();
            duplicateNumber->setText(QString::number(importer->duplicates().size()));
        }
        delete mw;
    }

    delete dlg;
}

/*
 * 3. Wizard page
 *
 * pipe the bibtex File from the previous page to the nepomuk storage
 */
void concurrentNepomukImport(BibTexImporter *importer)
{
    QStringList errorLog;
    importer->pipeToNepomuk(&errorLog);

    kDebug() << "pipeToNepomuk error" << errorLog;
}

NepomukImport::NepomukImport(QWidget *parent)
    : QWizardPage(parent)
{
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

}

