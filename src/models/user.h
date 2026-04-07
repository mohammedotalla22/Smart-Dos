#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>

struct User {
    int id = 0;
    QString username;
    QString passwordHash;
    QString displayName;
    QString role;       // admin, cashier, manager
    QString email;
    QString phone;
    bool active = true;
    QDateTime createdAt;
    QDateTime lastLogin;
};

#endif // USER_H
