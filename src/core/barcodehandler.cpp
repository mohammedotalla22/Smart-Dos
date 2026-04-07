#include "barcodehandler.h"
#include <Qt>

BarcodeHandler::BarcodeHandler(QObject *parent) : QObject(parent),
    m_enabled(true),
    m_minLength(4),
    m_maxInputTime(100)
{
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &BarcodeHandler::onTimeout);
}

void BarcodeHandler::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if (!enabled) {
        m_buffer.clear();
        m_timeoutTimer->stop();
    }
}

void BarcodeHandler::processKeyPress(int key, const QString &text)
{
    if (!m_enabled) return;

    if (key == Qt::Key_Return || key == Qt::Key_Enter) {
        if (m_buffer.length() >= m_minLength) {
            if (!m_inputTimer.isValid() || m_inputTimer.elapsed() < m_maxInputTime * m_buffer.length()) {
                emit barcodeScanned(m_buffer);
            }
        }
        m_buffer.clear();
        m_timeoutTimer->stop();
        return;
    }

    if (text.isEmpty() || !text[0].isPrint()) return;

    if (m_buffer.isEmpty()) {
        m_inputTimer.start();
    }

    m_buffer += text;
    m_timeoutTimer->start(m_maxInputTime * 3);
}

void BarcodeHandler::onTimeout()
{
    m_buffer.clear();
}
