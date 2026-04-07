#ifndef FIREBASE_H
#define FIREBASE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

class Firebase : public QObject
{
    Q_OBJECT

public:
    static Firebase* instance();

    void setConfig(const QString &projectId, const QString &apiKey, const QString &firestoreCollection);
    bool isConfigured() const;

    // Activation
    void checkActivation(const QString &hardwareId);
    void activateApp(const QString &hardwareId, const QString &licenseKey);

    // User Management via Firestore
    void checkUserBan(const QString &hardwareId);
    void checkAppBan();
    void registerDevice(const QString &hardwareId, const QString &deviceName);
    void updateDeviceLastSeen(const QString &hardwareId);

    // Firestore CRUD
    void getDocument(const QString &collection, const QString &docId);
    void setDocument(const QString &collection, const QString &docId, const QJsonObject &data);
    void updateDocument(const QString &collection, const QString &docId, const QJsonObject &fields);
    void deleteDocument(const QString &collection, const QString &docId);

    QString projectId() const { return m_projectId; }
    QString apiKey() const { return m_apiKey; }

signals:
    void activationSuccess(const QString &message);
    void activationFailed(const QString &error);
    void userBanned(const QString &reason);
    void appBanned(const QString &reason);
    void deviceRegistered();
    void connectionError(const QString &error);
    void documentReceived(const QString &collection, const QString &docId, const QJsonObject &data);
    void operationSuccess(const QString &operation);
    void operationFailed(const QString &operation, const QString &error);

private slots:
    void onActivationCheckReply(QNetworkReply *reply);
    void onActivateReply(QNetworkReply *reply);
    void onBanCheckReply(QNetworkReply *reply);
    void onAppBanCheckReply(QNetworkReply *reply);

private:
    explicit Firebase(QObject *parent = nullptr);
    static Firebase *m_instance;

    QNetworkAccessManager *m_networkManager;
    QString m_projectId;
    QString m_apiKey;
    QString m_firestoreCollection;

    QString firestoreUrl(const QString &path = QString()) const;
    QNetworkRequest createRequest(const QString &url) const;
};

#endif // FIREBASE_H
