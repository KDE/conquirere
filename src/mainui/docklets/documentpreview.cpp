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

#include "documentpreview.h"
#include "ui_documentpreview.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <KDE/KUrl>
#include <KDE/KMimeType>
#include <KDE/KMimeTypeTrader>
#include <KDE/KService>
#include <KParts/Part>
#include <KParts/OpenUrlEvent>

#include <QtGui/QDesktopServices>
#include <QtGui/QLabel>

#include <QtCore/QDebug>
#include <KDE/KDebug>

DocumentPreview::DocumentPreview(QWidget *parent)
    : QDockWidget(parent)
    , ui(new Ui::DocumentPreview)
    , m_visible(true)
{
    ui->setupUi(this);
    ui->openButton->setIcon(KIcon(QLatin1String("document-open")));

    connect(ui->urlSelector, SIGNAL(currentIndexChanged(int)),this, SLOT(showUrl(int)));
    connect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(toggled(bool)));

    m_labelNone = 0;
    m_labelInvalid = 0;
    m_part = 0;
}

DocumentPreview::~DocumentPreview()
{
    delete m_part;
    delete m_labelInvalid;
    delete m_labelNone;
    delete ui;
}

void DocumentPreview::setResource(Nepomuk::Resource & resource)
{
    ui->urlSelector->clear();

    if(resource.isValid()) {
        QList<Nepomuk::Resource> fileList;

        if(resource.hasType(Nepomuk::Vocabulary::NBIB::Reference())) {
            Nepomuk::Resource publication = resource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
            fileList = publication.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResourceList();
        }
        else if(resource.hasType(Nepomuk::Vocabulary::NBIB::Publication())) {
            fileList = resource.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResourceList();
        }
        else {
            fileList.append(resource);
        }

        // add all DataObjects to the preview
        foreach(const Nepomuk::Resource & r, fileList) {
            KIcon icon;
            KUrl url = KUrl(r.property(Nepomuk::Vocabulary::NIE::url()).toString());
            QString mimetype;

            if(r.hasType(Nepomuk::Vocabulary::NFO::RemoteDataObject())) {

            }
            else if(r.hasType(Nepomuk::Vocabulary::NFO::Website())) {
                icon = KIcon("text-html");
                mimetype = QLatin1String("text/html");
            }
            else if(r.hasType(Nepomuk::Vocabulary::NFO::FileDataObject())) {
                mimetype = r.property(Nepomuk::Vocabulary::NIE::mimeType()).toString();

                icon = KIcon(mimetype);
            }

            if(mimetype.isEmpty()) {
                qWarning() << "Nepomuk does not have any mimetype data for " << r.genericLabel();

                int accuracy = 0;
                KMimeType::Ptr mimeTypePtr = KMimeType::findByUrl(url, 0, url.isLocalFile(), true, &accuracy);
                if (accuracy < 50) {
                    kDebug() << "discarding mime type " << mimeTypePtr->name() << ", trying filename ";
                    mimeTypePtr = KMimeType::findByPath(url.fileName(), 0, true, &accuracy);
                }

                mimetype = mimeTypePtr->name();
                icon = KIcon(mimeTypePtr->iconName());
            }

            ui->urlSelector->addItem(icon,url.url(),QVariant(mimetype));
        }

        // add the DOI if available as preview
        QString doi;
        if(resource.hasType(Nepomuk::Vocabulary::NBIB::Reference())) {
            Nepomuk::Resource publication = resource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
            doi = publication.property(Nepomuk::Vocabulary::NBIB::doi()).toString();
        }
        else if(resource.hasType(Nepomuk::Vocabulary::NBIB::Publication())) {
            doi = resource.property(Nepomuk::Vocabulary::NBIB::doi()).toString();
        }

        if(!doi.isEmpty()) {
            if(!doi.startsWith(QLatin1String("http:/"))) {
                doi = QLatin1String("http://dx.doi.org/") + doi;
            }

            KIcon icon = KIcon("text-html");
            QString mimetype = QLatin1String("text/html");
            KUrl url (doi);
            ui->urlSelector->addItem(icon,url.url(),QVariant(mimetype));
        }
    }
}

void DocumentPreview::clear()
{
    Nepomuk::Resource empty;
    setResource(empty);
}

void DocumentPreview::showUrl(int index)
{
    if(index < 0 || isHidden() || !isVisible() || !m_visible)
        return;

    QString url = ui->urlSelector->itemText(index);
    QString mimetype = ui->urlSelector->itemData(index).toString();

    delete m_labelNone;
    m_labelNone = 0;
    delete m_labelInvalid;
    m_labelInvalid = 0;

    if(url.isEmpty() || url.startsWith(QLatin1String("akonadi"))) {
        m_labelNone = new QLabel(i18n("Cannot create preview\n\nNo url available."), ui->kpartWidget);
        m_labelNone->setAlignment(Qt::AlignCenter);
        ui->kpartWidget->layout()->addWidget(m_labelNone);

        if(m_part) {
            m_part->closeUrl();
            emit activateKPart(0);

            delete m_part;
            m_part = 0;
        }

        return;
    }

    KUrl urlInfo(url);

    KService::Ptr serivcePtr = KMimeTypeTrader::self()->preferredService(mimetype, QLatin1String("KParts/ReadOnlyPart"));

    QString partsName;
    if(!serivcePtr.isNull())
        partsName = serivcePtr->library();

    if(m_lastPartsName == partsName) {
        if(m_part) {
            m_part->openUrl(url);
        }
        return;
    }

    m_lastPartsName = partsName;
    emit activateKPart(0);
    delete m_part;
    m_part = 0;

    if (serivcePtr.isNull()) {
        return;
    }

    m_part = serivcePtr->createInstance<KParts::ReadOnlyPart>(0);

    if (m_part) {
        m_part->setProgressInfoEnabled(true);
        ui->kpartWidget->layout()->addWidget(m_part->widget());
        emit activateKPart(m_part);

        m_part->openUrl(url);

    } else {
        m_labelInvalid = new QLabel(i18n("Cannot create preview for\n%1\n\nNo part available.", urlInfo.fileName()), ui->kpartWidget);
        m_labelInvalid->setAlignment(Qt::AlignCenter);
        ui->kpartWidget->layout()->addWidget(m_labelInvalid);
        emit activateKPart(0);
    }
}

void DocumentPreview::openExternally() {
    KUrl url(ui->urlSelector->currentText());
    QDesktopServices::openUrl(url);
}

void DocumentPreview::hideEvent ( QHideEvent * event )
{
//    kDebug() << "visibility changed";
//    m_lastPartsName.clear();
    emit activateKPart(0);

//    delete m_part;
//    m_part = 0;
//    delete m_labelNone;
//    m_labelNone = 0;
//    delete m_labelInvalid;
//    m_labelInvalid = 0;
}

void DocumentPreview::showEvent ( QShowEvent * event )
{
    showUrl(0);
}

void DocumentPreview::toggled(bool status)
{
//    kDebug() << "visibility changed" << status;
    m_visible = status;
}
