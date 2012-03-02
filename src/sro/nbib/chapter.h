#ifndef _NBIB_CHAPTER_H_
#define _NBIB_CHAPTER_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/documentpart.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A chapter of a publication. 
 */
class Chapter : public virtual NBIB::DocumentPart
{
public:
    Chapter(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Chapter", QUrl::StrictMode)), NBIB::DocumentPart(uri, QUrl::fromEncoded("http://www.example.com/nbib#Chapter", QUrl::StrictMode)) {
    }

    Chapter(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Chapter", QUrl::StrictMode)), NBIB::DocumentPart(res, QUrl::fromEncoded("http://www.example.com/nbib#Chapter", QUrl::StrictMode)) {
    }

    Chapter& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Chapter", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#pageEnd. Ending 
     * page number of the chapter 
     */
    qint64 pageEnd() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#pageEnd", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#pageEnd", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#pageEnd. Ending 
     * page number of the chapter 
     */
    void setPageEnd(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#pageEnd", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#pageEnd. 
     * Ending page number of the chapter 
     */
    void addPageEnd(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#pageEnd", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#pageStart. Starting 
     * page number of the chapter 
     */
    qint64 pageStart() const {
        qint64 value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#pageStart", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#pageStart", QUrl::StrictMode)).first().value<qint64>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#pageStart. Starting 
     * page number of the chapter 
     */
    void setPageStart(const qint64& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#pageStart", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#pageStart. 
     * Starting page number of the chapter 
     */
    void addPageStart(const qint64& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#pageStart", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#chapterNumber. 
     * The number of the Chapter for example 1.2, II etc 
     */
    QString chapterNumber() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#chapterNumber", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#chapterNumber", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#chapterNumber. 
     * The number of the Chapter for example 1.2, II etc 
     */
    void setChapterNumber(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#chapterNumber", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#chapterNumber. 
     * The number of the Chapter for example 1.2, II etc 
     */
    void addChapterNumber(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#chapterNumber", QUrl::StrictMode), value);
    }

protected:
    Chapter(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::DocumentPart(uri, type) {
    }
    Chapter(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::DocumentPart(res, type) {
    }
};
}
}

#endif
