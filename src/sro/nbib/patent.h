#ifndef _NBIB_PATENT_H_
#define _NBIB_PATENT_H_

#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>

#include "dms-copy/simpleresource.h"

#include "nbib/publication.h"

namespace Nepomuk {
namespace NBIB {
/**
 * A patent. 
 */
class Patent : public virtual NBIB::Publication
{
public:
    Patent(const QUrl& uri = QUrl())
      : SimpleResource(uri), NIE::InformationElement(uri, QUrl::fromEncoded("http://www.example.com/nbib#Patent", QUrl::StrictMode)), NBIB::Publication(uri, QUrl::fromEncoded("http://www.example.com/nbib#Patent", QUrl::StrictMode)) {
    }

    Patent(const SimpleResource& res)
      : SimpleResource(res), NIE::InformationElement(res, QUrl::fromEncoded("http://www.example.com/nbib#Patent", QUrl::StrictMode)), NBIB::Publication(res, QUrl::fromEncoded("http://www.example.com/nbib#Patent", QUrl::StrictMode)) {
    }

    Patent& operator=(const SimpleResource& res) {
        SimpleResource::operator=(res);
        addType(QUrl::fromEncoded("http://www.example.com/nbib#Patent", QUrl::StrictMode));
        return *this;
    }

    /**
     * Get property http://www.example.com/nbib#patentReferences. 
     * The references for this patent 
     */
    QStringList patentReferenceses() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#patentReferences", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#patentReferences. 
     * The references for this patent 
     */
    void setPatentReferenceses(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#patentReferences", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#patentReferences. 
     * The references for this patent 
     */
    void addPatentReferences(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#patentReferences", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#assignee. The 
     * assignee of an patent 
     */
    QList<QUrl> assignees() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#assignee", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#assignee. The 
     * assignee of an patent 
     */
    void setAssignees(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#assignee", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#assignee. 
     * The assignee of an patent 
     */
    void addAssignee(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#assignee", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#attorneyAgent. 
     * A attorney agent related to a patent 
     */
    QList<QUrl> attorneyAgents() const {
        QList<QUrl> value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#attorneyAgent", QUrl::StrictMode)))
            value << v.value<QUrl>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#attorneyAgent. 
     * A attorney agent related to a patent 
     */
    void setAttorneyAgents(const QList<QUrl>& value) {
        QVariantList values;
        foreach(const QUrl& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#attorneyAgent", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#attorneyAgent. 
     * A attorney agent related to a patent 
     */
    void addAttorneyAgent(const QUrl& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#attorneyAgent", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#filingDate. 
     * The date when the patent was filed, the issue date is the same 
     * as the publication date 
     */
    QDateTime filingDate() const {
        QDateTime value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#filingDate", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#filingDate", QUrl::StrictMode)).first().value<QDateTime>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#filingDate. 
     * The date when the patent was filed, the issue date is the same 
     * as the publication date 
     */
    void setFilingDate(const QDateTime& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#filingDate", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#filingDate. 
     * The date when the patent was filed, the issue date is the same 
     * as the publication date 
     */
    void addFilingDate(const QDateTime& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#filingDate", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#applicationNumber. 
     * The application number of a patent 
     */
    QString applicationNumber() const {
        QString value;
        if(contains(QUrl::fromEncoded("http://www.example.com/nbib#applicationNumber", QUrl::StrictMode)))
            value = property(QUrl::fromEncoded("http://www.example.com/nbib#applicationNumber", QUrl::StrictMode)).first().value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#applicationNumber. 
     * The application number of a patent 
     */
    void setApplicationNumber(const QString& value) {
        QVariantList values;
        values << value;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#applicationNumber", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#applicationNumber. 
     * The application number of a patent 
     */
    void addApplicationNumber(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#applicationNumber", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#priorityNumbers. 
     * The priority number of a patent 
     */
    QStringList priorityNumberses() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#priorityNumbers", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#priorityNumbers. 
     * The priority number of a patent 
     */
    void setPriorityNumberses(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#priorityNumbers", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#priorityNumbers. 
     * The priority number of a patent 
     */
    void addPriorityNumbers(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#priorityNumbers", QUrl::StrictMode), value);
    }

    /**
     * Get property http://www.example.com/nbib#legalStatus. 
     * The legal status of this patent 
     */
    QStringList legalStatuses() const {
        QStringList value;
        foreach(const QVariant& v, property(QUrl::fromEncoded("http://www.example.com/nbib#legalStatus", QUrl::StrictMode)))
            value << v.value<QString>();
        return value;
    }

    /**
     * Set property http://www.example.com/nbib#legalStatus. 
     * The legal status of this patent 
     */
    void setLegalStatuses(const QStringList& value) {
        QVariantList values;
        foreach(const QString& v, value)
            values << v;
        setProperty(QUrl::fromEncoded("http://www.example.com/nbib#legalStatus", QUrl::StrictMode), values);
    }

    /**
     * Add value to property http://www.example.com/nbib#legalStatus. 
     * The legal status of this patent 
     */
    void addLegalStatus(const QString& value) {
        addProperty(QUrl::fromEncoded("http://www.example.com/nbib#legalStatus", QUrl::StrictMode), value);
    }

protected:
    Patent(const QUrl& uri, const QUrl& type)
      : SimpleResource(uri), NIE::InformationElement(uri, type), NBIB::Publication(uri, type) {
    }
    Patent(const SimpleResource& res, const QUrl& type)
      : SimpleResource(res), NIE::InformationElement(res, type), NBIB::Publication(res, type) {
    }
};
}
}

#endif
