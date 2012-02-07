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

#include "contactedit.h"

#include "dms-copy/datamanagement.h"
#include "dms-copy/storeresourcesjob.h"
#include "dms-copy/simpleresourcegraph.h"
#include "dms-copy/simpleresource.h"
#include <KDE/KJob>
#include "sro/nco/contact.h"

#include <Nepomuk/Vocabulary/NCO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Variant>

#include <KDE/KDebug>

using namespace Nepomuk::Vocabulary;
using namespace Soprano::Vocabulary;

ContactEdit::ContactEdit(QWidget *parent)
    : PropertyEdit(parent)
{
}

void ContactEdit::setupLabel()
{
    QString labelText;

    if(hasMultipleCardinality()) {
        QList<Nepomuk::Resource> authorList = resource().property(propertyUrl()).toResourceList();

        foreach(const Nepomuk::Resource & r, authorList) {
            QString fullname = r.property(NCO::fullname()).toString();
            labelText.append(fullname.trimmed());
            labelText.append(QLatin1String("; "));
        }

        labelText.chop(2);
    }
    else {
        Nepomuk::Resource author = resource().property(propertyUrl()).toResource();

        QString fullname = author.property(NCO::fullname()).toString();
        labelText.append(fullname.trimmed());
    }

    setLabelText(labelText);
}

void ContactEdit::updateResource(const QString & newContactNames)
{
    if(newContactNames.isEmpty()) {
        QList<QUrl> resourceUris; resourceUris << resource().uri();
        QList<QUrl> value; value << propertyUrl();
        Nepomuk::removeProperties(resourceUris, value);
        return;
    }

    QStringList entryList;
    if(hasMultipleCardinality()) {
        entryList = newContactNames.split(QLatin1String(";"));
    }
    else {
        entryList.append(newContactNames);
    }

    Nepomuk::SimpleResourceGraph graph;
    foreach(const QString & s, entryList) {
        if(s.trimmed().isEmpty()) { continue; }

        Nepomuk::NCO::Contact contact;

        contact.addProperty( NCO::fullname(), s.trimmed() );
        contact.addProperty( NAO::prefLabel(), s.trimmed() );

        graph << contact;
    }

    m_editedResource = resource();
    connect(Nepomuk::storeResources(graph, Nepomuk::IdentifyNew, Nepomuk::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(addContact(KJob*)));

}

void ContactEdit::addContact(KJob *job)
{
    if( job->error() != 0) {
        kDebug() << "could not create new contact" << job->errorString();
        return;
    }

    Nepomuk::StoreResourcesJob *srj = dynamic_cast<Nepomuk::StoreResourcesJob *>(job);

    // now get all the uris for the new contact
    QVariantList contactUris;
    foreach (QUrl uri, srj->mappings()) {
         contactUris << uri;
    }

    // add the crosslink reference <-> publicatio
    QList<QUrl> resourceUris; resourceUris << m_editedResource.uri();
    Nepomuk::setProperty(resourceUris, propertyUrl(), contactUris);

    //TODO remove when resourcewatcher is working..
    emit resourceCacheNeedsUpdate(m_editedResource);
}
