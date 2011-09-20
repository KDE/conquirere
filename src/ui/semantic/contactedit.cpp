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

#include "contactedit.h"

#include <Nepomuk/Variant>
#include <Nepomuk/Vocabulary/NCO>

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDebug>

ContactEdit::ContactEdit(QWidget *parent) :
    LabelEdit(parent)
{
}

ContactEdit::~ContactEdit()
{
}

void ContactEdit::updateResource(const QString & text)
{
    //BUG removeing all old authors and creating new resource of the inserted authors
    // will lead to duplicated persons with different resourceuri's!
    // remove all author informations
    resource().removeProperty( propertyUrl() );

    // create new contact resources for each fullname seperated by a ;
    //TODO make seperator configarable?
    QStringList contacts = text.split(QLatin1String(";"));

    foreach(QString s, contacts) {
        Nepomuk::Resource c(s, Nepomuk::Vocabulary::NCO::Contact());
        c.setProperty( Nepomuk::Vocabulary::NCO::fullname() , s);
        resource().addProperty( propertyUrl(), c);
    }
}

void ContactEdit::updateLabel()
{
    QList<Nepomuk::Resource> authors = resource().property( propertyUrl() ).toResourceList();

    QString labelText;
    foreach(Nepomuk::Resource r, authors) {
        //TODO don't use fullname() of contact but something else?

        labelText.append( r.property( Nepomuk::Vocabulary::NCO::fullname() ).toString() );
        labelText.append(QLatin1String("; "));
    }

    labelText.chop(2); // removes the last unused "; "

    setLabelText( labelText );
}
