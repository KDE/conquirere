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

#include "exportsettings.h"
#include "../build/src/mainui/ui_exportsettings.h"

#include "nbibio/conquirere.h"

#include <kbibtex/iconvlatex.h>

// from kbibtex
#define createDelimiterString(a, b) (QString("%1%2%3").arg(a).arg(QChar(8230)).arg(b))

ExportSettings::ExportSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ExportSettings)
{
    ui->setupUi(this);

    setupGui();
    resetSettings();

    connect(ui->cbProtectCasing, SIGNAL(toggled(bool)), this, SIGNAL(contentChanged()));
    connect(ui->editEncoding, SIGNAL(currentIndexChanged(int)), this, SIGNAL(contentChanged()));
    connect(ui->editDelimiters, SIGNAL(currentIndexChanged(int)), this, SIGNAL(contentChanged()));
    connect(ui->editQuoting, SIGNAL(currentIndexChanged(int)), this, SIGNAL(contentChanged()));
    connect(ui->editCasing, SIGNAL(currentIndexChanged(int)), this, SIGNAL(contentChanged()));
    connect(ui->editPersonFormating, SIGNAL(currentIndexChanged(int)), this, SIGNAL(contentChanged()));
    connect(ui->editPaperSize, SIGNAL(currentIndexChanged(int)), this, SIGNAL(contentChanged()));
    connect(ui->editBabelLang, SIGNAL(currentIndexChanged(int)), this, SIGNAL(contentChanged()));
    connect(ui->editBibStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(contentChanged()));
}

ExportSettings::~ExportSettings()
{
    delete ui;
}

void ExportSettings::resetSettings()
{
    ui->editRefCommand->setCurrentIndex(ConqSettings::referenceCommand());
    ui->editPipe->setText(ConqSettings::kileLyXPipe());

    ui->cbProtectCasing->setChecked(ConqSettings::protectCasing());
    ui->editEncoding->setCurrentIndex(ConqSettings::encoding());
    ui->editDelimiters->setCurrentIndex(ConqSettings::stringDelimiters());
    ui->editQuoting->setCurrentIndex(ConqSettings::commentQuoting());
    ui->editCasing->setCurrentIndex(ConqSettings::keywordCasing());
    ui->editPersonFormating->setCurrentIndex(ConqSettings::personNameFormatting());

    ui->editPaperSize->setCurrentIndex(ConqSettings::paperSize());

    int selectedRow = ui->editBibStyle->findText(ConqSettings::bibStyle());
    if(selectedRow == -1) {
        ui->editBibStyle->setEditText(ConqSettings::bibStyle());
    }
    else {
        ui->editBibStyle->setCurrentIndex(selectedRow);
    }

    selectedRow = ui->editBibStyle->findText(ConqSettings::babelLang());
    if(selectedRow == -1) {
        ui->editBabelLang->setEditText(ConqSettings::babelLang());
    }
    else {
        ui->editBabelLang->setCurrentIndex(selectedRow);
    }
}

void ExportSettings::applySettings()
{
    ConqSettings::setReferenceCommand(ui->editRefCommand->currentIndex());
    ConqSettings::setKileLyXPipe(ui->editPipe->text());

    ConqSettings::setProtectCasing(ui->cbProtectCasing->isChecked());
    ConqSettings::setEncoding(ui->editEncoding->currentIndex());
    ConqSettings::setStringDelimiters(ui->editDelimiters->currentIndex());
    ConqSettings::setCommentQuoting(ui->editQuoting->currentIndex());
    ConqSettings::setKeywordCasing(ui->editCasing->currentIndex());
    ConqSettings::setPersonNameFormatting(ui->editPersonFormating->currentIndex());

    ConqSettings::setPaperSize(ui->editPaperSize->currentIndex());
    ConqSettings::setBabelLang(ui->editBabelLang->currentText());
    ConqSettings::setBibStyle(ui->editBibStyle->currentText());

    ConqSettings::self()->writeConfig();
}

void ExportSettings::setupGui()
{
    ui->editRefCommand->addItem(QLatin1String("\\cite{") + QChar(0x2026) + QChar('}'));
    ui->editRefCommand->addItem(QLatin1String("\\citealt{") + QChar(0x2026) + QChar('}'));
    ui->editRefCommand->addItem(QLatin1String("\\citeauthor{") + QChar(0x2026) + QChar('}'));
    ui->editRefCommand->addItem(QLatin1String("\\citeauthor*{") + QChar(0x2026) + QChar('}'));
    ui->editRefCommand->addItem(QLatin1String("\\citeyear{") + QChar(0x2026) + QChar('}'));
    ui->editRefCommand->addItem(QLatin1String("\\citeyearpar{") + QChar(0x2026) + QChar('}'));
    ui->editRefCommand->addItem(QLatin1String("\\shortcite{") + QChar(0x2026) + QChar('}'));
    ui->editRefCommand->addItem(QLatin1String("\\citet{") + QChar(0x2026) + QChar('}'));
    ui->editRefCommand->addItem(QLatin1String("\\citet*{") + QChar(0x2026) + QChar('}'));
    ui->editRefCommand->addItem(QLatin1String("\\citep{") + QChar(0x2026) + QChar('}'));
    ui->editRefCommand->addItem(QLatin1String("\\citep*{") + QChar(0x2026) + QChar('}'));

    // all from kbibtex
    ui->editEncoding->addItem(QLatin1String("LaTeX"));
    ui->editEncoding->insertSeparator(1);
    ui->editEncoding->addItems(IConvLaTeX::encodings());

    ui->editDelimiters->addItem(createDelimiterString('"', '"'));
    ui->editDelimiters->addItem(createDelimiterString('{', '}'));
    ui->editDelimiters->addItem(createDelimiterString('(', ')'));

    ui->editQuoting->addItem(i18n("None"));
    ui->editQuoting->addItem(i18n("@comment{%1}", QChar(8230)));
    ui->editQuoting->addItem(i18n("%{%1}", QChar(8230)));

    ui->editCasing->addItem(i18n("lowercase"));
    ui->editCasing->addItem(i18n("Initial capital"));
    ui->editCasing->addItem(i18n("UpperCamelCase"));
    ui->editCasing->addItem(i18n("lowerCamelCase"));
    ui->editCasing->addItem(i18n("UPPERCASE"));

    ui->editPersonFormating->addItem(i18n("John Doe Jr."));
    ui->editPersonFormating->addItem(i18n("Doe, Jr., John"));


}
