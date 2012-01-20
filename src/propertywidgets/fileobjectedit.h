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

#ifndef FILEOBJECTEDIT_H
#define FILEOBJECTEDIT_H

#include "propertyedit.h"

class QStandardItemModel;

/**
  * @brief Used to add NFO::FileDataObject(), NFO::RemoteDataObject() or NFO::WebsiteDataObject() to the publication
  *
  * This is responsible for the connection to the localfiles or files hosted at a remote
  * place for the publication. Also the url part is handled via NFO::WebsiteDataObject()
  */
class FileObjectEdit : public PropertyEdit
{
    Q_OBJECT
public:
    explicit FileObjectEdit(QWidget *parent = 0);

    enum Mode {
        Local,   /**< add NFO::FileDataObject() */
        Remote   /**< add NFO::RemoteDataObject() */,
        Website  /**< add NFO::WebsiteDataObject() */
    };

    /**
      * Sets teh mode for this widget
      */
    void setMode(Mode mode);

protected:
    /**
      * shows a list of all used data objects split by ;
      */
    void setupLabel();

    /**
      * completion is ignored for this widget, as direct edit is not allowed either
      */
    virtual QList<QStandardItem*> createCompletionModel( const QList< Nepomuk::Query::Result > &entries );

    /**
      * is done externally via AddDataObject
      */
    virtual void updateResource( const QString & text );

private slots:
    /**
      * opens the file selection dialog AddDataObject
      */
    void showFileSelection();

private:
    Mode m_mode;
};

#endif // FILEOBJECTEDIT_H
