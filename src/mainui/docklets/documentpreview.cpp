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
    : QWidget(parent)
    , ui(new Ui::DocumentPreview)
    , m_visible(true)
{
    ui->setupUi(this);
    ui->openButton->setIcon(KIcon(QLatin1String("document-open")));

    connect(ui->urlSelector, SIGNAL(currentIndexChanged(int)),this, SLOT(showUrl(int)));

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
    if(m_part) {
        m_part->closeUrl();
    }

    if(resource.isValid()) {
        QList<Nepomuk::Resource> fileList;

        if(resource.hasType(Nepomuk::Vocabulary::NBIB::Reference())) {
            Nepomuk::Resource publication = resource.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
            fileList = publication.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResourceList();
            fileList.append( publication.property(Nepomuk::Vocabulary::NIE::links()).toResourceList() );
        }
        else if(resource.hasType(Nepomuk::Vocabulary::NBIB::Publication())) {
            fileList = resource.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResourceList();
            fileList.append( resource.property(Nepomuk::Vocabulary::NIE::links()).toResourceList() );
        }
        else {
            fileList.append(resource);
        }

        // add all DataObjects to the preview
        foreach(const Nepomuk::Resource & r, fileList) {
            KUrl url = KUrl(r.property(Nepomuk::Vocabulary::NIE::url()).toString());
            KIcon icon;
            QString mimetype;

            if( r.hasType(Nepomuk::Vocabulary::NFO::Website()) || r.hasType(Nepomuk::Vocabulary::NFO::WebDataObject())
                || url.scheme() == QLatin1String("http")) {

                QString favIcon = KMimeType::favIconForUrl(url);
                if(favIcon.isEmpty()) {
                    favIcon = QLatin1String("text-html");
                }

                icon = KIcon(favIcon);
                mimetype = QLatin1String("text/html");
            }
            else {
                KMimeType::Ptr mimeTypePtr = KMimeType::findByUrl(url);

                icon = KIcon(mimeTypePtr->iconName());
                mimetype = mimeTypePtr->name();
            }

            ui->urlSelector->addItem(icon,url.url(),QVariant(mimetype));
        }

        // add the DOI if available as preview

        // check if an url with a doi was already attached via the usual url stuff
        if( ui->urlSelector->findText(QLatin1String("dx.doi.org"), Qt::MatchContains ) != -1) {
            return;
        }

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

            KUrl url (doi);
            KIcon icon;
            QString mimetype = QLatin1String("text/html");

            QString favIcon = KMimeType::favIconForUrl(url);
            if(favIcon.isEmpty()) {
                favIcon = QLatin1String("text-html");
            }

            icon = KIcon(favIcon);
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

//            kDebug() << "open url" << url << "with KPart" << partsName;
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
        m_part->showProgressInfo(true);
        ui->kpartWidget->layout()->addWidget(m_part->widget());
        emit activateKPart(m_part);

        m_part->openUrl(url);

//        kDebug() << "open url" << url << "with KPart" << partsName;

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

void DocumentPreview::resizeEvent ( QResizeEvent * event )
{
    if( event->size().height() == event->oldSize().height()) {
        return;
    }

    m_visible = event->size().height() != 0;

    if(m_visible && event->oldSize().height() == 0) {
        showUrl(0);
        emit activateKPart(m_part);
    }

    if(!m_visible) {
        emit activateKPart(0);
    }
}

