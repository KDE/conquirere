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

#include "nepomukmodel.h"

#include "library.h"

#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/PIMO>

NepomukModel::NepomukModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_library(0)
{
    m_queryClient = new Nepomuk::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk::Query::Result>)), this, SLOT(addData(QList<Nepomuk::Query::Result>)));
    connect(m_queryClient, SIGNAL(entriesRemoved(QList<QUrl>)), this, SLOT(removeData(QList<QUrl>)));
    connect(m_queryClient, SIGNAL(resultCount(int)), this, SLOT(resultCount(int)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(listingsFinished()));
}

NepomukModel::~NepomukModel()
{
    m_queryClient->close();
    delete m_queryClient;

    m_fileList.clear();
}

int NepomukModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_fileList.size();
}

void NepomukModel::setLibrary(Library *library)
{
    m_library = library;
}

void NepomukModel::setResourceType(ResourceSelection selection)
{
    m_selection = selection;
}

Nepomuk::Resource NepomukModel::documentResource(const QModelIndex &selection)
{
    Nepomuk::Resource ret;

    if(!m_fileList.isEmpty() && selection.row() >= 0)
        ret = m_fileList.at(selection.row());

    return ret;
}

void NepomukModel::stopFetchData()
{
    m_queryClient->close();
}

void NepomukModel::removeSelectedFromProject(const QModelIndexList & indexes, Library *l)
{
    if(m_library->libraryType() == Library_System) {
        qWarning() << "try to remove data from the nepomuk system library @ PublicationModel::removeSelectedFromProject";
    }
    foreach(const QModelIndex & index, indexes) {
        // get the nepomuk data at the row
        Nepomuk::Resource nr = m_fileList.at(index.row());

        // remove project relation
        nr.removeProperty(Nepomuk::Vocabulary::PIMO::isRelated(), l->pimoLibrary());

        //Nepomuk query client will call the slot to remove the file from the index
    }
}

void NepomukModel::removeSelectedFromSystem(const QModelIndexList & indexes)
{
    foreach(const QModelIndex & index, indexes) {
        // get the nepomuk data at the row
        Nepomuk::Resource nr = m_fileList.at(index.row());

        //get all connected references
        //QList<Nepomuk::Resource> refList = nr.property(Nepomuk::Vocabulary::NBIB::)
        qWarning() << "TODO delete all references of the publication we are about to remove from nepomuk";
        // remove resource
        nr.remove();
        //Nepomuk query client will call the slot to remove the file from the index
    }
}

void NepomukModel::addData(const QList< Nepomuk::Query::Result > &entries)
{
    //qDebug() << "addData(...)" << entries.size();
    // two loops are necessary because addData is not only called on new entries, but with all changes
    // must be a bug in nepomuk check git master for different behaviour
    QList< Nepomuk::Resource > newEntries;
    foreach(const Nepomuk::Query::Result & r, entries) {
        if( !m_fileList.contains(r.resource()) ) {
            newEntries.append(r.resource());

            QList<Nepomuk::Tag> tagList = r.resource().tags();

            foreach(Nepomuk::Tag t, tagList) {
                emit hasTag(t.label());
            }
        }
    }

    if(newEntries.size() > 0) {
        beginInsertRows(QModelIndex(), m_fileList.size(), m_fileList.size() + newEntries.size()-1);
        m_fileList.append(newEntries);
        endInsertRows();
    }

    emit dataSizeChaged(m_fileList.size());
}

void NepomukModel::removeData( const QList< QUrl > &entries )
{
    // remove data is a bug by default
    // must change inplementation in the nepomuk query service

    // we just search through all data and remove the ones that are not valid anymore
    // this function gets called when new entries are created, some are removed or modified
    // sooner or later all deleted entries will be deleted

    QList<int> noValidEntries;
    for(int i = 0; i < m_fileList.size(); i++) {
        if(!m_fileList.at(i).isValid()) {
            noValidEntries.append(i);
        }
    }

    if(noValidEntries.isEmpty()) {
        return;
    }

    qDebug() << "remove values" << noValidEntries << "list size" << m_fileList.size();
    qDebug() << noValidEntries.first() << noValidEntries.last();
    beginRemoveRows(QModelIndex(), noValidEntries.first(), noValidEntries.last() );
    for(int j = 0; j < noValidEntries.size(); j++) {
        m_fileList.removeAt(noValidEntries.at(j) - j);
    }
    endRemoveRows();

    emit dataSizeChaged(m_fileList.size());
}

void NepomukModel::resultCount(int number)
{
    if(number == 0) {
        emit updateFetchDataFor(m_selection,false, m_library);
    }
}

void NepomukModel::listingsFinished()
{
    qDebug() << "listingsFinished" << "added something? oO" << m_fileList.size();

    emit updateFetchDataFor(m_selection,false, m_library);
}

void NepomukModel::listingsError(const QString & errorMessage)
{
    qDebug() << "query in rescourcemodel failed" << errorMessage;
}
