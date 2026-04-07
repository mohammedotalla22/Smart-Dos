#ifndef CUSTOMERSWIDGET_H
#define CUSTOMERSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>

class CustomersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CustomersWidget(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onAddCustomer();
    void onEditCustomer();
    void onDeleteCustomer();
    void onSearchChanged(const QString &text);

private:
    void setupUI();
    void loadCustomers();
    void showCustomerDialog(int customerId = -1);

    QTableWidget *m_table;
    QLineEdit *m_searchEdit;
    QPushButton *m_addBtn;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
};

#endif // CUSTOMERSWIDGET_H
