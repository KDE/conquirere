/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehrichs@gmx.de>
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

#include "tagcloud.h"

#include "library.h"
#include "projectsettings.h"
#include "config/conquirere.h"

#include <Nepomuk2/Variant>
#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/ResourceWatcher>
#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include "nbib.h"
#include <Soprano/Vocabulary/NAO>

using namespace Nepomuk2::Vocabulary;
using namespace Soprano::Vocabulary;



/*
select distinct ?label (COUNT(?label) as ?rcount) {

?r a nbib:Publication .
?r nao:hasTopic ?topic .
?topic nao:prefLabel ?label .

}

Order By DESC (?rcount)
LIMIT 20
*/






TagCloud::TagCloud(Library *lib, QObject *parent)
    : QObject(parent)
    , m_library(lib)
    , m_pauseUpdates(false)
{
    setup();
}

TagCloud::~TagCloud()
{
    m_newWatcher->stop();
    delete m_newWatcher;
}

void TagCloud::setup()
{
    // watch for new publications
    m_newWatcher = new Nepomuk2::ResourceWatcher(this);
    m_newWatcher->addType(NBIB::Publication());
    //FIXME: add/remove resource from changewatcher
    connect(m_newWatcher, SIGNAL(resourceCreated(Nepomuk2::Resource,QList<QUrl>)), this, SLOT(generateCloud()) );
    connect(m_newWatcher, SIGNAL(resourceCreated(Nepomuk2::Resource,QList<QUrl>)), this, SLOT(addToWatcher(Nepomuk2::Resource,QList<QUrl>)) );
    connect(m_newWatcher, SIGNAL(resourceRemoved(QUrl,QList<QUrl>)), this, SLOT(generateCloud()) );
    m_newWatcher->start();

    // call for items when added/removed from a library
    m_libWatcher = new Nepomuk2::ResourceWatcher(this);
    m_libWatcher->addType(NBIB::Publication());
    m_libWatcher->addProperty(NAO::isRelated());
    //FIXME: add/remove resource from libwatcher
    connect(m_libWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(generateCloud()) );
    connect(m_libWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(generateCloud()) );
    m_libWatcher->start();

    // watch existing resources when hasTopic changes
    m_changeWatcher = new Nepomuk2::ResourceWatcher(this);
    m_changeWatcher->addProperty(NAO::hasTopic());
    connect(m_changeWatcher, SIGNAL(propertyChanged(Nepomuk2::Resource,Nepomuk2::Types::Property,QVariantList,QVariantList)),
            this, SLOT(generateCloud()) );

    QString hideTypes;
    // add a filter to hide several publication types
    foreach(int i, ConqSettings::hiddenNbibPublications()) {
        hideTypes.append(QString(" FILTER NOT EXISTS { ?r a <%1> . } ").arg(BibGlobals::BibEntryTypeURL((BibGlobals::BibEntryType)i).toString()));
    }

    // fetch all publications for this library
    // helping string to filter for all documents that are related to the current project
    QString projectRelated;
    QString projectTag;
    if(m_library->libraryType() == BibGlobals::Library_Project) {
        projectRelated = QString("?r nao:isRelated  <%1> .").arg(m_library->settings()->projectThing().uri().toString());
        projectTag = QString("UNION { ?r nao:hasTag  <%1> . }").arg(m_library->settings()->projectTag().uri().toString() );
    }

    QString query = QString::fromLatin1("select distinct ?r where { {"
                                        " { ?r a nbib:Publication . "  + hideTypes.toLatin1() + " }"
                                        + projectRelated.toLatin1() + " }" + projectTag.toLatin1() +
                                        "}");

    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    while( it.next() ) {
        Soprano::BindingSet p = it.current();

        m_changeWatcher->addResource( Nepomuk2::Resource::fromResourceUri(p.value("r").toString()));
    }
    m_libWatcher->start();

    generateCloud();
}

void TagCloud::generateCloud()
{
    m_tagCloud.clear();

    QString hideTypes;
    // add a filter to hide several publication types
    foreach(int i, ConqSettings::hiddenNbibPublications()) {
        hideTypes.append(QString(" FILTER NOT EXISTS { ?r a <%1> . } ").arg(BibGlobals::BibEntryTypeURL((BibGlobals::BibEntryType)i).toString()));
    }

    QString projectRelated;
    QString projectTag;
    if(m_library->libraryType() == BibGlobals::Library_Project) {
        projectRelated = QString("?r nao:isRelated  <%1> .").arg(m_library->settings()->projectThing().uri().toString());
        projectTag = QString("UNION { ?r nao:hasTag  <%1> . }").arg(m_library->settings()->projectTag().uri().toString() );
    }

    QString query = QString::fromLatin1("select distinct ?label (COUNT(?label) as ?count) where { {"
                                        " { ?r a nbib:Publication . "  + hideTypes.toLatin1() + " }"
                                        "?r nao:hasTopic ?topic ."
                                        "?topic nao:prefLabel ?label ."
                                        + projectRelated.toLatin1() + " }" + projectTag.toLatin1() +
                                        "}"
                                        "Order By DESC (?count)"
                                        "LIMIT 20");
/*
    Soprano::Model* model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );

    while( it.next() ) {
        Soprano::BindingSet p = it.current();
        // the count will be devided by 2 beacuse the SPARQL results in double counting
        // seems any ?label will be listed twice as muc has it really appears
        m_tagCloud.append(QPair<int, QString>( p.value("count").toString().toInt() / 2,
                                              p.value("label").toString()));
    }
*/
    emit tagCloudChanged();
}

void TagCloud::addToWatcher(Nepomuk2::Resource resource ,QList<QUrl> types)
{
    Q_UNUSED(types);

    m_changeWatcher->addResource( resource );
}

QList<QPair<int, QString> > TagCloud::tagCloud()
{
    return m_tagCloud;
}

void TagCloud::pauseUpdates(bool pause)
{
    m_newWatcher->stop();
    m_libWatcher->stop();
    m_changeWatcher->stop();

    m_pauseUpdates = pause;

    if(!m_pauseUpdates) {
        m_newWatcher->start();
        m_libWatcher->start();
        m_changeWatcher->start();

        generateCloud();
    }
}
