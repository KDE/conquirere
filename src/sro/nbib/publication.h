#ifndef _NBIB_PUBLICATION_H_
#define _NBIB_PUBLICATION_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include <nepomuk2/simpleresource.h>

#include "nie/informationelement.h"

namespace Nepomuk2 {
namespace NBIB {
/**
 * A bibliographic reference resource type (article, book, paper 
 * and such) 
 */
class Publication : public virtual NIE::InformationElement
{
public:
    Publication(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Publication", QUrl::StrictMode)) {
    }

    Publication(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Publication", QUrl::StrictMode)) {
    }

    Publication& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Publication", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#shortTitle. 
     * The short version of the title 
     */
    QString shortTitle() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#shortTitle", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#shortTitle", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#shortTitle. 
     * The short version of the title 
     */
    void setShortTitle(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#shortTitle", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#shortTitle. 
     * The short version of the title 
     */
    void addShortTitle(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#shortTitle", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#volume. The volume 
     * of a journal or multi-volume book. 
     */
    QString volume() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#volume", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#volume", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#volume. The volume 
     * of a journal or multi-volume book. 
     */
    void setVolume(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#volume", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#volume. 
     * The volume of a journal or multi-volume book. 
     */
    void addVolume(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#volume", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#lccn. The Library 
     * of Congress Call Number. 
     */
    QString lccn() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#lccn", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#lccn", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#lccn. The Library 
     * of Congress Call Number. 
     */
    void setLccn(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#lccn", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#lccn. 
     * The Library of Congress Call Number. 
     */
    void addLccn(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#lccn", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#event. The event 
     * this publication was shown, represented, published etc 
     */
    QUrl event() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#event", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#event", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#event. The event 
     * this publication was shown, represented, published etc 
     */
    void setEvent(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#event", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#event. 
     * The event this publication was shown, represented, published 
     * etc 
     */
    void addEvent(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#event", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#archiveLocation. 
     * The location where the publication can be found in the archive 
     * specified via nbib::archive 
     */
    QString archiveLocation() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#archiveLocation", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#archiveLocation", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#archiveLocation. 
     * The location where the publication can be found in the archive 
     * specified via nbib::archive 
     */
    void setArchiveLocation(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#archiveLocation", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#archiveLocation. 
     * The location where the publication can be found in the archive 
     * specified via nbib::archive 
     */
    void addArchiveLocation(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#archiveLocation", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#isbn. The International 
     * Standard Book Number. 
     */
    QString isbn() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#isbn", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#isbn", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#isbn. The International 
     * Standard Book Number. 
     */
    void setIsbn(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#isbn", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#isbn. 
     * The International Standard Book Number. 
     */
    void addIsbn(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#isbn", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#organization. 
     * The organization that held the conference where the proceedings 
     * comes from or that is otherwise responsible for the publication 
     */
    QUrl organization() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#organization", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#organization", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#organization. 
     * The organization that held the conference where the proceedings 
     * comes from or that is otherwise responsible for the publication 
     */
    void setOrganization(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#organization", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#organization. 
     * The organization that held the conference where the proceedings 
     * comes from or that is otherwise responsible for the publication 
     */
    void addOrganization(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#organization", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#publicationMethod. 
     * How it was published, if the publishing method is nonstandard. 
     */
    QString publicationMethod() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#publicationMethod", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#publicationMethod", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#publicationMethod. 
     * How it was published, if the publishing method is nonstandard. 
     */
    void setPublicationMethod(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#publicationMethod", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#publicationMethod. 
     * How it was published, if the publishing method is nonstandard. 
     */
    void addPublicationMethod(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#publicationMethod", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#eprint. A specification 
     * of an electronic publication, often a preprint or a technical 
     * report. 
     */
    QString eprint() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#eprint", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#eprint", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#eprint. A specification 
     * of an electronic publication, often a preprint or a technical 
     * report. 
     */
    void setEprint(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#eprint", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#eprint. 
     * A specification of an electronic publication, often a preprint 
     * or a technical report. 
     */
    void addEprint(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#eprint", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#pubMed. The PubMed 
     * ID. 
     */
    QString pubMed() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#pubMed", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#pubMed", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#pubMed. The PubMed 
     * ID. 
     */
    void setPubMed(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#pubMed", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#pubMed. 
     * The PubMed ID. 
     */
    void addPubMed(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#pubMed", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#documentStatus. 
     * The status of the publication of a document. 
     */
    QUrl documentStatus() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#documentStatus", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#documentStatus", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#documentStatus. 
     * The status of the publication of a document. 
     */
    void setDocumentStatus(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#documentStatus", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#documentStatus. 
     * The status of the publication of a document. 
     */
    void addDocumentStatus(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#documentStatus", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#reference. All 
     * references of a publication. Used to quickly get all different 
     * references (different quotes, chapters etc) 
     */
    QList<QUrl> references() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#reference", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#reference. All 
     * references of a publication. Used to quickly get all different 
     * references (different quotes, chapters etc) 
     */
    void setReferences(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#reference", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#reference. 
     * All references of a publication. Used to quickly get all different 
     * references (different quotes, chapters etc) 
     */
    void addReference(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#reference", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#numberOfVolumes. 
     * The number of volumes that exists for a publication. 
     */
    qint64 numberOfVolumes() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#numberOfVolumes", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#numberOfVolumes", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#numberOfVolumes. 
     * The number of volumes that exists for a publication. 
     */
    void setNumberOfVolumes(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#numberOfVolumes", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#numberOfVolumes. 
     * The number of volumes that exists for a publication. 
     */
    void addNumberOfVolumes(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#numberOfVolumes", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#contributor. 
     * The contributor of a Publication 
     */
    QList<QUrl> contributors() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#contributor", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#contributor. 
     * The contributor of a Publication 
     */
    void setContributors(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#contributor", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#contributor. 
     * The contributor of a Publication 
     */
    void addContributor(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#contributor", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#reviewedAuthor. 
     * The person who reviewed a Publication 
     */
    QList<QUrl> reviewedAuthors() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#reviewedAuthor", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#reviewedAuthor. 
     * The person who reviewed a Publication 
     */
    void setReviewedAuthors(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#reviewedAuthor", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#reviewedAuthor. 
     * The person who reviewed a Publication 
     */
    void addReviewedAuthor(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#reviewedAuthor", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#inSeries. The 
     * series the publication was published in. (for example a series 
     * of books, a series of lecture notes, a journal series 
     */
    QUrl inSeries() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#inSeries", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#inSeries", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#inSeries. The 
     * series the publication was published in. (for example a series 
     * of books, a series of lecture notes, a journal series 
     */
    void setInSeries(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#inSeries", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#inSeries. 
     * The series the publication was published in. (for example a 
     * series of books, a series of lecture notes, a journal series 
     */
    void addInSeries(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#inSeries", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#translator. 
     * The translator of a Publication 
     */
    QList<QUrl> translators() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#translator", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#translator. 
     * The translator of a Publication 
     */
    void setTranslators(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#translator", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#translator. 
     * The translator of a Publication 
     */
    void addTranslator(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#translator", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#documentPart. 
     * Connects a publication to a part of it (for example a chapter 
     * or quoted passage) 
     */
    QList<QUrl> documentParts() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#documentPart", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#documentPart. 
     * Connects a publication to a part of it (for example a chapter 
     * or quoted passage) 
     */
    void setDocumentParts(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#documentPart", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#documentPart. 
     * Connects a publication to a part of it (for example a chapter 
     * or quoted passage) 
     */
    void addDocumentPart(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#documentPart", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#numberOfPages. 
     * The number of pages contained in the publication. 
     */
    qint64 numberOfPages() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#numberOfPages", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#numberOfPages", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#numberOfPages. 
     * The number of pages contained in the publication. 
     */
    void setNumberOfPages(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#numberOfPages", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#numberOfPages. 
     * The number of pages contained in the publication. 
     */
    void addNumberOfPages(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#numberOfPages", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#commenter. The 
     * commenter of a Publication. Mostly used for Blogs, Forum posts 
     */
    QList<QUrl> commenters() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#commenter", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#commenter. The 
     * commenter of a Publication. Mostly used for Blogs, Forum posts 
     */
    void setCommenters(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#commenter", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#commenter. 
     * The commenter of a Publication. Mostly used for Blogs, Forum 
     * posts 
     */
    void addCommenter(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#commenter", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#isPublicationOf. 
     * Links a publication to its document. 
     */
    QList<QUrl> isPublicationOfs() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#isPublicationOf", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#isPublicationOf. 
     * Links a publication to its document. 
     */
    void setIsPublicationOfs(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#isPublicationOf", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#isPublicationOf. 
     * Links a publication to its document. 
     */
    void addIsPublicationOf(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#isPublicationOf", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#publicationType. 
     * The type of tech-report or other kind of Publication, for example, 
     * 'Research Note'. 
     */
    QString publicationType() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#publicationType", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#publicationType", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#publicationType. 
     * The type of tech-report or other kind of Publication, for example, 
     * 'Research Note'. 
     */
    void setPublicationType(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#publicationType", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#publicationType. 
     * The type of tech-report or other kind of Publication, for example, 
     * 'Research Note'. 
     */
    void addPublicationType(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#publicationType", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#edition. The 
     * edition of a book, long form (such as 'first' or 'second'). 
     */
    QString edition() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#edition", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#edition", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#edition. The 
     * edition of a book, long form (such as 'first' or 'second'). 
     */
    void setEdition(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#edition", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#edition. 
     * The edition of a book, long form (such as 'first' or 'second'). 
     */
    void addEdition(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#edition", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#number. The '(issue) 
     * number' of a journal, magazine, or tech-report, pr the publicl 
     * aw Number of a Statute or the number of a patent etc 
     */
    QString number() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#number", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#number", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#number. The '(issue) 
     * number' of a journal, magazine, or tech-report, pr the publicl 
     * aw Number of a Statute or the number of a patent etc 
     */
    void setNumber(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#number", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#number. 
     * The '(issue) number' of a journal, magazine, or tech-report, 
     * pr the publicl aw Number of a Statute or the number of a patent 
     * etc 
     */
    void addNumber(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#number", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#archive. The 
     * archive where the publication can be found, see also nbib::archivelocation 
     */
    QString archive() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#archive", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#archive", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#archive. The 
     * archive where the publication can be found, see also nbib::archivelocation 
     */
    void setArchive(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#archive", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#archive. 
     * The archive where the publication can be found, see also nbib::archivelocation 
     */
    void addArchive(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#archive", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#translationOf. 
     * Relates a translated document to the original document. 
     */
    QUrl translationOf() const {
        QUrl value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#translationOf", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#translationOf", QUrl::StrictMode)).first().value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#translationOf. 
     * Relates a translated document to the original document. 
     */
    void setTranslationOf(const QUrl& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#translationOf", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#translationOf. 
     * Relates a translated document to the original document. 
     */
    void addTranslationOf(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#translationOf", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#libraryCatalog. 
     * The library catalog information where the publication can 
     * be found 
     */
    QString libraryCatalog() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#libraryCatalog", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#libraryCatalog", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#libraryCatalog. 
     * The library catalog information where the publication can 
     * be found 
     */
    void setLibraryCatalog(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#libraryCatalog", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#libraryCatalog. 
     * The library catalog information where the publication can 
     * be found 
     */
    void addLibraryCatalog(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#libraryCatalog", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#mrNumber. The 
     * Mathematical Reviews number. 
     */
    QString mrNumber() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#mrNumber", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#mrNumber", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#mrNumber. The 
     * Mathematical Reviews number. 
     */
    void setMrNumber(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#mrNumber", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#mrNumber. 
     * The Mathematical Reviews number. 
     */
    void addMrNumber(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#mrNumber", QUrl::StrictMode), value);
    }

protected:
    Publication(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type) {
    }
    Publication(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type) {
    }
};
}
}

#endif
