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

#include "bibtexexportdialog.h"
#include "ui_bibtexexportdialog.h"

#include "../libnbibio/nbibexporterbibtex.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/PIMO>

#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>

BibTexExportDialog::BibTexExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BibTexExportDialog)
{
    ui->setupUi(this);
}

BibTexExportDialog::~BibTexExportDialog()
{
    delete ui;
}

void BibTexExportDialog::accept()
{
    QList<Nepomuk::Resource> references;

    // fetcha data
    Nepomuk::Query::AndTerm andTerm;

    if(ui->onlyReferences->isChecked())
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Reference() ) );
    else
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Publication() ) );

    //if(ui->collection_Project->isChecked())
    //andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(), Nepomuk::Query::ResourceTerm(m_isRelatedTo) ) );

    Nepomuk::Query::Query query( andTerm );

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    foreach(Nepomuk::Query::Result r, queryResult) {
        references.append(r.resource());
    }

    qDebug() << "export " << references.size() << "to bibtex";

    //create file to export

    QString filename = ui->folder->text();
    filename.append(QLatin1String("/bibtex.bib"));

    QFile bibFile(filename);
    if (!bibFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "can't open file " << filename;
        return;
    }

    // start the export
    NBibExporterBibTex exporter;

    exporter.save(&bibFile, references);

    bibFile.close();

    close();
}
