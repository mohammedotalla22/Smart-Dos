#include "firebase.h"
#include <QBuffer>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonValue>
#include <QUrl>
#include <QUrlQuery>

Firebase* Firebase::m_instance = nullptr;

Firebase::Firebase(QObject *parent) : QObject(parent)
{
    m_networkManager = new QNetworkAccessManager(this);
}

Firebase* Firebase::instance()
{
    if (!m_instance)
        m_instance = new Firebase();
    return m_instance;
}

void Firebase::setConfig(const QString &projectId, const QString &apiKey, const QString &firestoreCollection)
{
    m_projectId = projectId;
    m_apiKey = apiKey;
    m_firestoreCollection = firestoreCollection.isEmpty() ? "devices" : firestoreCollection;
}

bool Firebase::isConfigured() const
{
    return !m_projectId.isEmpty() && !m_apiKey.isEmpty();
}

QString Firebase::firestoreUrl(const QString &path) const
{
    QString url = QString("https://firestore.googleapis.com/v1/projects/%1/databases/(default)/documents").arg(m_projectId);
    if (!path.isEmpty())
        url += "/" + path;
    return url;
}

QNetworkRequest Firebase::createRequest(const QString &url) const
{
    QUrl reqUrl(url);
    QNetworkRequest request(reqUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return request;
}

void Firebase::checkActivation(const QString &hardwareId)
{
    if (!isConfigured()) {
        emit activationFailed(QString::fromUtf8("\xd9\x84\xd9\x85 \xd9\x8a\xd8\xaa\xd9\x85 \xd8\xaa\xd9\x83\xd9\x88\xd9\x8a\xd9\x86 Firebase"));
        return;
    }

    QString url = firestoreUrl(m_firestoreCollection + "/" + hardwareId);
    QNetworkRequest request = createRequest(url);

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onActivationCheckReply(reply);
    });
}

void Firebase::activateApp(const QString &hardwareId, const QString &licenseKey)
{
    if (!isConfigured()) {
        emit activationFailed(QString::fromUtf8("\xd9\x84\xd9\x85 \xd9\x8a\xd8\xaa\xd9\x85 \xd8\xaa\xd9\x83\xd9\x88\xd9\x8a\xd9\x86 Firebase"));
        return;
    }

    // First verify the license key
    QString licUrl = firestoreUrl("licenses/" + licenseKey);
    QNetworkRequest request = createRequest(licUrl);

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, hardwareId, licenseKey]() {
        onActivateReply(reply);
        if (reply->error() == QNetworkReply::NoError) {
            // Register device
            QJsonObject fields;
            QJsonObject hwIdField;
            hwIdField["stringValue"] = hardwareId;
            fields["hardwareId"] = hwIdField;

            QJsonObject licField;
            licField["stringValue"] = licenseKey;
            fields["licenseKey"] = licField;

            QJsonObject activeField;
            activeField["booleanValue"] = true;
            fields["activated"] = activeField;

            QJsonObject bannedField;
            bannedField["booleanValue"] = false;
            fields["banned"] = bannedField;

            QJsonObject dateField;
            dateField["stringValue"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
            fields["activatedAt"] = dateField;

            QJsonObject lastSeenField;
            lastSeenField["stringValue"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
            fields["lastSeen"] = lastSeenField;

            QJsonObject doc;
            doc["fields"] = fields;

            QString devUrl = firestoreUrl(m_firestoreCollection + "/" + hardwareId);
            QNetworkRequest devReq = createRequest(devUrl);

            QByteArray data = QJsonDocument(doc).toJson();
            QBuffer *buf = new QBuffer();
            buf->setData(data);
            buf->open(QIODevice::ReadOnly);
            QNetworkReply *devReply = m_networkManager->sendCustomRequest(devReq, "PATCH", buf);
            buf->setParent(devReply);
        }
        reply->deleteLater();
    });
}

void Firebase::checkUserBan(const QString &hardwareId)
{
    if (!isConfigured()) return;

    QString url = firestoreUrl(m_firestoreCollection + "/" + hardwareId);
    QNetworkRequest request = createRequest(url);

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onBanCheckReply(reply);
    });
}

void Firebase::checkAppBan()
{
    if (!isConfigured()) return;

    QString url = firestoreUrl("settings/app_config");
    QNetworkRequest request = createRequest(url);

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onAppBanCheckReply(reply);
    });
}

void Firebase::registerDevice(const QString &hardwareId, const QString &deviceName)
{
    if (!isConfigured()) return;

    QJsonObject fields;
    QJsonObject hwField;
    hwField["stringValue"] = hardwareId;
    fields["hardwareId"] = hwField;

    QJsonObject nameField;
    nameField["stringValue"] = deviceName;
    fields["deviceName"] = nameField;

    QJsonObject activeField;
    activeField["booleanValue"] = false;
    fields["activated"] = activeField;

    QJsonObject bannedField;
    bannedField["booleanValue"] = false;
    fields["banned"] = bannedField;

    QJsonObject dateField;
    dateField["stringValue"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    fields["registeredAt"] = dateField;

    QJsonObject doc;
    doc["fields"] = fields;

    QString url = firestoreUrl(m_firestoreCollection + "/" + hardwareId);
    QNetworkRequest request = createRequest(url);

    QByteArray data = QJsonDocument(doc).toJson();
    QBuffer *buffer = new QBuffer();
    buffer->setData(data);
    buffer->open(QIODevice::ReadOnly);

    QNetworkReply *reply = m_networkManager->sendCustomRequest(request, "PATCH", buffer);
    buffer->setParent(reply);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            emit deviceRegistered();
        } else {
            emit connectionError(reply->errorString());
        }
        reply->deleteLater();
    });
}

void Firebase::updateDeviceLastSeen(const QString &hardwareId)
{
    if (!isConfigured()) return;

    QJsonObject fields;
    QJsonObject lastSeenField;
    lastSeenField["stringValue"] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    fields["lastSeen"] = lastSeenField;

    QJsonObject doc;
    doc["fields"] = fields;

    QString url = firestoreUrl(m_firestoreCollection + "/" + hardwareId) + "?updateMask.fieldPaths=lastSeen";
    QNetworkRequest request = createRequest(url);

    QByteArray data = QJsonDocument(doc).toJson();
    QBuffer *buffer = new QBuffer();
    buffer->setData(data);
    buffer->open(QIODevice::ReadOnly);

    QNetworkReply *reply = m_networkManager->sendCustomRequest(request, "PATCH", buffer);
    buffer->setParent(reply);
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
}

void Firebase::getDocument(const QString &collection, const QString &docId)
{
    if (!isConfigured()) return;

    QString url = firestoreUrl(collection + "/" + docId);
    QNetworkRequest request = createRequest(url);

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, collection, docId]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject fields = doc.object().value("fields").toObject();
            emit documentReceived(collection, docId, fields);
        } else {
            emit operationFailed("get", reply->errorString());
        }
        reply->deleteLater();
    });
}

void Firebase::setDocument(const QString &collection, const QString &docId, const QJsonObject &data)
{
    if (!isConfigured()) return;

    QJsonObject doc;
    doc["fields"] = data;

    QString url = firestoreUrl(collection + "/" + docId);
    QNetworkRequest request = createRequest(url);

    QByteArray jsonData = QJsonDocument(doc).toJson();
    QBuffer *buffer = new QBuffer();
    buffer->setData(jsonData);
    buffer->open(QIODevice::ReadOnly);

    QNetworkReply *reply = m_networkManager->sendCustomRequest(request, "PATCH", buffer);
    buffer->setParent(reply);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            emit operationSuccess("set");
        } else {
            emit operationFailed("set", reply->errorString());
        }
        reply->deleteLater();
    });
}

void Firebase::updateDocument(const QString &collection, const QString &docId, const QJsonObject &fields)
{
    setDocument(collection, docId, fields);
}

void Firebase::deleteDocument(const QString &collection, const QString &docId)
{
    if (!isConfigured()) return;

    QString url = firestoreUrl(collection + "/" + docId);
    QNetworkRequest request = createRequest(url);

    QNetworkReply *reply = m_networkManager->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            emit operationSuccess("delete");
        } else {
            emit operationFailed("delete", reply->errorString());
        }
        reply->deleteLater();
    });
}

void Firebase::onActivationCheckReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject fields = doc.object().value("fields").toObject();

        bool activated = fields.value("activated").toObject().value("booleanValue").toBool();
        bool banned = fields.value("banned").toObject().value("booleanValue").toBool();

        if (banned) {
            QString reason = fields.value("banReason").toObject().value("stringValue").toString();
            emit userBanned(reason);
        } else if (activated) {
            emit activationSuccess(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82 \xd9\x85\xd9\x81\xd8\xb9\xd9\x84"));
        } else {
            emit activationFailed(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd9\x81\xd8\xb9\xd9\x84"));
        }
    } else if (reply->error() == QNetworkReply::ContentNotFoundError) {
        emit activationFailed(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xac\xd9\x87\xd8\xa7\xd8\xb2 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xb3\xd8\xac\xd9\x84"));
    } else {
        emit connectionError(reply->errorString());
    }
    reply->deleteLater();
}

void Firebase::onActivateReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject fields = doc.object().value("fields").toObject();
        bool used = fields.value("used").toObject().value("booleanValue").toBool();

        if (used) {
            emit activationFailed(QString::fromUtf8("\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb1\xd8\xae\xd9\x8a\xd8\xb5 \xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85 \xd9\x85\xd8\xb3\xd8\xa8\xd9\x82\xd8\xa7\xd9\x8b"));
        } else {
            emit activationSuccess(QString::fromUtf8("\xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd9\x81\xd8\xb9\xd9\x8a\xd9\x84 \xd8\xa8\xd9\x86\xd8\xac\xd8\xa7\xd8\xad"));
        }
    } else if (reply->error() == QNetworkReply::ContentNotFoundError) {
        emit activationFailed(QString::fromUtf8("\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb1\xd8\xae\xd9\x8a\xd8\xb5 \xd8\xba\xd9\x8a\xd8\xb1 \xd8\xb5\xd8\xa7\xd9\x84\xd8\xad"));
    } else {
        emit connectionError(reply->errorString());
    }
}

void Firebase::onBanCheckReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject fields = doc.object().value("fields").toObject();
        bool banned = fields.value("banned").toObject().value("booleanValue").toBool();

        if (banned) {
            QString reason = fields.value("banReason").toObject().value("stringValue").toString();
            emit userBanned(reason);
        }
    }
    reply->deleteLater();
}

void Firebase::onAppBanCheckReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject fields = doc.object().value("fields").toObject();
        bool banned = fields.value("appBanned").toObject().value("booleanValue").toBool();

        if (banned) {
            QString reason = fields.value("banReason").toObject().value("stringValue").toString();
            emit appBanned(reason);
        }
    }
    reply->deleteLater();
}
