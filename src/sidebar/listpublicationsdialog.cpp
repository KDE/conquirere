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

#include "listpublicationsdialog.h"
#include "ui_listpublicationsdialog.h"

#include "core/library.h"

#include "nbib.h"
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/PIMO>

ListPublicationsDialog::ListPublicationsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ListPublicationsDialog)
{
    ui->setupUi(this);

    //create the query client to fetch resource data for the autocompletion
    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(addPublicationData(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(queryFinished()));
    connect(ui->listWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(showPublication(QListWidgetItem*)));
    connect(ui->checkBoxLibrary, SIGNAL(toggled(bool)), this, SLOT(fetchData()));

    fetchData();
}

ListPublicationsDialog::~ListPublicationsDialog()
{
    delete ui;
    m_queryClient->close();
    delete m_queryClient;
}

void ListPublicationsDialog::setLibrary(Library *p)
{
    m_library = p;
}

Nepomuk::Resource ListPublicationsDialog::selectedPublication()
{
    QUrl publicationURL = m_listCache.value( ui->listWidget->currentItem()->text() );

    return Nepomuk::Resource(publicationURL);
}

void ListPublicationsDialog::fetchData()
{
    m_listCache.clear();
    ui->listWidget->clear();
    Nepomuk::Query::AndTerm andTerm;
    andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Publication() ) );

    if(m_library && ui->checkBoxLibrary->isChecked()) {
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
                                                            Nepomuk::Query::ResourceTerm(m_library->pimoLibrary()) ) );
    }
    Nepomuk::Query::Query query( andTerm );
    m_queryClient->query(query);
}

void ListPublicationsDialog::addPublicationData(const QList< Nepomuk::Query::Result > &entries)
{
    foreach(const Nepomuk::Query::Result & e, entries) {
        QString title = e.resource().property(Nepomuk::Vocabulary::NIE::title()).toString();
        ui->listWidget->addItem(title);
        m_listCache.insert(title, e.resource().uri());
    }

}

void ListPublicationsDialog::queryFinished()
{
    m_queryClient->close();
}

void ListPublicationsDialog::showPublication( QListWidgetItem * item )
{
    Nepomuk::Resource publication = m_listCache.value(item->text());

    ui->entryTitle->setText(publication.property(Nepomuk::Vocabulary::NIE::title()).toString());

    QList<Nepomuk::Resource> authors = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();

    QString authorString;
    foreach(const Nepomuk::Resource & a, authors) {
        authorString.append(a.genericLabel());
        authorString.append(QLatin1String("; "));
    }
    authorString.chop(2);

    ui->entryAuthors->setText(authorString);

    Nepomuk::Resource type(publication.type());
    ui->entryType->setText(type.genericLabel());

    ui->entryDate->setText(publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString());

    ui->entryVolume->setText(publication.property(Nepomuk::Vocabulary::NBIB::volume()).toString());
}
