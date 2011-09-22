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
class QFocusEvent;
class QToolButton;

class PropertyEdit : public QWidget
{
    Q_OBJECT
public:
    enum Cardinality {
        UNIQUE_PROPERTY,
        MULTIPLE_PROPERTY
    };

    explicit PropertyEdit(QWidget *parent = 0);
    virtual ~PropertyEdit();

    /**
      * returns the resource that is beeing manipulated by this edit widget
      */
    Nepomuk::Resource resource();

    /**
      * returns a list of resources for all propertys inserted into the edit widget
      */
    QList<Nepomuk::Resource> propertyResources();
    QUrl propertyUrl();
    QString labelText();

    /**
      * Defines if the values are one single entry or various.
      *
      * This is multiple by default.
      * The default delimiter is ';'.
      */
    void setPropertyCardinality(PropertyEdit::Cardinality cardinality);
    bool hasMultipleCardinality();

    /**
      * Sets the property of the resource that should be changed
      */
    void setPropertyUrl(const QUrl & m_propertyUrl);

    void setUseDetailDialog(bool useIt);

signals:
    /**
      * emitted when detailEditRequested() is called
      * can be used to implement your own dialog t ochange the property
      * 
      * when the resource was editied update the PropertyEdit via setResource
      */
    void externalEditRequested(Nepomuk::Resource & resource, const QUrl & m_propertyUrl);

    // emits the text of the label
    void textChanged(const QString & newText);

    void widgetEnabled(bool enabled);

public slots:
    void setResource(Nepomuk::Resource & resource);

    void setLabelText(const QString & text);

    /**
      * When someone clicks on the detail button next to the editline
      *
      * Used to open a dialog where the property (Contact for example)
      * can be specified in greater detail
      *
      * calls externalEditRequested()
      */
    virtual void detailEditRequested();

    /**
      * When the resource is changed externally
      * For example by an externalEditRequested() Dialog
      */
    void resourceUpdatedExternally();

protected:
    virtual void setupLabel() = 0;

    /**
      * Has to be reimplemented for any subclass
      *
      * Creates the ItemModel for the QCompleter based on the result @p entries from nepomuk
      *
      * @see setCompletionModel();
      */
    virtual void createCompletionModel( const QList< Nepomuk::Query::Result > &entries ) = 0;

    /**
      * update the resource with the @p text from the edit field
      */
    virtual void updateResource(const QString & text) = 0;

    void setCompletionModel(QAbstractItemModel *model);

    void addPropertryEntry(const QString &entryname,const QUrl & propertyUrl);
    QUrl propertyEntry(const QString &entryname);

    void changeEvent( QEvent * event );

private slots:
    void updateCompleter();
    void insertCompletion(const QModelIndex & index);
    void addCompletionData(const QList< Nepomuk::Query::Result > &entries);

    // called when all values have been announced in addCompletionData
    // quits the queryservice and calls createCompletionModel()
    void queryFinished();

    void editingFinished();
    void editingAborted();

private:
    void mousePressEvent ( QMouseEvent * event );
    void keyPressEvent(QKeyEvent * e);

    QLabel    *m_label;
    QLineEdit *m_lineEdit;
    QToolButton *m_detailView;
    bool m_isListEdit;
    bool m_useDetailDialog;

    Nepomuk::Resource m_resource;
    QUrl m_propertyUrl;

    Nepomuk::Query::QueryServiceClient *m_queryClient;

    QCompleter *m_completer;

    QHash<QString, QUrl> m_listCache;

    //local cache to overcome nepomuk bug with blockingquery
    QList< Nepomuk::Query::Result > resultCache;
};

#endif // PROPERTYEDIT_H
