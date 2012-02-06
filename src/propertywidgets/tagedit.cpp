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

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/simpleresource.h"
#include <KDE/KJob>
#include "sro/nao/tag.h"
#include "sro/pimo/topic.h"

#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Variant>
#include <KDE/KDebug>

#include <QtCore/QUrl>

using namespace Soprano::Vocabulary;

TagEdit::TagEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void TagEdit::setupLabel()
{
    QString labelText;

    QList<Nepomuk::Resource> tagList = resource().property(propertyUrl()).toResourceList();

    foreach(const Nepomuk::Resource & t, tagList) {
        QString prefLabel = t.genericLabel();
        labelText.append(prefLabel);
        labelText.append(QLatin1String("; "));
    }

    labelText.chop(2);

    setLabelText(labelText);
}

void TagEdit::updateResource(const QString & text)
{
    QList<QUrl> resourceUris; resourceUris << resource().uri();
    QList<QUrl> value; value << propertyUrl();
    KJob *job = Nepomuk::removeProperties(resourceUris, value);
    job->exec();

    if(text.isEmpty()) { return; }

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

    Nepomuk::SimpleResourceGraph graph;
    foreach(const QString & s, entryList) {
        if( propertyUrl() == NAO::hasTag()) {
            Nepomuk::SimpleResource tagResource;
            Nepomuk::NAO::Tag tag (&tagResource);

            tagResource.addProperty( NAO::identifier(), QUrl::fromEncoded(s.toUtf8()) );
            tag.addPrefLabel( s );

            graph << tagResource;
        }
        else if(propertyUrl() == NAO::hasTopic()) {

            Nepomuk::PIMO::Topic topic;

            topic.addProperty( NAO::identifier(), QUrl::fromEncoded(s.toUtf8()) );
            topic.setTagLabel( s );

            graph << topic;
        }
        else {
            kWarning() << "unknown tag property set";
        }
    }

    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties), SIGNAL(finished()),this, SLOT(addTags(KJob*)));
}

void TagEdit::addTags(Nepomuk::StoreResourcesJob *job)
{
    // now get all the uris for the new tags
    QVariantList tagUris;
    foreach (QUrl uri, job->mappings()) {
         tagUris << uri;
    }

    QList<QUrl> resourceUris; resourceUris << resource().uri();
    // and attach all tags to the current Nepomuk resource
    Nepomuk::setProperty(resourceUris, propertyUrl(), tagUris);
}
