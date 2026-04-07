#ifndef GDIPRINTER_H
#define GDIPRINTER_H

#include <QObject>
#include <QPrinter>
#include <QPainter>
#include "../models/sale.h"

class GdiPrinter : public QObject
{
    Q_OBJECT

public:
    explicit GdiPrinter(QObject *parent = nullptr);

    void setPrinterName(const QString &name);
    QString printerName() const { return m_printerName; }

    // Print operations
    bool printReceipt(const Sale &sale, const QString &storeName, const QString &storePhone,
                      const QString &storeAddress, const QString &taxNumber);
    bool printReport(const QString &title, const QStringList &headers,
                     const QList<QStringList> &rows, const QString &summary = QString());
    bool printSalesReport(const QString &title, const QList<QPair<QString, double>> &data,
                          double total, const QDateTime &from, const QDateTime &to);
    bool printCustomReport(const QString &htmlContent);

    static QStringList availablePrinters();

signals:
    void printError(const QString &error);
    void printSuccess();

private:
    QString m_printerName;

    void drawHeader(QPainter &painter, QPrinter &printer, const QString &title,
                    const QString &storeName, int &yPos);
    void drawTableHeader(QPainter &painter, const QStringList &headers,
                         const QList<int> &colWidths, int yPos);
    void drawTableRow(QPainter &painter, const QStringList &values,
                      const QList<int> &colWidths, int yPos);
    void drawFooter(QPainter &painter, QPrinter &printer, int pageNumber);
    int checkPageBreak(QPainter &painter, QPrinter &printer, int yPos, int needed);
};

#endif // GDIPRINTER_H
