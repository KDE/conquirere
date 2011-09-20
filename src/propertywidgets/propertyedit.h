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

#ifndef PROPERTYEDIT_H
#define PROPERTYEDIT_H

#include <Nepomuk/Resource>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>

#include <QWidget>
#include <QModelIndex>
#include <QHash>
#include <QUrl>

class QLabel;
class QLineEdit;
class QCompleter;
class QAbstractItemModel;

class PropertyEdit : public QWidget
{
    Q_OBJECT
public:
    explicit PropertyEdit(QWidget *parent = 0);
    virtual ~PropertyEdit();

    Nepomuk::Resource resource();
    QUrl propertyUrl();
    QString labelText();

public slots:
    void setResource(Nepomuk::Resource & resource);

    /**
      * Sets the property of the resource that should be changed
      */
    void setPropertyUrl(const QUrl & m_propertyUrl);

    void setLabelText(const QString & text);

protected:
    virtual void setupLabel() = 0;
    /**
      * Has to be reimplemented for any subclass
      */
    virtual void createCompletionModel( const QList< Nepomuk::Query::Result > &entries ) = 0;

    /**
      * update the resource with the @p text from the edit field
      */
    virtual void updateResource(const QString & text) = 0;

    void setCompletionModel(QAbstractItemModel *model);

    void addPropertryEntry(const QString &entryname,const QUrl & propertyUrl);
    QUrl propertyEntry(const QString &entryname);

private slots:
    void updateCompleter();
    void insertCompletion(const QModelIndex & index);
    void addCompletionData(const QList< Nepomuk::Query::Result > &entries);

    //needed because blockingquery has a bug
    void queryFinished();

private:
    void mousePressEvent ( QMouseEvent * event );
    void keyPressEvent(QKeyEvent * e);
    void editingFinished();
    void editingAborted();

    QLabel    *m_label;
    QLineEdit *m_lineEdit;

    Nepomuk::Resource m_resource;
    QUrl m_propertyUrl;

    Nepomuk::Query::QueryServiceClient *m_queryClient;

    QCompleter *m_completer;

    QHash<QString, QUrl> m_listCache;

    //local cache to overcome nepomuk bug with blockingquery
    QList< Nepomuk::Query::Result > resultCache;
};

#endif // PROPERTYEDIT_H
