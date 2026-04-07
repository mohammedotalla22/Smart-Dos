#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QList>
#include "../models/product.h"
#include "../models/category.h"
#include "../models/customer.h"
#include "../models/sale.h"
#include "../models/user.h"

class Database : public QObject
{
    Q_OBJECT

public:
    static Database* instance();
    bool initialize(const QString &path = "smartpos.db");
    void close();

    // Users
    bool authenticateUser(const QString &username, const QString &password);
    User getUserByUsername(const QString &username);
    QList<User> getAllUsers();
    bool addUser(User &user);
    bool updateUser(const User &user);
    bool deleteUser(int id);
    bool changePassword(int userId, const QString &newPassword);

    // Categories
    QList<Category> getAllCategories();
    bool addCategory(Category &cat);
    bool updateCategory(const Category &cat);
    bool deleteCategory(int id);

    // Products
    QList<Product> getAllProducts();
    QList<Product> getProductsByCategory(int categoryId);
    Product getProductByBarcode(const QString &barcode);
    Product getProductById(int id);
    bool addProduct(Product &prod);
    bool updateProduct(const Product &prod);
    bool deleteProduct(int id);
    bool updateProductQuantity(int id, int quantityChange);
    QList<Product> getLowStockProducts();
    QList<Product> searchProducts(const QString &keyword);

    // Customers
    QList<Customer> getAllCustomers();
    Customer getCustomerById(int id);
    bool addCustomer(Customer &cust);
    bool updateCustomer(const Customer &cust);
    bool deleteCustomer(int id);
    QList<Customer> searchCustomers(const QString &keyword);

    // Sales
    bool addSale(Sale &sale);
    QList<Sale> getSalesByDateRange(const QDateTime &from, const QDateTime &to);
    QList<Sale> getSalesByCustomer(int customerId);
    Sale getSaleById(int id);
    QList<SaleItem> getSaleItems(int saleId);
    QString generateInvoiceNumber();

    // Reports
    double getTotalSales(const QDateTime &from, const QDateTime &to);
    double getTotalProfit(const QDateTime &from, const QDateTime &to);
    double getTotalExpenses(const QDateTime &from, const QDateTime &to);
    int getTotalTransactions(const QDateTime &from, const QDateTime &to);
    QList<QPair<QString, double>> getSalesByCategory(const QDateTime &from, const QDateTime &to);
    QList<QPair<QString, double>> getTopProducts(const QDateTime &from, const QDateTime &to, int limit = 10);
    QList<QPair<QString, double>> getDailySales(const QDateTime &from, const QDateTime &to);
    QList<QPair<QString, double>> getMonthlySales(int year);

private:
    explicit Database(QObject *parent = nullptr);
    static Database *m_instance;
    QSqlDatabase m_db;
    void createTables();
    void insertDefaultData();
    QString hashPassword(const QString &password);
};

#endif // DATABASE_H
