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

#ifndef LABELEDIT_H
#define LABELEDIT_H

#include <Nepomuk/Resource>

#include <QWidget>
#include <QUrl>

class QLabel;
class QLineEdit;

class LabelEdit : public QWidget
{
    Q_OBJECT
public:
    explicit LabelEdit(QWidget *parent = 0);
    ~LabelEdit();

    Nepomuk::Resource resource();

    void setPropertyUrl(const QUrl & m_propertyUrl);
    QUrl propertyUrl();

public slots:
    void setResource(Nepomuk::Resource & resource);

protected:
    void mousePressEvent ( QMouseEvent * e );
    void enterEvent ( QEvent * event );
    void leaveEvent ( QEvent * event );

private slots:
    void editingFinished();

private:
    QLabel    *m_label;
    QLineEdit *m_lineEdit;
    Nepomuk::Resource m_resource;
    QUrl m_propertyUrl;
};

#endif // LABELEDIT_H
