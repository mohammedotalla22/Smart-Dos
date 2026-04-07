#include "thermalprinter.h"
#include <QSerialPortInfo>
#include <QTextCodec>
#include <QDateTime>

ThermalPrinter::ThermalPrinter(QObject *parent) : QObject(parent),
    m_paperSize(Paper80mm)
{
    m_serial = new QSerialPort(this);
}

bool ThermalPrinter::openPort(const QString &portName, int baudRate)
{
    if (m_serial->isOpen())
        m_serial->close();

    m_serial->setPortName(portName);
    m_serial->setBaudRate(baudRate);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serial->open(QIODevice::WriteOnly)) {
        initialize();
        return true;
    }

    emit printError(m_serial->errorString());
    return false;
}

void ThermalPrinter::closePort()
{
    if (m_serial->isOpen())
        m_serial->close();
}

bool ThermalPrinter::isConnected() const
{
    return m_serial->isOpen();
}

void ThermalPrinter::setPaperSize(PaperSize size)
{
    m_paperSize = size;
}

int ThermalPrinter::charsPerLine() const
{
    return (m_paperSize == Paper58mm) ? 32 : 48;
}

void ThermalPrinter::initialize()
{
    sendCommand(QByteArray("\x1B\x40", 2)); // ESC @ - Initialize
    sendCommand(QByteArray("\x1C\x29", 2)); // Enable Arabic codepage
}

void ThermalPrinter::cut()
{
    feed(3);
    sendCommand(QByteArray("\x1D\x56\x00", 3)); // GS V 0 - Full cut
}

void ThermalPrinter::feed(int lines)
{
    sendCommand(QByteArray("\x1B\x64", 2) + QByteArray(1, (char)lines)); // ESC d n
}

void ThermalPrinter::openCashDrawer()
{
    sendCommand(QByteArray("\x1B\x70\x00\x19\xFA", 5)); // ESC p 0
}

void ThermalPrinter::setAlignment(int align)
{
    sendCommand(QByteArray("\x1B\x61", 2) + QByteArray(1, (char)align)); // ESC a n
}

void ThermalPrinter::setBold(bool bold)
{
    sendCommand(QByteArray("\x1B\x45", 2) + QByteArray(1, bold ? '\x01' : '\x00'));
}

void ThermalPrinter::setDoubleSize(bool doubleWidth, bool doubleHeight)
{
    char mode = 0;
    if (doubleWidth) mode |= 0x20;
    if (doubleHeight) mode |= 0x10;
    sendCommand(QByteArray("\x1D\x21", 2) + QByteArray(1, mode)); // GS ! n
}

void ThermalPrinter::setUnderline(bool underline)
{
    sendCommand(QByteArray("\x1B\x2D", 2) + QByteArray(1, underline ? '\x01' : '\x00'));
}

void ThermalPrinter::setRtl(bool rtl)
{
    Q_UNUSED(rtl)
    // Most thermal printers handle RTL via codepage
}

void ThermalPrinter::printText(const QString &text)
{
    sendData(encodeArabic(text));
}

void ThermalPrinter::printLine(const QString &text)
{
    printText(text + "\n");
}

void ThermalPrinter::printSeparator(QChar ch)
{
    printLine(QString(charsPerLine(), ch));
}

void ThermalPrinter::printTwoColumns(const QString &left, const QString &right)
{
    int totalWidth = charsPerLine();
    int rightLen = right.length();
    int leftLen = totalWidth - rightLen - 1;

    QString leftPart = left.left(leftLen);
    leftPart = leftPart.leftJustified(leftLen, ' ');

    printLine(leftPart + " " + right);
}

void ThermalPrinter::printThreeColumns(const QString &left, const QString &center, const QString &right)
{
    int totalWidth = charsPerLine();
    int colWidth = totalWidth / 3;

    QString l = left.left(colWidth).leftJustified(colWidth, ' ');
    QString c = center.left(colWidth);
    int pad = (colWidth - c.length()) / 2;
    c = QString(pad, ' ') + c + QString(colWidth - pad - c.length(), ' ');
    QString r = right.left(colWidth);
    r = QString(colWidth - r.length(), ' ') + r;

    printLine(l + c + r);
}

void ThermalPrinter::printReceipt(const Sale &sale, const QString &storeName,
                                   const QString &storePhone, const QString &storeAddress,
                                   const QString &taxNumber)
{
    if (!isConnected()) {
        emit printError(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9 \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xaa\xd8\xb5\xd9\x84\xd8\xa9"));
        return;
    }

    initialize();

    // Header
    setAlignment(1);
    setBold(true);
    setDoubleSize(true, true);
    printLine(storeName);
    setDoubleSize(false, false);
    setBold(false);

    if (!storeAddress.isEmpty()) printLine(storeAddress);
    if (!storePhone.isEmpty()) printLine(storePhone);
    if (!taxNumber.isEmpty()) printLine(QString::fromUtf8("\xd8\xb1\xd9\x82\xd9\x85 \xd8\xb6\xd8\xb1\xd9\x8a\xd8\xa8\xd9\x8a: ") + taxNumber);

    printSeparator('=');
    setAlignment(0);

    // Invoice info
    printTwoColumns(QString::fromUtf8("\xd9\x81\xd8\xa7\xd8\xaa\xd9\x88\xd8\xb1\xd8\xa9 \xd8\xb1\xd9\x82\xd9\x85:"), sale.invoiceNumber);
    printTwoColumns(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae:"), sale.createdAt.toString("yyyy/MM/dd hh:mm"));
    if (!sale.customerName.isEmpty())
        printTwoColumns(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb9\xd9\x85\xd9\x8a\xd9\x84:"), sale.customerName);
    printTwoColumns(QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x83\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xb1:"), sale.userName);

    printSeparator('-');

    // Column headers
    setBold(true);
    printThreeColumns(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb5\xd9\x86\xd9\x81"),
                      QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x83\xd9\x85\xd9\x8a\xd8\xa9"),
                      QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a"));
    setBold(false);
    printSeparator('-');

    // Items
    for (int i = 0; i < sale.items.size(); ++i) {
        const SaleItem &item = sale.items[i];
        printThreeColumns(item.productName,
                         QString::number(item.quantity),
                         QString::number(item.total, 'f', 2));
        if (item.discount > 0) {
            printTwoColumns(QString::fromUtf8("  \xd8\xae\xd8\xb5\xd9\x85:"),
                           "-" + QString::number(item.discount, 'f', 2));
        }
    }

    printSeparator('=');

    // Totals
    printTwoColumns(QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9:"),
                    QString::number(sale.subtotal, 'f', 2));
    if (sale.discount > 0)
        printTwoColumns(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xae\xd8\xb5\xd9\x85:"),
                        "-" + QString::number(sale.discount, 'f', 2));
    if (sale.tax > 0)
        printTwoColumns(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb6\xd8\xb1\xd9\x8a\xd8\xa8\xd8\xa9:"),
                        QString::number(sale.tax, 'f', 2));

    setBold(true);
    setDoubleSize(true, false);
    printTwoColumns(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a:"),
                    QString::number(sale.total, 'f', 2));
    setDoubleSize(false, false);
    setBold(false);

    printTwoColumns(QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd8\xaf\xd9\x81\xd9\x88\xd8\xb9:"),
                    QString::number(sale.paid, 'f', 2));
    if (sale.change > 0)
        printTwoColumns(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa8\xd8\xa7\xd9\x82\xd9\x8a:"),
                        QString::number(sale.change, 'f', 2));

    printSeparator('-');

    // Footer
    setAlignment(1);
    printLine(QString::fromUtf8("\xd8\xb4\xd9\x83\xd8\xb1\xd8\xa7\xd9\x8b \xd9\x84\xd8\xb2\xd9\x8a\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x83\xd9\x85"));
    printLine(QString::fromUtf8("\xd9\x86\xd8\xaa\xd9\x85\xd9\x86\xd9\x89 \xd9\x84\xd9\x83\xd9\x85 \xd9\x8a\xd9\x88\xd9\x85\xd8\xa7\xd9\x8b \xd8\xb3\xd8\xb9\xd9\x8a\xd8\xaf\xd8\xa7\xd9\x8b"));

    cut();

    emit printSuccess();
}

void ThermalPrinter::printBarcode(const QString &data)
{
    if (!isConnected()) return;

    setAlignment(1);

    // Set barcode height
    sendCommand(QByteArray("\x1D\x68\x50", 3)); // GS h 80

    // Set barcode width
    sendCommand(QByteArray("\x1D\x77\x02", 3)); // GS w 2

    // Print text below barcode
    sendCommand(QByteArray("\x1D\x48\x02", 3)); // GS H 2

    // CODE128
    QByteArray barcodeData = data.toLatin1();
    QByteArray cmd;
    cmd.append("\x1D\x6B\x49", 3); // GS k 73 (CODE128)
    cmd.append((char)barcodeData.length());
    cmd.append(barcodeData);
    sendCommand(cmd);

    feed(2);
    setAlignment(0);
}

QStringList ThermalPrinter::availablePorts()
{
    QStringList ports;
    QList<QSerialPortInfo> portInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : portInfos) {
        ports << info.portName() + " - " + info.description();
    }
    return ports;
}

void ThermalPrinter::sendCommand(const QByteArray &cmd)
{
    if (m_serial->isOpen()) {
        m_serial->write(cmd);
        m_serial->flush();
    }
}

void ThermalPrinter::sendData(const QByteArray &data)
{
    if (m_serial->isOpen()) {
        m_serial->write(data);
        m_serial->flush();
    }
}

QByteArray ThermalPrinter::encodeArabic(const QString &text)
{
    // Use Windows-1256 encoding for Arabic thermal printers
    QTextCodec *codec = QTextCodec::codecForName("Windows-1256");
    if (codec) {
        return codec->fromUnicode(text);
    }
    return text.toUtf8();
}
