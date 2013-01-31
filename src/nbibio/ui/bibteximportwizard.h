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

class QLabel;
class KUrlRequester;
class QProgressBar;
class QLineEdit;
class File;
class BibTexImporter;
class QPushButton;
class KComboBox;
class Library;
class LibraryManager;

namespace Ui {
    class BibTeXImportWizard;
}

class BibTeXImportWizard : public QWizard
{
    Q_OBJECT

public:
    explicit BibTeXImportWizard(QWidget *parent = 0);
    ~BibTeXImportWizard();

    void setLibraryManager(LibraryManager *lm);
    LibraryManager *libraryManager();

    void setImportLibrary(Library *l);
    Library *importLibrary();

    void setupUi();

private:
    Ui::BibTeXImportWizard *ui;
    LibraryManager *m_libraryManager;
    Library *m_importToLibrary;
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

public:
    void setupUi();
    KComboBox *fileType;
    KUrlRequester *fileName;
    KComboBox *projectImport;
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
    BibTexImporter *importer;

private:
    QProgressBar *progressBar;
    QFutureWatcher<bool> *futureWatcher;
    QLabel *entryNumber;
    QLabel *authorNumber;
    QLabel *duplicateNumber;
    QPushButton *mergeButton;
};

/**
  * Pipe BibTeX file to Nepomuk
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
};


#endif // BIBTEXIMPORTWIZARD_H
