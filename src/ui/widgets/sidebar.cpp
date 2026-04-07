#include "sidebar.h"
#include <QPainter>
#include <QStyleOption>

// ========== SidebarButton ==========

SidebarButton::SidebarButton(const QString &text, const QString &iconChar, QWidget *parent)
    : QPushButton(parent), m_active(false)
{
    setText(iconChar + "  " + text);
    setFixedHeight(48);
    setCursor(Qt::PointingHandCursor);
    setCheckable(true);
    setObjectName("sidebarButton");
}

void SidebarButton::setActive(bool active)
{
    m_active = active;
    setChecked(active);
    update();
}

void SidebarButton::paintEvent(QPaintEvent *event)
{
    QPushButton::paintEvent(event);
}

// ========== Sidebar ==========

Sidebar::Sidebar(QWidget *parent) : QWidget(parent), m_activeIndex(0)
{
    setObjectName("sidebar");
    setFixedWidth(240);
    setupUI();
}

void Sidebar::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(2);

    // Logo area
    QWidget *logoWidget = new QWidget(this);
    logoWidget->setObjectName("sidebarLogo");
    logoWidget->setFixedHeight(80);
    QVBoxLayout *logoLayout = new QVBoxLayout(logoWidget);
    logoLayout->setContentsMargins(15, 10, 15, 10);

    m_logoLabel = new QLabel(QString::fromUtf8("Smart POS"), logoWidget);
    m_logoLabel->setObjectName("logoLabel");
    m_logoLabel->setAlignment(Qt::AlignCenter);
    QFont logoFont("Arial", 20, QFont::Bold);
    m_logoLabel->setFont(logoFont);
    logoLayout->addWidget(m_logoLabel);
    m_layout->addWidget(logoWidget);

    // User info area
    QWidget *userWidget = new QWidget(this);
    userWidget->setObjectName("sidebarUser");
    userWidget->setFixedHeight(60);
    QVBoxLayout *userLayout = new QVBoxLayout(userWidget);
    userLayout->setContentsMargins(15, 5, 15, 5);
    userLayout->setSpacing(2);

    m_userNameLabel = new QLabel(this);
    m_userNameLabel->setObjectName("userNameLabel");
    m_userNameLabel->setAlignment(Qt::AlignCenter);
    QFont nameFont("Arial", 12, QFont::Bold);
    m_userNameLabel->setFont(nameFont);

    m_userRoleLabel = new QLabel(this);
    m_userRoleLabel->setObjectName("userRoleLabel");
    m_userRoleLabel->setAlignment(Qt::AlignCenter);
    QFont roleFont("Arial", 9);
    m_userRoleLabel->setFont(roleFont);

    userLayout->addWidget(m_userNameLabel);
    userLayout->addWidget(m_userRoleLabel);
    m_layout->addWidget(userWidget);

    // Separator
    QFrame *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName("sidebarSep");
    m_layout->addWidget(sep);

    // Menu buttons
    QWidget *menuWidget = new QWidget(this);
    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setContentsMargins(8, 8, 8, 8);
    menuLayout->setSpacing(4);

    struct MenuItem {
        QString text;
        QString icon;
    };

    QList<MenuItem> items;
    MenuItem m0; m0.text = QString::fromUtf8("\xd9\x86\xd9\x82\xd8\xb7\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xa8\xd9\x8a\xd8\xb9"); m0.icon = QString::fromUtf8("\xf0\x9f\x9b\x92"); items << m0;
    MenuItem m1; m1.text = QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xaa\xd8\xac\xd8\xa7\xd8\xaa"); m1.icon = QString::fromUtf8("\xf0\x9f\x93\xa6"); items << m1;
    MenuItem m2; m2.text = QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb9\xd9\x85\xd9\x84\xd8\xa7\xd8\xa1"); m2.icon = QString::fromUtf8("\xf0\x9f\x91\xa5"); items << m2;
    MenuItem m3; m3.text = QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xaa\xd9\x82\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xb1"); m3.icon = QString::fromUtf8("\xf0\x9f\x93\x8a"); items << m3;
    MenuItem m4; m4.text = QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85\xd9\x88\xd9\x86"); m4.icon = QString::fromUtf8("\xf0\x9f\x91\xa4"); items << m4;
    MenuItem m5; m5.text = QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa"); m5.icon = QString::fromUtf8("\xe2\x9a\x99"); items << m5;

    for (int i = 0; i < items.size(); ++i) {
        SidebarButton *btn = new SidebarButton(items[i].text, items[i].icon, menuWidget);
        connect(btn, &QPushButton::clicked, this, [this, i]() {
            setActiveButton(i);
            emit menuItemClicked(i);
        });
        menuLayout->addWidget(btn);
        m_buttons.append(btn);
    }

    menuLayout->addStretch();
    m_layout->addWidget(menuWidget, 1);

    // Logout button
    m_logoutBtn = new QPushButton(QString::fromUtf8("\xf0\x9f\x9a\xaa  \xd8\xaa\xd8\xb3\xd8\xac\xd9\x8a\xd9\x84 \xd8\xa7\xd9\x84\xd8\xae\xd8\xb1\xd9\x88\xd8\xac"), this);
    m_logoutBtn->setObjectName("logoutButton");
    m_logoutBtn->setFixedHeight(44);
    m_logoutBtn->setCursor(Qt::PointingHandCursor);
    connect(m_logoutBtn, &QPushButton::clicked, this, &Sidebar::logoutClicked);

    QWidget *logoutWidget = new QWidget(this);
    QVBoxLayout *logoutLayout = new QVBoxLayout(logoutWidget);
    logoutLayout->setContentsMargins(8, 4, 8, 12);
    logoutLayout->addWidget(m_logoutBtn);
    m_layout->addWidget(logoutWidget);

    setActiveButton(0);
}

void Sidebar::setUserInfo(const QString &name, const QString &role)
{
    m_userNameLabel->setText(name);
    QString roleText;
    if (role == "admin") roleText = QString::fromUtf8("\xd9\x85\xd8\xaf\xd9\x8a\xd8\xb1 \xd8\xa7\xd9\x84\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85");
    else if (role == "manager") roleText = QString::fromUtf8("\xd9\x85\xd8\xaf\xd9\x8a\xd8\xb1");
    else roleText = QString::fromUtf8("\xd9\x83\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xb1");
    m_userRoleLabel->setText(roleText);
}

void Sidebar::setActiveButton(int index)
{
    m_activeIndex = index;
    for (int i = 0; i < m_buttons.size(); ++i) {
        m_buttons[i]->setActive(i == index);
    }
}

void Sidebar::addMenuButton(const QString &text, const QString &iconChar)
{
    Q_UNUSED(text)
    Q_UNUSED(iconChar)
}
