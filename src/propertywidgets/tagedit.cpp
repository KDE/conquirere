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
#include "sro/nbib/publication.h"
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

void TagEdit::setPropertyUrl(const QUrl & propertyUrl)
{
    PropertyEdit::setPropertyUrl(propertyUrl);

    // change which resources the completer will select
    if(propertyUrl == NAO::hasTag()) {
        m_lineEdit->setNepomukCompleterLabel( NAO::prefLabel());
        m_lineEdit->setNepomukCompleterRange( NAO::Tag() );
    }
    else if(propertyUrl == NAO::hasTopic()) {
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

    // first get all tags/topic depending on propertyUrl
    QList<Nepomuk2::Resource> tlist = resource().property( propertyUrl() ).toResourceList();

    QVariantList valuelist;
    foreach(const Nepomuk2::Resource &r, tlist) {
        valuelist << r.uri();
    }

    // now remove them from the resource() and remove the hasSubresource for them
    Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), propertyUrl(), valuelist);
    Nepomuk2::removeProperty(QList<QUrl>() << resource().uri(), NAO::hasSubResource(), valuelist);

    //FIXME: delete Tags/Topics which are not used by any other resource anymore

    // exit here if no tags/topic are specified
    if(newTagNames.isEmpty()) {
        return;
    }

    QStringList entryList;
    if(hasMultipleCardinality()) {
        entryList = newTagNames.split(QLatin1String(";"));
    }
    else {
        entryList.append(newTagNames);
    }

    // creates the tags/topics again
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Publication publication(resource().uri());

    foreach(const QString & s, entryList) {
        if(s.trimmed().isEmpty()) { continue; }

        if( propertyUrl() == NAO::hasTag()) {
            Nepomuk2::NAO::Tag tag;

            tag.addProperty( NAO::identifier(), KUrl::fromEncoded(s.trimmed().toUtf8()) );
            tag.addPrefLabel( s.trimmed() );
            publication.addProperty(NAO::hasTag(), tag);
            publication.addProperty(NAO::hasSubResource(), tag);

            graph << tag;
        }
        else if(propertyUrl() == NAO::hasTopic()) {

            Nepomuk2::PIMO::Topic topic;

            topic.addProperty( NAO::identifier(), KUrl::fromEncoded(s.trimmed().toUtf8()) );
            topic.setTagLabel( s.trimmed() );
            publication.addProperty(NAO::hasTopic(), topic);
            publication.addProperty(NAO::hasSubResource(), topic);

            graph << topic;
        }
        else {
            kWarning() << "unknown tag property set";
        }
    }

    graph << publication;

    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );
}
