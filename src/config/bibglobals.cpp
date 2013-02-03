#include "bibglobals.h"

#include "nbib.h"

#include <KDE/KDebug>

const QString BibGlobals::BibEntryTypeTranslation(BibEntryType type)
{
    switch(type) {
    case BibType_Article:
        return I18N_NOOP("Article");
    case BibType_Book:
        return I18N_NOOP("Book");
    case BibType_Booklet:
        return I18N_NOOP("Booklet");
    case BibType_Collection:
        return I18N_NOOP("Collection");
    case BibType_Encyclopedia:
        return I18N_NOOP("Encyclopedia");
    case BibType_Dictionary:
        return I18N_NOOP("Dictionary");
    case BibType_Bachelorthesis:
        return I18N_NOOP("Bachelor thesis");
    case BibType_Mastersthesis:
        return I18N_NOOP("Master thesis");
    case BibType_Phdthesis:
        return I18N_NOOP("Phd thesis");
    case BibType_Thesis:
        return I18N_NOOP("Thesis");
    case BibType_Report:
        return I18N_NOOP("Report");
    case BibType_Techreport:
        return I18N_NOOP("Techreport");
    case BibType_Electronic:
        return I18N_NOOP("Electronic");
    case BibType_Forum:
        return I18N_NOOP("Forum");
    case BibType_ForumPost:
        return I18N_NOOP("Forum post");
    case BibType_Blog:
        return I18N_NOOP("Blog");
    case BibType_BlogPost:
        return I18N_NOOP("Blog post");
    case BibType_WebPage:
        return I18N_NOOP("Webpage");
    case BibType_WebSite:
        return I18N_NOOP("Website");
    case BibType_Script:
        return I18N_NOOP("Script");
    case BibType_Presentation:
        return I18N_NOOP("Presentation");
    case BibType_Unpublished:
        return I18N_NOOP("Unpublished");
    case BibType_Misc:
        return I18N_NOOP("Misc");
    case BibType_Manual:
        return I18N_NOOP("Manual");
    case BibType_Proceedings:
        return I18N_NOOP("Proceedings");
    case BibType_JournalIssue:
        return I18N_NOOP("Journal Issue");
    case BibType_NewspaperIssue:
        return I18N_NOOP("Newspaper Issue");
    case BibType_MagazinIssue:
        return I18N_NOOP("Magazin Issue");
    case BibType_Standard:
        return I18N_NOOP("Standard");
    case BibType_Patent:
        return I18N_NOOP("Patent");
    case BibType_CodeOfLaw:
        return I18N_NOOP("Code");
    case BibType_CourtReporter:
        return I18N_NOOP("CourtReporter");
    case BibType_Legislation:
        return I18N_NOOP("Legislation");
    case BibType_Bill:
        return I18N_NOOP("Bill");
    case BibType_Statute:
        return I18N_NOOP("Statute");
    case BibType_LegalCaseDocument:
        return I18N_NOOP("LegalCaseDocument");
    case BibType_Decision:
        return I18N_NOOP("Decision");
    case BibType_Brief:
        return I18N_NOOP("Brief");
    case BibType_Map:
        return I18N_NOOP("Map");

    case Max_BibTypes:
        kFatal("Request for Max_BibTypes where it shouldn't be");
        return QLatin1String("BUG DETECTED");
    }

    kFatal("Request for unknown BibTypes");
    return QLatin1String("BUG DETECTED");
}

const QString BibGlobals::BibEntryTypeIcon(BibEntryType type)
{
    switch(type) {
    case BibType_Article:
        return QLatin1String("knewsticker");
    case BibType_Book:
        return QLatin1String("acroread");
    case BibType_Booklet:
        return QLatin1String("acroread");
    case BibType_Collection:
        return QLatin1String("Collection");
    case BibType_Encyclopedia:
        return QLatin1String("accessories-dictionary");
    case BibType_Dictionary:
        return QLatin1String("accessories-dictionary");
    case BibType_Bachelorthesis:
        return QLatin1String("applications-education-university");
    case BibType_Mastersthesis:
        return QLatin1String("applications-education-university");
    case BibType_Phdthesis:
        return QLatin1String("applications-education-university");
    case BibType_Thesis:
        return QLatin1String("applications-education-university");
    case BibType_Report:
        return QLatin1String("kjournal");
    case BibType_Techreport:
        return QLatin1String("kjournal");
    case BibType_Electronic:
        return QLatin1String("applications-internet");
    case BibType_Forum:
        return QLatin1String("applications-internet");
    case BibType_ForumPost:
        return QLatin1String("applications-internet");
    case BibType_Blog:
        return QLatin1String("applications-internet");
    case BibType_BlogPost:
        return QLatin1String("applications-internet");
    case BibType_WebPage:
        return QLatin1String("applications-internet");
    case BibType_WebSite:
        return QLatin1String("applications-internet");
    case BibType_Script:
        return QLatin1String("kjournal");
    case BibType_Presentation:
        return QLatin1String("kpresenter");
    case BibType_Unpublished:
        return QLatin1String("acroread");
    case BibType_Misc:
        return QLatin1String("acroread");
    case BibType_Manual:
        return QLatin1String("kjournal");
    case BibType_Proceedings:
        return QLatin1String("preferences-system-time");
    case BibType_JournalIssue:
        return QLatin1String("kjournal");
    case BibType_NewspaperIssue:
        return QLatin1String("knewsticker");
    case BibType_MagazinIssue:
        return QLatin1String("kjournal");
    case BibType_Standard:
        return QLatin1String("knewsticker");
    case BibType_Patent:
        return QLatin1String("knewsticker");
    case BibType_CodeOfLaw:
        return QLatin1String("knewsticker");
    case BibType_CourtReporter:
        return QLatin1String("knewsticker");
    case BibType_Legislation:
        return QLatin1String("knewsticker");
    case BibType_Bill:
        return QLatin1String("knewsticker");
    case BibType_Statute:
        return QLatin1String("knewsticker");
    case BibType_LegalCaseDocument:
        return QLatin1String("knewsticker");
    case BibType_Decision:
        return QLatin1String("knewsticker");
    case BibType_Brief:
        return QLatin1String("knewsticker");
    case BibType_Map:
        return QLatin1String("services");

    case Max_BibTypes:
        kFatal("Request for Max_BibTypes where it shouldn't be");
        return QLatin1String("services");
    }

    kFatal("Request for unknown BibTypes");
    return QLatin1String("services");
}

const QUrl BibGlobals::BibEntryTypeURL(BibEntryType type)
{
    switch(type) {
    case BibType_Article:
        return Nepomuk2::Vocabulary::NBIB::Article();
    case BibType_Book:
        return Nepomuk2::Vocabulary::NBIB::Book();
    case BibType_Booklet:
        return Nepomuk2::Vocabulary::NBIB::Booklet();
    case BibType_Collection:
        return Nepomuk2::Vocabulary::NBIB::Collection();
    case BibType_Encyclopedia:
        return Nepomuk2::Vocabulary::NBIB::Encyclopedia();
    case BibType_Dictionary:
        return Nepomuk2::Vocabulary::NBIB::Dictionary();
    case BibType_Bachelorthesis:
        return Nepomuk2::Vocabulary::NBIB::BachelorThesis();
    case BibType_Mastersthesis:
        return Nepomuk2::Vocabulary::NBIB::MastersThesis();
    case BibType_Phdthesis:
        return Nepomuk2::Vocabulary::NBIB::PhdThesis();
    case BibType_Thesis:
        return Nepomuk2::Vocabulary::NBIB::Thesis();
    case BibType_Report:
        return Nepomuk2::Vocabulary::NBIB::Report();
    case BibType_Techreport:
        return Nepomuk2::Vocabulary::NBIB::Techreport();
    case BibType_Electronic:
        return Nepomuk2::Vocabulary::NBIB::Electronic();
    case BibType_Forum:
        return Nepomuk2::Vocabulary::NBIB::Forum();
    case BibType_ForumPost:
        return Nepomuk2::Vocabulary::NBIB::ForumPost();
    case BibType_Blog:
        return Nepomuk2::Vocabulary::NBIB::Blog();
    case BibType_BlogPost:
        return Nepomuk2::Vocabulary::NBIB::BlogPost();
    case BibType_WebPage:
        return Nepomuk2::Vocabulary::NBIB::Webpage();
    case BibType_WebSite:
        return Nepomuk2::Vocabulary::NBIB::Website();
    case BibType_Script:
        return Nepomuk2::Vocabulary::NBIB::Script();
    case BibType_Presentation:
        return Nepomuk2::Vocabulary::NBIB::Presentation();
    case BibType_Unpublished:
        return Nepomuk2::Vocabulary::NBIB::Unpublished();
    case BibType_Misc:
        return Nepomuk2::Vocabulary::NBIB::Publication();
    case BibType_Manual:
        return Nepomuk2::Vocabulary::NBIB::Manual();
    case BibType_Proceedings:
        return Nepomuk2::Vocabulary::NBIB::Proceedings();
    case BibType_JournalIssue:
        return Nepomuk2::Vocabulary::NBIB::JournalIssue();
    case BibType_NewspaperIssue:
        return Nepomuk2::Vocabulary::NBIB::NewspaperIssue();
    case BibType_MagazinIssue:
        return Nepomuk2::Vocabulary::NBIB::MagazinIssue();
    case BibType_Standard:
        return Nepomuk2::Vocabulary::NBIB::Standard();
    case BibType_Patent:
        return Nepomuk2::Vocabulary::NBIB::Patent();
    case BibType_CodeOfLaw:
        return Nepomuk2::Vocabulary::NBIB::CodeOfLaw();
    case BibType_CourtReporter:
        return Nepomuk2::Vocabulary::NBIB::CourtReporter();
    case BibType_Legislation:
        return Nepomuk2::Vocabulary::NBIB::Legislation();
    case BibType_Bill:
        return Nepomuk2::Vocabulary::NBIB::Bill();
    case BibType_Statute:
        return Nepomuk2::Vocabulary::NBIB::Statute();
    case BibType_LegalCaseDocument:
        return Nepomuk2::Vocabulary::NBIB::LegalCaseDocument();
    case BibType_Decision:
        return Nepomuk2::Vocabulary::NBIB::Decision();
    case BibType_Brief:
        return Nepomuk2::Vocabulary::NBIB::Brief();
    case BibType_Map:
        return Nepomuk2::Vocabulary::NBIB::Map();

    case Max_BibTypes:
        kFatal("Request for Max_BibTypes where it shouldn't be");
        return Nepomuk2::Vocabulary::NBIB::Publication();
    }

    kFatal("Request for unknown BibTypes");
    return Nepomuk2::Vocabulary::NBIB::Publication();
}

BibGlobals::BibEntryType BibGlobals::BibEntryTypeFromUrl(const Nepomuk2::Resource & resource)
{
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Proceedings())) {
        return BibType_Proceedings;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::MastersThesis())) {
        return BibType_Mastersthesis;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::PhdThesis())) {
        return BibType_Phdthesis;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::BachelorThesis())) {
        return BibType_Bachelorthesis;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Thesis())) {
        return BibType_Thesis;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Book())) {
        return BibType_Book;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Booklet())) {
        return BibType_Booklet;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Manual())) {
        return BibType_Manual;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Techreport())) {
        return BibType_Techreport;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Unpublished())) {
        return BibType_Unpublished;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Patent())) {
        return BibType_Patent;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Forum())) {
        return BibType_Forum;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::ForumPost())) {
        return BibType_ForumPost;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Blog())) {
        return BibType_Blog;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::BlogPost())) {
        return BibType_BlogPost;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Webpage())) {
        return BibType_WebPage;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Website())) {
        return BibType_WebSite;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::JournalIssue())) {
        return BibType_JournalIssue;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::NewspaperIssue())) {
        return BibType_NewspaperIssue;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::MagazinIssue())) {
        return BibType_MagazinIssue;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Report())) {
        return BibType_Report;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Script())) {
        return BibType_Script;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Presentation())) {
        return BibType_Presentation;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Encyclopedia())) {
        return BibType_Encyclopedia;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Dictionary())) {
        return BibType_Dictionary;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Standard())) {
        return BibType_Standard;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Bill())) {
        return BibType_Bill;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Decision())) {
        return BibType_Decision;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Brief())) {
        return BibType_Brief;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Statute())) {
        return BibType_Statute;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::CodeOfLaw())) {
        return BibType_CodeOfLaw;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::CourtReporter())) {
        return BibType_CourtReporter;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::LegalCaseDocument())) {
        return BibType_LegalCaseDocument;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Legislation())) {
        return BibType_Legislation;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Collection())) {
        return BibType_Collection;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Electronic())) {
        return BibType_Electronic;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Map())) {
        return BibType_Map;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Article())) {
        return BibType_Article;
    }
    return BibType_Misc;
}

BibGlobals::BibEntryType BibGlobals::BibEntryTypeFromUrl(const QList<QUrl> & urlList)
{
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Proceedings())) {
        return BibType_Proceedings;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::MastersThesis())) {
        return BibType_Mastersthesis;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::PhdThesis())) {
        return BibType_Phdthesis;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::BachelorThesis())) {
        return BibType_Bachelorthesis;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Thesis())) {
        return BibType_Thesis;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Book())) {
        return BibType_Book;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Booklet())) {
        return BibType_Booklet;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Manual())) {
        return BibType_Manual;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Techreport())) {
        return BibType_Techreport;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Unpublished())) {
        return BibType_Unpublished;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Patent())) {
        return BibType_Patent;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Forum())) {
        return BibType_Forum;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::ForumPost())) {
        return BibType_ForumPost;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Blog())) {
        return BibType_Blog;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::BlogPost())) {
        return BibType_BlogPost;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Webpage())) {
        return BibType_WebPage;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Website())) {
        return BibType_WebSite;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::JournalIssue())) {
        return BibType_JournalIssue;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::NewspaperIssue())) {
        return BibType_NewspaperIssue;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::MagazinIssue())) {
        return BibType_MagazinIssue;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Report())) {
        return BibType_Report;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Script())) {
        return BibType_Script;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Presentation())) {
        return BibType_Presentation;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Encyclopedia())) {
        return BibType_Encyclopedia;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Dictionary())) {
        return BibType_Dictionary;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Standard())) {
        return BibType_Standard;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Bill())) {
        return BibType_Bill;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Decision())) {
        return BibType_Decision;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Brief())) {
        return BibType_Brief;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Statute())) {
        return BibType_Statute;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::CodeOfLaw())) {
        return BibType_CodeOfLaw;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::CourtReporter())) {
        return BibType_CourtReporter;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::LegalCaseDocument())) {
        return BibType_LegalCaseDocument;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Legislation())) {
        return BibType_Legislation;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Collection())) {
        return BibType_Collection;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Electronic())) {
        return BibType_Electronic;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Map())) {
        return BibType_Map;
    }
    if(urlList.contains(Nepomuk2::Vocabulary::NBIB::Article())) {
        return BibType_Article;
    }
    return BibType_Misc;
}

const QString BibGlobals::SeriesTypeTranslation(SeriesType type)
{
    switch(type) {
    case SeriesType_Series:
        return I18N_NOOP("Misc Series");
    case SeriesType_BookSeries:
        return I18N_NOOP("Book Series");
    case SeriesType_Journal:
        return I18N_NOOP("Journal");
    case SeriesType_Magazin:
        return I18N_NOOP("Magazin");
    case SeriesType_Newspaper:
        return I18N_NOOP("Newspaper");
    case Max_SeriesTypes:
        kFatal("Request for Max_SeriesTypes where it shouldn't be");
        return QLatin1String("BUG DETECTED");
    }

    kFatal("Request for unknown SeriesType");
    return QLatin1String("BUG DETECTED");
}

const QString BibGlobals::SeriesTypeIcon(SeriesType type)
{
    switch(type) {
    case SeriesType_Series:
        return QLatin1String("acroread");
    case SeriesType_BookSeries:
        return QLatin1String("acroread");
    case SeriesType_Journal:
        return QLatin1String("kjournal");
    case SeriesType_Magazin:
        return QLatin1String("kjournal");
    case SeriesType_Newspaper:
        return QLatin1String("knewsticker");
    case Max_SeriesTypes:
        kFatal("Request for Max_SeriesTypes where it shouldn't be");
        return QLatin1String("acroread");
    }

    kFatal("Request for unknown SeriesType");
    return QLatin1String("acroread");
}

const QUrl BibGlobals::SeriesTypeURL(SeriesType type)
{
    switch(type) {
    case SeriesType_Series:
        return Nepomuk2::Vocabulary::NBIB::Series();
    case SeriesType_BookSeries:
        return Nepomuk2::Vocabulary::NBIB::BookSeries();
    case SeriesType_Journal:
        return Nepomuk2::Vocabulary::NBIB::Journal();
    case SeriesType_Magazin:
        return Nepomuk2::Vocabulary::NBIB::Magazin();
    case SeriesType_Newspaper:
        return Nepomuk2::Vocabulary::NBIB::Newspaper();
    case Max_SeriesTypes:
        kFatal("Request for Max_SeriesTypes where it shouldn't be");
        return Nepomuk2::Vocabulary::NBIB::Series();
    }

    kFatal("Request for unknown SeriesType");
    return Nepomuk2::Vocabulary::NBIB::Series();
}

BibGlobals::SeriesType BibGlobals::SeriesTypeFromUrl(const QList<QUrl> & urllist)
{
    if(urllist.contains(Nepomuk2::Vocabulary::NBIB::BookSeries())) {
        return SeriesType_BookSeries;
    }
    if(urllist.contains(Nepomuk2::Vocabulary::NBIB::Journal())) {
        return SeriesType_Journal;
    }
    if(urllist.contains(Nepomuk2::Vocabulary::NBIB::Magazin())) {
        return SeriesType_Magazin;
    }
    if(urllist.contains(Nepomuk2::Vocabulary::NBIB::Newspaper())) {
        return SeriesType_Newspaper;
    }
    return SeriesType_Series;
}

BibGlobals::SeriesType BibGlobals::SeriesTypeFromUrl(const Nepomuk2::Resource & resource)
{
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::BookSeries())) {
        return SeriesType_BookSeries;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Journal())) {
        return SeriesType_Journal;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Magazin())) {
        return SeriesType_Magazin;
    }
    if(resource.hasType(Nepomuk2::Vocabulary::NBIB::Newspaper())) {
        return SeriesType_Newspaper;
    }
    return SeriesType_Series;
}

const QStringList BibGlobals::keysForPersonDetection()
{
    return QStringList() << QString("translator")
                         << QString("contributor")
                         << QString("reviewedauthor")
                         << QString("serieseditor")
                         << QString("bookauthor")
                         << QString("commenter")
                         << QString("counsel")
                         << QString("cosponsor")
                         << QString("attorneyagent");
}
