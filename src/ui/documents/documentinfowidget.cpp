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

#include "documentinfowidget.h"
#include "ui_documentinfowidget.h"

#include "filebibtexwidget.h"

#include <KFileMetaDataWidget>
#include <KGlobalSettings>

#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QScrollArea>

DocumentInfoWidget::DocumentInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DocumentInfoWidget)
{
    ui->setupUi(this);

    m_metaDataWidget = new KFileMetaDataWidget();
    //m_metaDataWidget->setFont(KGlobalSettings::smallestReadableFont());
    //m_metaDataWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    //ui->horizontalLayout->addWidget(m_metaDataWidget);

    m_fileBibTexWidget = new FileBibTexWidget();
    ui->horizontalLayout->addWidget(m_fileBibTexWidget);

}

DocumentInfoWidget::~DocumentInfoWidget()
{
    delete ui;
    delete m_metaDataWidget;
    delete m_fileBibTexWidget;
}

void DocumentInfoWidget::setResource(Nepomuk::Resource &resource)
{
    m_fileBibTexWidget->setResource(resource);
}

void DocumentInfoWidget::clear()
{
    m_fileBibTexWidget->clear();
}

//void DocumentInfoWidget::showItem(/*const KFileItem& item*/)
//{
//        KUrl ku;
//        ku.setUrl(QLatin1String("/home/joerg/Dokumente/test_research/documents/OPTIM_2006_ID175_-_Prenzel_MASSiVE_final.pdf"));
//        KFileItem kfi(KFileItem::Unknown, KFileItem::Unknown, ku);
//    //    //m_documentInfo->showItem(kfi);
//    //    kfi.determineMimeType();

//    //    //qDebug() << kfi.mimetype();

//    //    KFileMetaDataWidget *m_metaDataWidget = new KFileMetaDataWidget(this);
//    //m_metaDataWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
//    //m_metaDataWidget->setItems(KFileItemList() << kfi);
//    //m_metaDataWidget->show();
//    //m_metaDataWidget->setItems(KFileItemList() << item);
//}
