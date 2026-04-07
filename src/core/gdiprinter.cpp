#include "gdiprinter.h"
#include <QPrinterInfo>
#include <QTextDocument>
#include <QFont>
#include <QDateTime>
#include <QApplication>

GdiPrinter::GdiPrinter(QObject *parent) : QObject(parent)
{
}

void GdiPrinter::setPrinterName(const QString &name)
{
    m_printerName = name;
}

QStringList GdiPrinter::availablePrinters()
{
    QStringList names;
    QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
    for (const QPrinterInfo &info : printers) {
        names << info.printerName();
    }
    return names;
}

bool GdiPrinter::printReceipt(const Sale &sale, const QString &storeName,
                               const QString &storePhone, const QString &storeAddress,
                               const QString &taxNumber)
{
    QPrinter printer(QPrinter::HighResolution);
    if (!m_printerName.isEmpty())
        printer.setPrinterName(m_printerName);
    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);

    QPainter painter;
    if (!painter.begin(&printer)) {
        emit printError(QString::fromUtf8("\xd9\x81\xd8\xb4\xd9\x84 \xd9\x81\xd9\x8a \xd8\xa8\xd8\xaf\xd8\xa1 \xd8\xa7\xd9\x84\xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9"));
        return false;
    }

    painter.setLayoutDirection(Qt::RightToLeft);
    int pageWidth = printer.pageRect().width();
    int yPos = 100;
    int lineHeight = 60;
    int margin = 100;

    // Header
    QFont titleFont(QString::fromUtf8("\xd8\xae\xd8\xb7 \xd8\xb9\xd8\xb1\xd8\xa8\xd9\x8a"), 24, QFont::Bold);
    QFont normalFont(QString::fromUtf8("\xd8\xae\xd8\xb7 \xd8\xb9\xd8\xb1\xd8\xa8\xd9\x8a"), 12);
    QFont boldFont(QString::fromUtf8("\xd8\xae\xd8\xb7 \xd8\xb9\xd8\xb1\xd8\xa8\xd9\x8a"), 12, QFont::Bold);

    // Try system fonts
    titleFont.setFamily("Arial");
    normalFont.setFamily("Arial");
    boldFont.setFamily("Arial");

    painter.setFont(titleFont);
    QRect titleRect(margin, yPos, pageWidth - 2 * margin, lineHeight * 2);
    painter.drawText(titleRect, Qt::AlignCenter, storeName);
    yPos += lineHeight * 2;

    painter.setFont(normalFont);
    if (!storeAddress.isEmpty()) {
        painter.drawText(QRect(margin, yPos, pageWidth - 2 * margin, lineHeight),
                        Qt::AlignCenter, storeAddress);
        yPos += lineHeight;
    }
    if (!storePhone.isEmpty()) {
        painter.drawText(QRect(margin, yPos, pageWidth - 2 * margin, lineHeight),
                        Qt::AlignCenter, storePhone);
        yPos += lineHeight;
    }
    if (!taxNumber.isEmpty()) {
        painter.drawText(QRect(margin, yPos, pageWidth - 2 * margin, lineHeight),
                        Qt::AlignCenter, QString::fromUtf8("\xd8\xb1\xd9\x82\xd9\x85 \xd8\xb6\xd8\xb1\xd9\x8a\xd8\xa8\xd9\x8a: ") + taxNumber);
        yPos += lineHeight;
    }

    // Separator
    yPos += 20;
    painter.drawLine(margin, yPos, pageWidth - margin, yPos);
    yPos += 30;

    // Invoice info
    painter.drawText(QRect(margin, yPos, pageWidth - 2 * margin, lineHeight),
                    Qt::AlignRight, QString::fromUtf8("\xd9\x81\xd8\xa7\xd8\xaa\xd9\x88\xd8\xb1\xd8\xa9 \xd8\xb1\xd9\x82\xd9\x85: ") + sale.invoiceNumber);
    yPos += lineHeight;
    painter.drawText(QRect(margin, yPos, pageWidth - 2 * margin, lineHeight),
                    Qt::AlignRight, QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae: ") + sale.createdAt.toString("yyyy/MM/dd hh:mm"));
    yPos += lineHeight;
    if (!sale.customerName.isEmpty()) {
        painter.drawText(QRect(margin, yPos, pageWidth - 2 * margin, lineHeight),
                        Qt::AlignRight, QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb9\xd9\x85\xd9\x8a\xd9\x84: ") + sale.customerName);
        yPos += lineHeight;
    }

    yPos += 20;
    painter.drawLine(margin, yPos, pageWidth - margin, yPos);
    yPos += 30;

    // Table header
    QList<int> colWidths;
    int tableWidth = pageWidth - 2 * margin;
    colWidths << tableWidth * 10 / 100   // #
              << tableWidth * 35 / 100   // Product
              << tableWidth * 15 / 100   // Qty
              << tableWidth * 20 / 100   // Price
              << tableWidth * 20 / 100;  // Total

    painter.setFont(boldFont);
    QStringList headers;
    headers << "#"
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb5\xd9\x86\xd9\x81")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x83\xd9\x85\xd9\x8a\xd8\xa9")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb3\xd8\xb9\xd8\xb1")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a");
    drawTableHeader(painter, headers, colWidths, yPos);
    yPos += lineHeight;

    painter.drawLine(margin, yPos, pageWidth - margin, yPos);
    yPos += 10;

    // Items
    painter.setFont(normalFont);
    for (int i = 0; i < sale.items.size(); ++i) {
        yPos = checkPageBreak(painter, printer, yPos, lineHeight);
        const SaleItem &item = sale.items[i];
        QStringList row;
        row << QString::number(i + 1)
            << item.productName
            << QString::number(item.quantity)
            << QString::number(item.unitPrice, 'f', 2)
            << QString::number(item.total, 'f', 2);
        drawTableRow(painter, row, colWidths, yPos);
        yPos += lineHeight;
    }

    yPos += 20;
    painter.drawLine(margin, yPos, pageWidth - margin, yPos);
    yPos += 30;

    // Totals
    int labelX = pageWidth / 2;
    int valueX = pageWidth - margin;

    painter.setFont(normalFont);
    painter.drawText(QRect(labelX, yPos, valueX - labelX, lineHeight),
                    Qt::AlignRight, QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9: ") + QString::number(sale.subtotal, 'f', 2));
    yPos += lineHeight;

    if (sale.discount > 0) {
        painter.drawText(QRect(labelX, yPos, valueX - labelX, lineHeight),
                        Qt::AlignRight, QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xae\xd8\xb5\xd9\x85: -") + QString::number(sale.discount, 'f', 2));
        yPos += lineHeight;
    }

    if (sale.tax > 0) {
        painter.drawText(QRect(labelX, yPos, valueX - labelX, lineHeight),
                        Qt::AlignRight, QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb6\xd8\xb1\xd9\x8a\xd8\xa8\xd8\xa9: ") + QString::number(sale.tax, 'f', 2));
        yPos += lineHeight;
    }

    painter.setFont(boldFont);
    painter.drawText(QRect(labelX, yPos, valueX - labelX, lineHeight),
                    Qt::AlignRight, QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a: ") + QString::number(sale.total, 'f', 2));
    yPos += lineHeight * 2;

    // Footer
    painter.setFont(normalFont);
    painter.drawText(QRect(margin, yPos, pageWidth - 2 * margin, lineHeight),
                    Qt::AlignCenter, QString::fromUtf8("\xd8\xb4\xd9\x83\xd8\xb1\xd8\xa7\xd9\x8b \xd9\x84\xd8\xb2\xd9\x8a\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x83\xd9\x85"));

    painter.end();

    emit printSuccess();
    return true;
}

bool GdiPrinter::printReport(const QString &title, const QStringList &headers,
                              const QList<QStringList> &rows, const QString &summary)
{
    QPrinter printer(QPrinter::HighResolution);
    if (!m_printerName.isEmpty())
        printer.setPrinterName(m_printerName);
    printer.setPageSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);

    QPainter painter;
    if (!painter.begin(&printer)) {
        emit printError(QString::fromUtf8("\xd9\x81\xd8\xb4\xd9\x84 \xd9\x81\xd9\x8a \xd8\xa8\xd8\xaf\xd8\xa1 \xd8\xa7\xd9\x84\xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9"));
        return false;
    }

    painter.setLayoutDirection(Qt::RightToLeft);
    int pageWidth = printer.pageRect().width();
    int margin = 100;
    int lineHeight = 50;
    int yPos = 100;

    // Title
    QFont titleFont("Arial", 18, QFont::Bold);
    QFont headerFont("Arial", 11, QFont::Bold);
    QFont normalFont("Arial", 10);

    painter.setFont(titleFont);
    painter.drawText(QRect(margin, yPos, pageWidth - 2 * margin, lineHeight * 2),
                    Qt::AlignCenter, title);
    yPos += lineHeight * 2 + 20;

    painter.drawLine(margin, yPos, pageWidth - margin, yPos);
    yPos += 20;

    // Calculate column widths
    int tableWidth = pageWidth - 2 * margin;
    QList<int> colWidths;
    int colCount = headers.size();
    for (int i = 0; i < colCount; ++i) {
        colWidths << tableWidth / colCount;
    }

    // Headers
    painter.setFont(headerFont);
    drawTableHeader(painter, headers, colWidths, yPos);
    yPos += lineHeight;
    painter.drawLine(margin, yPos, pageWidth - margin, yPos);
    yPos += 10;

    // Rows
    painter.setFont(normalFont);
    int pageNum = 1;
    for (int i = 0; i < rows.size(); ++i) {
        yPos = checkPageBreak(painter, printer, yPos, lineHeight);
        drawTableRow(painter, rows[i], colWidths, yPos);
        yPos += lineHeight;

        if (i < rows.size() - 1) {
            painter.setPen(Qt::lightGray);
            painter.drawLine(margin, yPos, pageWidth - margin, yPos);
            painter.setPen(Qt::black);
        }
    }

    // Summary
    if (!summary.isEmpty()) {
        yPos += 30;
        painter.drawLine(margin, yPos, pageWidth - margin, yPos);
        yPos += 20;
        painter.setFont(headerFont);
        painter.drawText(QRect(margin, yPos, pageWidth - 2 * margin, lineHeight),
                        Qt::AlignRight, summary);
    }

    painter.end();
    emit printSuccess();
    return true;
}

bool GdiPrinter::printSalesReport(const QString &title, const QList<QPair<QString, double>> &data,
                                   double total, const QDateTime &from, const QDateTime &to)
{
    QStringList headers;
    headers << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa8\xd9\x86\xd8\xaf")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9");

    QList<QStringList> rows;
    for (int i = 0; i < data.size(); ++i) {
        QStringList row;
        row << data[i].first << QString::number(data[i].second, 'f', 2);
        rows.append(row);
    }

    QString summary = QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a: ") + QString::number(total, 'f', 2);
    return printReport(title, headers, rows, summary);
}

bool GdiPrinter::printCustomReport(const QString &htmlContent)
{
    QPrinter printer(QPrinter::HighResolution);
    if (!m_printerName.isEmpty())
        printer.setPrinterName(m_printerName);
    printer.setPageSize(QPrinter::A4);

    QTextDocument doc;
    doc.setDefaultStyleSheet("body { direction: rtl; font-family: Arial; }");
    doc.setHtml(htmlContent);
    doc.print(&printer);

    emit printSuccess();
    return true;
}

void GdiPrinter::drawHeader(QPainter &painter, QPrinter &printer, const QString &title,
                             const QString &storeName, int &yPos)
{
    Q_UNUSED(printer)
    QFont titleFont("Arial", 18, QFont::Bold);
    painter.setFont(titleFont);

    int pageWidth = painter.device()->width();
    int margin = 100;

    painter.drawText(QRect(margin, yPos, pageWidth - 2 * margin, 80), Qt::AlignCenter, storeName);
    yPos += 100;

    QFont subFont("Arial", 14, QFont::Bold);
    painter.setFont(subFont);
    painter.drawText(QRect(margin, yPos, pageWidth - 2 * margin, 60), Qt::AlignCenter, title);
    yPos += 80;
}

void GdiPrinter::drawTableHeader(QPainter &painter, const QStringList &headers,
                                  const QList<int> &colWidths, int yPos)
{
    int margin = 100;
    int xPos = margin;
    for (int i = 0; i < headers.size() && i < colWidths.size(); ++i) {
        painter.drawText(QRect(xPos, yPos, colWidths[i], 50), Qt::AlignCenter, headers[i]);
        xPos += colWidths[i];
    }
}

void GdiPrinter::drawTableRow(QPainter &painter, const QStringList &values,
                                const QList<int> &colWidths, int yPos)
{
    int margin = 100;
    int xPos = margin;
    for (int i = 0; i < values.size() && i < colWidths.size(); ++i) {
        painter.drawText(QRect(xPos, yPos, colWidths[i], 50), Qt::AlignCenter, values[i]);
        xPos += colWidths[i];
    }
}

void GdiPrinter::drawFooter(QPainter &painter, QPrinter &printer, int pageNumber)
{
    Q_UNUSED(printer)
    int pageWidth = painter.device()->width();
    int pageHeight = painter.device()->height();
    QFont footFont("Arial", 8);
    painter.setFont(footFont);
    painter.drawText(QRect(0, pageHeight - 60, pageWidth, 50), Qt::AlignCenter,
                    QString::fromUtf8("\xd8\xb5\xd9\x81\xd8\xad\xd8\xa9 ") + QString::number(pageNumber));
}

int GdiPrinter::checkPageBreak(QPainter &painter, QPrinter &printer, int yPos, int needed)
{
    int pageHeight = printer.pageRect().height();
    if (yPos + needed > pageHeight - 100) {
        drawFooter(painter, printer, 0);
        printer.newPage();
        return 100;
    }
    return yPos;
}
