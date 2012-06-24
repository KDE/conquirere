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

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/SimpleResource>

#include <KDE/KJob>
#include "sro/nao/tag.h"
#include "sro/pimo/topic.h"

#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/PIMO>
#include <Nepomuk2/Variant>

#include <KDE/KUrl>
#include <KDE/KDebug>

using namespace Soprano::Vocabulary;
using namespace Nepomuk2::Vocabulary;

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

    QList<Nepomuk2::Resource> tagList = resource().property(propertyUrl()).toResourceList();

    foreach (const Nepomuk2::Resource & t, tagList) {
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
        QList<QUrl> resourceUris; resourceUris << resource().resourceUri();
        QList<QUrl> value; value << propertyUrl();
        Nepomuk2::removeProperties(resourceUris, value);
        return;
    }

    QStringList entryList;
    if(hasMultipleCardinality()) {
        entryList = newTagNames.split(QLatin1String(";"));
    }
    else {
        entryList.append(newTagNames);
    }

    Nepomuk2::SimpleResourceGraph graph;
    foreach(const QString & s, entryList) {
        if(s.trimmed().isEmpty()) { continue; }

        if( propertyUrl() == NAO::hasTag()) {
            Nepomuk2::SimpleResource tagResource;
            Nepomuk2::NAO::Tag tag (&tagResource);

            tagResource.addProperty( NAO::identifier(), KUrl::fromEncoded(s.trimmed().toUtf8()) );
            tag.addPrefLabel( s.trimmed() );

            graph << tagResource;
        }
        else if(propertyUrl() == NAO::hasTopic()) {

            Nepomuk2::PIMO::Topic topic;

            topic.addProperty( NAO::identifier(), KUrl::fromEncoded(s.trimmed().toUtf8()) );
            topic.setTagLabel( s.trimmed() );

            graph << topic;
        }
        else {
            kWarning() << "unknown tag property set";
        }
    }

    m_editedResource = resource();
    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties), SIGNAL(result(KJob*)),this, SLOT(addTags(KJob*)));
}

void TagEdit::addTags(KJob *job)
{
    if( job->error() != 0) {
        kDebug() << "could not create new tags" << job->errorString();
        return;
    }

    Nepomuk2::StoreResourcesJob *srj = dynamic_cast<Nepomuk2::StoreResourcesJob *>(job);

    // now get all the uris for the new tags
    QVariantList tagUris;
    foreach (const QUrl &uri, srj->mappings()) {
         tagUris << uri;
    }

    // and attach all tags to the current Nepomuk resource
    QList<QUrl> resourceUris; resourceUris << m_editedResource.resourceUri();
    Nepomuk2::setProperty(resourceUris, propertyUrl(), tagUris);

    //TODO remove when resourcewatcher is working..
    emit resourceCacheNeedsUpdate(m_editedResource);
}
