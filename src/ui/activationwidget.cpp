#include "activationwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QClipboard>
#include <QApplication>
#include <QFont>
#include "../core/firebase.h"
#include "../core/hardwareid.h"

ActivationWidget::ActivationWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void ActivationWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    QWidget *card = new QWidget(this);
    card->setObjectName("activationCard");
    card->setFixedSize(500, 450);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    cardLayout->setSpacing(16);

    // Title
    QLabel *titleLabel = new QLabel(
        QString::fromUtf8("\xd8\xaa\xd9\x81\xd8\xb9\xd9\x8a\xd9\x84 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82"),
        card);
    titleLabel->setObjectName("activationTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 22, QFont::Bold);
    titleLabel->setFont(titleFont);
    cardLayout->addWidget(titleLabel);

    cardLayout->addSpacing(10);

    // Hardware ID
    QLabel *hwLabel = new QLabel(
        QString::fromUtf8("\xd9\x85\xd8\xb9\xd8\xb1\xd9\x81 \xd8\xa7\xd9\x84\xd8\xac\xd9\x87\xd8\xa7\xd8\xb2:"),
        card);
    hwLabel->setObjectName("inputLabel");
    cardLayout->addWidget(hwLabel);

    QHBoxLayout *hwLayout = new QHBoxLayout();
    m_hwIdLabel = new QLabel(card);
    m_hwIdLabel->setObjectName("hwIdLabel");
    m_hwIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QFont monoFont("Courier New", 11);
    m_hwIdLabel->setFont(monoFont);
    hwLayout->addWidget(m_hwIdLabel, 1);

    m_copyHwIdBtn = new QPushButton(
        QString::fromUtf8("\xd9\x86\xd8\xb3\xd8\xae"),
        card);
    m_copyHwIdBtn->setObjectName("secondaryButton");
    m_copyHwIdBtn->setFixedSize(60, 36);
    m_copyHwIdBtn->setCursor(Qt::PointingHandCursor);
    hwLayout->addWidget(m_copyHwIdBtn);
    cardLayout->addLayout(hwLayout);

    cardLayout->addSpacing(10);

    // License key
    QLabel *licLabel = new QLabel(
        QString::fromUtf8("\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb1\xd8\xae\xd9\x8a\xd8\xb5:"),
        card);
    licLabel->setObjectName("inputLabel");
    cardLayout->addWidget(licLabel);

    m_licenseEdit = new QLineEdit(card);
    m_licenseEdit->setObjectName("loginInput");
    m_licenseEdit->setPlaceholderText(
        QString::fromUtf8("\xd8\xa3\xd8\xaf\xd8\xae\xd9\x84 \xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb1\xd8\xae\xd9\x8a\xd8\xb5"));
    m_licenseEdit->setFixedHeight(44);
    m_licenseEdit->setAlignment(Qt::AlignCenter);
    QFont licFont("Courier New", 13);
    m_licenseEdit->setFont(licFont);
    cardLayout->addWidget(m_licenseEdit);

    // Status
    m_statusLabel = new QLabel(card);
    m_statusLabel->setObjectName("statusLabel");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->hide();
    cardLayout->addWidget(m_statusLabel);

    cardLayout->addSpacing(10);

    // Activate button
    m_activateBtn = new QPushButton(
        QString::fromUtf8("\xd8\xaa\xd9\x81\xd8\xb9\xd9\x8a\xd9\x84"),
        card);
    m_activateBtn->setObjectName("primaryButton");
    m_activateBtn->setFixedHeight(48);
    m_activateBtn->setCursor(Qt::PointingHandCursor);
    QFont btnFont("Arial", 14, QFont::Bold);
    m_activateBtn->setFont(btnFont);
    cardLayout->addWidget(m_activateBtn);

    cardLayout->addStretch();

    mainLayout->addWidget(card);

    // Connections
    connect(m_activateBtn, &QPushButton::clicked, this, &ActivationWidget::onActivateClicked);
    connect(m_licenseEdit, &QLineEdit::returnPressed, this, &ActivationWidget::onActivateClicked);
    connect(m_copyHwIdBtn, &QPushButton::clicked, this, [this]() {
        QApplication::clipboard()->setText(m_hardwareId);
        m_statusLabel->setText(
            QString::fromUtf8("\xd8\xaa\xd9\x85 \xd9\x86\xd8\xb3\xd8\xae \xd9\x85\xd8\xb9\xd8\xb1\xd9\x81 \xd8\xa7\xd9\x84\xd8\xac\xd9\x87\xd8\xa7\xd8\xb2"));
        m_statusLabel->setStyleSheet("color: #2ecc71;");
        m_statusLabel->show();
    });

    Firebase *fb = Firebase::instance();
    connect(fb, &Firebase::activationSuccess, this, [this](const QString &msg) {
        m_statusLabel->setText(msg);
        m_statusLabel->setStyleSheet("color: #2ecc71;");
        m_statusLabel->show();
        emit activationSuccess();
    });
    connect(fb, &Firebase::activationFailed, this, [this](const QString &msg) {
        m_statusLabel->setText(msg);
        m_statusLabel->setStyleSheet("color: #e74c3c;");
        m_statusLabel->show();
    });
}

void ActivationWidget::setHardwareId(const QString &id)
{
    m_hardwareId = id;
    m_hwIdLabel->setText(id);
}

void ActivationWidget::onActivateClicked()
{
    QString license = m_licenseEdit->text().trimmed();
    if (license.isEmpty()) {
        m_statusLabel->setText(
            QString::fromUtf8("\xd9\x8a\xd8\xb1\xd8\xac\xd9\x89 \xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84 \xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb1\xd8\xae\xd9\x8a\xd8\xb5"));
        m_statusLabel->setStyleSheet("color: #e74c3c;");
        m_statusLabel->show();
        return;
    }

    m_statusLabel->setText(
        QString::fromUtf8("\xd8\xac\xd8\xa7\xd8\xb1\xd9\x8a \xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd9\x82\xd9\x82..."));
    m_statusLabel->setStyleSheet("color: #f39c12;");
    m_statusLabel->show();

    Firebase::instance()->activateApp(m_hardwareId, license);
}
