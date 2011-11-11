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

#include "bookmarkmodel.h"
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

#include <QtCore/QModelIndex>

#include <QtCore/QDebug>

enum PublicationColumnList {
    Column_StarRate,
    Column_Title,
    Column_Link,
    Column_Tags,
    Column_Date,

    Max_columns
};

BookmarkModel::BookmarkModel(QObject *parent) :
    NepomukModel(parent)
{
}

BookmarkModel::~BookmarkModel()
{
}

int BookmarkModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Max_columns;
}

QVariant BookmarkModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case Column_Link:
            return i18n("Link");
        case Column_StarRate:
            return i18n("Rating");
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
        case Column_Link:
            return i18n("Link");
        case Column_StarRate:
            return i18n("Rating");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

void BookmarkModel::startFetchData()
{
    emit updateFetchDataFor(m_selection,true, m_library);

    Nepomuk::Query::AndTerm andTerm;

    andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NFO::Bookmark() ) );

    if(m_library->libraryType() != Library_System) {
        andTerm.addSubTerm( Nepomuk::Query::ComparisonTerm( Nepomuk::Vocabulary::PIMO::isRelated(),
                                                            Nepomuk::Query::ResourceTerm(m_library->pimoLibrary()) ) );
    }

    // build the query
    Nepomuk::Query::Query query( andTerm );
    m_queryClient->query(query);
}

QVariantList BookmarkModel::createDisplayData(const Nepomuk::Resource & res)
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_Title: {
            QString titleSting = res.property(Nepomuk::Vocabulary::NIE::title()).toString();

            newEntry = titleSting;
            break;
        }
        case Column_Date: {
            QString dateSting = res.property(Nepomuk::Vocabulary::NIE::contentLastModified()).toString();
            if(dateSting.isEmpty()) {
                dateSting = res.property(Nepomuk::Vocabulary::NIE::contentLastModified()).toString();
            }
            newEntry = dateSting;
            break;
        }
        case Column_Tags: {
            QString tagString;
            QList<Nepomuk::Resource> tagList = res.property(Soprano::Vocabulary::NAO::hasTag()).toResourceList();

            foreach(const Nepomuk::Resource & nr, tagList) {
                tagString.append(nr.property(Soprano::Vocabulary::NAO::prefLabel()).toString());
                tagString.append(QLatin1String("; "));
            }
            tagString.chop(2);
            newEntry = tagString;
            break;
        }
        case Column_Link: {
            Nepomuk::Resource linkResource = res.property(Nepomuk::Vocabulary::NFO::bookmarks()).toResource();
            newEntry = linkResource.genericLabel();
            break;
        }
        case Column_StarRate: {
            int rating = res.rating();
            newEntry = rating;
            break;
        }
        default:
            newEntry = QVariant();
        }

        displayList.append(newEntry);
    }

    return displayList;
}

QVariantList BookmarkModel::createDecorationData(const Nepomuk::Resource & res)
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        decorationList.append(QVariant());
    }

    return decorationList;
}
