#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>
#include <QDateTime>

struct Product {
    int id = 0;
    QString name;
    QString barcode;
    QString description;
    int categoryId = 0;
    double purchasePrice = 0.0;
    double salePrice = 0.0;
    int quantity = 0;
    int minQuantity = 5;
    QString unit;
    QString imagePath;
    bool active = true;
    QDateTime createdAt;
    QDateTime updatedAt;
};

#endif // PRODUCT_H
