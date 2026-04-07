#ifndef USERSWIDGET_H
#define USERSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>

class UsersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UsersWidget(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onAddUser();
    void onEditUser();
    void onDeleteUser();
    void onResetPassword();

private:
    void setupUI();
    void loadUsers();
    void showUserDialog(int userId = -1);

    QTableWidget *m_table;
    QPushButton *m_addBtn;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
    QPushButton *m_resetPwdBtn;
};

#endif // USERSWIDGET_H
