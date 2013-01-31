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

#include "searchresultinfowidget.h"
#include "ui_searchresultinfowidget.h"

#include <KDE/KDebug>

#include <nepomuk-webminer/krossextractor.h>
#include <nepomuk-webminer/publicationpipe.h>

using namespace NepomukWebMiner::Extractor;
using namespace NepomukWebMiner::Pipe;

SearchResultInfoWidget::SearchResultInfoWidget(QWidget *parent) :
    SidebarComponent(parent),
    ui(new Ui::SearchResultInfoWidget)
{
    ui->setupUi(this);

    connect(ui->importButton, SIGNAL(clicked()), this, SLOT(importItem()) );
    connect(ui->importWithRefsButton, SIGNAL(clicked()), this, SLOT(importWithReferences()) );

    ui->importButton->setIcon(KIcon("download"));
    ui->importWithRefsButton->setIcon(KIcon("download"));
}

SearchResultInfoWidget::~SearchResultInfoWidget()
{
    delete ui;
}

Nepomuk2::Resource SearchResultInfoWidget::resource()
{
    qWarning() << "this shouldn't be used .... SearchResultInfoWidget can't return a resource";
    Nepomuk2::Resource empty;
    return empty;
}

void SearchResultInfoWidget::setResource(Nepomuk2::Resource & resource)
{
    Q_UNUSED(resource);
}

void SearchResultInfoWidget::setResource(SearchResultModel::SRCachedRowEntry webResult)
{
    kDebug() << "show info for" << webResult.detailsurl << webResult.engineScript;

    ui->infoTitle->setText( webResult.displayColums.at(SearchResultModel::Column_Name).toString() );
    ui->infoDate->setText( webResult.displayColums.at(SearchResultModel::Column_Date).toString() );
    ui->infoType->setText( webResult.displayColums.at(SearchResultModel::Column_EntryType).toString() );
    ui->infoAuthors->setText( webResult.displayColums.at(SearchResultModel::Column_Author).toString() );

    // remove the html tags added for the tablemodel
    QString excerpt = webResult.displayColums.at(SearchResultModel::Column_Details).toString();
    excerpt.remove(QLatin1String("<font size=\"90%\">"));
    excerpt.remove(QLatin1String("</font>"));
    ui->infoExcerpt->setText( excerpt );

    ui->kurllabel->setUrl( webResult.detailsurl.toString() );

    m_currentEntry = webResult;
}

void SearchResultInfoWidget::newButtonClicked()
{

}

void SearchResultInfoWidget::deleteButtonClicked()
{

}

void SearchResultInfoWidget::importItem()
{
    QVariantMap options;
    options.insert(QString("references"), false);

    m_extractor.addJob(m_currentEntry.detailsurl, options, m_currentEntry.displayColums.at(SearchResultModel::Column_Name).toString());
}

void SearchResultInfoWidget::importWithReferences()
{
    QVariantMap options;
    options.insert(QString("references"), true);

    m_extractor.addJob(m_currentEntry.detailsurl, options, m_currentEntry.displayColums.at(SearchResultModel::Column_Name).toString());
}
