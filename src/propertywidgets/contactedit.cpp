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

#include "kmultiitemedit.h"

#include <Nepomuk2/DataManagement>
#include <Nepomuk2/StoreResourcesJob>
#include <Nepomuk2/SimpleResourceGraph>
#include <Nepomuk2/SimpleResource>

#include <KDE/KJob>
#include "sro/nco/contact.h"
#include "sro/nbib/publication.h"

#include <Nepomuk2/Vocabulary/NCO>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk2/Vocabulary/NUAO>
#include <Nepomuk2/Variant>

#include <KDE/KDebug>
#include <QtCore/QDateTime>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;

ContactEdit::ContactEdit(QWidget *parent)
    : PropertyEdit(parent)
{
    m_lineEdit->setNepomukCompleterLabel( NCO::fullname() );
}

void ContactEdit::setupLabel()
{
    QString labelText;

    if(hasMultipleCardinality()) {
        QList<Nepomuk2::Resource> authorList = resource().property(propertyUrl()).toResourceList();

        foreach(const Nepomuk2::Resource & r, authorList) {
            QString fullname = r.property(NCO::fullname()).toString();
            labelText.append(fullname.trimmed());
            labelText.append(QLatin1String("; "));
        }

        labelText.chop(2);
    }
    else {
        Nepomuk2::Resource author = resource().property(propertyUrl()).toResource();

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
        Nepomuk2::removeProperties(resourceUris, value);
        return;
    }

    QStringList entryList;
    if(hasMultipleCardinality()) {
        entryList = newContactNames.split(QLatin1String(";"));
    }
    else {
        entryList.append(newContactNames);
    }

    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::SimpleResource publicationRes(resource().uri());
    Nepomuk2::NBIB::Publication publication(publicationRes);
    //BUG we need to set some property otherwise the DataManagement server complains the resource is invalid
    QDateTime datetime = QDateTime::currentDateTimeUtc();
    publicationRes.setProperty( NUAO::lastModification(), datetime.toString("yyyy-MM-ddTHH:mm:ssZ"));

    QVariantList contactUris;
    foreach(const QString & s, entryList) {
        if(s.trimmed().isEmpty()) { continue; }

        Nepomuk2::NCO::Contact contact;

        contact.addProperty( NCO::fullname(), s.trimmed() );
        contact.addProperty( NAO::prefLabel(), s.trimmed() );

        graph << contact;
        contactUris << contact.uri();
    }

    publication.setProperty(propertyUrl(), contactUris);
    graph << publication;

    m_changedResource = resource();
    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(updateEditedCacheResource()));

}
