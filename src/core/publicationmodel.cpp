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

#include "publicationmodel.h"
#include "library.h"
#include "../globals.h"

#include "nbib.h"

#include <KDE/KIcon>

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

PublicationModel::PublicationModel(QObject *parent)
    : NepomukModel(parent)
{
}

PublicationModel::~PublicationModel()
{
}

int PublicationModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return Max_columns;
}

QVariant PublicationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_fileList.size() || index.row() < 0)
        return QVariant();

    Nepomuk::Resource document = m_fileList.at(index.row());

    if(!document.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        if(index.column() == Column_ResourceType) {
            QString typeSting;
            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();

                BibEntryType type = BibEntryTypeFromUrl(publication);
                typeSting = BibEntryTypeTranslation.at(type);
            }
            else {
                BibEntryType type = BibEntryTypeFromUrl(document);
                typeSting = BibEntryTypeTranslation.at(type);
            }

            return typeSting;
        }
        if(index.column() == Column_Author) {
            QString authorSting;
            QList<Nepomuk::Resource> authorList;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                authorList = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();
            }
            else {
                authorList = document.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();
            }
            foreach(Nepomuk::Resource a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            return authorSting;
        }
        else if(index.column() == Column_Title) {
            QString titleSting;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                titleSting = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
            }
            else {
                titleSting = document.property(Nepomuk::Vocabulary::NIE::title()).toString();
            }

            return titleSting;
        }
        else if(index.column() == Column_Date) {
            QString dateSting;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                dateSting = publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();
            }
            else {
                dateSting = document.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();
            }

            return dateSting;
        }
        else if(index.column() == Column_Publisher) {
            QString authorSting;
            QList<Nepomuk::Resource> authorList;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                authorList = publication.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();
            }
            else {
                authorList = document.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();
            }
            foreach(Nepomuk::Resource a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            return authorSting;
        }
        else if(index.column() == Column_Editor) {
            QString authorSting;
            QList<Nepomuk::Resource> authorList;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                authorList = publication.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
            }
            else {
                authorList = document.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
            }
            foreach(Nepomuk::Resource a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);

            return authorSting;
        }
        else if(index.column() == Column_CiteKey) {
            QString titleSting;

            if(m_selection != Resource_Reference) {
                //TODO find citykey of a Publication
                return titleSting;
                //                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::usePublication()).toResource();
                //                titleSting = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
            }
            else {
                titleSting = document.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
            }

            return titleSting;
        }
        else if(index.column() == Column_StarRate) {
            int rating;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                rating = publication.rating();
            }
            else {
                rating = document.rating();
            }
            return rating;
        }
    }

    if (role == Qt::DecorationRole) {
        if(index.column() == Column_FileAvailable) {
            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = document.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                Nepomuk::Resource file = publication.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResource();
                if(file.isValid()) {
                    return  KIcon(QLatin1String("bookmarks-organize"));
                }
            }
            else {
                Nepomuk::Resource file = document.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResource();
                if(file.isValid()) {
                    return  KIcon(QLatin1String("bookmarks-organize"));
                }
            }
        }
        else if(index.column() == Column_Reviewed) {
            return KIcon(QLatin1String("dialog-ok-apply"));
        }
    }

    return QVariant();
}

QVariant PublicationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    if (role == Qt::DecorationRole) {
        switch (section) {
        case Column_Reviewed:
            return KIcon(QLatin1String("document-edit-verify"));
        case Column_FileAvailable:
            return KIcon(QLatin1String("bookmarks-organize"));
        default:
            return QVariant();
        }
    }

    if (role == Qt::DisplayRole) {
        switch (section) {
        case Column_Reviewed:
            return QVariant();
        case Column_FileAvailable:
            return QVariant();
        case Column_Author:
            return i18n("Author");
        case Column_Title:
            return i18n("Title");
        case Column_Date:
            return i18n("Date");
        case Column_Publisher:
            return i18n("Publisher");
        case Column_Editor:
            return i18n("Editor");
        case Column_CiteKey:
            return i18n("Citekey");
        case Column_ResourceType:
            return i18n("Type");
        case Column_StarRate:
            return i18n("Rating");
        default:
            return QVariant();
        }
    }

    if (role == Qt::ToolTipRole) {
        switch (section) {
        case Column_Reviewed:
            return i18n("Reviewed");
        case Column_FileAvailable:
            return i18n("File available");
        case Column_Author:
            return  i18n("The author of the document");
        case Column_Title:
            return i18n("The document title");
        case Column_Date:
            return i18n("The date of publishing");
        case Column_Publisher:
            return  i18n("The publisher of the document");
        case Column_Editor:
            return  i18n("The editor of the document");
        case Column_CiteKey:
            return i18n("Citekey");
        case Column_ResourceType:
            return i18n("How was this published");
        case Column_StarRate:
            return i18n("Rating");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

void PublicationModel::startFetchData()
{
    emit updateFetchDataFor(m_selection,true, m_library);

    Nepomuk::Query::AndTerm andTerm;

    switch(m_selection)
    {
    case Resource_Reference:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Reference() ) );
        break;
    case Resource_Publication:
        andTerm.addSubTerm( Nepomuk::Query::ResourceTypeTerm( Nepomuk::Vocabulary::NBIB::Publication() ) );
        break;
    }

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
