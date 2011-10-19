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

#ifndef DOCUMENTPREVIEW_H
#define DOCUMENTPREVIEW_H

#include <QDockWidget>

#include <Nepomuk/Resource>
#include <kparts/mainwindow.h>

namespace Ui {
    class DocumentPreview;
}

namespace KParts {
    class Part;
    class ReadOnlyPart;
}
class QLabel;

class DocumentPreview : public QDockWidget
{
    Q_OBJECT

public:
    explicit DocumentPreview(QWidget *parent = 0);
    ~DocumentPreview();

public slots:
    void setResource(Nepomuk::Resource & resource);
    void showUrl(int index);
    void openExternally();
    void toggled(bool status);

signals:
    void activateKPart(KParts::Part *part);

protected:
    void changeEvent ( QEvent * event );

private:
    Ui::DocumentPreview *ui;
    Nepomuk::Resource m_resource;
    KParts::ReadOnlyPart* m_part;
    QLabel *m_labelInvalid;
    QLabel *m_labelNone;
};

#endif // DOCUMENTPREVIEW_H
