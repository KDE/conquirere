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

#include "websiteedit.h"

#include "adddataobject.h"

#include <Nepomuk/Vocabulary/NIE>
#include <Nepomuk/Resource>
#include <Nepomuk/Variant>

#include <KDE/KUrl>

#include <KDE/KDebug>

#include <QtCore/QPointer>

using namespace Nepomuk::Vocabulary;

WebsiteEdit::WebsiteEdit(QWidget *parent) :
    PropertyEdit(parent)
{
    setDirectEdit(false);
    setUseDetailDialog(true);

    connect(this, SIGNAL(externalEditRequested(Nepomuk::Resource&,QUrl)), this, SLOT(showUrlSelection()));
}

void WebsiteEdit::setupLabel()
{
    QString showList;
    QList<Nepomuk::Resource> dataObjectList = resource().property(NIE::links()).toResourceList();

    foreach(const Nepomuk::Resource &r, dataObjectList) {
        KUrl url(r.property(NIE::url()).toString());
        showList.append( url.prettyUrl(KUrl::RemoveTrailingSlash));
        showList.append(QLatin1String("; "));
    }

    showList.chop(2);

    setLabelText(showList);
}

void WebsiteEdit::updateResource(const QString & newWebsite)
{
    Q_UNUSED(newWebsite);
}

void WebsiteEdit::showUrlSelection()
{
    QPointer<AddDataObject> ado = new AddDataObject(this);

    ado->setMode(AddDataObject::Website);
    ado->setResource(resource());
    ado->fillListWidget();

    ado->exec();

    delete ado;

    setupLabel();
}
