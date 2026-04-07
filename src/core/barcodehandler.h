#ifndef BARCODEHANDLER_H
#define BARCODEHANDLER_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QString>

class BarcodeHandler : public QObject
{
    Q_OBJECT

public:
    explicit BarcodeHandler(QObject *parent = nullptr);

    void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }
    void setMinLength(int len) { m_minLength = len; }
    void setMaxInputTime(int ms) { m_maxInputTime = ms; }
    void processKeyPress(int key, const QString &text);

signals:
    void barcodeScanned(const QString &barcode);

private slots:
    void onTimeout();

private:
    bool m_enabled;
    QString m_buffer;
    QElapsedTimer m_inputTimer;
    QTimer *m_timeoutTimer;
    int m_minLength;
    int m_maxInputTime;
};

#endif // BARCODEHANDLER_H
