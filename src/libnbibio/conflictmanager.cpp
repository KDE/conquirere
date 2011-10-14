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

#include "conflictmanager.h"

#include "nbib.h"
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Variant>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>
#include <Nepomuk/Query/QueryParser>
#include <Nepomuk/Query/LiteralTerm>

ConflictManager::ConflictManager(QObject *parent) :
    QObject(0)
{
}

Nepomuk::Resource ConflictManager::addEntry(Nepomuk::Resource entry)
{
    QList<Nepomuk::Resource> conflicts;

    bool publication = false;
    if(entry.hasType(Nepomuk::Vocabulary::NBIB::Publication())) {
        conflicts = conflictCheckPublication(entry);
        publication = true;
    }
    else {
        conflicts = conflictCheckReference(entry);
    }

    if(!conflicts.isEmpty()) {

        // check for exact duplicate
        // ....
        // else ...
        Conflict c;
        c.entry = entry;
        c.conflictEntries = conflicts;

        if(publication)
            m_publicationConflicts.append(c);
        else
            m_referenceConflicts.append(c);

        return entry;
    }
    else {
        return entry;
    }
}

QList<ConflictManager::Conflict> ConflictManager::publicationConflicts()
{
    return m_publicationConflicts;
}

QList<ConflictManager::Conflict> ConflictManager::referenceConflicts()
{
    return m_publicationConflicts;
}

QList<Nepomuk::Resource> ConflictManager::conflictCheckPublication(Nepomuk::Resource entry)
{
    //fetch all publication with the same title and same entry type
    QString titleString = entry.property(Nepomuk::Vocabulary::NIE::title()).toString();

    Nepomuk::Query::ComparisonTerm title( Nepomuk::Vocabulary::NIE::title(), Nepomuk::Query::LiteralTerm( titleString ) );
    Nepomuk::Query::ResourceTypeTerm type( resourceToUrl(entry) );
    Nepomuk::Query::Query query( Nepomuk::Query::AndTerm( type, title ) );

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    QList<Nepomuk::Resource> conflictingEntries;

    foreach(Nepomuk::Query::Result nqr, queryResult) {
        Nepomuk::Resource r = nqr.resource();
        if(r.resourceUri() != entry.resourceUri())
            conflictingEntries.append(r);
    }

    return conflictingEntries;
}

QList<Nepomuk::Resource> ConflictManager::conflictCheckReference(Nepomuk::Resource entry)
{
    //fetch all references with the same citekey
    QString citeKey = entry.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
    Nepomuk::Query::ComparisonTerm title( Nepomuk::Vocabulary::NBIB::citeKey(), Nepomuk::Query::LiteralTerm( citeKey ) );
    Nepomuk::Query::ResourceTypeTerm type( Nepomuk::Vocabulary::NBIB::Reference() );
    Nepomuk::Query::Query query( Nepomuk::Query::AndTerm( type, title ) );

    QList<Nepomuk::Query::Result> queryResult = Nepomuk::Query::QueryServiceClient::syncQuery(query);

    QList<Nepomuk::Resource> conflictingEntries;

    foreach(Nepomuk::Query::Result nqr, queryResult) {
        Nepomuk::Resource r = nqr.resource();
        if(r.resourceUri() != entry.resourceUri())
            conflictingEntries.append(r);
    }

    return conflictingEntries;
}

bool ConflictManager::hasConflicts()
{
    if(m_publicationConflicts.isEmpty() && m_referenceConflicts.isEmpty())
        return false;
    else
        return true;
}

QUrl ConflictManager::resourceToUrl(Nepomuk::Resource & resource)
{
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Article())) {
        return Nepomuk::Vocabulary::NBIB::Article();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Proceedings())) {
        return Nepomuk::Vocabulary::NBIB::Proceedings();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::BachelorThesis())) {
        return Nepomuk::Vocabulary::NBIB::BachelorThesis();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Book())) {
        return Nepomuk::Vocabulary::NBIB::Book();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Booklet())) {
        return Nepomuk::Vocabulary::NBIB::Booklet();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::InProceedings())) {
        return Nepomuk::Vocabulary::NBIB::InProceedings();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Manual())) {
        return Nepomuk::Vocabulary::NBIB::Manual();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::MastersThesis())) {
        return Nepomuk::Vocabulary::NBIB::MastersThesis();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::PhdThesis())) {
        return Nepomuk::Vocabulary::NBIB::PhdThesis();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Techreport())) {
        return Nepomuk::Vocabulary::NBIB::Techreport();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Unpublished())) {
        return Nepomuk::Vocabulary::NBIB::Unpublished();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Patent())) {
        return Nepomuk::Vocabulary::NBIB::Patent();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Electronic())) {
        return Nepomuk::Vocabulary::NBIB::Electronic();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::JournalIssue())) {
        return Nepomuk::Vocabulary::NBIB::JournalIssue();
    }
    if(resource.hasType(Nepomuk::Vocabulary::NBIB::Collection())) {
        return Nepomuk::Vocabulary::NBIB::Collection();
    }

    qWarning() << "Warning unknown resource type";
    return Nepomuk::Vocabulary::NBIB::Publication();
}
