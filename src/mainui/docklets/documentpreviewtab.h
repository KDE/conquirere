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

#ifndef DOCUMENTPREVIEWTAB_H
#define DOCUMENTPREVIEWTAB_H

#include <Nepomuk2/Resource>
#include <KParts/MainWindow>
#include <KParts/BrowserExtension>

#include <QtGui/QWidget>

namespace Ui {
    class DocumentPreviewTab;
}

namespace KParts {
    class Part;
    class ReadOnlyPart;
}
class QLabel;

/**
  * @brief Dockwidget to show the right KPart for a specific publication/document url
  *
  */
class DocumentPreviewTab : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentPreviewTab(QWidget *parent = 0);
    ~DocumentPreviewTab();

    Nepomuk2::Resource & resource();

public slots:
    void setResource(Nepomuk2::Resource & resource);
    void clear();
    void showUrl(int index, bool hidden = false);
    void openExternally();
    void setActive();

signals:
    void activateKPart(KParts::Part *part);

private slots:
    void openUrlRequestDelayed (const KUrl &url, const KParts::OpenUrlArguments &arguments, const KParts::BrowserArguments &browserArguments);

protected:
    void resizeEvent ( QResizeEvent * event );

private:
    Ui::DocumentPreviewTab *ui;
    Nepomuk2::Resource m_resource;
    KParts::ReadOnlyPart* m_part;
    QString m_lastPartsName;
    QLabel *m_labelInvalid;
    QLabel *m_labelNone;
    bool m_visible;
};

#endif // DOCUMENTPREVIEWTAB_H
