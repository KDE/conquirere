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

#include "kmultiitemedit.h"

#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/OrTerm>
#include <Nepomuk/Variant>

#include "nbib.h"
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NCAL>
#include <Nepomuk/Vocabulary/NFO>
#include <Soprano/Vocabulary/RDFS>

#include <KDE/KIcon>
#include <KDE/KSqueezedTextLabel>
#include <KDE/KLineEdit>
#include <KDE/KCompletion>
#include <KDE/KCompletionBox>
#include <KDE/KDebug>

#include <QtGui/QLabel>
#include <QtGui/QToolButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QKeyEvent>

PropertyEdit::PropertyEdit(QWidget *parent)
    : QWidget(parent)
    , m_isListEdit(true)
    , m_useDetailDialog(false)
    , m_directEditAllowed(true)
{
    m_label = new KSqueezedTextLabel();
    m_label->setWordWrap(false);
    m_label->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    m_label->setTextElideMode(Qt::ElideMiddle);

    m_lineEdit = new KMultiItemEdit();
    m_lineEdit->hide();
//    connect(m_lineEdit, SIGNAL(textEdited(QString)), this, SLOT(updateCompleter()));
    connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));

    m_detailView = new QToolButton();
    m_detailView->setIconSize(QSize(16,16));
    m_detailView->setMaximumSize(QSize(16,16));
    m_detailView->setMinimumSize(QSize(16,16));
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
    setUseDetailDialog(false);
}

PropertyEdit::~PropertyEdit()
{
    delete m_label;
    delete m_lineEdit;
}

void PropertyEdit::setResource(Nepomuk::Resource & resource)
{
    m_resource = resource;

    setupLabel();
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
    Nepomuk::Resource range = nr.property(Soprano::Vocabulary::RDFS::range()).toResource();

    if(range.isValid())
        m_lineEdit->setNepomukCompleterRange( range.resourceUri() );

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
//        updateCompleter();
        break;
    default:
        break;
    }
}

void PropertyEdit::editingFinished()
{
    if(m_label->fullText() != m_lineEdit->text()) {
        QString inputString = QLatin1String(m_lineEdit->text().toUtf8());
        inputString = inputString.trimmed();
        updateResource(inputString);
        setLabelText(m_lineEdit->text());

        emit resourceCacheNeedsUpdate(resource());
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

void PropertyEdit::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    emit widgetShown(visible);
}

void PropertyEdit::updateEditedCacheResource()
{
    //TODO remove when resourcewatcher is working..
    if(m_changedResource.exists())
        emit resourceCacheNeedsUpdate(m_changedResource);
}
