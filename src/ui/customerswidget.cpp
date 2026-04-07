#include "customerswidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QTextEdit>
#include <QLabel>
#include <QFont>
#include "../core/database.h"

CustomersWidget::CustomersWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadCustomers();
}

void CustomersWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    QLabel *title = new QLabel(
        QString::fromUtf8("\xf0\x9f\x91\xa5 \xd8\xa5\xd8\xaf\xd8\xa7\xd8\xb1\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xb9\xd9\x85\xd9\x84\xd8\xa7\xd8\xa1"), this);
    title->setObjectName("pageTitle");
    QFont titleFont("Arial", 20, QFont::Bold);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignRight);
    mainLayout->addWidget(title);

    QHBoxLayout *toolbar = new QHBoxLayout();

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
        QString::fromUtf8("+ \xd8\xa5\xd8\xb6\xd8\xa7\xd9\x81\xd8\xa9 \xd8\xb9\xd9\x85\xd9\x8a\xd9\x84"), this);
    m_addBtn->setObjectName("primaryButton");
    m_addBtn->setFixedHeight(38);
    m_addBtn->setCursor(Qt::PointingHandCursor);
    toolbar->addWidget(m_addBtn);

    toolbar->addStretch();

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setObjectName("searchInput");
    m_searchEdit->setPlaceholderText(
        QString::fromUtf8("\xd8\xa8\xd8\xad\xd8\xab \xd8\xb9\xd9\x86 \xd8\xb9\xd9\x85\xd9\x8a\xd9\x84..."));
    m_searchEdit->setFixedHeight(38);
    m_searchEdit->setFixedWidth(250);
    m_searchEdit->setAlignment(Qt::AlignRight);
    toolbar->addWidget(m_searchEdit);

    mainLayout->addLayout(toolbar);

    m_table = new QTableWidget(0, 7, this);
    m_table->setObjectName("dataTable");
    QStringList headers;
    headers << "#"
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x87\xd8\xa7\xd8\xaa\xd9\x81")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa8\xd8\xb1\xd9\x8a\xd8\xaf")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb1\xd8\xb5\xd9\x8a\xd8\xaf")
            << QString::fromUtf8("\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a \xd8\xa7\xd9\x84\xd9\x85\xd8\xb4\xd8\xaa\xd8\xb1\xd9\x8a\xd8\xa7\xd8\xaa");
    m_table->setHorizontalHeaderLabels(headers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->hide();
    m_table->setAlternatingRowColors(true);
    m_table->setLayoutDirection(Qt::RightToLeft);
    mainLayout->addWidget(m_table, 1);

    connect(m_addBtn, &QPushButton::clicked, this, &CustomersWidget::onAddCustomer);
    connect(m_editBtn, &QPushButton::clicked, this, &CustomersWidget::onEditCustomer);
    connect(m_deleteBtn, &QPushButton::clicked, this, &CustomersWidget::onDeleteCustomer);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &CustomersWidget::onSearchChanged);
    connect(m_table, &QTableWidget::doubleClicked, this, &CustomersWidget::onEditCustomer);
}

void CustomersWidget::refreshData()
{
    loadCustomers();
}

void CustomersWidget::loadCustomers()
{
    m_table->setRowCount(0);
    QString search = m_searchEdit->text().trimmed();
    QList<Customer> customers;
    if (!search.isEmpty()) {
        customers = Database::instance()->searchCustomers(search);
    } else {
        customers = Database::instance()->getAllCustomers();
    }

    for (int i = 0; i < customers.size(); ++i) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(customers[i].id)));
        m_table->setItem(row, 1, new QTableWidgetItem(customers[i].name));
        m_table->setItem(row, 2, new QTableWidgetItem(customers[i].phone));
        m_table->setItem(row, 3, new QTableWidgetItem(customers[i].email));
        m_table->setItem(row, 4, new QTableWidgetItem(customers[i].address));
        m_table->setItem(row, 5, new QTableWidgetItem(QString::number(customers[i].balance, 'f', 2)));
        m_table->setItem(row, 6, new QTableWidgetItem(QString::number(customers[i].totalPurchases, 'f', 2)));
    }
}

void CustomersWidget::showCustomerDialog(int customerId)
{
    Customer customer;
    bool isEdit = (customerId > 0);
    if (isEdit) {
        customer = Database::instance()->getCustomerById(customerId);
    }

    QDialog dialog(this);
    dialog.setWindowTitle(isEdit ?
        QString::fromUtf8("\xd8\xaa\xd8\xb9\xd8\xaf\xd9\x8a\xd9\x84 \xd8\xb9\xd9\x85\xd9\x8a\xd9\x84") :
        QString::fromUtf8("\xd8\xa5\xd8\xb6\xd8\xa7\xd9\x81\xd8\xa9 \xd8\xb9\xd9\x85\xd9\x8a\xd9\x84 \xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf"));
    dialog.setFixedSize(400, 400);
    dialog.setLayoutDirection(Qt::RightToLeft);

    QFormLayout *form = new QFormLayout(&dialog);
    form->setContentsMargins(20, 20, 20, 20);

    QLineEdit *nameEdit = new QLineEdit(customer.name, &dialog);
    nameEdit->setFixedHeight(36);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85:"), nameEdit);

    QLineEdit *phoneEdit = new QLineEdit(customer.phone, &dialog);
    phoneEdit->setFixedHeight(36);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x87\xd8\xa7\xd8\xaa\xd9\x81:"), phoneEdit);

    QLineEdit *emailEdit = new QLineEdit(customer.email, &dialog);
    emailEdit->setFixedHeight(36);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa8\xd8\xb1\xd9\x8a\xd8\xaf:"), emailEdit);

    QLineEdit *addrEdit = new QLineEdit(customer.address, &dialog);
    addrEdit->setFixedHeight(36);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86:"), addrEdit);

    QTextEdit *notesEdit = new QTextEdit(customer.notes, &dialog);
    notesEdit->setFixedHeight(80);
    form->addRow(QString::fromUtf8("\xd9\x85\xd9\x84\xd8\xa7\xd8\xad\xd8\xb8\xd8\xa7\xd8\xaa:"), notesEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setText(QString::fromUtf8("\xd8\xad\xd9\x81\xd8\xb8"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QString::fromUtf8("\xd8\xa5\xd9\x84\xd8\xba\xd8\xa7\xd8\xa1"));
    form->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        if (nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this,
                QString::fromUtf8("\xd8\xae\xd8\xb7\xd8\xa3"),
                QString::fromUtf8("\xd9\x8a\xd8\xb1\xd8\xac\xd9\x89 \xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84 \xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd8\xb9\xd9\x85\xd9\x8a\xd9\x84"));
            return;
        }
        customer.name = nameEdit->text().trimmed();
        customer.phone = phoneEdit->text().trimmed();
        customer.email = emailEdit->text().trimmed();
        customer.address = addrEdit->text().trimmed();
        customer.notes = notesEdit->toPlainText();

        bool ok;
        if (isEdit) {
            ok = Database::instance()->updateCustomer(customer);
        } else {
            ok = Database::instance()->addCustomer(customer);
        }
        if (ok) loadCustomers();
    }
}

void CustomersWidget::onAddCustomer() { showCustomerDialog(); }
void CustomersWidget::onEditCustomer() {
    int row = m_table->currentRow();
    if (row < 0) return;
    showCustomerDialog(m_table->item(row, 0)->text().toInt());
}
void CustomersWidget::onDeleteCustomer() {
    int row = m_table->currentRow();
    if (row < 0) return;
    if (QMessageBox::question(this,
        QString::fromUtf8("\xd8\xaa\xd8\xa3\xd9\x83\xd9\x8a\xd8\xaf"),
        QString::fromUtf8("\xd9\x87\xd9\x84 \xd8\xaa\xd8\xb1\xd9\x8a\xd8\xaf \xd8\xad\xd8\xb0\xd9\x81 \xd9\x87\xd8\xb0\xd8\xa7 \xd8\xa7\xd9\x84\xd8\xb9\xd9\x85\xd9\x8a\xd9\x84\xd8\x9f"),
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        Database::instance()->deleteCustomer(m_table->item(row, 0)->text().toInt());
        loadCustomers();
    }
}
void CustomersWidget::onSearchChanged(const QString &) { loadCustomers(); }
