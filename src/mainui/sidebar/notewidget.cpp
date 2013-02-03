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

#include "notewidget.h"
#include "ui_notewidget.h"

#include "core/library.h"
#include "core/projectsettings.h"
#include "core/librarymanager.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <KDE/KJob>
#include "sro/pimo/note.h"

#include <Nepomuk2/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/NFO>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Variant>

#include <KDE/KDebug>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

NoteWidget::NoteWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::NoteWidget)
{
    ui->setupUi(this);

    ui->frameWidget->setEnabled(false);

    ui->editTags->setPropertyCardinality(PropertyEdit::MULTIPLE_PROPERTY);
    ui->editTags->setPropertyUrl( NAO::hasTag() );
    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(changeRating(int)));
}

NoteWidget::~NoteWidget()
{
    delete ui;
}

Nepomuk2::Resource NoteWidget::resource()
{
    return m_note;
}

void NoteWidget::setResource(Nepomuk2::Resource & resource)
{
    m_note = resource;

    if(!m_note.isValid()) {
        ui->frameWidget->setEnabled(false);
    }
    else {
        ui->frameWidget->setEnabled(true);
    }

    // discard simply shows the original content before saving any changes
    discardNote();

    ui->editRating->setRating((int)m_note.rating());
    ui->editTags->setResource(m_note);
}

void NoteWidget::newButtonClicked()
{
    // create temp Resource via DMS
    // if the user clicks cancel in the next dialog, the resource will be deleted again
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::PIMO::Note note;

    note.addType( NIE::InformationElement() );
    note.setProperty(NIE::title(), i18n("New Note"));

    graph << note;

    //blocking graph save
    Nepomuk2::StoreResourcesJob *srj = Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNone );
    if( !srj->exec() ) {
        kWarning() << "could not create temporay publication" << srj->errorString();
        return;
    }

    m_note = Nepomuk2::Resource::fromResourceUri( srj->mappings().value( note.uri() ) );

    // we add a dummy title and save the note
    ui->editTitle->setText(i18n("New note title"));
    saveNote();

    Library *curUsedLib = libraryManager()->currentUsedLibrary();
    if(curUsedLib && curUsedLib->libraryType() == BibGlobals::Library_Project) {
        curUsedLib->addResource( m_note );
    }

    setResource(m_note);
}

void NoteWidget::deleteButtonClicked()
{
    libraryManager()->systemLibrary()->deleteResource( m_note );

    Nepomuk2::Resource invalid;
    setResource(invalid);
}

void NoteWidget::saveNote()
{
    QList<QUrl> resUri; resUri << m_note.uri();
    QVariantList value; value << ui->editTitle->text();
    Nepomuk2::setProperty(resUri, NIE::title(), value);
    Nepomuk2::setProperty(resUri, NAO::prefLabel(), value);

    resUri.clear(); resUri << m_note.uri();
    value.clear(); value << ui->editContent->document()->toPlainText();
    Nepomuk2::setProperty(resUri, NIE::plainTextContent(), value);

    resUri.clear(); resUri << m_note.uri();
    value.clear(); value << ui->editContent->document()->toHtml();
    Nepomuk2::setProperty(resUri, NIE::htmlContent(), value);
}

void NoteWidget::discardNote()
{
    //show note content
    QString title;
    title = m_note.property(NIE::title()).toString();

    if(title.isEmpty()) {
        title = m_note.property( NAO::prefLabel()).toString();
    }

    ui->editTitle->setText(title);

    QString content = m_note.property(NIE::htmlContent()).toString();
    if(content.isEmpty()) {
        content = m_note.property(NIE::plainTextContent()).toString();
    }

    ui->editContent->document()->setHtml(content);
}

void NoteWidget::changeRating(int newRating)
{
    if(newRating == (int)m_note.rating() ) {
        return;
    }

    QList<QUrl> resourceUris;
    resourceUris << m_note.uri();
    QVariantList rating;
    rating <<  newRating;
    KJob *job = Nepomuk2::setProperty(resourceUris, Soprano::Vocabulary::NAO::numericRating(), rating);
    job->exec();
}
