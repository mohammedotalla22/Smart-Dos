#include "loginwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QKeyEvent>
#include "../core/database.h"

LoginWidget::LoginWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void LoginWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    QWidget *loginCard = new QWidget(this);
    loginCard->setObjectName("loginCard");
    loginCard->setFixedSize(420, 480);

    QVBoxLayout *cardLayout = new QVBoxLayout(loginCard);
    cardLayout->setContentsMargins(40, 40, 40, 40);
    cardLayout->setSpacing(16);

    // Logo
    QLabel *logoLabel = new QLabel("Smart POS", loginCard);
    logoLabel->setObjectName("loginLogo");
    logoLabel->setAlignment(Qt::AlignCenter);
    QFont logoFont("Arial", 28, QFont::Bold);
    logoLabel->setFont(logoFont);
    cardLayout->addWidget(logoLabel);

    // Subtitle
    QLabel *subtitleLabel = new QLabel(
        QString::fromUtf8("\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd9\x86\xd9\x82\xd8\xa7\xd8\xb7 \xd8\xa7\xd9\x84\xd8\xa8\xd9\x8a\xd8\xb9 \xd8\xa7\xd9\x84\xd8\xb0\xd9\x83\xd9\x8a"),
        loginCard);
    subtitleLabel->setObjectName("loginSubtitle");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    QFont subFont("Arial", 12);
    subtitleLabel->setFont(subFont);
    cardLayout->addWidget(subtitleLabel);

    cardLayout->addSpacing(20);

    // Username
    QLabel *userLabel = new QLabel(
        QString::fromUtf8("\xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85"),
        loginCard);
    userLabel->setObjectName("inputLabel");
    cardLayout->addWidget(userLabel);

    m_usernameEdit = new QLineEdit(loginCard);
    m_usernameEdit->setObjectName("loginInput");
    m_usernameEdit->setPlaceholderText(
        QString::fromUtf8("\xd8\xa3\xd8\xaf\xd8\xae\xd9\x84 \xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85"));
    m_usernameEdit->setFixedHeight(44);
    m_usernameEdit->setAlignment(Qt::AlignRight);
    cardLayout->addWidget(m_usernameEdit);

    // Password
    QLabel *passLabel = new QLabel(
        QString::fromUtf8("\xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1"),
        loginCard);
    passLabel->setObjectName("inputLabel");
    cardLayout->addWidget(passLabel);

    m_passwordEdit = new QLineEdit(loginCard);
    m_passwordEdit->setObjectName("loginInput");
    m_passwordEdit->setPlaceholderText(
        QString::fromUtf8("\xd8\xa3\xd8\xaf\xd8\xae\xd9\x84 \xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1"));
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setFixedHeight(44);
    m_passwordEdit->setAlignment(Qt::AlignRight);
    cardLayout->addWidget(m_passwordEdit);

    // Error label
    m_errorLabel = new QLabel(loginCard);
    m_errorLabel->setObjectName("errorLabel");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->hide();
    cardLayout->addWidget(m_errorLabel);

    cardLayout->addSpacing(10);

    // Login button
    m_loginBtn = new QPushButton(
        QString::fromUtf8("\xd8\xaa\xd8\xb3\xd8\xac\xd9\x8a\xd9\x84 \xd8\xa7\xd9\x84\xd8\xaf\xd8\xae\xd9\x88\xd9\x84"),
        loginCard);
    m_loginBtn->setObjectName("primaryButton");
    m_loginBtn->setFixedHeight(48);
    m_loginBtn->setCursor(Qt::PointingHandCursor);
    QFont btnFont("Arial", 14, QFont::Bold);
    m_loginBtn->setFont(btnFont);
    cardLayout->addWidget(m_loginBtn);

    cardLayout->addStretch();

    // Version
    m_versionLabel = new QLabel("Smart POS v1.0.0", loginCard);
    m_versionLabel->setObjectName("versionLabel");
    m_versionLabel->setAlignment(Qt::AlignCenter);
    QFont verFont("Arial", 8);
    m_versionLabel->setFont(verFont);
    cardLayout->addWidget(m_versionLabel);

    mainLayout->addWidget(loginCard);

    // Connections
    connect(m_loginBtn, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginWidget::onLoginClicked);
    connect(m_usernameEdit, &QLineEdit::returnPressed, m_passwordEdit,
            static_cast<void(QLineEdit::*)()>(&QLineEdit::setFocus));
}

void LoginWidget::onLoginClicked()
{
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        m_errorLabel->setText(
            QString::fromUtf8("\xd9\x8a\xd8\xb1\xd8\xac\xd9\x89 \xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84 \xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85 \xd9\x88\xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1"));
        m_errorLabel->show();
        return;
    }

    Database *db = Database::instance();
    if (db->authenticateUser(username, password)) {
        User user = db->getUserByUsername(username);
        m_errorLabel->hide();
        m_usernameEdit->clear();
        m_passwordEdit->clear();
        emit loginSuccess(user);
    } else {
        m_errorLabel->setText(
            QString::fromUtf8("\xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85 \xd8\xa3\xd9\x88 \xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1 \xd8\xba\xd9\x8a\xd8\xb1 \xd8\xb5\xd8\xad\xd9\x8a\xd8\xad\xd8\xa9"));
        m_errorLabel->show();
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
    }
}
