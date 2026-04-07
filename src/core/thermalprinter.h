#ifndef THERMALPRINTER_H
#define THERMALPRINTER_H

#include <QObject>
#include <QSerialPort>
#include "../models/sale.h"

class ThermalPrinter : public QObject
{
    Q_OBJECT

public:
    enum PaperSize { Paper58mm, Paper80mm };
    Q_ENUM(PaperSize)

    explicit ThermalPrinter(QObject *parent = nullptr);

    bool openPort(const QString &portName, int baudRate = 9600);
    void closePort();
    bool isConnected() const;

    void setPaperSize(PaperSize size);
    PaperSize paperSize() const { return m_paperSize; }
    int charsPerLine() const;

    // Basic operations
    void initialize();
    void cut();
    void feed(int lines = 3);
    void openCashDrawer();

    // Text formatting
    void setAlignment(int align); // 0=left, 1=center, 2=right
    void setBold(bool bold);
    void setDoubleSize(bool doubleWidth, bool doubleHeight);
    void setUnderline(bool underline);
    void setRtl(bool rtl);

    // Print operations
    void printText(const QString &text);
    void printLine(const QString &text);
    void printSeparator(QChar ch = QChar('-'));
    void printTwoColumns(const QString &left, const QString &right);
    void printThreeColumns(const QString &left, const QString &center, const QString &right);

    // High-level printing
    void printReceipt(const Sale &sale, const QString &storeName, const QString &storePhone,
                      const QString &storeAddress, const QString &taxNumber);
    void printBarcode(const QString &data);

    static QStringList availablePorts();

signals:
    void printError(const QString &error);
    void printSuccess();

private:
    QSerialPort *m_serial;
    PaperSize m_paperSize;
    void sendCommand(const QByteArray &cmd);
    void sendData(const QByteArray &data);
    QByteArray encodeArabic(const QString &text);
};

#endif // THERMALPRINTER_H
