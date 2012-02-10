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

#include "kmultiitemedit.h"

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/simpleresource.h"
#include <KDE/KJob>
#include "sro/nao/tag.h"
#include "sro/pimo/topic.h"

#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Variant>
#include <KDE/KDebug>

#include <QtCore/QUrl>

using namespace Soprano::Vocabulary;
using namespace Nepomuk::Vocabulary;

TagEdit::TagEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void TagEdit::setPropertyUrl(const QUrl & m_propertyUrl)
{
    PropertyEdit::setPropertyUrl(m_propertyUrl);

    if(m_propertyUrl == NAO::hasTag()) {
        m_lineEdit->setNepomukCompleterLabel( NAO::prefLabel());
        m_lineEdit->setNepomukCompleterRange( NAO::Tag() );
    }
    else if(m_propertyUrl == NAO::hasTopic()) {
        m_lineEdit->setNepomukCompleterLabel( PIMO::tagLabel());
        m_lineEdit->setNepomukCompleterRange( PIMO::Topic() );
    }
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

void TagEdit::updateResource(const QString & newTagNames)
{
    if(newTagNames.isEmpty()) {
        QList<QUrl> resourceUris; resourceUris << resource().uri();
        QList<QUrl> value; value << propertyUrl();
        Nepomuk::removeProperties(resourceUris, value);
        return;
    }

    QStringList entryList;
    if(hasMultipleCardinality()) {
        entryList = newTagNames.split(QLatin1String(";"));
    }
    else {
        entryList.append(newTagNames);
    }

    Nepomuk::SimpleResourceGraph graph;
    foreach(const QString & s, entryList) {
        if(s.trimmed().isEmpty()) { continue; }

        if( propertyUrl() == NAO::hasTag()) {
            Nepomuk::SimpleResource tagResource;
            Nepomuk::NAO::Tag tag (&tagResource);

            tagResource.addProperty( NAO::identifier(), QUrl::fromEncoded(s.trimmed().toUtf8()) );
            tag.addPrefLabel( s.trimmed() );

            graph << tagResource;
        }
        else if(propertyUrl() == NAO::hasTopic()) {

            Nepomuk::PIMO::Topic topic;

            topic.addProperty( NAO::identifier(), QUrl::fromEncoded(s.trimmed().toUtf8()) );
            topic.setTagLabel( s.trimmed() );

            graph << topic;
        }
        else {
            kWarning() << "unknown tag property set";
        }
    }

    m_editedResource = resource();
    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties), SIGNAL(result(KJob*)),this, SLOT(addTags(KJob*)));
}

void TagEdit::addTags(KJob *job)
{
    if( job->error() != 0) {
        kDebug() << "could not create new tags" << job->errorString();
        return;
    }

    Nepomuk::StoreResourcesJob *srj = dynamic_cast<Nepomuk::StoreResourcesJob *>(job);

    // now get all the uris for the new tags
    QVariantList tagUris;
    foreach (QUrl uri, srj->mappings()) {
         tagUris << uri;
    }

    // and attach all tags to the current Nepomuk resource
    QList<QUrl> resourceUris; resourceUris << m_editedResource.uri();
    Nepomuk::setProperty(resourceUris, propertyUrl(), tagUris);

    //TODO remove when resourcewatcher is working..
    emit resourceCacheNeedsUpdate(m_editedResource);
}
