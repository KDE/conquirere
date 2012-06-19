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

#ifndef DOCUMENTPREVIEW_H
#define DOCUMENTPREVIEW_H

#include <Nepomuk/Resource>
#include <QtGui/QWidget>

namespace Ui {
    class DocumentPreview;
}

namespace KParts {
    class Part;
}

class DocumentPreviewTab;

class DocumentPreview : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentPreview(QWidget *parent = 0);
    ~DocumentPreview();

signals:
    void activeDocumentChanged(Nepomuk::Resource & resource);

public slots:
    void setResource(Nepomuk::Resource & resource, bool inNewTab = false);
    void clear();

private slots:
    void closeRequest( QWidget * documentTab);
    void currentIndexChanged(int index);

signals:
    void activateKPart(KParts::Part *part);

private:
    Ui::DocumentPreview *ui;
};

#endif // DOCUMENTPREVIEW_H
