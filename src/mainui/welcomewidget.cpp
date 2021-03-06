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

#include "welcomewidget.h"

#include "config/bibglobals.h"
#include "../core/library.h"
#include "../core/projectsettings.h"
#include "../core/tagcloud.h"
#include "../core/models/nepomukmodel.h"

#include <KDE/KHTMLPart>
#include <KDE/KHTMLView>
#include <KDE/DOM/HTMLDocument>
#include <KDE/KStandardDirs>

#include <QtGui/QVBoxLayout>
#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QAbstractItemModel>

WelcomeWidget::WelcomeWidget(Library *library, QWidget *parent)
    : QWidget(parent)
    , m_library(library)
    , m_htmlPart(0)
{
    setupGui();

    connect(m_library->settings(), SIGNAL(projectDetailsChanged(Library*)), this, SLOT(generateHtml()));
}

WelcomeWidget::~WelcomeWidget()
{
    delete m_htmlPart;
}

void WelcomeWidget::setupGui()
{
    QVBoxLayout *layout = new QVBoxLayout;

    m_htmlPart = new KHTMLPart(this);
    m_htmlPart->setJScriptEnabled(true);
    m_htmlPart->setPluginsEnabled(true);
    layout->addWidget(m_htmlPart->view());
    setLayout(layout);

    generateHtml();

    QMap<BibGlobals::ResourceSelection, QSortFilterProxyModel*> models = m_library->viewModels();

    foreach (QSortFilterProxyModel *model, models) {
        QAbstractItemModel *aim = model->sourceModel();
        NepomukModel *m = qobject_cast<NepomukModel *>(aim);
        connect(m, SIGNAL(dataSizeChaged(int)),this, SLOT(updateStatistics()));
    }

    connect(m_library->tagCloud(), SIGNAL(tagCloudChanged()), this, SLOT(updateTagCloud()));

    updateTagCloud();
}

void WelcomeWidget::updateStatistics()
{
    QMapIterator<BibGlobals::ResourceSelection, QSortFilterProxyModel*> i(m_library->viewModels());
    while (i.hasNext()) {
        i.next();

        QAbstractItemModel *aim = i.value()->sourceModel();
        NepomukModel *m = qobject_cast<NepomukModel *>(aim);
        QString rowCount = QString::number(m->rowCount());

        QString jsFunction;
        switch(i.key()) {
        case BibGlobals::Resource_Document:
            jsFunction = QLatin1String("makeTxt('countdocuments','");
            jsFunction.append(rowCount);
            jsFunction.append(QLatin1String("')"));
            break;
        case BibGlobals::Resource_Mail:
            jsFunction = QLatin1String("makeTxt('countmail','");
            jsFunction.append(rowCount);
            jsFunction.append(QLatin1String("')"));
            break;
        case BibGlobals::Resource_Media:
            jsFunction = QLatin1String("makeTxt('countmedia','");
            jsFunction.append(rowCount);
            jsFunction.append(QLatin1String("')"));
            break;
        case BibGlobals::Resource_Reference:
            jsFunction = QLatin1String("makeTxt('countreference','");
            jsFunction.append(rowCount);
            jsFunction.append(QLatin1String("')"));
            break;
        case BibGlobals::Resource_Publication:
            jsFunction = QLatin1String("makeTxt('countpublication','");
            jsFunction.append(rowCount);
            jsFunction.append(QLatin1String("')"));
            break;
        case BibGlobals::Resource_Website:
            jsFunction = QLatin1String("makeTxt('countbookmark','");
            jsFunction.append(rowCount);
            jsFunction.append(QLatin1String("')"));
            break;
        case BibGlobals::Resource_Note:
            jsFunction = QLatin1String("makeTxt('countnote','");
            jsFunction.append(rowCount);
            jsFunction.append(QLatin1String("')"));
            break;
        case BibGlobals::Resource_Event:
            jsFunction = QLatin1String("makeTxt('countevents','");
            jsFunction.append(rowCount);
            jsFunction.append(QLatin1String("')"));
            break;
        case BibGlobals::Resource_Series:
            jsFunction = QLatin1String("makeTxt('countseries','");
            jsFunction.append(rowCount);
            jsFunction.append(QLatin1String("')"));
            break;
        case BibGlobals::Resource_SearchResults:
        case BibGlobals::Resource_Library:
        case BibGlobals::Max_ResourceTypes:
            break;
        }
        m_htmlPart->executeScript(m_htmlPart->htmlDocument(), jsFunction );
    }
}

void WelcomeWidget::updateTagCloud()
{
    QString tagCloud;
    QListIterator<QPair<int, QString> > k(m_library->tagCloud()->tagCloud());

    while (k.hasNext()) {
        QPair<int, QString> p = k.next();
        tagCloud.append(p.second);
        tagCloud.append(QLatin1String(" ("));
        tagCloud.append(QString::number(p.first));
        tagCloud.append(QLatin1String("), "));
    }
    tagCloud.chop(2);

    QString jsFunction = QLatin1String("setTagCloud('");
    jsFunction.append(tagCloud);
    jsFunction.append(QLatin1String("')"));
    m_htmlPart->executeScript(m_htmlPart->htmlDocument(), jsFunction );
}

void WelcomeWidget::generateHtml()
{
    QString htmlTemplate;
    if(m_library->libraryType() == BibGlobals::Library_System) {
        htmlTemplate = QLatin1String("html/index_system.html");
    }
    else {
        htmlTemplate = QLatin1String("html/index_library.html");
    }
    const QString htmlFilename = KGlobal::dirs()->findResource("appdata", htmlTemplate);
    const QString cssFilename = KGlobal::dirs()->findResource("appdata", QLatin1String("html/application.css"));
    QFile file(htmlFilename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        kDebug() << "can't open html file " << htmlFilename;
        return;
    }

    QTextStream in(&file);
    QString htmlPage;
    while (!in.atEnd()) {
        htmlPage.append( in.readLine() );
    }

    QString libraryName;
    QString libraryIntro;
    if(m_library->libraryType() == BibGlobals::Library_System) {
        libraryName = i18n("System Library");
        libraryIntro = i18n("The system library contains all known documents, publications, notes and so on that are stored in the Nepomuk Storage");
    }
    else {
        libraryName = m_library->settings()->name();
        libraryIntro = m_library->settings()->description();
        libraryIntro.replace(QLatin1String("\n"),QLatin1String("<br>"));
    }

    htmlPage.replace(QLatin1String("#CSSFILE#"), cssFilename);
    htmlPage.replace(QLatin1String("#LIBRARYNAME#"), libraryName);
    htmlPage.replace(QLatin1String("#LIBRARYINTRO#"), libraryIntro);
    htmlPage.replace(QLatin1String("#STATISTICHEADER#"), i18n("Statistics"));
    htmlPage.replace(QLatin1String("#TAGCLOUDHEADER#"), i18n("Topic Cloud"));

    if(m_library->libraryType() == BibGlobals::Library_System) {
        htmlPage.replace(QLatin1String("#LATESTPROJECTSHEADER#"), i18n("Latest Research"));
        htmlPage.replace(QLatin1String("#LATESTPROJECTS#"), QString());
    }

    m_htmlPart->begin();
    m_htmlPart->write(htmlPage);
    m_htmlPart->end();
}
