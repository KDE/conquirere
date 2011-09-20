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

#include "labeledit.h"

#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Variant>

#include <KDE/KLineEdit>

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDebug>

LabelEdit::LabelEdit(QWidget *parent) :
    QWidget(parent)
{
    m_label = new QLabel();
    m_label->setWordWrap(true);
    m_label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    m_label->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

    m_lineEdit = new KLineEdit();
    m_lineEdit->hide();

    connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(editingFinished()));
    connect(this, SIGNAL(resourceNeedsUpdate(QString)), this, SLOT(updateResource(QString)));
    connect(this, SIGNAL(labelNeedsUpdate()), this, SLOT(updateLabel()));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_label);
    layout->addWidget(m_lineEdit);
    setLayout(layout);

    setMouseTracking(true);

    setMaximumHeight(m_lineEdit->size().height());

    //create the query client to fetch resource data for the autocompletion
    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(addCompletionData(QList<Nepomuk::Query::Result>)));
}

LabelEdit::~LabelEdit()
{
    delete m_label;
    delete m_lineEdit;
    delete m_queryClient;
}

void LabelEdit::setResource(Nepomuk::Resource & resource)
{
    m_resource = resource;

    emit labelNeedsUpdate();
}

void LabelEdit::updateResource(const QString & text)
{
    // easy as this is only a string to enter
    // can internelly be xsd:dateTime or something else
    // here we don't check what was entered
    m_resource.setProperty(m_propertyUrl, text);
}

void LabelEdit::updateLabel()
{
    setLabelText( m_resource.property( m_propertyUrl ).toString() );
}

void LabelEdit::setLabelText(const QString & text)
{
    m_label->setText(text );
}

Nepomuk::Resource LabelEdit::resource()
{
    return m_resource;
}

void LabelEdit::setPropertyUrl(const QUrl & propertyUrl)
{
    m_propertyUrl = propertyUrl;

    // now create a completion model
    // this will look through all nepomuk resources from a specific type and complete
    // it for the user

    //get the range of the property (so what we are allowed to enter)
    Nepomuk::Resource nr(m_propertyUrl);
    Nepomuk::Resource range = nr.property(QLatin1String("http://www.w3.org/2000/01/rdf-schema#range")).toResource();

    if(range.isValid() && range.resourceUri().isValid()) {
        qDebug() << "range of" << nr.genericLabel() << " is " << range.genericLabel();

        // get all resources of type range
        Nepomuk::Query::Query query( Nepomuk::Query::ResourceTypeTerm( range.resourceUri() ) );
        m_queryClient->blockingQuery(query);
    }
    // if we can't use the range for the check
    // get all entries for the propertyUrl tha texist, maybe there is something in it we could reuse
    else {
        // get all resources of type range
        //Nepomuk::Query::Query query( Nepomuk::Query::ResourceTypeTerm( range.resourceUri() ) );
        //m_queryClient->blockingQuery(query);
    }

}

QUrl LabelEdit::propertyUrl()
{
    return m_propertyUrl;
}

void LabelEdit::mousePressEvent ( QMouseEvent * e )
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
}

void LabelEdit::editingFinished()
{
    if(m_label->text() != m_lineEdit->text()) {
        m_label->setText(m_lineEdit->text());

        emit resourceNeedsUpdate(m_lineEdit->text());

    }

    m_lineEdit->hide();
    m_label->show();
}

void LabelEdit::addCompletionData(const QList< Nepomuk::Query::Result > &entries)
{
    if(!entries.isEmpty()) {
        KCompletion *kc = m_lineEdit->completionObject(true);

        // as we have a blocking query, here are all result at once

        foreach(Nepomuk::Query::Result result, entries) {
            kc->addItem(result.resource().genericLabel());

            qDebug() << result.resource().genericLabel();
        }
    }
}
