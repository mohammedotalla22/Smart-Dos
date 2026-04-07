#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "../models/user.h"

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);

signals:
    void loginSuccess(const User &user);

private slots:
    void onLoginClicked();

private:
    void setupUI();

    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginBtn;
    QLabel *m_errorLabel;
    QLabel *m_versionLabel;
};

#endif // LOGINWIDGET_H
