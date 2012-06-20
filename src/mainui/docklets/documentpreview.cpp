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

#include "documentpreview.h"
#include "ui_documentpreview.h"

#include "documentpreviewtab.h"

#include <QtCore/QDebug>

DocumentPreview::DocumentPreview(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DocumentPreview)
{
    ui->setupUi(this);

    DocumentPreviewTab *firstTab = new DocumentPreviewTab(this);
    connect(firstTab, SIGNAL(activateKPart(KParts::Part*)), this, SIGNAL(activateKPart(KParts::Part*)));

    ui->ktabwidget->addTab( firstTab, QString(""));

    ui->ktabwidget->setTabBarHidden( true );

    connect( ui->ktabwidget, SIGNAL(closeRequest(QWidget*)), this, SLOT(closeRequest(QWidget*)));
    connect( ui->ktabwidget, SIGNAL(currentChanged(int)), this, SLOT(currentIndexChanged(int)));
}

DocumentPreview::~DocumentPreview()
{
    delete ui;
}

void DocumentPreview::setResource(Nepomuk::Resource & resource, bool inNewTab)
{
    if(inNewTab) {
        DocumentPreviewTab *newTab = new DocumentPreviewTab(this);
        connect(newTab, SIGNAL(activateKPart(KParts::Part*)), this, SIGNAL(activateKPart(KParts::Part*)));

        int tabIndex = ui->ktabwidget->addTab( newTab, resource.genericLabel().left(20));
        ui->ktabwidget->setTabToolTip(tabIndex, resource.genericLabel());

        ui->ktabwidget->setTabBarHidden( false );

        newTab->setResource( resource );
    }
    else {
        DocumentPreviewTab *newTab = qobject_cast<DocumentPreviewTab *>(ui->ktabwidget->widget(0));
        newTab->setResource( resource );

        ui->ktabwidget->setTabText(0, resource.genericLabel().left(20));
        ui->ktabwidget->setTabToolTip(0, resource.genericLabel());
    }
}

void DocumentPreview::clear()
{
    DocumentPreviewTab *newTab = qobject_cast<DocumentPreviewTab *>(ui->ktabwidget->widget(0));
    newTab->clear();
}

void DocumentPreview::closeRequest( QWidget * documentTab)
{
    int currentIndex = ui->ktabwidget->indexOf( documentTab );

    DocumentPreviewTab *tab = 0;
    if(currentIndex == 0) {
        tab = qobject_cast<DocumentPreviewTab *>(ui->ktabwidget->widget(currentIndex+1));
    }
    else {
        tab = qobject_cast<DocumentPreviewTab *>(ui->ktabwidget->widget(currentIndex-1));
    }
    tab->setActive();

    ui->ktabwidget->removeTab(ui->ktabwidget->indexOf( documentTab ) );
    delete documentTab;

    if( ui->ktabwidget->count() == 1) {
        ui->ktabwidget->setTabBarHidden( true );
    }
}

void DocumentPreview::currentIndexChanged(int index)
{
    DocumentPreviewTab *tab = qobject_cast<DocumentPreviewTab *>(ui->ktabwidget->widget(index));
    tab->setActive();

    emit activeDocumentChanged(tab->resource());
}
