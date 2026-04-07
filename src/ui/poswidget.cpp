#include "poswidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QSpinBox>
#include <QMessageBox>
#include <QDateTime>
#include <QFont>
#include <QFrame>
#include <QSettings>
#include "../core/database.h"
#include "../core/thermalprinter.h"
#include "../core/gdiprinter.h"

PosWidget::PosWidget(QWidget *parent) : QWidget(parent),
    m_subtotal(0), m_discount(0), m_tax(0), m_total(0)
{
    setupUI();
    loadCategories();
    loadProducts();
}

void PosWidget::setCurrentUser(const User &user)
{
    m_currentUser = user;
}

void PosWidget::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // ===== LEFT PANEL - Products =====
    QWidget *leftPanel = new QWidget(this);
    leftPanel->setObjectName("posLeftPanel");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(12, 12, 12, 12);
    leftLayout->setSpacing(8);

    // Search bar
    QHBoxLayout *searchLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit(leftPanel);
    m_searchEdit->setObjectName("searchInput");
    m_searchEdit->setPlaceholderText(
        QString::fromUtf8("\xd8\xa8\xd8\xad\xd8\xab \xd8\xb9\xd9\x86 \xd9\x85\xd9\x86\xd8\xaa\xd8\xac... (\xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa3\xd9\x88 \xd8\xa8\xd8\xa7\xd8\xb1\xd9\x83\xd9\x88\xd8\xaf)"));
    m_searchEdit->setFixedHeight(40);
    m_searchEdit->setAlignment(Qt::AlignRight);
    searchLayout->addWidget(m_searchEdit);

    m_categoryCombo = new QComboBox(leftPanel);
    m_categoryCombo->setObjectName("categoryCombo");
    m_categoryCombo->setFixedHeight(40);
    m_categoryCombo->setMinimumWidth(150);
    searchLayout->addWidget(m_categoryCombo);
    leftLayout->addLayout(searchLayout);

    // Products grid in scroll area
    m_scrollArea = new QScrollArea(leftPanel);
    m_scrollArea->setObjectName("productsScroll");
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_productsGrid = new QWidget(m_scrollArea);
    m_productsGrid->setObjectName("productsGrid");
    m_gridLayout = new QGridLayout(m_productsGrid);
    m_gridLayout->setSpacing(8);
    m_gridLayout->setContentsMargins(4, 4, 4, 4);

    m_scrollArea->setWidget(m_productsGrid);
    leftLayout->addWidget(m_scrollArea, 1);

    mainLayout->addWidget(leftPanel, 3);

    // ===== RIGHT PANEL - Cart =====
    QWidget *rightPanel = new QWidget(this);
    rightPanel->setObjectName("posRightPanel");
    rightPanel->setFixedWidth(420);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(12, 12, 12, 12);
    rightLayout->setSpacing(8);

    // Cart title
    QLabel *cartTitle = new QLabel(
        QString::fromUtf8("\xf0\x9f\x9b\x92 \xd8\xb3\xd9\x84\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd8\xb4\xd8\xaa\xd8\xb1\xd9\x8a\xd8\xa7\xd8\xaa"),
        rightPanel);
    cartTitle->setObjectName("cartTitle");
    QFont cartFont("Arial", 16, QFont::Bold);
    cartTitle->setFont(cartFont);
    cartTitle->setAlignment(Qt::AlignRight);
    rightLayout->addWidget(cartTitle);

    // Customer selection
    m_customerCombo = new QComboBox(rightPanel);
    m_customerCombo->setObjectName("customerCombo");
    m_customerCombo->setFixedHeight(36);
    m_customerCombo->addItem(
        QString::fromUtf8("\xd8\xb9\xd9\x85\xd9\x8a\xd9\x84 \xd9\x86\xd9\x82\xd8\xaf\xd9\x8a"), 0);
    QList<Customer> customers = Database::instance()->getAllCustomers();
    for (int i = 0; i < customers.size(); ++i) {
        m_customerCombo->addItem(customers[i].name, customers[i].id);
    }
    rightLayout->addWidget(m_customerCombo);

    // Cart table
    m_cartTable = new QTableWidget(0, 5, rightPanel);
    m_cartTable->setObjectName("cartTable");
    QStringList headers;
    headers << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb5\xd9\x86\xd9\x81")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb3\xd8\xb9\xd8\xb1")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x83\xd9\x85\xd9\x8a\xd8\xa9")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a")
            << "";
    m_cartTable->setHorizontalHeaderLabels(headers);
    m_cartTable->horizontalHeader()->setStretchLastSection(false);
    m_cartTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_cartTable->setColumnWidth(1, 70);
    m_cartTable->setColumnWidth(2, 60);
    m_cartTable->setColumnWidth(3, 80);
    m_cartTable->setColumnWidth(4, 40);
    m_cartTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_cartTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_cartTable->verticalHeader()->hide();
    m_cartTable->setLayoutDirection(Qt::RightToLeft);
    rightLayout->addWidget(m_cartTable, 1);

    // Totals area
    QWidget *totalsWidget = new QWidget(rightPanel);
    totalsWidget->setObjectName("totalsWidget");
    QVBoxLayout *totalsLayout = new QVBoxLayout(totalsWidget);
    totalsLayout->setContentsMargins(8, 8, 8, 8);
    totalsLayout->setSpacing(6);

    // Subtotal
    QHBoxLayout *subLayout = new QHBoxLayout();
    m_subtotalLabel = new QLabel("0.00", totalsWidget);
    m_subtotalLabel->setObjectName("subtotalValue");
    QFont valFont("Arial", 12);
    m_subtotalLabel->setFont(valFont);
    subLayout->addWidget(m_subtotalLabel);
    subLayout->addStretch();
    QLabel *subLabel = new QLabel(
        QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9:"), totalsWidget);
    subLabel->setFont(valFont);
    subLayout->addWidget(subLabel);
    totalsLayout->addLayout(subLayout);

    // Discount
    QHBoxLayout *discLayout = new QHBoxLayout();
    m_discountEdit = new QLineEdit("0", totalsWidget);
    m_discountEdit->setObjectName("discountInput");
    m_discountEdit->setFixedWidth(80);
    m_discountEdit->setFixedHeight(32);
    m_discountEdit->setAlignment(Qt::AlignCenter);
    discLayout->addWidget(m_discountEdit);
    discLayout->addStretch();
    QLabel *discLabel = new QLabel(
        QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xae\xd8\xb5\xd9\x85:"), totalsWidget);
    discLabel->setFont(valFont);
    discLayout->addWidget(discLabel);
    totalsLayout->addLayout(discLayout);

    // Total
    QHBoxLayout *totalLayout = new QHBoxLayout();
    m_totalLabel = new QLabel("0.00", totalsWidget);
    m_totalLabel->setObjectName("totalValue");
    QFont totalFont("Arial", 20, QFont::Bold);
    m_totalLabel->setFont(totalFont);
    totalLayout->addWidget(m_totalLabel);
    totalLayout->addStretch();
    QLabel *totLabel = new QLabel(
        QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a:"), totalsWidget);
    QFont totFont("Arial", 16, QFont::Bold);
    totLabel->setFont(totFont);
    totalLayout->addWidget(totLabel);
    totalsLayout->addLayout(totalLayout);

    // Payment method
    QHBoxLayout *payLayout = new QHBoxLayout();
    m_paymentCombo = new QComboBox(totalsWidget);
    m_paymentCombo->addItem(QString::fromUtf8("\xd9\x86\xd9\x82\xd8\xaf\xd9\x8a"), "cash");
    m_paymentCombo->addItem(QString::fromUtf8("\xd8\xa8\xd8\xb7\xd8\xa7\xd9\x82\xd8\xa9"), "card");
    m_paymentCombo->addItem(QString::fromUtf8("\xd8\xaa\xd8\xad\xd9\x88\xd9\x8a\xd9\x84"), "transfer");
    m_paymentCombo->setFixedHeight(32);
    payLayout->addWidget(m_paymentCombo);
    payLayout->addStretch();
    QLabel *payLabel = new QLabel(
        QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xaf\xd9\x81\xd8\xb9:"), totalsWidget);
    payLabel->setFont(valFont);
    payLayout->addWidget(payLabel);
    totalsLayout->addLayout(payLayout);

    // Paid amount
    QHBoxLayout *paidLayout = new QHBoxLayout();
    m_paidEdit = new QLineEdit("0", totalsWidget);
    m_paidEdit->setObjectName("paidInput");
    m_paidEdit->setFixedWidth(100);
    m_paidEdit->setFixedHeight(32);
    m_paidEdit->setAlignment(Qt::AlignCenter);
    paidLayout->addWidget(m_paidEdit);
    paidLayout->addStretch();
    QLabel *paidLabel = new QLabel(
        QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd8\xaf\xd9\x81\xd9\x88\xd8\xb9:"), totalsWidget);
    paidLabel->setFont(valFont);
    paidLayout->addWidget(paidLabel);
    totalsLayout->addLayout(paidLayout);

    // Change
    QHBoxLayout *changeLayout = new QHBoxLayout();
    m_changeLabel = new QLabel("0.00", totalsWidget);
    m_changeLabel->setObjectName("changeValue");
    QFont changeFont("Arial", 14, QFont::Bold);
    m_changeLabel->setFont(changeFont);
    changeLayout->addWidget(m_changeLabel);
    changeLayout->addStretch();
    QLabel *chLabel = new QLabel(
        QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa8\xd8\xa7\xd9\x82\xd9\x8a:"), totalsWidget);
    chLabel->setFont(valFont);
    changeLayout->addWidget(chLabel);
    totalsLayout->addLayout(changeLayout);

    rightLayout->addWidget(totalsWidget);

    // Action buttons
    QHBoxLayout *actionLayout = new QHBoxLayout();

    m_clearBtn = new QPushButton(
        QString::fromUtf8("\xd8\xa5\xd9\x84\xd8\xba\xd8\xa7\xd8\xa1"),
        rightPanel);
    m_clearBtn->setObjectName("dangerButton");
    m_clearBtn->setFixedHeight(48);
    m_clearBtn->setCursor(Qt::PointingHandCursor);
    actionLayout->addWidget(m_clearBtn);

    m_printBtn = new QPushButton(
        QString::fromUtf8("\xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9"),
        rightPanel);
    m_printBtn->setObjectName("secondaryButton");
    m_printBtn->setFixedHeight(48);
    m_printBtn->setCursor(Qt::PointingHandCursor);
    m_printBtn->setEnabled(false);
    actionLayout->addWidget(m_printBtn);

    m_checkoutBtn = new QPushButton(
        QString::fromUtf8("\xd8\xaf\xd9\x81\xd8\xb9"),
        rightPanel);
    m_checkoutBtn->setObjectName("successButton");
    m_checkoutBtn->setFixedHeight(48);
    m_checkoutBtn->setCursor(Qt::PointingHandCursor);
    QFont checkFont("Arial", 16, QFont::Bold);
    m_checkoutBtn->setFont(checkFont);
    actionLayout->addWidget(m_checkoutBtn, 2);

    rightLayout->addLayout(actionLayout);

    mainLayout->addWidget(rightPanel);

    // Connections
    connect(m_searchEdit, &QLineEdit::textChanged, this, &PosWidget::onSearchChanged);
    connect(m_categoryCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &PosWidget::onCategorySelected);
    connect(m_checkoutBtn, &QPushButton::clicked, this, &PosWidget::onCheckout);
    connect(m_clearBtn, &QPushButton::clicked, this, &PosWidget::onClearCart);
    connect(m_discountEdit, &QLineEdit::textChanged, this, &PosWidget::onDiscountChanged);
    connect(m_paidEdit, &QLineEdit::textChanged, this, &PosWidget::onPaidChanged);
    connect(m_printBtn, &QPushButton::clicked, this, &PosWidget::onPrintReceipt);
}

void PosWidget::loadCategories()
{
    m_categoryCombo->clear();
    m_categoryCombo->addItem(
        QString::fromUtf8("\xd8\xac\xd9\x85\xd9\x8a\xd8\xb9 \xd8\xa7\xd9\x84\xd8\xa3\xd9\x82\xd8\xb3\xd8\xa7\xd9\x85"), -1);
    QList<Category> categories = Database::instance()->getAllCategories();
    for (int i = 0; i < categories.size(); ++i) {
        m_categoryCombo->addItem(categories[i].name, categories[i].id);
    }
}

void PosWidget::loadProducts(int categoryId, const QString &search)
{
    // Clear existing grid
    QLayoutItem *child;
    while ((child = m_gridLayout->takeAt(0)) != 0) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    QList<Product> products;
    if (!search.isEmpty()) {
        products = Database::instance()->searchProducts(search);
    } else if (categoryId > 0) {
        products = Database::instance()->getProductsByCategory(categoryId);
    } else {
        products = Database::instance()->getAllProducts();
    }

    int col = 0, row = 0;
    int maxCols = 4;
    for (int i = 0; i < products.size(); ++i) {
        QWidget *card = createProductCard(products[i]);
        m_gridLayout->addWidget(card, row, col);
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }

    // Add spacer
    m_gridLayout->setRowStretch(row + 1, 1);
}

QWidget* PosWidget::createProductCard(const Product &product)
{
    QWidget *card = new QWidget();
    card->setObjectName("productCard");
    card->setFixedSize(150, 130);
    card->setCursor(Qt::PointingHandCursor);
    card->setProperty("productId", product.id);

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(4);

    QLabel *nameLabel = new QLabel(product.name, card);
    nameLabel->setObjectName("productName");
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setWordWrap(true);
    QFont nameFont("Arial", 11, QFont::Bold);
    nameLabel->setFont(nameFont);
    layout->addWidget(nameLabel);

    QLabel *priceLabel = new QLabel(QString::number(product.salePrice, 'f', 2), card);
    priceLabel->setObjectName("productPrice");
    priceLabel->setAlignment(Qt::AlignCenter);
    QFont priceFont("Arial", 14, QFont::Bold);
    priceLabel->setFont(priceFont);
    layout->addWidget(priceLabel);

    QLabel *qtyLabel = new QLabel(
        QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x83\xd9\x85\xd9\x8a\xd8\xa9: ") +
        QString::number(product.quantity), card);
    qtyLabel->setObjectName("productQty");
    qtyLabel->setAlignment(Qt::AlignCenter);
    QFont qtyFont("Arial", 9);
    qtyLabel->setFont(qtyFont);
    if (product.quantity <= product.minQuantity) {
        qtyLabel->setStyleSheet("color: #e74c3c;");
    }
    layout->addWidget(qtyLabel);

    // Click handler using event filter approach
    card->installEventFilter(this);

    // Use mouse press via property
    int pid = product.id;
    card->setProperty("productId", pid);

    // We use a transparent button overlay
    QPushButton *overlay = new QPushButton(card);
    overlay->setObjectName("cardOverlay");
    overlay->setStyleSheet("background: transparent; border: none;");
    overlay->setGeometry(0, 0, 150, 130);
    overlay->setCursor(Qt::PointingHandCursor);
    connect(overlay, &QPushButton::clicked, this, [this, pid]() {
        onProductClicked(pid);
    });

    return card;
}

void PosWidget::addProductByBarcode(const QString &barcode)
{
    Product product = Database::instance()->getProductByBarcode(barcode);
    if (product.id > 0) {
        addProductToCart(product);
    } else {
        QMessageBox::warning(this,
            QString::fromUtf8("\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd9\x88\xd8\xac\xd9\x88\xd8\xaf"),
            QString::fromUtf8("\xd9\x84\xd9\x85 \xd9\x8a\xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xb9\xd8\xab\xd9\x88\xd8\xb1 \xd8\xb9\xd9\x84\xd9\x89 \xd9\x85\xd9\x86\xd8\xaa\xd8\xac \xd8\xa8\xd9\x87\xd8\xb0\xd8\xa7 \xd8\xa7\xd9\x84\xd8\xa8\xd8\xa7\xd8\xb1\xd9\x83\xd9\x88\xd8\xaf: ") + barcode);
    }
}

void PosWidget::onSearchChanged(const QString &text)
{
    int catId = m_categoryCombo->currentData().toInt();
    loadProducts(catId, text);
}

void PosWidget::onCategorySelected(int index)
{
    Q_UNUSED(index)
    int catId = m_categoryCombo->currentData().toInt();
    loadProducts(catId, m_searchEdit->text());
}

void PosWidget::onProductClicked(int productId)
{
    Product product = Database::instance()->getProductById(productId);
    if (product.id > 0) {
        if (product.quantity <= 0) {
            QMessageBox::warning(this,
                QString::fromUtf8("\xd9\x86\xd9\x81\xd8\xb0 \xd8\xa7\xd9\x84\xd9\x85\xd8\xae\xd8\xb2\xd9\x88\xd9\x86"),
                QString::fromUtf8("\xd9\x87\xd8\xb0\xd8\xa7 \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xaa\xd8\xac \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd8\xaa\xd9\x88\xd9\x81\xd8\xb1 \xd8\xad\xd8\xa7\xd9\x84\xd9\x8a\xd8\xa7\xd9\x8b"));
            return;
        }
        addProductToCart(product);
    }
}

void PosWidget::addProductToCart(const Product &product, int quantity)
{
    // Check if already in cart
    for (int i = 0; i < m_cartItems.size(); ++i) {
        if (m_cartItems[i].productId == product.id) {
            m_cartItems[i].quantity += quantity;
            m_cartItems[i].total = m_cartItems[i].quantity * m_cartItems[i].unitPrice;

            // Update table
            QSpinBox *spin = qobject_cast<QSpinBox*>(m_cartTable->cellWidget(i, 2));
            if (spin) spin->setValue(m_cartItems[i].quantity);
            m_cartTable->item(i, 3)->setText(QString::number(m_cartItems[i].total, 'f', 2));
            updateCartTotals();
            return;
        }
    }

    // Add new item
    SaleItem item;
    item.productId = product.id;
    item.productName = product.name;
    item.barcode = product.barcode;
    item.quantity = quantity;
    item.unitPrice = product.salePrice;
    item.total = quantity * product.salePrice;
    m_cartItems.append(item);

    int row = m_cartTable->rowCount();
    m_cartTable->insertRow(row);

    m_cartTable->setItem(row, 0, new QTableWidgetItem(product.name));
    m_cartTable->setItem(row, 1, new QTableWidgetItem(QString::number(product.salePrice, 'f', 2)));

    QSpinBox *qtySpin = new QSpinBox();
    qtySpin->setMinimum(1);
    qtySpin->setMaximum(product.quantity);
    qtySpin->setValue(quantity);
    qtySpin->setAlignment(Qt::AlignCenter);
    connect(qtySpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [this, row](int) { onQuantityChanged(row); });
    m_cartTable->setCellWidget(row, 2, qtySpin);

    m_cartTable->setItem(row, 3, new QTableWidgetItem(QString::number(item.total, 'f', 2)));

    QPushButton *removeBtn = new QPushButton(QString::fromUtf8("\xc3\x97"));
    removeBtn->setObjectName("removeBtn");
    removeBtn->setFixedSize(30, 30);
    removeBtn->setCursor(Qt::PointingHandCursor);
    connect(removeBtn, &QPushButton::clicked, this, [this, row]() { onRemoveItem(row); });
    m_cartTable->setCellWidget(row, 4, removeBtn);

    updateCartTotals();
}

void PosWidget::onRemoveItem(int row)
{
    if (row >= 0 && row < m_cartItems.size()) {
        m_cartItems.removeAt(row);
        m_cartTable->removeRow(row);
        updateCartTotals();
    }
}

void PosWidget::onQuantityChanged(int row)
{
    if (row >= 0 && row < m_cartItems.size()) {
        QSpinBox *spin = qobject_cast<QSpinBox*>(m_cartTable->cellWidget(row, 2));
        if (spin) {
            m_cartItems[row].quantity = spin->value();
            m_cartItems[row].total = m_cartItems[row].quantity * m_cartItems[row].unitPrice;
            m_cartTable->item(row, 3)->setText(QString::number(m_cartItems[row].total, 'f', 2));
            updateCartTotals();
        }
    }
}

void PosWidget::updateCartTotals()
{
    m_subtotal = 0;
    for (int i = 0; i < m_cartItems.size(); ++i) {
        m_subtotal += m_cartItems[i].total;
    }
    m_subtotalLabel->setText(QString::number(m_subtotal, 'f', 2));

    m_discount = m_discountEdit->text().toDouble();
    m_total = m_subtotal - m_discount;
    if (m_total < 0) m_total = 0;
    m_totalLabel->setText(QString::number(m_total, 'f', 2));

    double paid = m_paidEdit->text().toDouble();
    double change = paid - m_total;
    if (change < 0) change = 0;
    m_changeLabel->setText(QString::number(change, 'f', 2));
}

void PosWidget::onDiscountChanged()
{
    updateCartTotals();
}

void PosWidget::onPaidChanged()
{
    updateCartTotals();
}

void PosWidget::onCheckout()
{
    if (m_cartItems.isEmpty()) {
        QMessageBox::warning(this,
            QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb3\xd9\x84\xd8\xa9 \xd9\x81\xd8\xa7\xd8\xb1\xd8\xba\xd8\xa9"),
            QString::fromUtf8("\xd9\x8a\xd8\xb1\xd8\xac\xd9\x89 \xd8\xa5\xd8\xb6\xd8\xa7\xd9\x81\xd8\xa9 \xd9\x85\xd9\x86\xd8\xaa\xd8\xac\xd8\xa7\xd8\xaa \xd8\xa5\xd9\x84\xd9\x89 \xd8\xa7\xd9\x84\xd8\xb3\xd9\x84\xd8\xa9"));
        return;
    }

    double paid = m_paidEdit->text().toDouble();
    if (paid < m_total && m_paymentCombo->currentData().toString() == "cash") {
        QMessageBox::warning(this,
            QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd8\xa8\xd9\x84\xd8\xba \xd8\xba\xd9\x8a\xd8\xb1 \xd9\x83\xd8\xa7\xd9\x81\xd9\x8a"),
            QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd8\xa8\xd9\x84\xd8\xba \xd8\xa7\xd9\x84\xd9\x85\xd8\xaf\xd9\x81\xd9\x88\xd8\xb9 \xd8\xa3\xd9\x82\xd9\x84 \xd9\x85\xd9\x86 \xd8\xa7\xd9\x84\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a"));
        return;
    }

    Sale sale;
    sale.items = m_cartItems;
    sale.subtotal = m_subtotal;
    sale.discount = m_discount;
    sale.tax = m_tax;
    sale.total = m_total;
    sale.paid = paid;
    sale.change = paid - m_total;
    sale.paymentMethod = m_paymentCombo->currentData().toString();
    sale.userId = m_currentUser.id;
    sale.userName = m_currentUser.displayName;
    sale.customerId = m_customerCombo->currentData().toInt();
    sale.customerName = m_customerCombo->currentText();
    sale.createdAt = QDateTime::currentDateTime();

    if (Database::instance()->addSale(sale)) {
        m_lastSale = sale;
        m_printBtn->setEnabled(true);

        QMessageBox::information(this,
            QString::fromUtf8("\xd8\xaa\xd9\x85\xd8\xaa \xd8\xa7\xd9\x84\xd8\xb9\xd9\x85\xd9\x84\xd9\x8a\xd8\xa9"),
            QString::fromUtf8("\xd8\xaa\xd9\x85 \xd8\xad\xd9\x81\xd8\xb8 \xd8\xa7\xd9\x84\xd9\x81\xd8\xa7\xd8\xaa\xd9\x88\xd8\xb1\xd8\xa9 \xd8\xa8\xd9\x86\xd8\xac\xd8\xa7\xd8\xad\n\xd8\xb1\xd9\x82\xd9\x85 \xd8\xa7\xd9\x84\xd9\x81\xd8\xa7\xd8\xaa\xd9\x88\xd8\xb1\xd8\xa9: ") + sale.invoiceNumber);

        // Auto print if enabled
        QSettings settings("SmartPOS", "SmartPOS");
        if (settings.value("printer/autoPrint", false).toBool()) {
            onPrintReceipt();
        }

        clearCart();
        loadProducts();
    } else {
        QMessageBox::critical(this,
            QString::fromUtf8("\xd8\xae\xd8\xb7\xd8\xa3"),
            QString::fromUtf8("\xd9\x81\xd8\xb4\xd9\x84 \xd9\x81\xd9\x8a \xd8\xad\xd9\x81\xd8\xb8 \xd8\xa7\xd9\x84\xd9\x81\xd8\xa7\xd8\xaa\xd9\x88\xd8\xb1\xd8\xa9"));
    }
}

void PosWidget::onClearCart()
{
    if (m_cartItems.isEmpty()) return;

    QMessageBox::StandardButton reply = QMessageBox::question(this,
        QString::fromUtf8("\xd8\xaa\xd8\xa3\xd9\x83\xd9\x8a\xd8\xaf"),
        QString::fromUtf8("\xd9\x87\xd9\x84 \xd8\xaa\xd8\xb1\xd9\x8a\xd8\xaf \xd8\xa5\xd9\x84\xd8\xba\xd8\xa7\xd8\xa1 \xd8\xa7\xd9\x84\xd9\x81\xd8\xa7\xd8\xaa\xd9\x88\xd8\xb1\xd8\xa9\xd8\x9f"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        clearCart();
    }
}

void PosWidget::clearCart()
{
    m_cartItems.clear();
    m_cartTable->setRowCount(0);
    m_discountEdit->setText("0");
    m_paidEdit->setText("0");
    updateCartTotals();
}

void PosWidget::onPrintReceipt()
{
    if (m_lastSale.id == 0) return;

    QSettings settings("SmartPOS", "SmartPOS");
    QString printerType = settings.value("printer/type", "thermal").toString();
    QString storeName = settings.value("store/name", "Smart POS").toString();
    QString storePhone = settings.value("store/phone").toString();
    QString storeAddress = settings.value("store/address").toString();
    QString taxNumber = settings.value("store/taxNumber").toString();

    if (printerType == "thermal") {
        ThermalPrinter *tp = new ThermalPrinter(this);
        QString port = settings.value("printer/thermalPort").toString();
        int baud = settings.value("printer/thermalBaud", 9600).toInt();
        QString sizeStr = settings.value("printer/thermalSize", "80").toString();
        tp->setPaperSize(sizeStr == "58" ? ThermalPrinter::Paper58mm : ThermalPrinter::Paper80mm);

        if (tp->openPort(port, baud)) {
            tp->printReceipt(m_lastSale, storeName, storePhone, storeAddress, taxNumber);
            tp->closePort();
        } else {
            QMessageBox::warning(this,
                QString::fromUtf8("\xd8\xae\xd8\xb7\xd8\xa3 \xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9"),
                QString::fromUtf8("\xd9\x84\xd8\xa7 \xd9\x8a\xd9\x85\xd9\x83\xd9\x86 \xd8\xa7\xd9\x84\xd8\xa7\xd8\xaa\xd8\xb5\xd8\xa7\xd9\x84 \xd8\xa8\xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xad\xd8\xb1\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xa9"));
        }
        tp->deleteLater();
    } else {
        GdiPrinter *gp = new GdiPrinter(this);
        QString gdiName = settings.value("printer/gdiName").toString();
        if (!gdiName.isEmpty()) gp->setPrinterName(gdiName);
        gp->printReceipt(m_lastSale, storeName, storePhone, storeAddress, taxNumber);
        gp->deleteLater();
    }
}
