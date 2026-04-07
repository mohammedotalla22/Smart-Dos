#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>
#include <QDateTime>
#include <QDir>

Database* Database::m_instance = nullptr;

Database::Database(QObject *parent) : QObject(parent) {}

Database* Database::instance()
{
    if (!m_instance)
        m_instance = new Database();
    return m_instance;
}

bool Database::initialize(const QString &path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);

    if (!m_db.open()) {
        qCritical() << "Database error:" << m_db.lastError().text();
        return false;
    }

    createTables();
    insertDefaultData();
    return true;
}

void Database::close()
{
    if (m_db.isOpen())
        m_db.close();
}

void Database::createTables()
{
    QSqlQuery q(m_db);

    q.exec("CREATE TABLE IF NOT EXISTS users ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "username TEXT UNIQUE NOT NULL,"
           "password_hash TEXT NOT NULL,"
           "display_name TEXT,"
           "role TEXT DEFAULT 'cashier',"
           "email TEXT,"
           "phone TEXT,"
           "active INTEGER DEFAULT 1,"
           "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
           "last_login DATETIME)");

    q.exec("CREATE TABLE IF NOT EXISTS categories ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "name TEXT NOT NULL,"
           "description TEXT,"
           "color TEXT DEFAULT '#3498db',"
           "icon TEXT,"
           "active INTEGER DEFAULT 1)");

    q.exec("CREATE TABLE IF NOT EXISTS products ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "name TEXT NOT NULL,"
           "barcode TEXT UNIQUE,"
           "description TEXT,"
           "category_id INTEGER,"
           "purchase_price REAL DEFAULT 0,"
           "sale_price REAL DEFAULT 0,"
           "quantity INTEGER DEFAULT 0,"
           "min_quantity INTEGER DEFAULT 5,"
           "unit TEXT DEFAULT 'piece',"
           "image_path TEXT,"
           "active INTEGER DEFAULT 1,"
           "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
           "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
           "FOREIGN KEY (category_id) REFERENCES categories(id))");

    q.exec("CREATE TABLE IF NOT EXISTS customers ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "name TEXT NOT NULL,"
           "phone TEXT,"
           "email TEXT,"
           "address TEXT,"
           "balance REAL DEFAULT 0,"
           "total_purchases REAL DEFAULT 0,"
           "notes TEXT,"
           "active INTEGER DEFAULT 1,"
           "created_at DATETIME DEFAULT CURRENT_TIMESTAMP)");

    q.exec("CREATE TABLE IF NOT EXISTS sales ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "invoice_number TEXT UNIQUE NOT NULL,"
           "customer_id INTEGER,"
           "customer_name TEXT,"
           "user_id INTEGER,"
           "user_name TEXT,"
           "subtotal REAL DEFAULT 0,"
           "discount REAL DEFAULT 0,"
           "tax REAL DEFAULT 0,"
           "total REAL DEFAULT 0,"
           "paid REAL DEFAULT 0,"
           "change_amount REAL DEFAULT 0,"
           "payment_method TEXT DEFAULT 'cash',"
           "notes TEXT,"
           "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
           "FOREIGN KEY (customer_id) REFERENCES customers(id),"
           "FOREIGN KEY (user_id) REFERENCES users(id))");

    q.exec("CREATE TABLE IF NOT EXISTS sale_items ("
           "id INTEGER PRIMARY KEY AUTOINCREMENT,"
           "sale_id INTEGER NOT NULL,"
           "product_id INTEGER,"
           "product_name TEXT,"
           "barcode TEXT,"
           "quantity INTEGER DEFAULT 1,"
           "unit_price REAL DEFAULT 0,"
           "discount REAL DEFAULT 0,"
           "total REAL DEFAULT 0,"
           "FOREIGN KEY (sale_id) REFERENCES sales(id),"
           "FOREIGN KEY (product_id) REFERENCES products(id))");

    q.exec("CREATE TABLE IF NOT EXISTS settings ("
           "key TEXT PRIMARY KEY,"
           "value TEXT)");
}

void Database::insertDefaultData()
{
    QSqlQuery q(m_db);
    q.prepare("SELECT COUNT(*) FROM users");
    q.exec();
    q.next();
    if (q.value(0).toInt() == 0) {
        User admin;
        admin.username = "admin";
        admin.displayName = QString::fromUtf8("\xd9\x85\xd8\xaf\xd9\x8a\xd8\xb1 \xd8\xa7\xd9\x84\xd9\x86\xd8\xb8\xd8\xa7\xd9\x85");
        admin.role = "admin";
        admin.passwordHash = hashPassword("admin");
        q.prepare("INSERT INTO users (username, password_hash, display_name, role) VALUES (?,?,?,?)");
        q.addBindValue(admin.username);
        q.addBindValue(admin.passwordHash);
        q.addBindValue(admin.displayName);
        q.addBindValue(admin.role);
        q.exec();
    }

    q.prepare("SELECT COUNT(*) FROM categories");
    q.exec();
    q.next();
    if (q.value(0).toInt() == 0) {
        QStringList names;
        names << QString::fromUtf8("\xd9\x85\xd8\xb4\xd8\xb1\xd9\x88\xd8\xa8\xd8\xa7\xd8\xaa")
              << QString::fromUtf8("\xd9\x85\xd8\xa3\xd9\x83\xd9\x88\xd9\x84\xd8\xa7\xd8\xaa")
              << QString::fromUtf8("\xd8\xa5\xd9\x84\xd9\x83\xd8\xaa\xd8\xb1\xd9\x88\xd9\x86\xd9\x8a\xd8\xa7\xd8\xaa")
              << QString::fromUtf8("\xd9\x85\xd9\x84\xd8\xa7\xd8\xa8\xd8\xb3")
              << QString::fromUtf8("\xd8\xa3\xd8\xae\xd8\xb1\xd9\x89");
        QStringList colors;
        colors << "#e74c3c" << "#2ecc71" << "#3498db" << "#9b59b6" << "#f39c12";
        for (int i = 0; i < names.size(); ++i) {
            q.prepare("INSERT INTO categories (name, color) VALUES (?,?)");
            q.addBindValue(names[i]);
            q.addBindValue(colors[i]);
            q.exec();
        }
    }
}

QString Database::hashPassword(const QString &password)
{
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

// ========== Users ==========

bool Database::authenticateUser(const QString &username, const QString &password)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT password_hash FROM users WHERE username=? AND active=1");
    q.addBindValue(username);
    q.exec();
    if (q.next()) {
        if (q.value(0).toString() == hashPassword(password)) {
            QSqlQuery u(m_db);
            u.prepare("UPDATE users SET last_login=CURRENT_TIMESTAMP WHERE username=?");
            u.addBindValue(username);
            u.exec();
            return true;
        }
    }
    return false;
}

User Database::getUserByUsername(const QString &username)
{
    User user;
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM users WHERE username=?");
    q.addBindValue(username);
    q.exec();
    if (q.next()) {
        user.id = q.value("id").toInt();
        user.username = q.value("username").toString();
        user.passwordHash = q.value("password_hash").toString();
        user.displayName = q.value("display_name").toString();
        user.role = q.value("role").toString();
        user.email = q.value("email").toString();
        user.phone = q.value("phone").toString();
        user.active = q.value("active").toBool();
        user.createdAt = q.value("created_at").toDateTime();
        user.lastLogin = q.value("last_login").toDateTime();
    }
    return user;
}

QList<User> Database::getAllUsers()
{
    QList<User> list;
    QSqlQuery q(m_db);
    q.exec("SELECT * FROM users ORDER BY id");
    while (q.next()) {
        User u;
        u.id = q.value("id").toInt();
        u.username = q.value("username").toString();
        u.displayName = q.value("display_name").toString();
        u.role = q.value("role").toString();
        u.email = q.value("email").toString();
        u.phone = q.value("phone").toString();
        u.active = q.value("active").toBool();
        u.createdAt = q.value("created_at").toDateTime();
        u.lastLogin = q.value("last_login").toDateTime();
        list.append(u);
    }
    return list;
}

bool Database::addUser(User &user)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO users (username, password_hash, display_name, role, email, phone) VALUES (?,?,?,?,?,?)");
    q.addBindValue(user.username);
    q.addBindValue(hashPassword(user.passwordHash));
    q.addBindValue(user.displayName);
    q.addBindValue(user.role);
    q.addBindValue(user.email);
    q.addBindValue(user.phone);
    if (q.exec()) {
        user.id = q.lastInsertId().toInt();
        return true;
    }
    return false;
}

bool Database::updateUser(const User &user)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE users SET display_name=?, role=?, email=?, phone=?, active=? WHERE id=?");
    q.addBindValue(user.displayName);
    q.addBindValue(user.role);
    q.addBindValue(user.email);
    q.addBindValue(user.phone);
    q.addBindValue(user.active ? 1 : 0);
    q.addBindValue(user.id);
    return q.exec();
}

bool Database::deleteUser(int id)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE users SET active=0 WHERE id=?");
    q.addBindValue(id);
    return q.exec();
}

bool Database::changePassword(int userId, const QString &newPassword)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE users SET password_hash=? WHERE id=?");
    q.addBindValue(hashPassword(newPassword));
    q.addBindValue(userId);
    return q.exec();
}

// ========== Categories ==========

QList<Category> Database::getAllCategories()
{
    QList<Category> list;
    QSqlQuery q(m_db);
    q.exec("SELECT * FROM categories WHERE active=1 ORDER BY name");
    while (q.next()) {
        Category c;
        c.id = q.value("id").toInt();
        c.name = q.value("name").toString();
        c.description = q.value("description").toString();
        c.color = q.value("color").toString();
        c.icon = q.value("icon").toString();
        c.active = q.value("active").toBool();
        list.append(c);
    }
    return list;
}

bool Database::addCategory(Category &cat)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO categories (name, description, color, icon) VALUES (?,?,?,?)");
    q.addBindValue(cat.name);
    q.addBindValue(cat.description);
    q.addBindValue(cat.color);
    q.addBindValue(cat.icon);
    if (q.exec()) {
        cat.id = q.lastInsertId().toInt();
        return true;
    }
    return false;
}

bool Database::updateCategory(const Category &cat)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE categories SET name=?, description=?, color=?, icon=? WHERE id=?");
    q.addBindValue(cat.name);
    q.addBindValue(cat.description);
    q.addBindValue(cat.color);
    q.addBindValue(cat.icon);
    q.addBindValue(cat.id);
    return q.exec();
}

bool Database::deleteCategory(int id)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE categories SET active=0 WHERE id=?");
    q.addBindValue(id);
    return q.exec();
}

// ========== Products ==========

QList<Product> Database::getAllProducts()
{
    QList<Product> list;
    QSqlQuery q(m_db);
    q.exec("SELECT * FROM products WHERE active=1 ORDER BY name");
    while (q.next()) {
        Product p;
        p.id = q.value("id").toInt();
        p.name = q.value("name").toString();
        p.barcode = q.value("barcode").toString();
        p.description = q.value("description").toString();
        p.categoryId = q.value("category_id").toInt();
        p.purchasePrice = q.value("purchase_price").toDouble();
        p.salePrice = q.value("sale_price").toDouble();
        p.quantity = q.value("quantity").toInt();
        p.minQuantity = q.value("min_quantity").toInt();
        p.unit = q.value("unit").toString();
        p.imagePath = q.value("image_path").toString();
        p.active = q.value("active").toBool();
        p.createdAt = q.value("created_at").toDateTime();
        p.updatedAt = q.value("updated_at").toDateTime();
        list.append(p);
    }
    return list;
}

QList<Product> Database::getProductsByCategory(int categoryId)
{
    QList<Product> list;
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM products WHERE category_id=? AND active=1 ORDER BY name");
    q.addBindValue(categoryId);
    q.exec();
    while (q.next()) {
        Product p;
        p.id = q.value("id").toInt();
        p.name = q.value("name").toString();
        p.barcode = q.value("barcode").toString();
        p.categoryId = q.value("category_id").toInt();
        p.purchasePrice = q.value("purchase_price").toDouble();
        p.salePrice = q.value("sale_price").toDouble();
        p.quantity = q.value("quantity").toInt();
        p.minQuantity = q.value("min_quantity").toInt();
        p.unit = q.value("unit").toString();
        p.active = q.value("active").toBool();
        list.append(p);
    }
    return list;
}

Product Database::getProductByBarcode(const QString &barcode)
{
    Product p;
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM products WHERE barcode=? AND active=1");
    q.addBindValue(barcode);
    q.exec();
    if (q.next()) {
        p.id = q.value("id").toInt();
        p.name = q.value("name").toString();
        p.barcode = q.value("barcode").toString();
        p.description = q.value("description").toString();
        p.categoryId = q.value("category_id").toInt();
        p.purchasePrice = q.value("purchase_price").toDouble();
        p.salePrice = q.value("sale_price").toDouble();
        p.quantity = q.value("quantity").toInt();
        p.minQuantity = q.value("min_quantity").toInt();
        p.unit = q.value("unit").toString();
        p.imagePath = q.value("image_path").toString();
        p.active = q.value("active").toBool();
    }
    return p;
}

Product Database::getProductById(int id)
{
    Product p;
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM products WHERE id=?");
    q.addBindValue(id);
    q.exec();
    if (q.next()) {
        p.id = q.value("id").toInt();
        p.name = q.value("name").toString();
        p.barcode = q.value("barcode").toString();
        p.description = q.value("description").toString();
        p.categoryId = q.value("category_id").toInt();
        p.purchasePrice = q.value("purchase_price").toDouble();
        p.salePrice = q.value("sale_price").toDouble();
        p.quantity = q.value("quantity").toInt();
        p.minQuantity = q.value("min_quantity").toInt();
        p.unit = q.value("unit").toString();
        p.imagePath = q.value("image_path").toString();
        p.active = q.value("active").toBool();
    }
    return p;
}

bool Database::addProduct(Product &prod)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO products (name, barcode, description, category_id, purchase_price, "
              "sale_price, quantity, min_quantity, unit, image_path) VALUES (?,?,?,?,?,?,?,?,?,?)");
    q.addBindValue(prod.name);
    q.addBindValue(prod.barcode);
    q.addBindValue(prod.description);
    q.addBindValue(prod.categoryId);
    q.addBindValue(prod.purchasePrice);
    q.addBindValue(prod.salePrice);
    q.addBindValue(prod.quantity);
    q.addBindValue(prod.minQuantity);
    q.addBindValue(prod.unit);
    q.addBindValue(prod.imagePath);
    if (q.exec()) {
        prod.id = q.lastInsertId().toInt();
        return true;
    }
    return false;
}

bool Database::updateProduct(const Product &prod)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE products SET name=?, barcode=?, description=?, category_id=?, purchase_price=?, "
              "sale_price=?, quantity=?, min_quantity=?, unit=?, image_path=?, updated_at=CURRENT_TIMESTAMP WHERE id=?");
    q.addBindValue(prod.name);
    q.addBindValue(prod.barcode);
    q.addBindValue(prod.description);
    q.addBindValue(prod.categoryId);
    q.addBindValue(prod.purchasePrice);
    q.addBindValue(prod.salePrice);
    q.addBindValue(prod.quantity);
    q.addBindValue(prod.minQuantity);
    q.addBindValue(prod.unit);
    q.addBindValue(prod.imagePath);
    q.addBindValue(prod.id);
    return q.exec();
}

bool Database::deleteProduct(int id)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE products SET active=0 WHERE id=?");
    q.addBindValue(id);
    return q.exec();
}

bool Database::updateProductQuantity(int id, int quantityChange)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE products SET quantity=quantity+?, updated_at=CURRENT_TIMESTAMP WHERE id=?");
    q.addBindValue(quantityChange);
    q.addBindValue(id);
    return q.exec();
}

QList<Product> Database::getLowStockProducts()
{
    QList<Product> list;
    QSqlQuery q(m_db);
    q.exec("SELECT * FROM products WHERE quantity <= min_quantity AND active=1 ORDER BY quantity");
    while (q.next()) {
        Product p;
        p.id = q.value("id").toInt();
        p.name = q.value("name").toString();
        p.barcode = q.value("barcode").toString();
        p.quantity = q.value("quantity").toInt();
        p.minQuantity = q.value("min_quantity").toInt();
        p.salePrice = q.value("sale_price").toDouble();
        list.append(p);
    }
    return list;
}

QList<Product> Database::searchProducts(const QString &keyword)
{
    QList<Product> list;
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM products WHERE (name LIKE ? OR barcode LIKE ?) AND active=1 ORDER BY name");
    QString pattern = "%" + keyword + "%";
    q.addBindValue(pattern);
    q.addBindValue(pattern);
    q.exec();
    while (q.next()) {
        Product p;
        p.id = q.value("id").toInt();
        p.name = q.value("name").toString();
        p.barcode = q.value("barcode").toString();
        p.categoryId = q.value("category_id").toInt();
        p.purchasePrice = q.value("purchase_price").toDouble();
        p.salePrice = q.value("sale_price").toDouble();
        p.quantity = q.value("quantity").toInt();
        p.unit = q.value("unit").toString();
        list.append(p);
    }
    return list;
}

// ========== Customers ==========

QList<Customer> Database::getAllCustomers()
{
    QList<Customer> list;
    QSqlQuery q(m_db);
    q.exec("SELECT * FROM customers WHERE active=1 ORDER BY name");
    while (q.next()) {
        Customer c;
        c.id = q.value("id").toInt();
        c.name = q.value("name").toString();
        c.phone = q.value("phone").toString();
        c.email = q.value("email").toString();
        c.address = q.value("address").toString();
        c.balance = q.value("balance").toDouble();
        c.totalPurchases = q.value("total_purchases").toDouble();
        c.notes = q.value("notes").toString();
        c.active = q.value("active").toBool();
        c.createdAt = q.value("created_at").toDateTime();
        list.append(c);
    }
    return list;
}

Customer Database::getCustomerById(int id)
{
    Customer c;
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM customers WHERE id=?");
    q.addBindValue(id);
    q.exec();
    if (q.next()) {
        c.id = q.value("id").toInt();
        c.name = q.value("name").toString();
        c.phone = q.value("phone").toString();
        c.email = q.value("email").toString();
        c.address = q.value("address").toString();
        c.balance = q.value("balance").toDouble();
        c.totalPurchases = q.value("total_purchases").toDouble();
        c.notes = q.value("notes").toString();
        c.active = q.value("active").toBool();
        c.createdAt = q.value("created_at").toDateTime();
    }
    return c;
}

bool Database::addCustomer(Customer &cust)
{
    QSqlQuery q(m_db);
    q.prepare("INSERT INTO customers (name, phone, email, address, notes) VALUES (?,?,?,?,?)");
    q.addBindValue(cust.name);
    q.addBindValue(cust.phone);
    q.addBindValue(cust.email);
    q.addBindValue(cust.address);
    q.addBindValue(cust.notes);
    if (q.exec()) {
        cust.id = q.lastInsertId().toInt();
        return true;
    }
    return false;
}

bool Database::updateCustomer(const Customer &cust)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE customers SET name=?, phone=?, email=?, address=?, balance=?, notes=? WHERE id=?");
    q.addBindValue(cust.name);
    q.addBindValue(cust.phone);
    q.addBindValue(cust.email);
    q.addBindValue(cust.address);
    q.addBindValue(cust.balance);
    q.addBindValue(cust.notes);
    q.addBindValue(cust.id);
    return q.exec();
}

bool Database::deleteCustomer(int id)
{
    QSqlQuery q(m_db);
    q.prepare("UPDATE customers SET active=0 WHERE id=?");
    q.addBindValue(id);
    return q.exec();
}

QList<Customer> Database::searchCustomers(const QString &keyword)
{
    QList<Customer> list;
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM customers WHERE (name LIKE ? OR phone LIKE ?) AND active=1 ORDER BY name");
    QString pattern = "%" + keyword + "%";
    q.addBindValue(pattern);
    q.addBindValue(pattern);
    q.exec();
    while (q.next()) {
        Customer c;
        c.id = q.value("id").toInt();
        c.name = q.value("name").toString();
        c.phone = q.value("phone").toString();
        c.email = q.value("email").toString();
        c.balance = q.value("balance").toDouble();
        c.totalPurchases = q.value("total_purchases").toDouble();
        list.append(c);
    }
    return list;
}

// ========== Sales ==========

bool Database::addSale(Sale &sale)
{
    m_db.transaction();

    QSqlQuery q(m_db);
    sale.invoiceNumber = generateInvoiceNumber();

    q.prepare("INSERT INTO sales (invoice_number, customer_id, customer_name, user_id, user_name, "
              "subtotal, discount, tax, total, paid, change_amount, payment_method, notes) "
              "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?)");
    q.addBindValue(sale.invoiceNumber);
    q.addBindValue(sale.customerId > 0 ? sale.customerId : QVariant());
    q.addBindValue(sale.customerName);
    q.addBindValue(sale.userId);
    q.addBindValue(sale.userName);
    q.addBindValue(sale.subtotal);
    q.addBindValue(sale.discount);
    q.addBindValue(sale.tax);
    q.addBindValue(sale.total);
    q.addBindValue(sale.paid);
    q.addBindValue(sale.change);
    q.addBindValue(sale.paymentMethod);
    q.addBindValue(sale.notes);

    if (!q.exec()) {
        m_db.rollback();
        return false;
    }

    sale.id = q.lastInsertId().toInt();

    for (int i = 0; i < sale.items.size(); ++i) {
        SaleItem &item = sale.items[i];
        item.saleId = sale.id;

        QSqlQuery qi(m_db);
        qi.prepare("INSERT INTO sale_items (sale_id, product_id, product_name, barcode, "
                   "quantity, unit_price, discount, total) VALUES (?,?,?,?,?,?,?,?)");
        qi.addBindValue(item.saleId);
        qi.addBindValue(item.productId);
        qi.addBindValue(item.productName);
        qi.addBindValue(item.barcode);
        qi.addBindValue(item.quantity);
        qi.addBindValue(item.unitPrice);
        qi.addBindValue(item.discount);
        qi.addBindValue(item.total);

        if (!qi.exec()) {
            m_db.rollback();
            return false;
        }

        updateProductQuantity(item.productId, -item.quantity);
    }

    if (sale.customerId > 0) {
        QSqlQuery qc(m_db);
        qc.prepare("UPDATE customers SET total_purchases=total_purchases+? WHERE id=?");
        qc.addBindValue(sale.total);
        qc.addBindValue(sale.customerId);
        qc.exec();
    }

    m_db.commit();
    return true;
}

QList<Sale> Database::getSalesByDateRange(const QDateTime &from, const QDateTime &to)
{
    QList<Sale> list;
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM sales WHERE created_at BETWEEN ? AND ? ORDER BY created_at DESC");
    q.addBindValue(from.toString("yyyy-MM-dd HH:mm:ss"));
    q.addBindValue(to.toString("yyyy-MM-dd HH:mm:ss"));
    q.exec();
    while (q.next()) {
        Sale s;
        s.id = q.value("id").toInt();
        s.invoiceNumber = q.value("invoice_number").toString();
        s.customerId = q.value("customer_id").toInt();
        s.customerName = q.value("customer_name").toString();
        s.userId = q.value("user_id").toInt();
        s.userName = q.value("user_name").toString();
        s.subtotal = q.value("subtotal").toDouble();
        s.discount = q.value("discount").toDouble();
        s.tax = q.value("tax").toDouble();
        s.total = q.value("total").toDouble();
        s.paid = q.value("paid").toDouble();
        s.change = q.value("change_amount").toDouble();
        s.paymentMethod = q.value("payment_method").toString();
        s.notes = q.value("notes").toString();
        s.createdAt = q.value("created_at").toDateTime();
        list.append(s);
    }
    return list;
}

QList<Sale> Database::getSalesByCustomer(int customerId)
{
    QList<Sale> list;
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM sales WHERE customer_id=? ORDER BY created_at DESC");
    q.addBindValue(customerId);
    q.exec();
    while (q.next()) {
        Sale s;
        s.id = q.value("id").toInt();
        s.invoiceNumber = q.value("invoice_number").toString();
        s.customerName = q.value("customer_name").toString();
        s.total = q.value("total").toDouble();
        s.paymentMethod = q.value("payment_method").toString();
        s.createdAt = q.value("created_at").toDateTime();
        list.append(s);
    }
    return list;
}

Sale Database::getSaleById(int id)
{
    Sale s;
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM sales WHERE id=?");
    q.addBindValue(id);
    q.exec();
    if (q.next()) {
        s.id = q.value("id").toInt();
        s.invoiceNumber = q.value("invoice_number").toString();
        s.customerId = q.value("customer_id").toInt();
        s.customerName = q.value("customer_name").toString();
        s.userId = q.value("user_id").toInt();
        s.userName = q.value("user_name").toString();
        s.subtotal = q.value("subtotal").toDouble();
        s.discount = q.value("discount").toDouble();
        s.tax = q.value("tax").toDouble();
        s.total = q.value("total").toDouble();
        s.paid = q.value("paid").toDouble();
        s.change = q.value("change_amount").toDouble();
        s.paymentMethod = q.value("payment_method").toString();
        s.notes = q.value("notes").toString();
        s.createdAt = q.value("created_at").toDateTime();
        s.items = getSaleItems(s.id);
    }
    return s;
}

QList<SaleItem> Database::getSaleItems(int saleId)
{
    QList<SaleItem> list;
    QSqlQuery q(m_db);
    q.prepare("SELECT * FROM sale_items WHERE sale_id=?");
    q.addBindValue(saleId);
    q.exec();
    while (q.next()) {
        SaleItem item;
        item.id = q.value("id").toInt();
        item.saleId = q.value("sale_id").toInt();
        item.productId = q.value("product_id").toInt();
        item.productName = q.value("product_name").toString();
        item.barcode = q.value("barcode").toString();
        item.quantity = q.value("quantity").toInt();
        item.unitPrice = q.value("unit_price").toDouble();
        item.discount = q.value("discount").toDouble();
        item.total = q.value("total").toDouble();
        list.append(item);
    }
    return list;
}

QString Database::generateInvoiceNumber()
{
    QString prefix = QDateTime::currentDateTime().toString("yyyyMMdd");
    QSqlQuery q(m_db);
    q.prepare("SELECT COUNT(*) FROM sales WHERE invoice_number LIKE ?");
    q.addBindValue(prefix + "%");
    q.exec();
    q.next();
    int count = q.value(0).toInt() + 1;
    return prefix + QString("-%1").arg(count, 4, 10, QChar('0'));
}

// ========== Reports ==========

double Database::getTotalSales(const QDateTime &from, const QDateTime &to)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT COALESCE(SUM(total),0) FROM sales WHERE created_at BETWEEN ? AND ?");
    q.addBindValue(from.toString("yyyy-MM-dd HH:mm:ss"));
    q.addBindValue(to.toString("yyyy-MM-dd HH:mm:ss"));
    q.exec();
    q.next();
    return q.value(0).toDouble();
}

double Database::getTotalProfit(const QDateTime &from, const QDateTime &to)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT COALESCE(SUM(si.total - (p.purchase_price * si.quantity)),0) "
              "FROM sale_items si "
              "JOIN sales s ON si.sale_id = s.id "
              "LEFT JOIN products p ON si.product_id = p.id "
              "WHERE s.created_at BETWEEN ? AND ?");
    q.addBindValue(from.toString("yyyy-MM-dd HH:mm:ss"));
    q.addBindValue(to.toString("yyyy-MM-dd HH:mm:ss"));
    q.exec();
    q.next();
    return q.value(0).toDouble();
}

double Database::getTotalExpenses(const QDateTime &from, const QDateTime &to)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT COALESCE(SUM(p.purchase_price * si.quantity),0) "
              "FROM sale_items si "
              "JOIN sales s ON si.sale_id = s.id "
              "LEFT JOIN products p ON si.product_id = p.id "
              "WHERE s.created_at BETWEEN ? AND ?");
    q.addBindValue(from.toString("yyyy-MM-dd HH:mm:ss"));
    q.addBindValue(to.toString("yyyy-MM-dd HH:mm:ss"));
    q.exec();
    q.next();
    return q.value(0).toDouble();
}

int Database::getTotalTransactions(const QDateTime &from, const QDateTime &to)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT COUNT(*) FROM sales WHERE created_at BETWEEN ? AND ?");
    q.addBindValue(from.toString("yyyy-MM-dd HH:mm:ss"));
    q.addBindValue(to.toString("yyyy-MM-dd HH:mm:ss"));
    q.exec();
    q.next();
    return q.value(0).toInt();
}

QList<QPair<QString, double>> Database::getSalesByCategory(const QDateTime &from, const QDateTime &to)
{
    QList<QPair<QString, double>> list;
    QSqlQuery q(m_db);
    q.prepare("SELECT c.name, COALESCE(SUM(si.total),0) as total "
              "FROM sale_items si "
              "JOIN sales s ON si.sale_id = s.id "
              "LEFT JOIN products p ON si.product_id = p.id "
              "LEFT JOIN categories c ON p.category_id = c.id "
              "WHERE s.created_at BETWEEN ? AND ? "
              "GROUP BY c.name ORDER BY total DESC");
    q.addBindValue(from.toString("yyyy-MM-dd HH:mm:ss"));
    q.addBindValue(to.toString("yyyy-MM-dd HH:mm:ss"));
    q.exec();
    while (q.next()) {
        list.append(qMakePair(q.value(0).toString(), q.value(1).toDouble()));
    }
    return list;
}

QList<QPair<QString, double>> Database::getTopProducts(const QDateTime &from, const QDateTime &to, int limit)
{
    QList<QPair<QString, double>> list;
    QSqlQuery q(m_db);
    q.prepare("SELECT product_name, COALESCE(SUM(total),0) as total "
              "FROM sale_items si JOIN sales s ON si.sale_id = s.id "
              "WHERE s.created_at BETWEEN ? AND ? "
              "GROUP BY product_name ORDER BY total DESC LIMIT ?");
    q.addBindValue(from.toString("yyyy-MM-dd HH:mm:ss"));
    q.addBindValue(to.toString("yyyy-MM-dd HH:mm:ss"));
    q.addBindValue(limit);
    q.exec();
    while (q.next()) {
        list.append(qMakePair(q.value(0).toString(), q.value(1).toDouble()));
    }
    return list;
}

QList<QPair<QString, double>> Database::getDailySales(const QDateTime &from, const QDateTime &to)
{
    QList<QPair<QString, double>> list;
    QSqlQuery q(m_db);
    q.prepare("SELECT DATE(created_at) as day, COALESCE(SUM(total),0) "
              "FROM sales WHERE created_at BETWEEN ? AND ? "
              "GROUP BY day ORDER BY day");
    q.addBindValue(from.toString("yyyy-MM-dd HH:mm:ss"));
    q.addBindValue(to.toString("yyyy-MM-dd HH:mm:ss"));
    q.exec();
    while (q.next()) {
        list.append(qMakePair(q.value(0).toString(), q.value(1).toDouble()));
    }
    return list;
}

QList<QPair<QString, double>> Database::getMonthlySales(int year)
{
    QList<QPair<QString, double>> list;
    QSqlQuery q(m_db);
    q.prepare("SELECT strftime('%m', created_at) as month, COALESCE(SUM(total),0) "
              "FROM sales WHERE strftime('%Y', created_at)=? "
              "GROUP BY month ORDER BY month");
    q.addBindValue(QString::number(year));
    q.exec();
    while (q.next()) {
        list.append(qMakePair(q.value(0).toString(), q.value(1).toDouble()));
    }
    return list;
}
