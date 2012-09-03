/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "bibtexvariant.h"

#include "globals.h"

#include <kbibtex/file.h>
#include <kbibtex/value.h>
#include <kbibtex/entry.h>
#include <kbibtex/macro.h>

#include <nepomukmetadataextractor/nepomukpipe.h>

#include <KDE/KDebug>
#include <QtCore/QMapIterator>

using namespace NepomukMetaDataExtractor::Pipe;

QVariantList BibTexVariant::toVariant(const File &bibtexFile)
{
    QVariantList bibList;

    QMap<QString, QString> macroLookup;

    // we start by filling the lookuptable for all macros
    // in BibTeX files macros are used to create abbreviations for some fields that can be used all over again
    foreach(QSharedPointer<Element> e, bibtexFile ) {
        Macro *macro = dynamic_cast<Macro *>(e.data());
        if(macro) {
            macroLookup.insert(macro->key(), PlainTextValue::text(macro->value()));
        }
    }

    //foreach bib entry
    foreach(QSharedPointer<Element> e, bibtexFile ) {
        Entry *entry = dynamic_cast<Entry *>(e.data());

        if(!entry) { continue; }

        // the crossref part means we fetch bibtex entries from other bibtex entries
        // so the title is only added to one and related to all others via the crossref
        if(entry->contains(Entry::ftCrossRef)) {
            entry = Entry::resolveCrossref(*entry, &bibtexFile);
        }

        QVariantMap bibentry = entryToMap(entry,macroLookup);
        bibentry.insert("bibtexentrytype", entry->type());
        bibentry.insert("bibtexcitekey", entry->id());

        bibList.append(bibentry);
    }

    return bibList;
}

QVariantMap BibTexVariant::entryToMap(const Entry *e, QMap<QString, QString> lookup)
{
    QVariantMap entryMap;

    QMapIterator<QString, Value> i(*e);
    while (i.hasNext()) {
        i.next();

        foreach(QSharedPointer<ValueItem> vi, e->value(i.key()) ) {
            QString newValue = entryMap.value( i.key() ).toString();

            if( !newValue.isEmpty() ) {
                newValue.append(QLatin1String("; "));
            }

            Person *person = dynamic_cast<Person *>(vi.data());

            if(person) {
                QString fullname = QString("%1 %2 %3").arg(person->firstName()).arg(person->lastName()).arg(person->suffix());
                newValue.append( lookup.value(fullname,fullname).trimmed() );
                entryMap.insert( i.key(), newValue );
            }
            else {
                QString oldValue = PlainTextValue::text(*vi.data());
                newValue.append( lookup.value(oldValue,oldValue) );
                entryMap.insert( i.key(), newValue );
            }
        }
    }

    return entryMap;
}

File *BibTexVariant::fromVariant(const QVariantList &bibtexList)
{
    File *bibtexFile = new File;

    foreach(const QVariant &v, bibtexList) {
        QVariantMap entryMap = v.toMap();

        QString citeKey = entryMap.value(QLatin1String("bibtexcitekey")).toString();
        QString entryType = entryMap.value(QLatin1String("bibtexentrytype")).toString();

        QSharedPointer<Entry>entry(new Entry);
        Entry *e = entry.data();
        e->setType(entryType);
        e->setId(citeKey);

        QMapIterator<QString, QVariant> i(entryMap);
        while (i.hasNext()) {
            i.next();

            if(i.key() == QLatin1String("bibtexcitekey") || i.key() == QLatin1String("bibtexentrytype")) {
                continue;
            }
            if(keysForPersonDetection.contains(i.key()) || i.key() == QLatin1String("author") || i.key() == QLatin1String("editor")) {
                Value personValue;
                QList<NepomukPipe::NepomukPipe::Name> personList = NepomukPipe::splitPersonList( i.value().toString() );

                foreach(const NepomukPipe::NepomukPipe::Name &n, personList) {
                    Person *person = new Person(n.first.trimmed(), n.last.trimmed(), n.suffix.trimmed());
                    personValue.append(QSharedPointer<ValueItem>(person));
                }

                e->insert( i.key() , personValue);
            }
            else if(i.key() == QLatin1String("keyword")) {
                Value keywordValue;
                QStringList keywordList = i.value().toString().split(QLatin1String("; "));

                foreach(const QString &keywordString, keywordList) {
                    Keyword *keyword = new Keyword(keywordString);
                    keywordValue.append(QSharedPointer<ValueItem>(keyword));
                }

                e->insert( i.key() , keywordValue);
            }
            else {
                Value normalValue;
                normalValue.append( QSharedPointer<ValueItem>(new PlainText(i.value().toString())) );
                e->insert( i.key() , normalValue);
            }
        }

        bibtexFile->append(entry);
    }

    return bibtexFile;
}
