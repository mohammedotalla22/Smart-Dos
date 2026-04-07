#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <QString>
#include <QDateTime>

struct Customer {
    int id = 0;
    QString name;
    QString phone;
    QString email;
    QString address;
    double balance = 0.0;
    double totalPurchases = 0.0;
    QString notes;
    bool active = true;
    QDateTime createdAt;
};

#endif // CUSTOMER_H
