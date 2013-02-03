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

#include "itemmergedialog.h"
#include "ui_itemmergedialog.h"

#include "config/bibglobals.h"

#include "core/library.h"
#include "core/projectsettings.h"
#include "nbibio/pipe/nepomuktovariantpipe.h"

#include "sync.h"
#include "nbib.h"
#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Nepomuk2/Vocabulary/NIE>

#include <KDE/KIcon>
#include <KDE/KComboBox>
#include <KDE/KDebug>

#include <QtCore/QList>
#include <QtGui/QScrollArea>
#include <QtGui/QFormLayout>

using namespace Nepomuk2::Vocabulary;

ItemMergeDialog::ItemMergeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ItemMergeDialog)
    , m_libraryToSyncWith(0)
    , m_currentItem(0)
    , m_serverScrollArea(0)
    , m_localScrollArea(0)
{
    ui->setupUi(this);

    connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(showNext()));
    connect(ui->previousButton, SIGNAL(clicked()), this, SLOT(showPrevious()));
    connect(ui->finishButton, SIGNAL(clicked()), this, SLOT(finish()));

    QVBoxLayout *serverGroupboxLayout = new QVBoxLayout();
    ui->groupBoxServer->setLayout(serverGroupboxLayout);
    QVBoxLayout *localGroupboxLayout = new QVBoxLayout();
    ui->groupBoxLocal->setLayout(localGroupboxLayout);

    //REFACTOR: bibtex keytranslations from common global class
    m_keyTranslate.insert(QLatin1String("abstract"), i18n("Abstract"));
    m_keyTranslate.insert(QLatin1String("annote"), i18n("Annotation"));
    m_keyTranslate.insert(QLatin1String("archive"), i18n("Archive"));
    m_keyTranslate.insert(QLatin1String("history"), i18n("History"));
    m_keyTranslate.insert(QLatin1String("scale"), i18n("Scale"));
    m_keyTranslate.insert(QLatin1String("archivelocation"), i18n("Archive location"));
    m_keyTranslate.insert(QLatin1String("librarycatalog"), i18n("Library catalog"));
    m_keyTranslate.insert(QLatin1String("copyright"), i18n("Copyright"));
    m_keyTranslate.insert(QLatin1String("doi"), i18n("DOI"));
    m_keyTranslate.insert(QLatin1String("edition"), i18n("Edition"));
    m_keyTranslate.insert(QLatin1String("eprint"), i18n("ePrint"));
    m_keyTranslate.insert(QLatin1String("howpublished"), i18n("How Published"));
    m_keyTranslate.insert(QLatin1String("isbn"), i18n("isbn"));
    m_keyTranslate.insert(QLatin1String("language"), i18n("language"));
    m_keyTranslate.insert(QLatin1String("lccn"), i18n("lccn"));
    m_keyTranslate.insert(QLatin1String("mrnumber"), i18n("Mr Number"));
    m_keyTranslate.insert(QLatin1String("note"), i18n("Note"));
    m_keyTranslate.insert(QLatin1String("number"), i18n("Number"));
    m_keyTranslate.insert(QLatin1String("pages"), i18n("Pages"));
    m_keyTranslate.insert(QLatin1String("numpages"), i18n("Number of Pages"));
    m_keyTranslate.insert(QLatin1String("numberofvolumes"), i18n("Number of Volumes"));
    m_keyTranslate.insert(QLatin1String("pubmed"), i18n("PubMed"));
    m_keyTranslate.insert(QLatin1String("shorttitle"), i18n("Shorttitle"));
    m_keyTranslate.insert(QLatin1String("type"), i18n("Type"));
    m_keyTranslate.insert(QLatin1String("applicationnumber"), i18n("Application Number"));
    m_keyTranslate.insert(QLatin1String("prioritynumbers"), i18n("Priority Numbers"));
    m_keyTranslate.insert(QLatin1String("legalstatus"), i18n("Legal Status"));
    m_keyTranslate.insert(QLatin1String("references"), i18n("References"));
    m_keyTranslate.insert(QLatin1String("filingdate"), i18n("Filing Date"));
    m_keyTranslate.insert(QLatin1String("volume"), i18n("Volume"));
    m_keyTranslate.insert(QLatin1String("accessdate"), i18n("Access Date"));
    m_keyTranslate.insert(QLatin1String("date"), i18n("Date"));
    m_keyTranslate.insert(QLatin1String("author"), i18n("Author"));
    m_keyTranslate.insert(QLatin1String("bookauthor"), i18n("Book Author"));
    m_keyTranslate.insert(QLatin1String("contributor"), i18n("Contributor"));
    m_keyTranslate.insert(QLatin1String("translator"), i18n("Translator"));
    m_keyTranslate.insert(QLatin1String("reviewedauthor"), i18n("Reviewed Author"));
    m_keyTranslate.insert(QLatin1String("attorneyagent"), i18n("Attorney Agent"));
    m_keyTranslate.insert(QLatin1String("counsel"), i18n("Counsel"));
    m_keyTranslate.insert(QLatin1String("cosponsor"), i18n("CoSponsor"));
    m_keyTranslate.insert(QLatin1String("commenter"), i18n("Commenter"));
    m_keyTranslate.insert(QLatin1String("serieseditor"), i18n("Series Editor"));
    m_keyTranslate.insert(QLatin1String("booktitle"), i18n("Book Title"));
    m_keyTranslate.insert(QLatin1String("chapter"), i18n("Chapter"));
    m_keyTranslate.insert(QLatin1String("editor"), i18n("Editor"));
    m_keyTranslate.insert(QLatin1String("institution"), i18n("Institution"));
    m_keyTranslate.insert(QLatin1String("issn"), i18n("issn"));
    m_keyTranslate.insert(QLatin1String("organization"), i18n("Organization"));
    m_keyTranslate.insert(QLatin1String("legislativebody"), i18n("Legislative Body"));
    m_keyTranslate.insert(QLatin1String("code"), i18n("Code of Law"));
    m_keyTranslate.insert(QLatin1String("month"), i18n("Month"));
    m_keyTranslate.insert(QLatin1String("codenumber"), i18n("Code Number"));
    m_keyTranslate.insert(QLatin1String("codevolume"), i18n("Code Volume"));
    m_keyTranslate.insert(QLatin1String("reporter"), i18n("Reporter"));
    m_keyTranslate.insert(QLatin1String("reportervolume"), i18n("Reporter Volume"));
    m_keyTranslate.insert(QLatin1String("publisher"), i18n("Publisher"));
    m_keyTranslate.insert(QLatin1String("school"), i18n("School"));
    m_keyTranslate.insert(QLatin1String("series"), i18n("Series"));
    m_keyTranslate.insert(QLatin1String("conferencename"), i18n("Conferencename"));
    m_keyTranslate.insert(QLatin1String("meetingname"), i18n("Meetingname"));
    m_keyTranslate.insert(QLatin1String("event"), i18n("Event"));
    m_keyTranslate.insert(QLatin1String("url"), i18n("URL"));
    m_keyTranslate.insert(QLatin1String("title"), i18n("Title"));
    m_keyTranslate.insert(QLatin1String("localfile"), i18n("Local File"));
    m_keyTranslate.insert(QLatin1String("biburl"), i18n("Abstract"));
    m_keyTranslate.insert(QLatin1String("bibsource"), i18n("BibURL"));
    m_keyTranslate.insert(QLatin1String("ee"), i18n("ee"));
    m_keyTranslate.insert(QLatin1String("address"), i18n("Address"));
    m_keyTranslate.insert(QLatin1String("year"), i18n("Year"));
    m_keyTranslate.insert(QLatin1String("keywords"), i18n("Keywords"));
    m_keyTranslate.insert(QLatin1String("assignee"), i18n("Assignee"));
    m_keyTranslate.insert(QLatin1String("descriptor"), i18n("Descriptor"));
    m_keyTranslate.insert(QLatin1String("classification"), i18n("Classification"));
    m_keyTranslate.insert(QLatin1String("thesaurus"), i18n("Thesaurus"));
    m_keyTranslate.insert(QLatin1String("subject"), i18n("Subject"));
}

ItemMergeDialog::~ItemMergeDialog()
{
    delete ui;
}

void ItemMergeDialog::setItemsToMerge(const QList<SyncMergeDetails> &items)
{
    NepomukToVariantPipe ntvp;

    foreach(const SyncMergeDetails &sd, items) {
        Nepomuk2::Resource resource;

        QUrl syncType = sd.syncResource.property(SYNC::syncDataType()).toUrl();

        if( syncType == SYNC::Note()) {
            resource = sd.syncResource.property(SYNC::note()).toResource();
        }
        else if ( syncType == SYNC::Attachment()) {
            resource = sd.syncResource.property(SYNC::attachment()).toResource();
        }
        else {
            resource = sd.syncResource.property(SYNC::reference()).toResource();
        }

        ntvp.pipeExport(QList<Nepomuk2::Resource>() << resource);
        QVariantList localExportedList = ntvp.variantList();
        MergedResults mr;
        mr.localSyncResource = sd.syncResource;
        mr.localEntry = localExportedList.first().toMap();
        mr.serverEntry = sd.externalResource;

        // get each key that has a different content
        //TODO: put this into NepomukToVariantPipe ?
        QMapIterator<QString, QVariant> i(mr.serverEntry);
         while (i.hasNext()) {
             i.next();

             QString keyToCheck = i.key().toLower();

             // the server item has no content, and the key does not exist in the local storage
             if( i.value().toString().isEmpty() && !mr.localEntry.contains(keyToCheck) ) {
                 continue;
             }

             if( i.value() != mr.localEntry.value(keyToCheck) ) {
                 QVariant localValue = mr.localEntry.value(keyToCheck);
                 // exception for date handling
                 if(keyToCheck == QLatin1String("accessdate")) {
                     // ignore if server is 2012-01-12 and local is 2012-01-13T00:00:00Z
                     if(localValue.toString().startsWith(i.value().toString()))
                         continue;
                 }

                 //TODO: for names/tags check also different order of the entry
                 // otherwise add the entry differeces
                 mr.differendKeys << keyToCheck;
             }
         }

        m_mergeResults.append(mr);
    }

    ui->previousButton->setEnabled(false);
    if(m_mergeResults.size() > 1) {
        ui->nextButton->setEnabled(true);
        ui->finishButton->setEnabled(false);
    }
    else {
        ui->nextButton->setEnabled(false);
        ui->finishButton->setEnabled(true);
    }

    showItem(0);

}

void ItemMergeDialog::setProviderDetails(ProviderSyncDetails psd)
{
    m_psd = psd;
}

void ItemMergeDialog::setLibraryToSyncWith(Library *l)
{
    m_libraryToSyncWith = l;
}

void ItemMergeDialog::showNext()
{
    m_currentItem++;
    if(m_currentItem == m_mergeResults.size() - 1) {
        ui->nextButton->setEnabled(false);
        ui->finishButton->setEnabled(true);
    }
    else {
        ui->nextButton->setEnabled(true);
        ui->finishButton->setEnabled(false);
    }
    ui->previousButton->setEnabled(true);

    showItem(m_currentItem);
}

void ItemMergeDialog::showPrevious()
{
    m_currentItem--;

    ui->finishButton->setEnabled(false);

    if(m_currentItem == 0) {
        ui->previousButton->setEnabled(false);
    }
    else {
        ui->previousButton->setEnabled(true);
    }

    if( m_mergeResults.size() > 1) {
        ui->nextButton->setEnabled(true);
    }

    showItem(m_currentItem);
}

void ItemMergeDialog::finish()
{
    kDebug() << "finish ... merge all differences";
    /*
    BibTexToNepomukPipe btnp;
    btnp.setSyncDetails(m_psd.url, m_psd.userName);

    if(m_libraryToSyncWith && m_libraryToSyncWith->libraryType() == Library_Project) {
        btnp.setProjectPimoThing(m_libraryToSyncWith->settings()->projectThing());
    }

    foreach(const MergedResults &mr, m_mergeResults) {
        btnp.mergeManual(mr.localSyncResource, mr.mergedChanges);
    }

    */

    accept();
}

void ItemMergeDialog::replaceSelection()
{
    KComboBox *cb = qobject_cast<KComboBox*>(sender());

    MergedResults mr = m_mergeResults.at(m_currentItem);

    int index = cb->currentIndex();
    QString key = cb->itemData(index).toString();

    if(index == 0) { // stick to local
        mr.mergedChanges.remove( key );
    }
    else { // select server changes
        mr.mergedChanges.insert( key , mr.serverEntry.value(key));
    }

    m_mergeResults.replace(m_currentItem, mr);
}

void ItemMergeDialog::showItem(int index)
{
    ui->countLabel->setText(i18n("Merge item %1 of %2", m_currentItem+1, m_mergeResults.size()));

    MergedResults mr = m_mergeResults.at(index);

    Nepomuk2::Resource publication;
    Nepomuk2::Resource reference;

    QUrl syncType = mr.localSyncResource.property(SYNC::syncDataType()).toUrl();

    kDebug() << "sync type" << syncType << "entry item type" << mr.localEntry.value(QLatin1String("bibtexentrytype"));

    //TODO: put icon stuff in MergeResource creation
    if( syncType == SYNC::Note()) {
        publication = mr.localSyncResource.property(SYNC::note()).toResource();
        ui->itemIcon->setPixmap(KIcon("knotes").pixmap(22,22));
    }
    else if ( syncType == SYNC::Attachment()) {
        publication = mr.localSyncResource.property(SYNC::attachment()).toResource();
        ui->itemIcon->setPixmap(KIcon("application-pdf").pixmap(22,22));
    }
    else {
        publication = mr.localSyncResource.property(SYNC::publication()).toResource();
        reference = mr.localSyncResource.property(SYNC::reference()).toResource();
        BibGlobals::BibEntryType bet = BibGlobals::BibEntryTypeFromUrl(publication);
        ui->itemIcon->setPixmap(KIcon(BibGlobals::BibEntryTypeIcon(bet)).pixmap(22,22));
    }

    ui->itemTitle->setText( publication.property(NIE::title()).toString());


    QWidget *serverData = new QWidget();
    QFormLayout *serverLayout = new QFormLayout();
    serverData->setLayout(serverLayout);

    QWidget *localData = new QWidget();
    QFormLayout *localLayout = new QFormLayout();
    localData->setLayout(localLayout);

    foreach(const QString &key, mr.differendKeys) {
        if(key.startsWith( QLatin1String("sync-") ))
            continue;

        QString localValue = mr.localEntry.value(key).toString();
        QString serverValue = mr.serverEntry.value(key).toString();

        // add server row
        serverLayout->addRow(QLatin1String("<b>") + m_keyTranslate.value(key, key) + QLatin1String(":</b>"), new QLabel(serverValue));


        //add local row
        KComboBox *cb = new KComboBox;
        cb->addItem( localValue, key );
        cb->addItem( serverValue, key );
        localLayout->addRow(QLatin1String("<b>") + m_keyTranslate.value(key, key) + QLatin1String(":</b>"), cb);

        connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(replaceSelection()));

        // automatically select localvalue as default
        mr.mergedChanges.insert( key , localValue);

        kDebug() << "insert key" << key << "with" << localValue;
    }

    // complete server groupbox
    QLayout *serverGroupboxLayout = ui->groupBoxServer->layout();
    serverGroupboxLayout->removeWidget(m_serverScrollArea);
    delete m_serverScrollArea;
    m_serverScrollArea = new QScrollArea;
    m_serverScrollArea->setWidget(serverData);
    serverGroupboxLayout->addWidget(m_serverScrollArea);

    // complete local groupbox
    QLayout *localGroupboxLayout = ui->groupBoxLocal->layout();
    localGroupboxLayout->removeWidget(m_localScrollArea);
    delete m_localScrollArea;
    m_localScrollArea = new QScrollArea;
    m_localScrollArea->setWidget(localData);
    localGroupboxLayout->addWidget(m_localScrollArea);

}
