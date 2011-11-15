/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp -m -i dbustypes.h -p datamanagementinterface /home/chrigi/devel/kde/kdepim-runtime/agents/nepomukfeeder/dms-copy/interfaces/org.kde.nepomuk.DataManagement.xml
 *
 * qdbusxml2cpp is Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef DATAMANAGEMENTINTERFACE_H_1312387679
#define DATAMANAGEMENTINTERFACE_H_1312387679

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "dbustypes.h"

/*
 * Proxy class for interface org.kde.nepomuk.DataManagement
 */
class OrgKdeNepomukDataManagementInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.kde.nepomuk.DataManagement"; }

public:
    OrgKdeNepomukDataManagementInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~OrgKdeNepomukDataManagementInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> addProperty(const QString &resource, const QString &property, const QDBusVariant &value, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resource) << qVariantFromValue(property) << qVariantFromValue(value) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("addProperty"), argumentList);
    }

    inline QDBusPendingReply<> addProperty(const QStringList &resources, const QString &property, const QVariantList &values, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(property) << qVariantFromValue(values) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("addProperty"), argumentList);
    }

    inline QDBusPendingReply<QString> createResource(const QString &type, const QString &label, const QString &description, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(type) << qVariantFromValue(label) << qVariantFromValue(description) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("createResource"), argumentList);
    }

    inline QDBusPendingReply<QString> createResource(const QStringList &types, const QString &label, const QString &description, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(types) << qVariantFromValue(label) << qVariantFromValue(description) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("createResource"), argumentList);
    }

    inline QDBusPendingReply<QList<Nepomuk::SimpleResource> > describeResources(const QStringList &resources, bool includeSubResources)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(includeSubResources);
        return asyncCallWithArgumentList(QLatin1String("describeResources"), argumentList);
    }

    inline QDBusPendingReply<> importResources(const QString &url, const QString &serialization, int identificationMode, int flags, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(url) << qVariantFromValue(serialization) << qVariantFromValue(identificationMode) << qVariantFromValue(flags) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("importResources"), argumentList);
    }

    inline QDBusPendingReply<> importResources(const QString &url, const QString &serialization, int identificationMode, int flags, Nepomuk::PropertyHash additionalMetadata, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(url) << qVariantFromValue(serialization) << qVariantFromValue(identificationMode) << qVariantFromValue(flags) << qVariantFromValue(additionalMetadata) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("importResources"), argumentList);
    }

    inline QDBusPendingReply<> mergeResources(const QString &resource1, const QString &resource2, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resource1) << qVariantFromValue(resource2) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("mergeResources"), argumentList);
    }

    inline QDBusPendingReply<> removeDataByApplication(int flags, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(flags) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeDataByApplication"), argumentList);
    }

    inline QDBusPendingReply<> removeDataByApplication(const QStringList &resources, int flags, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(flags) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeDataByApplication"), argumentList);
    }

    inline QDBusPendingReply<> removeProperties(const QString &resource, const QString &property, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resource) << qVariantFromValue(property) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeProperties"), argumentList);
    }

    inline QDBusPendingReply<> removeProperties(const QStringList &resources, const QStringList &properties, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(properties) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeProperties"), argumentList);
    }

    inline QDBusPendingReply<> removeProperty(const QString &resource, const QString &property, const QDBusVariant &value, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resource) << qVariantFromValue(property) << qVariantFromValue(value) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeProperty"), argumentList);
    }

    inline QDBusPendingReply<> removeProperty(const QStringList &resources, const QString &property, const QVariantList &values, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(property) << qVariantFromValue(values) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeProperty"), argumentList);
    }

    inline QDBusPendingReply<> removeResources(const QString &resource, int flags, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resource) << qVariantFromValue(flags) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeResources"), argumentList);
    }

    inline QDBusPendingReply<> removeResources(const QStringList &resources, int flags, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(flags) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("removeResources"), argumentList);
    }

    inline QDBusPendingReply<> setProperty(const QString &resource, const QString &property, const QDBusVariant &value, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resource) << qVariantFromValue(property) << qVariantFromValue(value) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("setProperty"), argumentList);
    }

    inline QDBusPendingReply<> setProperty(const QStringList &resources, const QString &property, const QVariantList &values, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(property) << qVariantFromValue(values) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("setProperty"), argumentList);
    }

    inline QDBusPendingReply<> storeResources(const QList<Nepomuk::SimpleResource> &resources, int identificationMode, int flags, Nepomuk::PropertyHash additionalMetadata, const QString &app)
    {
        QList<QVariant> argumentList;
        argumentList << qVariantFromValue(resources) << qVariantFromValue(identificationMode) << qVariantFromValue(flags) << qVariantFromValue(additionalMetadata) << qVariantFromValue(app);
        return asyncCallWithArgumentList(QLatin1String("storeResources"), argumentList);
    }

Q_SIGNALS: // SIGNALS
};

namespace org {
  namespace kde {
    namespace nepomuk {
      typedef ::OrgKdeNepomukDataManagementInterface DataManagement;
    }
  }
}
#endif
