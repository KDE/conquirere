/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#include "documentwidget.h"
#include "ui_documentwidget.h"

#include "publicationwidget.h"

#include "nbib.h"
#include <KGlobalSettings>
#include <KMimeType>
#include <KIconLoader>
#include <kfilemetadatawidget.h>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Variant>

#include <QString>
#include <QVBoxLayout>
#include <QDebug>

DocumentWidget::DocumentWidget(QWidget *parent) :
    SidebarComponent(parent),
    ui(new Ui::DocumentWidget)
{
    ui->setupUi(this);

    setFont(KGlobalSettings::smallestReadableFont());
    m_kfmd = new KFileMetaDataWidget();

    QVBoxLayout *vbl = qobject_cast<QVBoxLayout *>(ui->contents->layout());
    vbl->insertWidget(0, m_kfmd);
    m_kfmd->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    m_kfmd->show();

    connect(ui->createPublication, SIGNAL(clicked()), this, SLOT(createPublication()));
    connect(ui->removePublication, SIGNAL(clicked()), this, SLOT(removePublication()));
}

DocumentWidget::~DocumentWidget()
{
    delete ui;
}

void DocumentWidget::setResource(Nepomuk::Resource & resource)
{
    m_document = resource;

    if(m_document.isValid()) {
        //fetch the documents icon
        QString fileUrl = m_document.property(Nepomuk::Vocabulary::NIE::url()).toString();

        QString mimeType = KMimeType::iconNameForUrl( fileUrl);

        KIconLoader kil;
        QPixmap fileIcon = kil.loadMimeTypeIcon(mimeType,  KIconLoader::Desktop);

        ui->icon->setPixmap(fileIcon);

        // show file meta data
        KFileItem kf(KFileItem::Unknown, KFileItem::Unknown, KUrl(fileUrl));
        kf.refresh();
        kf.refreshMimeType();
        KFileItemList kfil;
        kfil.append(kf);
        m_kfmd->setItems(kfil);

        Nepomuk::Resource pa = m_document.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();
        if(pa.isValid()) {
                 ui->createPublication->setVisible(false);
                 ui->removePublication->setVisible(true);
        }
        else {
            ui->createPublication->setVisible(true);
            ui->removePublication->setVisible(false);
        }
    }
}

void DocumentWidget::clear()
{
}

void DocumentWidget::createPublication()
{
    //TODO don't jus tcreate a publication, also select from existing ones
    KDialog showPublicationWidget;

    // create a temporary Publication object
    Nepomuk::Resource tempRef(QUrl(), Nepomuk::Vocabulary::NBIB::Publication());
    tempRef.setProperty(Nepomuk::Vocabulary::NBIB::isPublicationOf(), m_document);

    PublicationWidget *rw = new PublicationWidget();
    rw->setDialogMode(true);
    rw->setResource(tempRef);
    rw->setProject(project());

    showPublicationWidget.setMainWidget(rw);
    showPublicationWidget.setInitialSize(QSize(300,300));

    int ret = showPublicationWidget.exec();

    if(ret == KDialog::Accepted) {
        // add backreference
        m_document.setProperty(Nepomuk::Vocabulary::NBIB::publishedAs(), tempRef);
    }
    else {
        // remove temp publication again
        tempRef.remove();
    }

    //update
    setResource(m_document);
}

void DocumentWidget::removePublication()
{
    //TODO ask if publication should be deleted or just the link removed
    Nepomuk::Resource pa = m_document.property(Nepomuk::Vocabulary::NBIB::publishedAs()).toResource();
    pa.remove();

    m_document.removeProperty(Nepomuk::Vocabulary::NBIB::publishedAs());

    //update
    setResource(m_document);
}

