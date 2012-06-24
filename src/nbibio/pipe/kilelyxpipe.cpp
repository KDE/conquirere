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

#include "kilelyxpipe.h"

#include "nbibio/conquirere.h"

#include <kbibtex/element.h>
#include <kbibtex/entry.h>

#include "nbib.h"
#include <Nepomuk2/Resource>
#include <Nepomuk2/Variant>

#include <KDE/KConfig>
#include <KDE/KConfigGroup>
#include <KDE/KDebug>

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

KileLyxPipe::KileLyxPipe()
    :m_errorLog(new QStringList)
{
}

KileLyxPipe::~KileLyxPipe()
{
    delete m_errorLog;
    m_errorLog = 0;
}

void KileLyxPipe::setErrorLog(QStringList *errorLog)
{
    m_errorLog = errorLog;
}

void KileLyxPipe::pipeExport(File & bibEntries)
{
    QString refs;
    foreach(const QSharedPointer<Element> &e, bibEntries) {
        const Entry *bibEntry = dynamic_cast<const Entry *>(e.data());
        if(bibEntry) {
            refs.append(bibEntry->id());
            refs.append(QLatin1String(", "));
        }
    }
    refs.chop(2);

    sendReferences(refs);
}

void KileLyxPipe::pipeExport(QList<Nepomuk2::Resource> resources)
{
    QStringList refList;
    foreach(const Nepomuk2::Resource &r, resources) {
        if(r.hasType(Nepomuk2::Vocabulary::NBIB::Publication())) {
            QList<Nepomuk2::Resource> references = r.property(Nepomuk2::Vocabulary::NBIB::reference()).toResourceList();
            foreach(const Nepomuk2::Resource &refs, references) {
                refList.append(refs.property(Nepomuk2::Vocabulary::NBIB::citeKey()).toString());
            }
        }
        else if(r.hasType(Nepomuk2::Vocabulary::NBIB::Reference())) {
            refList.append(r.property(Nepomuk2::Vocabulary::NBIB::citeKey()).toString());
        }
        else {
            m_errorLog->append(i18n("No valid resource used to create citekeys from it"));
        }
    }

    sendReferences(refList.join(QLatin1String(", ")));
}

QString KileLyxPipe::findLyXKilePipe()
{
    return ConqSettings::kileLyXPipe();
}

void KileLyxPipe::sendReferences(const QString &refs)
{
    // taken mostly from KBibTeX lyx.cpp

    const QString defaultHintOnLyXProblems = i18n("Check that LyX or Kile are running and configured to receive references.");
    /// LyX pipe name has to determined always fresh in case LyX or Kile exited
    const QString pipeName = findLyXKilePipe();

    if (pipeName.isEmpty()) {
        m_errorLog->append(i18n("No \"LyX/Kile server pipe\" was detected."));
        m_errorLog->append(defaultHintOnLyXProblems);
        return;
    }

    if (refs.isEmpty()) {
        m_errorLog->append(i18n("No references to send to LyX/Kile."));
        return;
    }

    QFile pipe(pipeName);
    if (!QFileInfo(pipeName).exists() || !pipe.open(QFile::WriteOnly)) {
        m_errorLog->append(i18n("Could not open LyX/Kile server pipe \"%1\".", pipeName));
        m_errorLog->append(defaultHintOnLyXProblems);
        return;
    }

    QTextStream ts(&pipe);
    QString msg = QString("LYXCMD:kbibtex:citation-insert:%1").arg( refs );

    ts << msg << endl;
    ts.flush();

    pipe.close();
}
