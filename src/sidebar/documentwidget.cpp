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
    }
}

void DocumentWidget::clear()
{
}

void DocumentWidget::createPublication()
{

}

void DocumentWidget::removePublication()
{

}

