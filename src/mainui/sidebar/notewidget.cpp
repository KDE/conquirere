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
#include "mainui/librarymanager.h"

#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Variant>

NoteWidget::NoteWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::NoteWidget)
{
    ui->setupUi(this);

    ui->frameWidget->setEnabled(false);

    ui->editTags->setPropertyCardinality(PropertyEdit::MULTIPLE_PROPERTY);
    ui->editTags->setPropertyUrl( Soprano::Vocabulary::NAO::hasTag() );
    connect(ui->editRating, SIGNAL(ratingChanged(int)), this, SLOT(changeRating(int)));

    //TODO remove and use ResourceWatcher later on
    connect(ui->editTags, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)), this, SIGNAL(resourceCacheNeedsUpdate(Nepomuk::Resource)));
}

NoteWidget::~NoteWidget()
{
    delete ui;
}

Nepomuk::Resource NoteWidget::note()
{
    return m_note;
}

void NoteWidget::setResource(Nepomuk::Resource & resource)
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

    ui->editRating->setRating(m_note.rating());
    ui->editTags->setResource(m_note);
}

void NoteWidget::newButtonClicked()
{
    m_note = Nepomuk::Resource(QUrl(), Nepomuk::Vocabulary::PIMO::Note());

    // we add a dummy title and save the note
    ui->editTitle->setText(i18n("New note title"));
    saveNote();

    Library *curUsedLib = libraryManager()->currentUsedLibrary();
    if(curUsedLib && curUsedLib->libraryType() == Library_Project) {
        //relate it to the project
        m_note.setProperty(Nepomuk::Vocabulary::PIMO::isRelated() , curUsedLib->settings()->projectThing());
    }

    setResource(m_note);
}

void NoteWidget::deleteButtonClicked()
{
    //check if we have a pimo:note, we should delete the note and the connected conntent resource
    Nepomuk::Resource nr = m_note.property(Nepomuk::Vocabulary::PIMO::groundingOccurrence()).toResource();
    nr.remove();

    m_note.remove();
    setResource(m_note);
}

void NoteWidget::saveNote()
{
    m_note.setProperty(Nepomuk::Vocabulary::NIE::title(), ui->editTitle->text());

    // these exist because of the Semnote implementation
    m_note.setProperty(Soprano::Vocabulary::NAO::prefLabel(), ui->editTitle->text());

    //now the content for Notably style directly to the PIMO:note
    m_note.setProperty(Nepomuk::Vocabulary::NIE::plainTextContent(), ui->editContent->document()->toPlainText());
    m_note.setProperty(Nepomuk::Vocabulary::NIE::htmlContent(), ui->editContent->document()->toHtml());

    emit resourceCacheNeedsUpdate(m_note);
}

void NoteWidget::discardNote()
{
    //show note content
    QString title;
    title = m_note.property(Nepomuk::Vocabulary::NIE::title()).toString();

    if(title.isEmpty())
        title = m_note.property(Soprano::Vocabulary::NAO::prefLabel()).toString();

    ui->editTitle->setText(title);

    QString content = m_note.property(Nepomuk::Vocabulary::NIE::htmlContent()).toString();

    ui->editContent->document()->setHtml(content);
}

void NoteWidget::changeRating(int newRating)
{
    if(newRating != m_note.rating()) {
        m_note.setRating(newRating);

        emit resourceCacheNeedsUpdate(m_note);
    }
}
