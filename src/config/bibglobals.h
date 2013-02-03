#ifndef BIBGLOBALS_H
#define BIBGLOBALS_H

#include <Nepomuk2/Resource>

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUrl>

class BibGlobals
{
public:
    enum ProjectTreeRole {
        Role_LibraryType = Qt::UserRole,    /**< keeps the LibraryType */
        Role_ResourceType,                  /**< keeps one of the ResourceSelection entries */
        Role_ResourceFilter,                /**< keeps one of the BibEntryType entries */
        Role_ProjectName,                   /**< keeps the name of the conected project */
        Role_ProjectThing                   /**< keeps the pimo::thing uri conected project so we can identify it*/
    };

    enum LibraryType {
        Library_System,
        Library_Project
    };

    enum ResourceSelection {
        Resource_SearchResults,
        Resource_Library,
        Resource_Document,
        Resource_Mail,
        Resource_Media,
        Resource_Reference,
        Resource_Publication,
        Resource_Series,
        Resource_Website,
        Resource_Note,
        Resource_Event,

        Max_ResourceTypes
    };

    enum BibEntryType {
        BibType_Article = 0,       /**< An article from a journal or magazine. */
        BibType_Book,              /**< A book with an explicit publisher. */
        BibType_Booklet,           /**< A work that is printed and bound, but without a named publisher or sponsoring institution. */
        BibType_Collection,        /**< A collection of different publications */
        BibType_Encyclopedia,      /**< A encyclopedia publications */
        BibType_Dictionary,        /**< A dictionary publications */
        BibType_Bachelorthesis,    /**< A Bachelor thesis. */
        BibType_Mastersthesis,     /**< A Master's thesis. */
        BibType_Phdthesis,         /**< A PhD thesis. */
        BibType_Thesis,            /**< any kind of thesis */
        BibType_Report,
        BibType_Techreport,        /**< A report published by a school or other institution, usually numbered within a series. */
        BibType_Electronic ,       /**< A electronic publication */
        BibType_Forum ,            /**< A electronic publication */
        BibType_ForumPost ,        /**< A electronic publication */
        BibType_Blog ,             /**< A electronic publication */
        BibType_BlogPost ,         /**< A electronic publication */
        BibType_WebPage ,          /**< A electronic publication */
        BibType_WebSite ,          /**< A electronic publication */
        BibType_Script,
        BibType_Presentation,
        BibType_Unpublished,       /**< A document having an author and title, but not formally published. */
        BibType_Misc,              /**< Use this type when nothing else fits. */
        BibType_Manual,            /**< Technical documentation. */
        BibType_Proceedings,       /**< The proceedings of a conference. */
        BibType_JournalIssue,      /**< An issue of journal */
        BibType_NewspaperIssue,    /**< An issue of a newspaper */
        BibType_MagazinIssue,      /**< An issue of a magazine */
        BibType_Standard,
        BibType_Patent,            /**< A patent. */
        BibType_CodeOfLaw,
        BibType_CourtReporter,
        BibType_Legislation,
        BibType_Bill,
        BibType_Statute,
        BibType_LegalCaseDocument,
        BibType_Decision,
        BibType_Brief,
        BibType_Map,

        Max_BibTypes
    };

    static const QString BibEntryTypeTranslation(BibEntryType type);
    static const QString BibEntryTypeIcon(BibEntryType type);
    static const QUrl BibEntryTypeURL(BibEntryType type);
    static BibEntryType BibEntryTypeFromUrl(const Nepomuk2::Resource & resource);
    static BibEntryType BibEntryTypeFromUrl(const QList<QUrl> & urlList);


    enum SeriesType {
        SeriesType_Series = 0,
        SeriesType_BookSeries,
        SeriesType_Journal,
        SeriesType_Magazin,
        SeriesType_Newspaper,

        Max_SeriesTypes
    };

    static const QString SeriesTypeTranslation(SeriesType type);
    static const QString SeriesTypeIcon(SeriesType type) ;
    static const QUrl SeriesTypeURL(SeriesType type);
    static SeriesType SeriesTypeFromUrl(const QList<QUrl> & urllist);
    static SeriesType SeriesTypeFromUrl(const Nepomuk2::Resource & resource);

    static const QStringList keysForPersonDetection();
};

#endif // BIBGLOBALS_H
