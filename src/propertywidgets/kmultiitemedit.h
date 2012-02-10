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

#ifndef KMULTIITEMEDIT_H
#define KMULTIITEMEDIT_H

#include <KDE/KLineEdit>

#include <Nepomuk/Query/QueryServiceClient>
#include <Nepomuk/Query/Result>

class KMultiItemEdit : public KLineEdit
{
    Q_OBJECT
public:
    explicit KMultiItemEdit(QWidget *parent = 0);

    void setNepomukCompleterRange(KUrl classRange);
    void setNepomukCompleterLabel(KUrl label);

protected slots:
    void makeCompletion (const QString &substring);
    void addToCompleterList( const QList< Nepomuk::Query::Result > &entries );

private slots:
    void _k_slotCompletionBoxTextChanged( const QString& text );

    void startCompleterSearch();
    void clearCompleterData();
    void queryFinished();

private:
    void setCompletedText( const QString& t, bool marked );
    QString currentSubstring() const;

    KUrl m_range;
    KUrl m_labelProperty;
    bool m_nepomukDataFetched;
    Nepomuk::Query::QueryServiceClient *m_queryClient;
};

#endif // KMULTIITEMEDIT_H
