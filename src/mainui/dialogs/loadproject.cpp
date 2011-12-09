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

#include "loadproject.h"
#include "ui_loadproject.h"

#include "../../core/library.h"

#include <Nepomuk/Resource>
#include <Nepomuk/Variant>

#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>

#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KLineEdit>

#include <QtGui/QListWidgetItem>

#include <QtCore/QDebug>

LoadProject::LoadProject(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadProject)
{
    ui->setupUi(this);

    loadCollections();

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(showCollection(int)));
    connect(ui->listWidget, SIGNAL(currentRowChanged(int)), ui->kurlrequester, SLOT(clear()));
    connect(ui->listWidget, SIGNAL(clicked(QModelIndex)), ui->kurlrequester, SLOT(clear()));
    connect(ui->kurlrequester, SIGNAL(textChanged(QString)), this, SLOT(showExternalCollection(QString)));

    ui->kurlrequester->lineEdit()->setReadOnly(true);
}

LoadProject::~LoadProject()
{
    delete ui;
}

Library *LoadProject::loadedLibrary() const
{
    return m_loadLibrary;
}

void LoadProject::showCollection(int currentRow)
{
    if(currentRow >= 0) {
        ui->listWidget->clearSelection();

        QListWidgetItem *item = ui->listWidget->item(currentRow);

        Nepomuk::Resource collection = Nepomuk::Resource(item->data(Qt::UserRole).toString());

        ui->labelName->setText(collection.property(Nepomuk::Vocabulary::NIE::title()).toString());
        ui->labelDescription->setText(collection.property(Soprano::Vocabulary::NAO::description()).toString());
    }
}

void LoadProject::showExternalCollection(const QString &newurl)
{
    if(newurl.isEmpty())
        return;

    KConfig libconfig( newurl, KConfig::SimpleConfig );
    KConfigGroup generalGroup( &libconfig, "Conquirere" );

    ui->labelDescription->setText(generalGroup.readEntry(QLatin1String("description")));
    ui->labelName->setText(generalGroup.readEntry(QLatin1String("name")));
}

void LoadProject::accept()
{
    m_loadLibrary = new Library(Library_Project);
    if(!ui->kurlrequester->url().isEmpty()) {
        m_loadLibrary->loadLibrary(ui->kurlrequester->url().path());
    }
    else {
        QListWidgetItem *curItem = ui->listWidget->currentItem();
        Nepomuk::Resource collection = Nepomuk::Resource(curItem->data(Qt::UserRole).toString());
        m_loadLibrary->loadLibrary(collection);
    }

    QDialog::accept();
}

void LoadProject::loadCollections()
{
    KSharedConfigPtr config = KSharedConfig::openConfig("conquirererc");

    KConfigGroup generalGroup = config->group("General");
    QString NepomukCollection = generalGroup.readEntry( "NepomukCollection", QString() );

    Nepomuk::Resource conquiereCollections = Nepomuk::Resource(NepomukCollection);

    QList<Nepomuk::Resource> collections = conquiereCollections.property(Nepomuk::Vocabulary::PIMO::isRelated()).toResourceList();

    foreach(const Nepomuk::Resource &r, collections) {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(r.property(Nepomuk::Vocabulary::NIE::title()).toString());
        newItem->setData(Qt::UserRole, r.resourceUri());
        ui->listWidget->addItem(newItem);
    }
}