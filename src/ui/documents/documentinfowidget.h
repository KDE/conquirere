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

#ifndef DOCUMENTINFOWIDGET_H
#define DOCUMENTINFOWIDGET_H

#include <QWidget>
#include <Nepomuk/Resource>

namespace Ui {
    class DocumentInfoWidget;
}

class KFileMetaDataWidget;
class QScrollArea;
class FileBibTexWidget;

class DocumentInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DocumentInfoWidget(QWidget *parent = 0);
    ~DocumentInfoWidget();

public slots:
    void setResource(Nepomuk::Resource &resource);
    void clear();

private:
    Ui::DocumentInfoWidget *ui;

    QScrollArea* m_metaDataArea;
    KFileMetaDataWidget *m_metaDataWidget;
    FileBibTexWidget * m_fileBibTexWidget;
};

#endif // DOCUMENTINFOWIDGET_H
