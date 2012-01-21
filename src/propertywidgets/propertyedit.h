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

#include <QtGui/QWidget>
#include <QtCore/QModelIndex>
#include <QtCore/QHash>
#include <QtCore/QUrl>
#include <QtCore/QFutureWatcher>

class KLineEdit;
class KSqueezedTextLabel;
class QCompleter;
class QAbstractItemModel;
class QFocusEvent;
class QToolButton;
class QStandardItemModel;
class QStandardItem;

/**
  * @brief Helper class to easily manipulate Nepomuk data
  *
  * The Property edit is an abstract base that can't be used directly.
  *
  * The idea is that subclasses simply have to implement
  * setupLabel()
  * createCompletionModel()
  * updateResource()
  *
  * This class offers than the possibility to show a simple QLabel
  * with the content of the property from @p propertyUrl() from the resource
  * @p resource()
  *
  * When the user clicks on the label, a QLineEdit field is shown instead.
  * The user can directly manipulate the data there. In addition nepomuk is
  * querried in the background to fill the content of a QCompleter object.
  *
  * The user will be offered a list of already available resources to select from.
  * Furthermore if @p hasMultipleCardinality() returns true the user can split each
  * new entry with a ";" and the completer ofers new selection from this start on
  *
  * @todo in the long run the QCompleter should be replaced by a direct query to nepomuk everytime we enter something
  *       like it is done with KRunner. When done, another way to get the nepomuk resource for a string entered (contact name for example)
  *       should be found, so we do not create duplicate entries when not the completer is used but the correct name is inserted
  *       directly.
  */
class PropertyEdit : public QWidget
{
    Q_OBJECT
public:
    /**
      * Defines if the widget can have more than 1 value. Multiple values will be split by an ;
      */
    enum Cardinality {
        UNIQUE_PROPERTY,
        MULTIPLE_PROPERTY
    };

    explicit PropertyEdit(QWidget *parent = 0);
    virtual ~PropertyEdit();

    /**
      * returns the resource that is being manipulated by this edit widget
      */
    Nepomuk::Resource resource();

    /**
      * returns a list of resources for all propertys inserted into the edit widget
      */
    QList<Nepomuk::Resource> propertyResources();

    /**
      * Returns the property url for this widget
      */
    QUrl propertyUrl();

    /**
      * returns the label text that is shown
      */
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

    /**
      * If @p useIt is true a small toolbutton is shown next to the QLabel
      * When pressed the externalEditRequested() signal is emitted that allows
      * to open a selection dialog.
      */
    void setUseDetailDialog(bool useIt);

    /**
      * If @p directEdit is false no editbox will be shown when the user clicks on the label.
      *
      * In this case setUseDetailDialog() will be set to true automatically and the vale can
      * only be changed externally.
      */
    void setDirectEdit(bool directEdit);

signals:
    /**
      * emitted when detailEditRequested() is called
      * can be used to implement your own dialog to change the property
      * 
      * when the resource was editied update the PropertyEdit via setResource()
      */
    void externalEditRequested(Nepomuk::Resource & resource, const QUrl & m_propertyUrl);

    /**
      * Emits the text of the label
      */
    void textChanged(const QString & newText);

    /**
      * Enable / Disable the widget.
      *
      * Used to set the setEnabled property of the QLabel/QEditlabel
      */
    void widgetEnabled(bool enabled);

    /**
      * Hide / Show the widget.
      *
      * Used to hide/show the setVisible property of the QLabel/QEditlabel
      */
    void widgetShown(bool shown);
    void widgetHidden(bool hidden);

    /**
      * This signal gets thrown when the resource was changed and must be updated in the table model cache
      *
      * @todo This should be replaced by the Nepomuk::ResourceWatcher later
      */
    void resourceCacheNeedsUpdate(Nepomuk::Resource resource);

public slots:
    /**
      * Sets the resource that should be shown/manipulated
      */
    void setResource(Nepomuk::Resource & resource);

    /**
      * Force the label to set certain text
      * Used by the subclasses to change the QLabel
      */
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

    void setVisible(bool visible);

    void setBulkUpdateInProgress(bool inprogress);

protected:
    /**
      * Defines how the Nepomuk::Resource of the widget should be shown.
      *
      * Subclasses must implement this. If the property has a rage of xsd:string a simple call
      * to setLabelText() is enough. If the propery has a specific resource on its own its possible to define
      * what values will be shown here. For example fullname() of a nco:Contact
      */
    virtual void setupLabel() = 0;

    /**
      * update the resource with the @p text from the edit field
      *
      * Must be implemented by any subclass. This defines how the text entered in the editbox will be used
      * to create the data for the property. If the range of the property is not xsd:string
      * this function allows to create a new resource type and se tthe proper data on it.
      * For example interprete the enteret text as fullname for a nco:Contact resource
      */
    virtual void updateResource(const QString & text) = 0;

    /**
      * Updates the ItemModel for the QCompleter based on the result @p entries from nepomuk
      */
    virtual void insertCompletionModel( const QList< Nepomuk::Query::Result > &entries, QStandardItemModel *completerModel);

    /**
      * Saves the Nepomuk::Resource URI for an entry displayed in the Label.
      *
      * Helps to easily lookup existing resources inserted by hand or via the QCompleter
      */
    void addPropertryEntry(const QString &entryname,const QUrl & propertyUrl);

    /**
      * Returns the cached resource uri of the string entry
      */
    QUrl propertyEntry(const QString &entryname);

private slots:
    void editingFinished();
    void editingAborted();

    void updateCompleter();
    void insertCompletion(const QModelIndex & index);

    /**
      * Fills the completer with the query results from the m_queryClient
      *
      * @see insertCompletionModel
      */
    void addCompletionData(const QList< Nepomuk::Query::Result > &entries);
    void completionModelProcessed();
    void startUpQueryFinished();

protected:
    KLineEdit *m_lineEdit;
    QCompleter *m_completer;

private:
    void mousePressEvent ( QMouseEvent * event );
    void keyPressEvent(QKeyEvent * e);

    KSqueezedTextLabel *m_label;
    QToolButton *m_detailView;
    bool m_isListEdit;
    bool m_useDetailDialog;
    bool m_directEditAllowed;

    Nepomuk::Resource m_resource;
    QUrl m_propertyUrl;
    QHash<QString, QUrl> m_listCache; /**< caches the label text with its nepomuk uri to easily retrieve the resource */

    QUrl m_range;
    Nepomuk::Query::QueryServiceClient *m_queryClient;

    // Nepomuk does not like when we "hammer" on a huge bunch of result querys for each single result in parallel via threads.
    // especially on startup this will sometimes crash with ***double free or incorrupt data*
    // Thus on startup and when we import large data sets, we enable the bulkupdate
    // this caches all single results returned from the queryclient and only if we finished (either when the queryclient throw the
    // finishedListing signal or if we say the import is done via setBulkUpdateInProgress(false) we progress all data at once
    bool m_bulkUpdateEnable;
    QList< Nepomuk::Query::Result > m_bulkCache;
};

#endif // PROPERTYEDIT_H
