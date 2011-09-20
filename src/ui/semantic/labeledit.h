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
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>

#include <QWidget>
#include <QUrl>

class QLabel;
class KLineEdit;

class StringEdit : public QWidget
{
    Q_OBJECT
public:
    explicit StringEdit(QWidget *parent = 0);
    virtual ~StringEdit();

    Nepomuk::Resource resource();

    void setPropertyUrl(const QUrl & m_propertyUrl);
    QUrl propertyUrl();

    void setLabelText(const QString & text);
    QString getLabelText();

signals:
    void resourceNeedsUpdate(const QString & text);
    void labelNeedsUpdate();

public slots:
    void setResource(Nepomuk::Resource & resource);

    /**
      * Called when the user edited the contend.
      *
      * This function has to be inplemented if the range of the property url
      * is not a simple string but rather a resource on its own.
      *
      * In this function a new Resource (for example nco:Contact) could be created
      * The @p text added to it (as nco:fullname for example) and than the
      * new Resource inserted into the property of the resource
    */
    virtual void updateResource(const QString & text);

    /**
      * Called when the user edited the contend
      *
      * This allows reimplemented functions to change the appereance of the property content
      *
      * For example split the list of authors with fullname by a ";" and display then all of them
      *
      * You have to call setLabelText() at the end of the function to display your results
      */
    virtual void updateLabel();


protected:
    void mousePressEvent ( QMouseEvent * e );

private slots:
    void editingFinished();

    /**
      * @bug don't just add genericLabel() for autocompletion offer a way to map the completed string back to the nepomuk:/res uri
      */
    void addCompletionData(const QList< Nepomuk::Query::Result > &entries);

private:
    QLabel    *m_label;
    KLineEdit *m_lineEdit;

    Nepomuk::Resource m_resource;
    QUrl m_propertyUrl;


    Nepomuk::Query::QueryServiceClient *m_queryClient;
};

#endif // LABELEDIT_H
