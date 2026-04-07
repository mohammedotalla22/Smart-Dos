#include "userswidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QFont>
#include <QInputDialog>
#include "../core/database.h"

UsersWidget::UsersWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadUsers();
}

void UsersWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    QLabel *title = new QLabel(
        QString::fromUtf8("\xf0\x9f\x91\xa4 \xd8\xa5\xd8\xaf\xd8\xa7\xd8\xb1\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85\xd9\x8a\xd9\x86"), this);
    title->setObjectName("pageTitle");
    QFont titleFont("Arial", 20, QFont::Bold);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignRight);
    mainLayout->addWidget(title);

    QHBoxLayout *toolbar = new QHBoxLayout();

    m_resetPwdBtn = new QPushButton(
        QString::fromUtf8("\xd8\xa5\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9 \xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1"), this);
    m_resetPwdBtn->setObjectName("secondaryButton");
    m_resetPwdBtn->setFixedHeight(38);
    m_resetPwdBtn->setCursor(Qt::PointingHandCursor);
    toolbar->addWidget(m_resetPwdBtn);

    m_deleteBtn = new QPushButton(
        QString::fromUtf8("\xd8\xad\xd8\xb0\xd9\x81"), this);
    m_deleteBtn->setObjectName("dangerButton");
    m_deleteBtn->setFixedHeight(38);
    m_deleteBtn->setCursor(Qt::PointingHandCursor);
    toolbar->addWidget(m_deleteBtn);

    m_editBtn = new QPushButton(
        QString::fromUtf8("\xd8\xaa\xd8\xb9\xd8\xaf\xd9\x8a\xd9\x84"), this);
    m_editBtn->setObjectName("secondaryButton");
    m_editBtn->setFixedHeight(38);
    m_editBtn->setCursor(Qt::PointingHandCursor);
    toolbar->addWidget(m_editBtn);

    m_addBtn = new QPushButton(
        QString::fromUtf8("+ \xd8\xa5\xd8\xb6\xd8\xa7\xd9\x81\xd8\xa9 \xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85"), this);
    m_addBtn->setObjectName("primaryButton");
    m_addBtn->setFixedHeight(38);
    m_addBtn->setCursor(Qt::PointingHandCursor);
    toolbar->addWidget(m_addBtn);

    toolbar->addStretch();
    mainLayout->addLayout(toolbar);

    m_table = new QTableWidget(0, 7, this);
    m_table->setObjectName("dataTable");
    QStringList headers;
    headers << "#"
            << QString::fromUtf8("\xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x83\xd8\xa7\xd9\x85\xd9\x84")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xaf\xd9\x88\xd8\xb1")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x87\xd8\xa7\xd8\xaa\xd9\x81")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9")
            << QString::fromUtf8("\xd8\xa2\xd8\xae\xd8\xb1 \xd8\xaf\xd8\xae\xd9\x88\xd9\x84");
    m_table->setHorizontalHeaderLabels(headers);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->hide();
    m_table->setAlternatingRowColors(true);
    m_table->setLayoutDirection(Qt::RightToLeft);
    mainLayout->addWidget(m_table, 1);

    connect(m_addBtn, &QPushButton::clicked, this, &UsersWidget::onAddUser);
    connect(m_editBtn, &QPushButton::clicked, this, &UsersWidget::onEditUser);
    connect(m_deleteBtn, &QPushButton::clicked, this, &UsersWidget::onDeleteUser);
    connect(m_resetPwdBtn, &QPushButton::clicked, this, &UsersWidget::onResetPassword);
    connect(m_table, &QTableWidget::doubleClicked, this, &UsersWidget::onEditUser);
}

void UsersWidget::refreshData()
{
    loadUsers();
}

void UsersWidget::loadUsers()
{
    m_table->setRowCount(0);
    QList<User> users = Database::instance()->getAllUsers();
    for (int i = 0; i < users.size(); ++i) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(users[i].id)));
        m_table->setItem(row, 1, new QTableWidgetItem(users[i].username));
        m_table->setItem(row, 2, new QTableWidgetItem(users[i].displayName));

        QString roleText;
        if (users[i].role == "admin") roleText = QString::fromUtf8("\xd9\x85\xd8\xaf\xd9\x8a\xd8\xb1");
        else if (users[i].role == "manager") roleText = QString::fromUtf8("\xd9\x85\xd8\xb4\xd8\xb1\xd9\x81");
        else roleText = QString::fromUtf8("\xd9\x83\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xb1");
        m_table->setItem(row, 3, new QTableWidgetItem(roleText));

        m_table->setItem(row, 4, new QTableWidgetItem(users[i].phone));

        QString status = users[i].active ?
            QString::fromUtf8("\xd9\x86\xd8\xb4\xd8\xb7") :
            QString::fromUtf8("\xd9\x85\xd9\x88\xd9\x82\xd9\x88\xd9\x81");
        QTableWidgetItem *statusItem = new QTableWidgetItem(status);
        if (!users[i].active) statusItem->setForeground(QBrush(QColor("#e74c3c")));
        m_table->setItem(row, 5, statusItem);

        m_table->setItem(row, 6, new QTableWidgetItem(
            users[i].lastLogin.isValid() ? users[i].lastLogin.toString("yyyy/MM/dd hh:mm") : "-"));
    }
}

void UsersWidget::showUserDialog(int userId)
{
    User user;
    bool isEdit = (userId > 0);
    if (isEdit) {
        QList<User> users = Database::instance()->getAllUsers();
        for (int i = 0; i < users.size(); ++i) {
            if (users[i].id == userId) { user = users[i]; break; }
        }
    }

    QDialog dialog(this);
    dialog.setWindowTitle(isEdit ?
        QString::fromUtf8("\xd8\xaa\xd8\xb9\xd8\xaf\xd9\x8a\xd9\x84 \xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85") :
        QString::fromUtf8("\xd8\xa5\xd8\xb6\xd8\xa7\xd9\x81\xd8\xa9 \xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85 \xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf"));
    dialog.setFixedSize(400, 380);
    dialog.setLayoutDirection(Qt::RightToLeft);

    QFormLayout *form = new QFormLayout(&dialog);
    form->setContentsMargins(20, 20, 20, 20);

    QLineEdit *usernameEdit = new QLineEdit(user.username, &dialog);
    usernameEdit->setFixedHeight(36);
    if (isEdit) usernameEdit->setReadOnly(true);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85:"), usernameEdit);

    QLineEdit *passwordEdit = new QLineEdit(&dialog);
    passwordEdit->setFixedHeight(36);
    passwordEdit->setEchoMode(QLineEdit::Password);
    if (isEdit) passwordEdit->setPlaceholderText(
        QString::fromUtf8("\xd8\xa7\xd8\xaa\xd8\xb1\xd9\x83 \xd9\x81\xd8\xa7\xd8\xb1\xd8\xba\xd8\xa7\xd9\x8b \xd9\x84\xd8\xb9\xd8\xaf\xd9\x85 \xd8\xa7\xd9\x84\xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1"));
    form->addRow(QString::fromUtf8("\xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1:"), passwordEdit);

    QLineEdit *displayEdit = new QLineEdit(user.displayName, &dialog);
    displayEdit->setFixedHeight(36);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x83\xd8\xa7\xd9\x85\xd9\x84:"), displayEdit);

    QComboBox *roleCombo = new QComboBox(&dialog);
    roleCombo->setFixedHeight(36);
    roleCombo->addItem(QString::fromUtf8("\xd9\x83\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xb1"), "cashier");
    roleCombo->addItem(QString::fromUtf8("\xd9\x85\xd8\xb4\xd8\xb1\xd9\x81"), "manager");
    roleCombo->addItem(QString::fromUtf8("\xd9\x85\xd8\xaf\xd9\x8a\xd8\xb1"), "admin");
    if (user.role == "manager") roleCombo->setCurrentIndex(1);
    else if (user.role == "admin") roleCombo->setCurrentIndex(2);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xaf\xd9\x88\xd8\xb1:"), roleCombo);

    QLineEdit *phoneEdit = new QLineEdit(user.phone, &dialog);
    phoneEdit->setFixedHeight(36);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x87\xd8\xa7\xd8\xaa\xd9\x81:"), phoneEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setText(QString::fromUtf8("\xd8\xad\xd9\x81\xd8\xb8"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QString::fromUtf8("\xd8\xa5\xd9\x84\xd8\xba\xd8\xa7\xd8\xa1"));
    form->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        if (usernameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this,
                QString::fromUtf8("\xd8\xae\xd8\xb7\xd8\xa3"),
                QString::fromUtf8("\xd9\x8a\xd8\xb1\xd8\xac\xd9\x89 \xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84 \xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85"));
            return;
        }

        user.username = usernameEdit->text().trimmed();
        user.displayName = displayEdit->text().trimmed();
        user.role = roleCombo->currentData().toString();
        user.phone = phoneEdit->text().trimmed();

        bool ok;
        if (isEdit) {
            ok = Database::instance()->updateUser(user);
        } else {
            if (passwordEdit->text().isEmpty()) {
                QMessageBox::warning(this,
                    QString::fromUtf8("\xd8\xae\xd8\xb7\xd8\xa3"),
                    QString::fromUtf8("\xd9\x8a\xd8\xb1\xd8\xac\xd9\x89 \xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84 \xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1"));
                return;
            }
            user.passwordHash = passwordEdit->text();
            ok = Database::instance()->addUser(user);
        }

        if (ok) {
            if (isEdit && !passwordEdit->text().isEmpty()) {
                Database::instance()->changePassword(user.id, passwordEdit->text());
            }
            loadUsers();
        }
    }
}

void UsersWidget::onAddUser() { showUserDialog(); }
void UsersWidget::onEditUser() {
    int row = m_table->currentRow();
    if (row < 0) return;
    showUserDialog(m_table->item(row, 0)->text().toInt());
}
void UsersWidget::onDeleteUser() {
    int row = m_table->currentRow();
    if (row < 0) return;
    if (QMessageBox::question(this,
        QString::fromUtf8("\xd8\xaa\xd8\xa3\xd9\x83\xd9\x8a\xd8\xaf"),
        QString::fromUtf8("\xd9\x87\xd9\x84 \xd8\xaa\xd8\xb1\xd9\x8a\xd8\xaf \xd8\xad\xd8\xb0\xd9\x81 \xd9\x87\xd8\xb0\xd8\xa7 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb3\xd8\xaa\xd8\xae\xd8\xaf\xd9\x85\xd8\x9f"),
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        Database::instance()->deleteUser(m_table->item(row, 0)->text().toInt());
        loadUsers();
    }
}
void UsersWidget::onResetPassword() {
    int row = m_table->currentRow();
    if (row < 0) return;
    bool ok;
    QString newPass = QInputDialog::getText(this,
        QString::fromUtf8("\xd8\xa5\xd8\xb9\xd8\xa7\xd8\xaf\xd8\xa9 \xd8\xaa\xd8\xb9\xd9\x8a\xd9\x8a\xd9\x86 \xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1"),
        QString::fromUtf8("\xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf\xd8\xa9:"),
        QLineEdit::Password, "", &ok);
    if (ok && !newPass.isEmpty()) {
        int id = m_table->item(row, 0)->text().toInt();
        Database::instance()->changePassword(id, newPass);
        QMessageBox::information(this,
            QString::fromUtf8("\xd8\xaa\xd9\x85"),
            QString::fromUtf8("\xd8\xaa\xd9\x85 \xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1 \xd9\x83\xd9\x84\xd9\x85\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb1\xd9\x88\xd8\xb1 \xd8\xa8\xd9\x86\xd8\xac\xd8\xa7\xd8\xad"));
    }
}
