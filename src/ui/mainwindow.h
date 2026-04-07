#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QSettings>
#include "widgets/sidebar.h"
#include "loginwidget.h"
#include "activationwidget.h"
#include "poswidget.h"
#include "productswidget.h"
#include "customerswidget.h"
#include "reportswidget.h"
#include "settingswidget.h"
#include "userswidget.h"
#include "../core/barcodehandler.h"
#include "../models/user.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void applyTheme(const QString &theme);
    QString currentTheme() const { return m_currentTheme; }

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onLoginSuccess(const User &user);
    void onLogout();
    void onMenuItemClicked(int index);
    void onActivationSuccess();
    void onBarcodeScanned(const QString &barcode);
    void onThemeChanged(const QString &theme);

private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    void checkActivation();
    void showMainInterface();
    void showLogin();

    QWidget *m_centralWidget;
    QStackedWidget *m_mainStack;
    QWidget *m_appWidget;
    Sidebar *m_sidebar;
    QStackedWidget *m_contentStack;

    LoginWidget *m_loginWidget;
    ActivationWidget *m_activationWidget;
    PosWidget *m_posWidget;
    ProductsWidget *m_productsWidget;
    CustomersWidget *m_customersWidget;
    ReportsWidget *m_reportsWidget;
    SettingsWidget *m_settingsWidget;
    UsersWidget *m_usersWidget;

    BarcodeHandler *m_barcodeHandler;
    QSettings *m_settings;
    User m_currentUser;
    QString m_currentTheme;
};

#endif // MAINWINDOW_H
