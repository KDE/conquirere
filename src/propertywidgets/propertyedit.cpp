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

#include "propertyedit.h"

#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Variant>

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>

#include <QDebug>

PropertyEdit::PropertyEdit(QWidget *parent) :
    QWidget(parent)
{
    m_label = new QLabel();
    m_label->setWordWrap(true);
    m_label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    m_label->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    m_lineEdit = new QLineEdit();
    m_lineEdit->hide();
    connect(m_lineEdit, SIGNAL(textEdited(QString)), this, SLOT(updateCompleter()));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_label);
    layout->addWidget(m_lineEdit);
    setLayout(layout);

    setMouseTracking(true);

    setMaximumHeight(m_lineEdit->size().height());

    //create the query client to fetch resource data for the autocompletion
    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(addCompletionData(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(queryFinished()));

    m_completer = new QCompleter(this);
    m_completer->setWidget(m_lineEdit);
    m_completer->setCaseSensitivity ( Qt::CaseInsensitive );
    connect(m_completer, SIGNAL(activated(QModelIndex)), this, SLOT(insertCompletion(QModelIndex)));
}

PropertyEdit::~PropertyEdit()
{
    delete m_label;
    delete m_lineEdit;
    delete m_queryClient;
    delete m_completer;
}

void PropertyEdit::setResource(Nepomuk::Resource & resource)
{
    m_resource = resource;

    // if we set the property before the resource
    // it is now time to set the label the first time
    if(m_propertyUrl.isValid()) {
        setupLabel();
    }
}

Nepomuk::Resource PropertyEdit::resource()
{
    return m_resource;
}

void PropertyEdit::setLabelText(const QString & text)
{
    m_label->setText(text );
}

QString PropertyEdit::labelText()
{
    return m_label->text();
}

void PropertyEdit::setPropertyUrl(const QUrl & propertyUrl)
{
    m_propertyUrl = propertyUrl;

    //get the range of the property (so what we are allowed to enter)
    Nepomuk::Resource nr(m_propertyUrl);
    Nepomuk::Resource range = nr.property(QLatin1String("http://www.w3.org/2000/01/rdf-schema#range")).toResource();

    if(range.isValid() && range.resourceUri().isValid()) {
        // get all resources of type range
        resultCache.clear();
        Nepomuk::Query::Query query( Nepomuk::Query::ResourceTypeTerm( range.resourceUri() ) );
        m_queryClient->blockingQuery(query);
    }
    // if we can't use the range for the check
    // get all entries for the propertyUrl that exist, maybe there is something in it we could reuse
    else {
        // get all resources of type range
        //Nepomuk::Query::Query query( Nepomuk::Query::ResourceTypeTerm( range.resourceUri() ) );
        //m_queryClient->blockingQuery(query);
    }

    // if we set the resource before the property
    // it is now time to set the label the first time
    if(m_resource.isValid()) {
        setupLabel();
    }
}

QUrl PropertyEdit::propertyUrl()
{
    return m_propertyUrl;
}

void PropertyEdit::mousePressEvent ( QMouseEvent * e )
{
    if(m_label->isVisible()) {
        m_lineEdit->setText(m_label->text());
        m_label->hide();
        m_lineEdit->show();
        m_lineEdit->setFocus();
    }
    else {
        if(m_label->text() != m_lineEdit->text()) {
            m_label->setText(m_lineEdit->text());
        }
        m_lineEdit->hide();
        m_label->show();
    }

    QWidget::mousePressEvent(e);
}

void PropertyEdit::setCompletionModel(QAbstractItemModel *model)
{
    m_completer->setModel(model);
}

void PropertyEdit::addPropertryEntry(const QString &entryname,const QUrl & propertyUrl)
{
    m_listCache.insert(entryname, propertyUrl);
}

QUrl PropertyEdit::propertyEntry(const QString &entryname)
{
    return m_listCache.value(entryname, QUrl());
}

void PropertyEdit::keyPressEvent(QKeyEvent * e)
{
    switch(e->key()){
    case Qt::Key_Escape:
        editingAborted();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        editingFinished();
        break;
    case Qt::Key_Left:
    case Qt::Key_Right:
        updateCompleter();
        break;
    default:
        break;
    }
}

void PropertyEdit::updateCompleter()
{
    QAbstractItemView* completionView = m_completer->popup();

    //we take full string and split it at delimiter
    QStringList entrylist = m_lineEdit->text().split(QLatin1String(";"));

    // get the position of the cursor
    int pos = m_lineEdit->cursorPosition();

    int tempLength = 0;
    int substringPos;
    QString subString;
    // find the substring that is beeing manipulated
    foreach(QString s, entrylist) {
        tempLength += s.length();
        if(tempLength >= pos-1) {
            subString = s;
            if(tempLength != pos) {
                substringPos = tempLength - pos;
            }
            else {
                substringPos = tempLength;
            }
            break;
        }
    }

    //remove any string after text position
    subString = subString.left(substringPos).trimmed();

    //ok we have the string that needs autocompletion
    if(!subString.isEmpty()) {
        m_completer->setCompletionPrefix(subString);
        m_completer->complete();
    } else {
        completionView->hide();
    }
}

void PropertyEdit::insertCompletion(const QModelIndex & index)
{
    // get current list
    QStringList entrylist = m_lineEdit->text().split(QLatin1String(";"));
    QString exitString;

    // get the position of the cursor
    int pos = m_lineEdit->cursorPosition();

    int tempLength = 0;
    // find the substring that is beeing manipulated and replace it with the selected suggestion
    bool inserted = false;
    foreach(QString s, entrylist) {
        tempLength += s.length();
        if( !inserted && tempLength >= pos-1) {
            exitString.append(index.data().toString());

            addPropertryEntry(index.data().toString(), index.data(Qt::UserRole + 1).toString());
            inserted = true;
        }
        else {
            exitString.append(s);
        }

        exitString.append(QLatin1String("; "));
    }

    exitString.chop(2);

    m_lineEdit->setText(exitString);
}

void PropertyEdit::editingFinished()
{
    if(m_label->text() != m_lineEdit->text()) {
        updateResource(m_lineEdit->text());
        m_label->setText(m_lineEdit->text());
    }

    m_lineEdit->hide();
    m_label->show();
}

void PropertyEdit::editingAborted()
{
    m_lineEdit->hide();
    m_label->show();
}

void PropertyEdit::addCompletionData(const QList< Nepomuk::Query::Result > &entries)
{
    if(!entries.isEmpty()) {
        resultCache.append(entries);
    }
}

void PropertyEdit::queryFinished()
{
    createCompletionModel(resultCache);
}
