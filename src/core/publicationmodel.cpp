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

    // build the query
    Nepomuk::Query::Query query( andTerm );
    m_queryClient->query(query);
}

QVariantList PublicationModel::createDisplayData(const Nepomuk::Resource & res)
{
    QVariantList displayList;
    displayList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_ResourceType: {
            QString typeSting;
            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = res.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();

                BibEntryType type = BibEntryTypeFromUrl(publication);
                typeSting = BibEntryTypeTranslation.at(type);
            }
            else {
                BibEntryType type = BibEntryTypeFromUrl(res);
                typeSting = BibEntryTypeTranslation.at(type);
            }
            newEntry = typeSting;
            break;
        }
        case Column_Author: {
            QString authorSting;
            QList<Nepomuk::Resource> authorList;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = res.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                authorList = publication.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();
            }
            else {
                authorList = res.property(Nepomuk::Vocabulary::NCO::creator()).toResourceList();
            }
            foreach(const Nepomuk::Resource & a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);
            newEntry = authorSting;
            break;
        }
        case Column_Title: {
            QString titleSting;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = res.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                titleSting = publication.property(Nepomuk::Vocabulary::NIE::title()).toString();
            }
            else {
                titleSting = res.property(Nepomuk::Vocabulary::NIE::title()).toString();
            }

            newEntry = titleSting;
            break;
        }
        case Column_Date: {
            QString dateSting;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = res.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                dateSting = publication.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();
            }
            else {
                dateSting = res.property(Nepomuk::Vocabulary::NBIB::publicationDate()).toString();
            }
            newEntry = dateSting;
            break;
        }
        case Column_Editor: {
            QString authorSting;
            QList<Nepomuk::Resource> authorList;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = res.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                authorList = publication.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();
            }
            else {
                authorList = res.property(Nepomuk::Vocabulary::NBIB::editor()).toResourceList();
            }
            foreach(const Nepomuk::Resource & a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);
            newEntry = authorSting;
            break;
        }
        case Column_Publisher: {
            QString authorSting;
            QList<Nepomuk::Resource> authorList;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = res.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                authorList = publication.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
            }
            else {
                authorList = res.property(Nepomuk::Vocabulary::NCO::publisher()).toResourceList();
            }
            foreach(const Nepomuk::Resource & a, authorList) {
                authorSting.append(a.genericLabel());
                authorSting.append(QLatin1String("; "));
            }
            authorSting.chop(2);
            newEntry = authorSting;
            break;
        }
        case Column_CiteKey: {
            QString citekeySting;

            if(m_selection != Resource_Reference) {
                QList<Nepomuk::Resource> refs = res.property(Nepomuk::Vocabulary::NBIB::reference()).toResourceList();

                foreach(const Nepomuk::Resource & r, refs) {
                    QString citykey = r.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
                    if(citykey.isEmpty()) {
                        citykey = i18n("unknown Citekey");
                    }
                    citekeySting.append(citykey);
                    citekeySting.append(QLatin1String("; "));
                }
                citekeySting.chop(2);
            }
            else {
                citekeySting = res.property(Nepomuk::Vocabulary::NBIB::citeKey()).toString();
            }
            newEntry = citekeySting;
            break;
        }
        case Column_StarRate: {
            int rating;

            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = res.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                rating = publication.rating();
            }
            else {
                rating = res.rating();
            }
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

QVariantList PublicationModel::createDecorationData(const Nepomuk::Resource & res)
{
    QVariantList decorationList;
    decorationList.reserve(Max_columns-1);

    for(int i = 0; i < Max_columns; i++) {
        QVariant newEntry;
        switch(i) {
        case Column_FileAvailable:
            if(m_selection == Resource_Reference) {
                Nepomuk::Resource publication = res.property(Nepomuk::Vocabulary::NBIB::publication()).toResource();
                Nepomuk::Resource file = publication.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResource();
                if(file.isValid()) {
                    newEntry = KIcon(QLatin1String("bookmarks-organize"));
                }
            }
            else {
                Nepomuk::Resource file = res.property(Nepomuk::Vocabulary::NBIB::isPublicationOf()).toResource();
                if(file.isValid()) {
                    newEntry = KIcon(QLatin1String("bookmarks-organize"));
                }
            }
            break;
        case Column_Reviewed:
            newEntry = KIcon(QLatin1String("dialog-ok-apply"));
            break;
        default:
            newEntry = QVariant();
        }

        decorationList.append(newEntry);
    }

    return decorationList;
}
