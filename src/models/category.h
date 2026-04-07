#ifndef CATEGORY_H
#define CATEGORY_H

#include <QString>

struct Category {
    int id = 0;
    QString name;
    QString description;
    QString color;
    QString icon;
    bool active = true;
};

#endif // CATEGORY_H
