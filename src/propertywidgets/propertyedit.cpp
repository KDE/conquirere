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

#include <KDE/KIcon>
#include <KDE/KSqueezedTextLabel>
#include <KDE/KLineEdit>

#include <QtGui/QLabel>
#include <QtGui/QToolButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QCompleter>
#include <QtGui/QKeyEvent>
#include <QtGui/QAbstractItemView>
#include <QtCore/QThread>
#include <QtCore/QtConcurrentRun>
#include <QtGui/QStandardItemModel>

#include <QtCore/QDebug>

PropertyEdit::PropertyEdit(QWidget *parent)
    : QWidget(parent)
    , m_isListEdit(true)
    , m_useDetailDialog(false)
    , m_directEditAllowed(true)
    , m_initialQueryFinished(false)
{
    m_label = new KSqueezedTextLabel();
    m_label->setWordWrap(true);
    m_label->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    m_label->setTextElideMode(Qt::ElideMiddle);

    m_lineEdit = new KLineEdit();
    m_lineEdit->hide();
    connect(m_lineEdit, SIGNAL(textEdited(QString)), this, SLOT(updateCompleter()));
    connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));

    m_detailView = new QToolButton();
    m_detailView->setIcon(KIcon(QLatin1String("document-edit-verify")));
    connect(m_detailView, SIGNAL(clicked()), this, SLOT(detailEditRequested()));

    QVBoxLayout *hlayout = new QVBoxLayout;
    hlayout->addWidget(m_label);
    hlayout->addWidget(m_lineEdit);

    QHBoxLayout *vlayout = new QHBoxLayout;
    vlayout->addLayout(hlayout);
    vlayout->addWidget(m_detailView);

    setLayout(vlayout);

    setMouseTracking(true);

    setMaximumHeight(m_lineEdit->size().height());

    //create the query client to fetch resource data for the autocompletion
    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(addCompletionData(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(initialQueryFinished()));

    m_completer = new QCompleter(this);
    m_completer->setWidget(m_lineEdit);
    m_completer->setCaseSensitivity ( Qt::CaseInsensitive );
    connect(m_completer, SIGNAL(activated(QModelIndex)), this, SLOT(insertCompletion(QModelIndex)));

    setUseDetailDialog(false);
}

PropertyEdit::~PropertyEdit()
{
    m_queryClient->close();

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

    emit textChanged(text);
}

QString PropertyEdit::labelText()
{
    return m_label->fullText();
}

void PropertyEdit::setPropertyCardinality(PropertyEdit::Cardinality cardinality)
{
    if(cardinality == PropertyEdit::MULTIPLE_PROPERTY) {
        m_isListEdit = true;
    }
    else {
        m_isListEdit = false;
    }
}

bool PropertyEdit::hasMultipleCardinality()
{
    return m_isListEdit;
}

void PropertyEdit::setUseDetailDialog(bool useIt)
{
    m_useDetailDialog = useIt;

    if(m_useDetailDialog) {
        m_detailView->show();
    }
    else {
        m_detailView->hide();
    }
}

void PropertyEdit::setDirectEdit(bool directEdit)
{
    m_directEditAllowed = directEdit;
    if(!m_directEditAllowed)
        setUseDetailDialog(true);
}

void PropertyEdit::setPropertyUrl(const QUrl & propertyUrl)
{
    m_propertyUrl = propertyUrl;

    //get the range of the property (so what we are allowed to enter)
    Nepomuk::Resource nr(m_propertyUrl);
    Nepomuk::Resource range = nr.property(QUrl(QLatin1String("http://www.w3.org/2000/01/rdf-schema#range"))).toResource();

    if(range.isValid() && !range.uri().isEmpty()) {
        // get all resources of type range
        Nepomuk::Query::Query query( Nepomuk::Query::ResourceTypeTerm( QUrl(range.uri()) ) );
        m_queryClient->query(query);
    }
    // if we can't use the range for the check
    // get all entries for the propertyUrl that exist, maybe there is something in it we could reuse
    else {
        // get all resources of type range
        //Nepomuk::Query::Query query( Nepomuk::Query::ResourceTypeTerm( range.resourceUri() ) );
        //m_queryClient->query(query);
    }

    // if we set the resource before the property
    // it is now time to set the label the first time
    if(m_resource.isValid()) {
        setupLabel();
    }
}

QList<Nepomuk::Resource> PropertyEdit::propertyResources()
{
    return resource().property(propertyUrl()).toResourceList();
}

QUrl PropertyEdit::propertyUrl()
{
    return m_propertyUrl;
}

void PropertyEdit::mousePressEvent ( QMouseEvent * e )
{
    if(m_directEditAllowed) {
        if(m_label->isVisible()) {
            m_lineEdit->setText(m_label->fullText());
            m_label->hide();
            m_lineEdit->show();
            m_lineEdit->setFocus();
        }
        else {
            if(m_label->fullText() != m_lineEdit->text()) {
                setLabelText(m_lineEdit->text());
            }
            m_lineEdit->hide();
            m_label->show();
        }
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

void PropertyEdit::changeEvent( QEvent * event )
{
    if(event->type() == QEvent::EnabledChange)
    {
        emit widgetEnabled(isEnabled());
    }
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
        //inserting the completion via Enter is done by signal activated()
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

    QString subString;

    if(m_isListEdit) {
        //we take full string and split it at delimiter
        QStringList entrylist = m_lineEdit->text().split(QLatin1String(";"));

        // get the position of the cursor
        int pos = m_lineEdit->cursorPosition();

        int tempLength = 0;
        int substringPos;
        // find the substring that is being manipulated
        foreach(const QString & s, entrylist) {
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
            tempLength++; //adds the ";" to the length
        }

        //remove any string after text position
        subString = subString.left(substringPos).trimmed();
    }
    else {
        subString = m_lineEdit->text();
    }

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
    QString exitString;
    // get current list
    if(m_isListEdit) {
        QStringList entrylist = m_lineEdit->text().split(QLatin1String(";"));

        // get the position of the cursor
        int pos = m_lineEdit->cursorPosition();

        int tempLength = 0;
        // find the substring that is being manipulated and replace it with the selected suggestion
        bool inserted = false;
        foreach(const QString & s, entrylist) {
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
    }
    else {
        exitString = index.data().toString();
        addPropertryEntry(index.data().toString(), index.data(Qt::UserRole + 1).toString());
    }

    m_lineEdit->setText(exitString);
}

void PropertyEdit::editingFinished()
{
    //don't switch to label view when enter is pressed for the completion
    if(m_completer->popup()->isVisible() )
        return;

    if(m_label->fullText() != m_lineEdit->text()) {
        QString inputString = QLatin1String(m_lineEdit->text().toUtf8());
        updateResource(inputString);
        setLabelText(m_lineEdit->text());

        emit resourceUpdated(resource());
    }

    m_lineEdit->hide();
    m_label->show();
}

void PropertyEdit::editingAborted()
{
    m_lineEdit->hide();
    m_label->show();
}

void PropertyEdit::detailEditRequested()
{
    emit externalEditRequested(m_resource, m_propertyUrl);
}

void PropertyEdit::resourceUpdatedExternally()
{
    setupLabel();
    editingFinished();
}

void PropertyEdit::addCompletionData(const QList< Nepomuk::Query::Result > &entries)
{
    if(m_initialQueryFinished) {
        m_initialCompleterCache.append(entries);
        // start background thread the data
        QFuture<QStandardItemModel*> future = QtConcurrent::run(this, &PropertyEdit::createCompletionModel, m_initialCompleterCache);

        m_futureWatcher = new QFutureWatcher<QStandardItemModel*>();
        m_futureWatcher->setFuture(future);
        connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(completionModelProcessed()));
    }
    else {
        m_initialCompleterCache.append(entries);
    }
}

void PropertyEdit::initialQueryFinished()
{
    m_initialQueryFinished = true;
    // don't close the query client, this allows to update the completer

    // start background thread the data
    QFuture<QStandardItemModel*> future = QtConcurrent::run(this, &PropertyEdit::createCompletionModel, m_initialCompleterCache);

    m_futureWatcher = new QFutureWatcher<QStandardItemModel*>();
    m_futureWatcher->setFuture(future);
    connect(m_futureWatcher, SIGNAL(finished()),this, SLOT(completionModelProcessed()));
}

void PropertyEdit::completionModelProcessed()
{
    QStandardItemModel* result = m_futureWatcher->future().result();

    m_completer->setModel(result);

    disconnect(m_futureWatcher, SIGNAL(finished()),this, SLOT(completionModelProcessed()));
}
