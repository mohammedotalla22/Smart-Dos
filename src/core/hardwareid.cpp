#include "hardwareid.h"
#include <QCryptographicHash>
#include <QNetworkInterface>
#include <QSysInfo>
#include <QProcess>

QString HardwareId::generate()
{
    QString raw = getCpuId() + "|" + getMacAddress() + "|" + getDiskSerial();
    return QString(QCryptographicHash::hash(raw.toUtf8(), QCryptographicHash::Sha256).toHex()).left(32).toUpper();
}

QString HardwareId::getCpuId()
{
#ifdef Q_OS_WIN
    QProcess process;
    process.start("wmic", QStringList() << "cpu" << "get" << "ProcessorId");
    process.waitForFinished(3000);
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split("\n", QString::SkipEmptyParts);
    if (lines.size() >= 2) {
        return lines[1].trimmed();
    }
#else
    QProcess process;
    process.start("cat", QStringList() << "/proc/cpuinfo");
    process.waitForFinished(3000);
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split("\n");
    for (const QString &line : lines) {
        if (line.contains("Serial") || line.contains("model name")) {
            QStringList parts = line.split(":");
            if (parts.size() >= 2)
                return parts[1].trimmed();
        }
    }
#endif
    return QSysInfo::machineUniqueId().isEmpty() ?
           QSysInfo::machineHostName() : QString(QSysInfo::machineUniqueId());
}

QString HardwareId::getMacAddress()
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        if (!(iface.flags() & QNetworkInterface::IsLoopBack) &&
            iface.flags() & QNetworkInterface::IsUp) {
            QString mac = iface.hardwareAddress();
            if (!mac.isEmpty() && mac != "00:00:00:00:00:00") {
                return mac;
            }
        }
    }
    return "NO-MAC";
}

QString HardwareId::getDiskSerial()
{
#ifdef Q_OS_WIN
    QProcess process;
    process.start("wmic", QStringList() << "diskdrive" << "get" << "SerialNumber");
    process.waitForFinished(3000);
    QString output = process.readAllStandardOutput();
    QStringList lines = output.split("\n", QString::SkipEmptyParts);
    if (lines.size() >= 2) {
        return lines[1].trimmed();
    }
#else
    QProcess process;
    process.start("lsblk", QStringList() << "-ndo" << "SERIAL" << "/dev/sda");
    process.waitForFinished(3000);
    QString output = process.readAllStandardOutput().trimmed();
    if (!output.isEmpty()) return output;
#endif
    return "NO-DISK-SERIAL";
}
