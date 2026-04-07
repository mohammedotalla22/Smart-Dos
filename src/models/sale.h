#ifndef SALE_H
#define SALE_H

#include <QString>
#include <QDateTime>
#include <QList>

struct SaleItem {
    int id = 0;
    int saleId = 0;
    int productId = 0;
    QString productName;
    QString barcode;
    int quantity = 1;
    double unitPrice = 0.0;
    double discount = 0.0;
    double total = 0.0;
};

struct Sale {
    int id = 0;
    QString invoiceNumber;
    int customerId = 0;
    QString customerName;
    int userId = 0;
    QString userName;
    QList<SaleItem> items;
    double subtotal = 0.0;
    double discount = 0.0;
    double tax = 0.0;
    double total = 0.0;
    double paid = 0.0;
    double change = 0.0;
    QString paymentMethod;
    QString notes;
    QDateTime createdAt;
};

#endif // SALE_H
