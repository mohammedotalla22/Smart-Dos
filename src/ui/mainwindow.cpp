#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QFile>
#include <QApplication>
#include <QMessageBox>
#include "../core/database.h"
#include "../core/firebase.h"
#include "../core/hardwareid.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_settings = new QSettings("SmartPOS", "SmartPOS", this);
    m_barcodeHandler = new BarcodeHandler(this);

    connect(m_barcodeHandler, &BarcodeHandler::barcodeScanned,
            this, &MainWindow::onBarcodeScanned);

    loadSettings();
    setupUI();

    setWindowTitle(QString::fromUtf8("Smart POS - \xd9\x86\xd8\xb8\xd8\xa7\xd9\x85 \xd9\x86\xd9\x82\xd8\xa7\xd8\xb7 \xd8\xa7\xd9\x84\xd8\xa8\xd9\x8a\xd8\xb9"));
    setMinimumSize(1024, 700);
    resize(1280, 800);

    // Initialize database
    QString dbPath = m_settings->value("database/path", "smartpos.db").toString();
    Database::instance()->initialize(dbPath);

    // Configure Firebase
    QString fbProject = m_settings->value("firebase/projectId").toString();
    QString fbApiKey = m_settings->value("firebase/apiKey").toString();
    QString fbCollection = m_settings->value("firebase/collection", "devices").toString();
    if (!fbProject.isEmpty() && !fbApiKey.isEmpty()) {
        Firebase::instance()->setConfig(fbProject, fbApiKey, fbCollection);
    }

    checkActivation();
}

MainWindow::~MainWindow()
{
    saveSettings();
    Database::instance()->close();
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_mainStack = new QStackedWidget(m_centralWidget);

    // Login widget
    m_loginWidget = new LoginWidget(m_mainStack);
    connect(m_loginWidget, &LoginWidget::loginSuccess,
            this, &MainWindow::onLoginSuccess);
    m_mainStack->addWidget(m_loginWidget);

    // Activation widget
    m_activationWidget = new ActivationWidget(m_mainStack);
    connect(m_activationWidget, &ActivationWidget::activationSuccess,
            this, &MainWindow::onActivationSuccess);
    m_mainStack->addWidget(m_activationWidget);

    // Main app widget
    m_appWidget = new QWidget(m_mainStack);
    QHBoxLayout *appLayout = new QHBoxLayout(m_appWidget);
    appLayout->setContentsMargins(0, 0, 0, 0);
    appLayout->setSpacing(0);

    // Sidebar
    m_sidebar = new Sidebar(m_appWidget);
    connect(m_sidebar, &Sidebar::menuItemClicked,
            this, &MainWindow::onMenuItemClicked);
    connect(m_sidebar, &Sidebar::logoutClicked,
            this, &MainWindow::onLogout);
    appLayout->addWidget(m_sidebar);

    // Content area
    m_contentStack = new QStackedWidget(m_appWidget);

    m_posWidget = new PosWidget(m_contentStack);
    m_productsWidget = new ProductsWidget(m_contentStack);
    m_customersWidget = new CustomersWidget(m_contentStack);
    m_reportsWidget = new ReportsWidget(m_contentStack);
    m_usersWidget = new UsersWidget(m_contentStack);
    m_settingsWidget = new SettingsWidget(m_contentStack);

    connect(m_settingsWidget, &SettingsWidget::themeChanged,
            this, &MainWindow::onThemeChanged);

    m_contentStack->addWidget(m_posWidget);       // 0
    m_contentStack->addWidget(m_productsWidget);   // 1
    m_contentStack->addWidget(m_customersWidget);  // 2
    m_contentStack->addWidget(m_reportsWidget);    // 3
    m_contentStack->addWidget(m_usersWidget);      // 4
    m_contentStack->addWidget(m_settingsWidget);   // 5

    appLayout->addWidget(m_contentStack, 1);
    m_mainStack->addWidget(m_appWidget);

    mainLayout->addWidget(m_mainStack);

    // Apply theme
    applyTheme(m_currentTheme);
}

void MainWindow::loadSettings()
{
    m_currentTheme = m_settings->value("appearance/theme", "dark").toString();
}

void MainWindow::saveSettings()
{
    m_settings->setValue("appearance/theme", m_currentTheme);
}

void MainWindow::checkActivation()
{
    Firebase *fb = Firebase::instance();
    if (!fb->isConfigured()) {
        // No Firebase configured, skip activation
        showLogin();
        return;
    }

    QString hwId = HardwareId::generate();

    connect(fb, &Firebase::activationSuccess, this, [this](const QString &) {
        showLogin();
    }, Qt::UniqueConnection);

    connect(fb, &Firebase::activationFailed, this, [this](const QString &) {
        m_activationWidget->setHardwareId(HardwareId::generate());
        m_mainStack->setCurrentWidget(m_activationWidget);
    }, Qt::UniqueConnection);

    connect(fb, &Firebase::userBanned, this, [this](const QString &reason) {
        QMessageBox::critical(this,
            QString::fromUtf8("\xd8\xaa\xd9\x85 \xd8\xad\xd8\xb8\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xac\xd9\x87\xd8\xa7\xd8\xb2"),
            QString::fromUtf8("\xd8\xaa\xd9\x85 \xd8\xad\xd8\xb8\xd8\xb1 \xd9\x87\xd8\xb0\xd8\xa7 \xd8\xa7\xd9\x84\xd8\xac\xd9\x87\xd8\xa7\xd8\xb2.\n\xd8\xa7\xd9\x84\xd8\xb3\xd8\xa8\xd8\xa8: ") + reason);
        QApplication::quit();
    }, Qt::UniqueConnection);

    connect(fb, &Firebase::appBanned, this, [this](const QString &reason) {
        QMessageBox::critical(this,
            QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82 \xd9\x85\xd9\x88\xd9\x82\xd9\x88\xd9\x81"),
            QString::fromUtf8("\xd8\xaa\xd9\x85 \xd8\xa5\xd9\x8a\xd9\x82\xd8\xa7\xd9\x81 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xb7\xd8\xa8\xd9\x8a\xd9\x82.\n\xd8\xa7\xd9\x84\xd8\xb3\xd8\xa8\xd8\xa8: ") + reason);
        QApplication::quit();
    }, Qt::UniqueConnection);

    connect(fb, &Firebase::connectionError, this, [this](const QString &) {
        // Offline mode - allow login
        showLogin();
    }, Qt::UniqueConnection);

    fb->checkActivation(hwId);
    fb->checkAppBan();
}

void MainWindow::showMainInterface()
{
    m_sidebar->setUserInfo(m_currentUser.displayName, m_currentUser.role);
    m_posWidget->setCurrentUser(m_currentUser);
    m_mainStack->setCurrentWidget(m_appWidget);
    m_contentStack->setCurrentIndex(0);
    m_sidebar->setActiveButton(0);
}

void MainWindow::showLogin()
{
    m_mainStack->setCurrentWidget(m_loginWidget);
}

void MainWindow::onLoginSuccess(const User &user)
{
    m_currentUser = user;

    // Update last seen on Firebase
    Firebase *fb = Firebase::instance();
    if (fb->isConfigured()) {
        fb->updateDeviceLastSeen(HardwareId::generate());
    }

    showMainInterface();
}

void MainWindow::onLogout()
{
    m_currentUser = User();
    showLogin();
}

void MainWindow::onMenuItemClicked(int index)
{
    if (index >= 0 && index < m_contentStack->count()) {
        m_contentStack->setCurrentIndex(index);

        if (index == 3) {
            m_reportsWidget->refreshData();
        } else if (index == 1) {
            m_productsWidget->refreshData();
        } else if (index == 2) {
            m_customersWidget->refreshData();
        } else if (index == 4) {
            m_usersWidget->refreshData();
        }
    }
}

void MainWindow::onActivationSuccess()
{
    showLogin();
}

void MainWindow::onBarcodeScanned(const QString &barcode)
{
    if (m_contentStack->currentWidget() == m_posWidget) {
        m_posWidget->addProductByBarcode(barcode);
    }
}

void MainWindow::onThemeChanged(const QString &theme)
{
    applyTheme(theme);
    m_settings->setValue("appearance/theme", theme);
}

void MainWindow::applyTheme(const QString &theme)
{
    m_currentTheme = theme;
    QString qssPath = (theme == "light") ? ":/styles/light.qss" : ":/styles/dark.qss";

    QFile file(qssPath);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString stylesheet = file.readAll();
        qApp->setStyleSheet(stylesheet);
        file.close();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    m_barcodeHandler->processKeyPress(event->key(), event->text());
    QMainWindow::keyPressEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    QMainWindow::closeEvent(event);
}
