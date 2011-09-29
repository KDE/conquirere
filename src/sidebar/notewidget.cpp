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

#include <QDebug>

NoteWidget::NoteWidget(QWidget *parent)
    : SidebarComponent(parent)
    , ui(new Ui::NoteWidget)
{
    ui->setupUi(this);
}

NoteWidget::~NoteWidget()
{
    delete ui;
}

void NoteWidget::setResource(Nepomuk::Resource & resource)
{
    m_note = resource;

    if(!m_note.isValid()) {
        //ui->tabWidget->setEnabled(false);
    }
    else {
        //ui->tabWidget->setEnabled(true);
    }

    emit resourceChanged(m_note);
}

void NoteWidget::newButtonClicked()
{
//    Nepomuk::Resource nb;
//    QList<QUrl> types;
//    types.append(Nepomuk::Vocabulary::NBIB::Publication());
//    nb.setTypes(types);

//    setResource(nb);
}

void NoteWidget::deleteButtonClicked()
{
    // link document to resource
//    m_publication.remove();

//    setResource(m_publication);
}
