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

#include <Nepomuk2/Resource>

#include <QtGui/QWidget>
#include <QtCore/QUrl>

class KMultiItemEdit;
class KSqueezedTextLabel;
class QToolButton;
class QMouseEvent;
class QKeyEvent;
class KJob;

/**
  * @brief Helper class to easily manipulate Nepomuk data
  *
  * The Property edit is an abstract base that can't be used directly.
  *
  * The idea is that subclasses simply have to implement
  * setupLabel()
  * updateResource()
  *
  * This class offers than the possibility to show a simple QLabel
  * with the content of the property from @p propertyUrl() of the resource @p resource()
  *
  * When the user clicks on the label, a QLineEdit field is shown instead.
  * The user can directly manipulate the data there. In addition nepomuk is
  * querried with a simple sparql search to alow autocompletions
  *
  * Only the name as one would enter directly into the lineedit is shown and will be
  * inserted on selection. This could be the @c nco:fullname for contacts or @c nie:title for other
  * resources. When the user saves his choices a new resource is created for this selecten and based
  * on same name/resource type merged thanks to the nepomuk DMS or the current resource is updated.
  *
  * Furthermore if @p hasMultipleCardinality() returns true the user can split each
  * new entry with a @c ";" and the completer ofers new selection from this start on
  *
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
    Nepomuk2::Resource resource();

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
      * This is single by default.
      * The default delimiter is ';'.
      */
    void setPropertyCardinality(PropertyEdit::Cardinality cardinality);
    bool hasMultipleCardinality();

    /**
      * Sets the property of the resource that should be changed
      */
    virtual void setPropertyUrl(const QUrl & m_propertyUrl);

    /**
      * If @p useIt is true a small toolbutton is shown next to the QLabel
      * When pressed the externalEditRequested() signal is emitted that allows
      * to open a selection dialog.
      */
    void setUseDetailDialog(bool useIt);

    /**
      * If @p directEdit is false no editbox will be shown when the user clicks on the label.
      *
      * In this case setUseDetailDialog() will be set to true automatically and the value can
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
    void externalEditRequested(Nepomuk2::Resource & resource, const QUrl & m_propertyUrl);

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
      * Used to hide/show the setVisible property of the QLabel and QEditlabel
      */
    void widgetShown(bool shown);
    void widgetHidden(bool hidden);

public slots:
    /**
      * Sets the resource that should be shown/manipulated
      */
    void setResource(Nepomuk2::Resource & resource);

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

protected:
    /**
      * Defines how the Nepomuk2::Resource of the widget should be shown.
      *
      * Subclasses must implement this. If the property has a rage of xsd:string a simple call
      * to setLabelText() is enough. If the propery has a specific resource on its own its possible to define
      * what values will be shown here. For example @c nco:fullname() of a @c nco:Contact
      */
    virtual void setupLabel() = 0;

    /**
      * update the resource with the @p text from the edit field
      *
      * Must be implemented by any subclass. This defines how the text entered in the editbox will be used
      * to create the data for the property. If the range of the property is not @c xsd:string
      * this function allows to create a new resource type and set the proper data on it.
      * For example interprete the enteret text as @c fullname for a @c nco:Contact resource
      */
    virtual void updateResource(const QString & text) = 0;

protected slots:
    virtual void editingFinished();
    virtual void editingAborted();

    /**
     * @brief Prints the errorStrong of a failed DMS KJob execution to kDebug()
     */
    void showDMSError(KJob *job);

protected:
    virtual void mousePressEvent ( QMouseEvent * event );
    KSqueezedTextLabel *m_label;
    KMultiItemEdit *m_lineEdit;

private:
    void keyPressEvent(QKeyEvent * e);

    QToolButton *m_detailView;
    bool m_isListEdit;
    bool m_useDetailDialog;
    bool m_directEditAllowed;

    Nepomuk2::Resource m_resource;
    QUrl m_propertyUrl;
};

#endif // PROPERTYEDIT_H
