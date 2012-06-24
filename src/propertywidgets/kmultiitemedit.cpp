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

#include "kmultiitemedit.h"

#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>
#include <Soprano/Vocabulary/NAO>

#include <KDE/KCompletionBox>
#include <KDE/KDebug>

KMultiItemEdit::KMultiItemEdit(QWidget *parent)
    : KLineEdit(parent)
    , m_nepomukDataFetched(false)
{
    completionObject()->setIgnoreCase(true);

    //query client for the auto completion
    m_queryClient = new Nepomuk2::Query::QueryServiceClient();
    connect(m_queryClient, SIGNAL(newEntries(QList<Nepomuk2::Query::Result>)), this, SLOT(addToCompleterList(QList<Nepomuk2::Query::Result>)));
    connect(m_queryClient, SIGNAL(finishedListing()), this, SLOT(queryFinished()));

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(startCompleterSearch()));
    connect(this, SIGNAL(editingFinished()), this, SLOT(clearCompleterData()));

    m_labelProperty = Soprano::Vocabulary::NAO::prefLabel();
}

void KMultiItemEdit::setNepomukCompleterRange(KUrl classRange)
{
    m_range = classRange;
}

void KMultiItemEdit::setNepomukCompleterLabel(KUrl label)
{
    m_labelProperty = label;
}

void KMultiItemEdit::makeCompletion(const QString &substring)
{
    KCompletion *comp = compObj();
    KGlobalSettings::Completion mode = completionMode();

    if ( !comp || mode == KGlobalSettings::CompletionNone )
        return;  // No completion object...

    const QString match = comp->makeCompletion( currentSubstring() );

    if ( mode == KGlobalSettings::CompletionPopup ||
    mode == KGlobalSettings::CompletionPopupAuto )
    {
        if ( match.isEmpty() )
        {
            if ( completionBox() )
            {
                completionBox() ->hide();
                completionBox() ->clear();
            }
        }
        else
            setCompletedItems( comp->allMatches() );
    }
    else // Auto,  ShortAuto (Man) and Shell
    {
        // all other completion modes
        // If no match or the same match, simply return without completing.
        if ( match.isEmpty() || match == currentSubstring() )
            return;

        if ( mode != KGlobalSettings::CompletionShell )
            setUserSelection(false);

        if ( autoSuggest() )
            KLineEdit::setCompletedText( match );
    }

}
void KMultiItemEdit::_k_slotCompletionBoxTextChanged( const QString& text )
{
    setCompletedText(text, false);
}

void KMultiItemEdit::setCompletedText( const QString& completedText, bool marked )
{
    int startSelection = 0;
    QString exitString;
    // get current list
    QStringList entrylist = text().split(QLatin1String(";"));

    // get the position of the cursor
    int pos = cursorPosition();

    int tempLength = 0;
    // find the substring that is being manipulated and replace it with the selected suggestion
    bool inserted = false;
    int curentry = 1;
    foreach(const QString & s, entrylist) {
        tempLength += s.length();
        if( !inserted && tempLength >= pos-1) {
            startSelection =tempLength;
            exitString.append( completedText );
            inserted = true;
        }
        else {
            exitString.append(s.trimmed());
        }

        exitString.append(QLatin1String("; "));
        // helps to adjust to all the whitespaces that come from the "; " so we still add it to the right place
        tempLength += curentry;
        curentry++;
    }

    exitString.chop(2);

    setText(exitString);

    if ( marked ) {
        setSelection(startSelection, completedText.length());
    }
    setUserSelection(false);
}

QString KMultiItemEdit::currentSubstring() const
{
    QString subString;
    //we take full string and split it at delimiter
    QStringList entrylist = this->text().split(QLatin1String(";"));

    // get the position of the cursor
    int pos = this->cursorPosition();

    int tempLength = 0;
    int substringPos = 0;
    // find the substring that is being manipulated
    foreach(const QString & s, entrylist) {
        tempLength += s.length();
        if(tempLength >= pos-1) {
            subString = s;
            if(tempLength != pos) {
                substringPos = tempLength - pos;
            }
            else {
                substringPos = tempLength;
            }
            break;
        }
        tempLength++; //adds the ";" to the length
    }

    //remove any string after text position
    subString = subString.left(substringPos).trimmed();

    return subString;
}

void KMultiItemEdit::startCompleterSearch()
{
    QString css = currentSubstring();

    if(css.size() < 1) {
        clearCompleterData();
        return;
    }

    if(!m_nepomukDataFetched && !m_range.isEmpty()) {

        QString css = currentSubstring();

        compObj()->clear();
        m_nepomukDataFetched = true;

        QString query ="select DISTINCT ?r where {"
                       "?r a <" + m_range.url() + "> ."
                       "?r <" + m_labelProperty.url() + "> ?name ."
                       "Filter ( regex(?name, \"^" + css + "\", \"i\"))"
                       "}";

        m_queryClient->sparqlQuery( query );
    }
}

void KMultiItemEdit::clearCompleterData()
{
    if(m_nepomukDataFetched) {
        compObj()->clear();
        m_queryClient->close();
        m_nepomukDataFetched = false;
    }
}

void KMultiItemEdit::addToCompleterList( const QList< Nepomuk2::Query::Result > &entries )
{
    foreach(const Nepomuk2::Query::Result &nqr, entries) {
        completionObject()->addItem( nqr.resource().property( m_labelProperty ).toString() );
    }
}

void KMultiItemEdit::queryFinished()
{
    m_queryClient->close();
    makeCompletion( currentSubstring() );
}
