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

#ifndef BIBTEXIMPORTWIZARD_H
#define BIBTEXIMPORTWIZARD_H

#include <QWizard>
#include <QtCore/QFutureWatcher>
#include <Akonadi/Collection>

class QLabel;
class KUrlRequester;
class QProgressBar;
class QLineEdit;
class File;
class BibTexToNepomukPipe;
class NBibImporterBibTex;
class QPushButton;
class KComboBox;
class Library;

namespace Ui {
    class BibTeXImportWizard;
}

class BibTeXImportWizard : public QWizard
{
    Q_OBJECT

public:
    explicit BibTeXImportWizard(QWidget *parent = 0);
    ~BibTeXImportWizard();

    /**
      * Set the system library to allow disabling live population
      *
      * Especially for the tag cloud generation this is really slow
      */
    void setSystemLibrary(Library *sl);
    Library *systemLibrary();

private:
    Ui::BibTeXImportWizard *ui;
    Library *m_sl;
};

/**
  * Intro page
  */
class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = 0);

private slots:
    bool isComplete() const;
    void collectionsReceived( const Akonadi::Collection::List& );

public:
    KComboBox *addressComboBox;
    KUrlRequester *fileName;
};

/**
  * Parse bibtex file via KBibTeX
  */
class ParseFile : public QWizardPage
{
    Q_OBJECT

public:
    ParseFile(QWidget *parent = 0);

    void initializePage();

private slots:
    bool isComplete() const;
    void importFinished();
    void cleanupPage();
    void showMergeDialog();

public:
    NBibImporterBibTex *importer;

private:
    QProgressBar *progressBar;
    QFutureWatcher<bool> *futureWatcher;
    QLabel *entryNumber;
    QLabel *authorNumber;
    QLabel *duplicateNumber;
    QPushButton *mergeButton;
};

/**
  * Parse bibtex file via KBibTeX
  */
class NepomukImport : public QWizardPage
{
    Q_OBJECT

public:
    NepomukImport(QWidget *parent = 0);

    void initializePage();

private slots:
    bool isComplete() const;
    void importFinished();

private:
    QProgressBar *progressBar;
    QFutureWatcher<void> *m_futureWatcher;
    BibTexToNepomukPipe *btnp;
};


#endif // BIBTEXIMPORTWIZARD_H
