#ifndef PRODUCTSWIDGET_H
#define PRODUCTSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>

class ProductsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProductsWidget(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onAddProduct();
    void onEditProduct();
    void onDeleteProduct();
    void onSearchChanged(const QString &text);
    void onCategoryFilterChanged(int index);

private:
    void setupUI();
    void loadProducts();
    void showProductDialog(int productId = -1);

    QTableWidget *m_table;
    QLineEdit *m_searchEdit;
    QComboBox *m_categoryFilter;
    QPushButton *m_addBtn;
    QPushButton *m_editBtn;
    QPushButton *m_deleteBtn;
};

#endif // PRODUCTSWIDGET_H
