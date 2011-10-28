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

#include "tagedit.h"

#include <QStandardItemModel>

#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Tag>
#include <Nepomuk/Variant>

TagEdit::TagEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void TagEdit::setupLabel()
{
    QString labelText;

    // we can always assume there can be more than 1 tag for a resource
    //if(hasMultipleCardinality()) {

    QList<Nepomuk::Tag> tagList = resource().tags();

    foreach(Nepomuk::Tag t, tagList) {
        QString prefLabel = t.genericLabel();
        labelText.append(prefLabel);
        addPropertryEntry(prefLabel, t.uri());

        labelText.append(QLatin1String("; "));
    }

    labelText.chop(2);

    setLabelText(labelText);
}

void TagEdit::updateResource(const QString & text)
{
    QStringList entryList;
    if(hasMultipleCardinality()) {
        // for the contact we get a list of contact names divided by ;
        // where each contact is also available as nepomuk:/res in the cache
        // if not, a new contact with the full name of "string" will be created
        entryList = text.split(QLatin1String(";"));
    }
    else {
        entryList.append(text);
    }

    foreach(QString s, entryList) {
        s = s.trimmed();
        // try to find the propertyurl of an already existing contact
        QUrl propUrl = propertyEntry(s);
        if(propUrl.isValid()) {
            resource().addTag(Nepomuk::Tag(propUrl));
        }
        else {
            // create a new contact with the string s as fullname
            Nepomuk::Tag newTag;
            //FIXME Nepomuk::Tag does not add type Tag but twice type Resource!
            newTag.addType(Soprano::Vocabulary::NAO::Tag());
            newTag.setProperty(Soprano::Vocabulary::NAO::prefLabel(), s);
            s.replace(QLatin1String(" "), QLatin1String("%20"));
            newTag.setProperty(Soprano::Vocabulary::NAO::identifier(), s);
            resource().addTag(newTag);
        }
    }
}

void TagEdit::createCompletionModel( const QList< Nepomuk::Query::Result > &entries )
{
    QStandardItemModel *model = new QStandardItemModel();
    QStandardItem *parentItem = model->invisibleRootItem();

    foreach(Nepomuk::Query::Result r, entries) {
        QStandardItem *item = new QStandardItem(r.resource().genericLabel());
        // save the resource uri with the model item
        // this helps to identify the selected entry even if the generic label has
        // the same result on two different items
        // also it is not necessary to ask nepomuk for the resource used later again
        item->setData(r.resource().uri());

        parentItem->appendRow(item);
    }

    setCompletionModel(model);
}
