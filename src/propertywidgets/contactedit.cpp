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
    // first get rid off the old contacts
    Nepomuk2::removeProperties(QList<QUrl>() << resource().uri(), QList<QUrl> () << propertyUrl());

    if(newContactNames.isEmpty()) {
        return;
    }

    QStringList entryList;
    if(hasMultipleCardinality()) {
        entryList = newContactNames.split(QLatin1String(";"));
    }
    else {
        entryList.append(newContactNames);
    }

    // Now add all th enew ones
    Nepomuk2::SimpleResourceGraph graph;
    Nepomuk2::NBIB::Publication publication(resource().uri());

    foreach(const QString & s, entryList) {
        if(s.trimmed().isEmpty()) { continue; }

        Nepomuk2::NCO::Contact contact;

        contact.setFullname( s.trimmed() );
        contact.addProperty( NAO::prefLabel(), s.trimmed() );
        publication.addProperty(propertyUrl(), contact.uri());
        publication.addProperty(NAO::hasSubResource(), contact.uri() );

        graph << contact;
    }

    graph << publication;

    connect(Nepomuk2::storeResources(graph, Nepomuk2::IdentifyNew, Nepomuk2::OverwriteProperties),
            SIGNAL(result(KJob*)),this, SLOT(showDMSError(KJob*)) );

}
