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

#include "welcomewidget.h"
#include "ui_welcomewidget.h"

#include "../core/library.h"

WelcomeWidget::WelcomeWidget(Library *library, QWidget *parent)
    : QWidget(parent)
    , m_library(library)
    , ui(new Ui::WelcomeWidget)
{
    ui->setupUi(this);
    setupGui();
}

WelcomeWidget::~WelcomeWidget()
{
    delete ui;
}

void WelcomeWidget::setupGui()
{
    if(m_library->libraryType() == Library_System) {
        ui->labelTitle->setText(i18n("System Library"));
    }
    else {
        QString libraryHeader = i18n("Library:");
        libraryHeader.append(QLatin1String(" "));
        libraryHeader.append(m_library->name());
        ui->labelTitle->setText(libraryHeader);
    }
}
