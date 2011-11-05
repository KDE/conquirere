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

#ifndef ADDDATAOBJECT_H
#define ADDDATAOBJECT_H

#include <QDialog>
#include <QUrl>

#include "fileobjectedit.h"

namespace Ui {
    class AddDataObject;
}

class KUrlRequester;

/**
  * @bug use Nepomuk::Vocabulary::NFO::WebDataObject() instead of Nepomuk::Vocabulary::NFO::Website() as soon as an updated soprano ontology is available
  */
class AddDataObject : public QDialog
{
    Q_OBJECT

public:
    explicit AddDataObject(QWidget *parent = 0);
    ~AddDataObject();

    void setMode(FileObjectEdit::Mode mode);
    void setResource(Nepomuk::Resource resource);
    void fillListWidget();

private slots:
    void addItem(const QString & text);
    void removeItem(const QString & text);

private:
    Ui::AddDataObject *ui;

    FileObjectEdit::Mode m_mode;
    Nepomuk::Resource m_resource;
    QUrl m_propertyUrl;
    KUrlRequester * m_kurlrequester;
};

#endif // ADDDATAOBJECT_H

