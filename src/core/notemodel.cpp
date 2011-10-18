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

#include "notemodel.h"
#include "library.h"

#include "nbib.h"

#include <KDE/KIcon>

#include <Nepomuk/Resource>
#include <Nepomuk/Query/Term>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/ResourceTerm>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/NegationTerm>
#include <Nepomuk/Vocabulary/NFO>
#include <Nepomuk/Vocabulary/NMO>
#include <Nepomuk/Vocabulary/NCO>
#include <Nepomuk/Vocabulary/NIE>
#include <Soprano/Vocabulary/NAO>
#include <Nepomuk/Vocabulary/PIMO>
#include <Nepomuk/Variant>

#include <QModelIndex>

#include <QDebug>

enum PublicationColumnList {
    Column_Title,
    Column_Tags,
    Column_Date,

    Max_columns
};

NoteModel::NoteModel(QObject *parent) :
    NepomukModel(parent)
{
}

NoteModel::~NoteModel()
{
}

int NoteModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Max_columns;
}

QVariant NoteModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_fileList.size() || index.row() < 0)
        return QVariant();

    Nepomuk::Resource document = m_fileList.at(index.row());

    if(!document.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        if(index.column() == Column_Title) {
            QString titleSting = document.property(Soprano::Vocabulary::NAO::prefLabel()).toString();

            return titleSting;
        }
        else if(index.column() == Column_Date) {

            Nepomuk::Resource note = document.property(Nepomuk::Vocabulary::PIMO::groundingOccurrence()).toResource();

            QString dateSting = note.property(Soprano::Vocabulary::NAO::lastModified()).toString();
            if(dateSting.isEmpty())
                dateSting = document.property(Soprano::Vocabulary::NAO::created()).toString();

            return dateSting;
        }
        else if(index.column() == Column_Tags) {
            QString tagString;
            QList<Nepomuk::Resource> tagList = document.property(Soprano::Vocabulary::NAO::hasTag()).toResourceList();

            foreach(Nepomuk::Resource nr, tagList) {
                tagString.append(nr.property(Soprano::Vocabulary::NAO::prefLabel()).toString());
                tagString.append(QLatin1String("; "));
            }

            tagString.chop(2);

            return tagString;
        }
    }

    return QVariant();
}

QVariant NoteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case Column_Tags:
            return i18n("Tags");
        case Column_Title:
            return i18n("Title");
        case Column_Date:
            return i18n("Last Modified");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case Column_Tags:
            return i18n("Tags");
        case Column_Title:
            return i18n("Title");
        case Column_Date:
            return i18n("Last Modified");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

void NoteModel::startFetchData()
{
    emit updateFetchDataFor(m_selection,true, m_library);

    Nepomuk::Query::AndTerm andTerm;

    andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::PIMO::Note() ) );

    if(m_library->libraryType() != Library_System) {
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
                                                            Nepomuk::Query::ResourceTerm(m_library->pimoLibrary()) ) );
    }

    //sort result by edit date to get only the newest if we have to many results
    //Nepomuk::Query::ComparisonTerm term(Soprano::Vocabulary::NAO::lastModified(), Nepomuk::Query::Term());
    //term.setSortWeight(1, Qt::DescendingOrder);
    //andTerm.addSubTerm(term);

    // build the query
    Nepomuk::Query::Query query( andTerm );
    //query.setLimit(100);
    m_queryClient->query(query);
}
