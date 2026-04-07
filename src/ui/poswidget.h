#ifndef POSWIDGET_H
#define POSWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QComboBox>
#include <QGridLayout>
#include <QScrollArea>
#include "../models/product.h"
#include "../models/sale.h"
#include "../models/user.h"

class PosWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PosWidget(QWidget *parent = nullptr);

    void setCurrentUser(const User &user);
    void addProductByBarcode(const QString &barcode);

private slots:
    void onSearchChanged(const QString &text);
    void onCategorySelected(int index);
    void onProductClicked(int productId);
    void onRemoveItem(int row);
    void onQuantityChanged(int row);
    void onCheckout();
    void onClearCart();
    void onDiscountChanged();
    void onPaidChanged();
    void onPrintReceipt();

private:
    void setupUI();
    void loadCategories();
    void loadProducts(int categoryId = -1, const QString &search = QString());
    void addProductToCart(const Product &product, int quantity = 1);
    void updateCartTotals();
    void clearCart();
    QWidget* createProductCard(const Product &product);

    // Left panel - products
    QLineEdit *m_searchEdit;
    QComboBox *m_categoryCombo;
    QWidget *m_productsGrid;
    QGridLayout *m_gridLayout;
    QScrollArea *m_scrollArea;

    // Right panel - cart
    QTableWidget *m_cartTable;
    QLabel *m_subtotalLabel;
    QLineEdit *m_discountEdit;
    QLabel *m_taxLabel;
    QLabel *m_totalLabel;
    QLineEdit *m_paidEdit;
    QLabel *m_changeLabel;
    QComboBox *m_paymentCombo;
    QComboBox *m_customerCombo;
    QPushButton *m_checkoutBtn;
    QPushButton *m_clearBtn;
    QPushButton *m_printBtn;

    QList<SaleItem> m_cartItems;
    double m_subtotal;
    double m_discount;
    double m_tax;
    double m_total;
    User m_currentUser;
    Sale m_lastSale;
};

#endif // POSWIDGET_H
