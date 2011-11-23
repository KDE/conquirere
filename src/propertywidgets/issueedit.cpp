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

#include "issueedit.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Variant>

#include <QtGui/QStandardItemModel>
#include <QtGui/QInputDialog>
#include <QtCore/QUrl>

IssueEdit::IssueEdit(QWidget *parent)
    :PropertyEdit(parent)
{
    setUseDetailDialog(true);

    m_issueType = Nepomuk::Vocabulary::NBIB::Collection();
    m_seriesType =  Nepomuk::Vocabulary::NBIB::Series();
}

void IssueEdit::setupLabel()
{
    //check if resource() is an publication with attached Issue collection
    QString title;

    Nepomuk::Resource issueResource = resource().property(Nepomuk::Vocabulary::NBIB::collection()).toResource();
    if(issueResource.isValid()) {
        Nepomuk::Resource seriesResource = issueResource.property(Nepomuk::Vocabulary::NBIB::inSeries()).toResource();
        title = seriesResource.property(Nepomuk::Vocabulary::NIE::title()).toString();

        addPropertryEntry(title, resource().uri());

        m_issueType = issueResource.type(); // saves which subclass of collection is used
        m_seriesType = seriesResource.type(); // saves which subclass of series is used
    }

    setLabelText(title);
}

void IssueEdit::updateResource(const QString & text)
{
    // remove the existing collection (issue) from the article
    resource().removeProperty( Nepomuk::Vocabulary::NBIB::collection() );

    if(text.isEmpty())
        return;

    // try to find the propertyurl of an already existing collection (issue)
    QUrl propUrl = propertyEntry(text);

    if(propUrl.isValid()) {
        resource().addProperty( propertyUrl(), Nepomuk::Resource(propUrl));
    }
    else {
        // create a new collection with the string text as title
        Nepomuk::Resource newIssue(QUrl(), m_issueType);
        newIssue.setProperty(Nepomuk::Vocabulary::NIE::title(), text);

        // any issue is also part of an series.
        Nepomuk::Resource newSeries(QUrl(), m_seriesType);
        newSeries.setProperty(Nepomuk::Vocabulary::NIE::title(), text);
        newSeries.addType(Nepomuk::Vocabulary::NBIB::Series()); // seems to be a bug, not the full hierachry will be set otherwise
        newSeries.addType(Nepomuk::Vocabulary::NIE::InformationElement());

        // important to note, the Issue and Series have the same name. Usually an issue does not need a name, as it
        // could simple use the name of the connected Series. but the genericLabel in the resource view
        // looks better this way around ;)
        // its therfore more like a nice debug help

        // connect series and collection
        newIssue.setProperty(Nepomuk::Vocabulary::NBIB::inSeries(), newSeries);
        newSeries.addProperty(Nepomuk::Vocabulary::NBIB::seriesOf(), newIssue);

        //connect article and issue
        resource().addProperty( Nepomuk::Vocabulary::NBIB::collection(), newIssue);
        newIssue.addProperty(Nepomuk::Vocabulary::NBIB::article(), resource());
    }
}

QStandardItemModel* IssueEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *parentItem = model->invisibleRootItem();

    foreach(const Nepomuk::Query::Result & r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().property(Nepomuk::Vocabulary::NIE::title()).toString());

        item->setData(r.resource().uri());

        parentItem->appendRow(item);
    }

    return model;
}

void IssueEdit::detailEditRequested()
{
    QStringList items;
    int curItem;
    items << i18n("Collection") << i18n("Journal") << i18n("Magazine") << i18n("Newspaper");

    if(m_seriesType == Nepomuk::Vocabulary::NBIB::Journal()) {
        curItem = 1;
    }
    else if(m_seriesType == Nepomuk::Vocabulary::NBIB::Magazin()) {
        curItem = 2;
    }
    else if(m_seriesType == Nepomuk::Vocabulary::NBIB::Newspaper()) {
        curItem = 3;
    }
    else
        curItem = 0;

    bool ok;
    QString item = QInputDialog::getItem(this, i18n("Issue type of the article"),
                                         i18n("Define the type of the issue:"), items, curItem, false, &ok);
    if (ok && !item.isEmpty()) {
        int i = items.indexOf(item);
        switch(i) {
        case 1:
            m_issueType = Nepomuk::Vocabulary::NBIB::JournalIssue();
            m_seriesType =  Nepomuk::Vocabulary::NBIB::Journal();
            break;
        case 2:
            m_issueType = Nepomuk::Vocabulary::NBIB::MagazinIssue();
            m_seriesType =  Nepomuk::Vocabulary::NBIB::Magazin();
            break;
        case 3:
            m_issueType = Nepomuk::Vocabulary::NBIB::NewspaperIssue();
            m_seriesType =  Nepomuk::Vocabulary::NBIB::Newspaper();
            break;
        default:
            m_issueType = Nepomuk::Vocabulary::NBIB::Collection();
            m_seriesType =  Nepomuk::Vocabulary::NBIB::Series();
            break;
        }

        // now update the resource type
        Nepomuk::Resource issueResource = resource().property((Nepomuk::Vocabulary::NBIB::collection())).toResource();
        Nepomuk::Resource seriesResource = issueResource.property((Nepomuk::Vocabulary::NBIB::inSeries())).toResource();

        Nepomuk::Resource x(QUrl(), m_seriesType);
        seriesResource.setTypes(x.types());

        Nepomuk::Resource y(QUrl(), m_issueType);
        issueResource.setTypes(y.types());
    }
}
