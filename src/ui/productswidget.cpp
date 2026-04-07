#include "productswidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QLabel>
#include <QFont>
#include "../core/database.h"

ProductsWidget::ProductsWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadProducts();
}

void ProductsWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // Title
    QLabel *title = new QLabel(
        QString::fromUtf8("\xf0\x9f\x93\xa6 \xd8\xa5\xd8\xaf\xd8\xa7\xd8\xb1\xd8\xa9 \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xaa\xd8\xac\xd8\xa7\xd8\xaa"), this);
    title->setObjectName("pageTitle");
    QFont titleFont("Arial", 20, QFont::Bold);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignRight);
    mainLayout->addWidget(title);

    // Toolbar
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
        QString::fromUtf8("+ \xd8\xa5\xd8\xb6\xd8\xa7\xd9\x81\xd8\xa9 \xd9\x85\xd9\x86\xd8\xaa\xd8\xac"), this);
    m_addBtn->setObjectName("primaryButton");
    m_addBtn->setFixedHeight(38);
    m_addBtn->setCursor(Qt::PointingHandCursor);
    toolbar->addWidget(m_addBtn);

    toolbar->addStretch();

    m_categoryFilter = new QComboBox(this);
    m_categoryFilter->setFixedHeight(38);
    m_categoryFilter->setMinimumWidth(150);
    m_categoryFilter->addItem(
        QString::fromUtf8("\xd8\xac\xd9\x85\xd9\x8a\xd8\xb9 \xd8\xa7\xd9\x84\xd8\xa3\xd9\x82\xd8\xb3\xd8\xa7\xd9\x85"), -1);
    QList<Category> cats = Database::instance()->getAllCategories();
    for (int i = 0; i < cats.size(); ++i) {
        m_categoryFilter->addItem(cats[i].name, cats[i].id);
    }
    toolbar->addWidget(m_categoryFilter);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setObjectName("searchInput");
    m_searchEdit->setPlaceholderText(
        QString::fromUtf8("\xd8\xa8\xd8\xad\xd8\xab..."));
    m_searchEdit->setFixedHeight(38);
    m_searchEdit->setFixedWidth(250);
    m_searchEdit->setAlignment(Qt::AlignRight);
    toolbar->addWidget(m_searchEdit);

    mainLayout->addLayout(toolbar);

    // Table
    m_table = new QTableWidget(0, 8, this);
    m_table->setObjectName("dataTable");
    QStringList headers;
    headers << "#"
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa8\xd8\xa7\xd8\xb1\xd9\x83\xd9\x88\xd8\xaf")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x82\xd8\xb3\xd9\x85")
            << QString::fromUtf8("\xd8\xb3\xd8\xb9\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xb4\xd8\xb1\xd8\xa7\xd8\xa1")
            << QString::fromUtf8("\xd8\xb3\xd8\xb9\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xa8\xd9\x8a\xd8\xb9")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x83\xd9\x85\xd9\x8a\xd8\xa9")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9");
    m_table->setHorizontalHeaderLabels(headers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->hide();
    m_table->setAlternatingRowColors(true);
    m_table->setLayoutDirection(Qt::RightToLeft);
    mainLayout->addWidget(m_table, 1);

    // Connections
    connect(m_addBtn, &QPushButton::clicked, this, &ProductsWidget::onAddProduct);
    connect(m_editBtn, &QPushButton::clicked, this, &ProductsWidget::onEditProduct);
    connect(m_deleteBtn, &QPushButton::clicked, this, &ProductsWidget::onDeleteProduct);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &ProductsWidget::onSearchChanged);
    connect(m_categoryFilter, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &ProductsWidget::onCategoryFilterChanged);
    connect(m_table, &QTableWidget::doubleClicked, this, &ProductsWidget::onEditProduct);
}

void ProductsWidget::refreshData()
{
    loadProducts();
}

void ProductsWidget::loadProducts()
{
    m_table->setRowCount(0);

    QString search = m_searchEdit->text().trimmed();
    int catId = m_categoryFilter->currentData().toInt();

    QList<Product> products;
    if (!search.isEmpty()) {
        products = Database::instance()->searchProducts(search);
    } else if (catId > 0) {
        products = Database::instance()->getProductsByCategory(catId);
    } else {
        products = Database::instance()->getAllProducts();
    }

    QList<Category> allCats = Database::instance()->getAllCategories();
    QMap<int, QString> catMap;
    for (int i = 0; i < allCats.size(); ++i) {
        catMap[allCats[i].id] = allCats[i].name;
    }

    for (int i = 0; i < products.size(); ++i) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(products[i].id)));
        m_table->setItem(row, 1, new QTableWidgetItem(products[i].name));
        m_table->setItem(row, 2, new QTableWidgetItem(products[i].barcode));
        m_table->setItem(row, 3, new QTableWidgetItem(catMap.value(products[i].categoryId)));
        m_table->setItem(row, 4, new QTableWidgetItem(QString::number(products[i].purchasePrice, 'f', 2)));
        m_table->setItem(row, 5, new QTableWidgetItem(QString::number(products[i].salePrice, 'f', 2)));

        QTableWidgetItem *qtyItem = new QTableWidgetItem(QString::number(products[i].quantity));
        if (products[i].quantity <= products[i].minQuantity) {
            qtyItem->setForeground(QBrush(QColor("#e74c3c")));
        }
        m_table->setItem(row, 6, qtyItem);

        QString status = products[i].quantity > 0 ?
            QString::fromUtf8("\xd9\x85\xd8\xaa\xd9\x88\xd9\x81\xd8\xb1") :
            QString::fromUtf8("\xd9\x86\xd9\x81\xd8\xb0");
        m_table->setItem(row, 7, new QTableWidgetItem(status));
    }
}

void ProductsWidget::showProductDialog(int productId)
{
    Product product;
    bool isEdit = (productId > 0);
    if (isEdit) {
        product = Database::instance()->getProductById(productId);
    }

    QDialog dialog(this);
    dialog.setWindowTitle(isEdit ?
        QString::fromUtf8("\xd8\xaa\xd8\xb9\xd8\xaf\xd9\x8a\xd9\x84 \xd9\x85\xd9\x86\xd8\xaa\xd8\xac") :
        QString::fromUtf8("\xd8\xa5\xd8\xb6\xd8\xa7\xd9\x81\xd8\xa9 \xd9\x85\xd9\x86\xd8\xaa\xd8\xac \xd8\xac\xd8\xaf\xd9\x8a\xd8\xaf"));
    dialog.setFixedSize(450, 500);
    dialog.setLayoutDirection(Qt::RightToLeft);

    QFormLayout *form = new QFormLayout(&dialog);
    form->setContentsMargins(20, 20, 20, 20);
    form->setSpacing(10);

    QLineEdit *nameEdit = new QLineEdit(product.name, &dialog);
    nameEdit->setFixedHeight(36);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85:"), nameEdit);

    QLineEdit *barcodeEdit = new QLineEdit(product.barcode, &dialog);
    barcodeEdit->setFixedHeight(36);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa8\xd8\xa7\xd8\xb1\xd9\x83\xd9\x88\xd8\xaf:"), barcodeEdit);

    QComboBox *catCombo = new QComboBox(&dialog);
    catCombo->setFixedHeight(36);
    QList<Category> cats = Database::instance()->getAllCategories();
    for (int i = 0; i < cats.size(); ++i) {
        catCombo->addItem(cats[i].name, cats[i].id);
        if (cats[i].id == product.categoryId) catCombo->setCurrentIndex(i);
    }
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x82\xd8\xb3\xd9\x85:"), catCombo);

    QDoubleSpinBox *purchaseSpin = new QDoubleSpinBox(&dialog);
    purchaseSpin->setFixedHeight(36);
    purchaseSpin->setMaximum(999999);
    purchaseSpin->setDecimals(2);
    purchaseSpin->setValue(product.purchasePrice);
    form->addRow(QString::fromUtf8("\xd8\xb3\xd8\xb9\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xb4\xd8\xb1\xd8\xa7\xd8\xa1:"), purchaseSpin);

    QDoubleSpinBox *saleSpin = new QDoubleSpinBox(&dialog);
    saleSpin->setFixedHeight(36);
    saleSpin->setMaximum(999999);
    saleSpin->setDecimals(2);
    saleSpin->setValue(product.salePrice);
    form->addRow(QString::fromUtf8("\xd8\xb3\xd8\xb9\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xa8\xd9\x8a\xd8\xb9:"), saleSpin);

    QSpinBox *qtySpin = new QSpinBox(&dialog);
    qtySpin->setFixedHeight(36);
    qtySpin->setMaximum(999999);
    qtySpin->setValue(product.quantity);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x83\xd9\x85\xd9\x8a\xd8\xa9:"), qtySpin);

    QSpinBox *minQtySpin = new QSpinBox(&dialog);
    minQtySpin->setFixedHeight(36);
    minQtySpin->setMaximum(999999);
    minQtySpin->setValue(product.minQuantity > 0 ? product.minQuantity : 5);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xad\xd8\xaf \xd8\xa7\xd9\x84\xd8\xa3\xd8\xaf\xd9\x86\xd9\x89:"), minQtySpin);

    QLineEdit *unitEdit = new QLineEdit(product.unit.isEmpty() ? QString::fromUtf8("\xd9\x82\xd8\xb7\xd8\xb9\xd8\xa9") : product.unit, &dialog);
    unitEdit->setFixedHeight(36);
    form->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x88\xd8\xad\xd8\xaf\xd8\xa9:"), unitEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    buttons->button(QDialogButtonBox::Ok)->setText(
        QString::fromUtf8("\xd8\xad\xd9\x81\xd8\xb8"));
    buttons->button(QDialogButtonBox::Cancel)->setText(
        QString::fromUtf8("\xd8\xa5\xd9\x84\xd8\xba\xd8\xa7\xd8\xa1"));
    form->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        if (nameEdit->text().trimmed().isEmpty()) {
            QMessageBox::warning(this,
                QString::fromUtf8("\xd8\xae\xd8\xb7\xd8\xa3"),
                QString::fromUtf8("\xd9\x8a\xd8\xb1\xd8\xac\xd9\x89 \xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84 \xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xaa\xd8\xac"));
            return;
        }

        product.name = nameEdit->text().trimmed();
        product.barcode = barcodeEdit->text().trimmed();
        product.categoryId = catCombo->currentData().toInt();
        product.purchasePrice = purchaseSpin->value();
        product.salePrice = saleSpin->value();
        product.quantity = qtySpin->value();
        product.minQuantity = minQtySpin->value();
        product.unit = unitEdit->text().trimmed();

        bool ok;
        if (isEdit) {
            ok = Database::instance()->updateProduct(product);
        } else {
            ok = Database::instance()->addProduct(product);
        }

        if (ok) {
            loadProducts();
        } else {
            QMessageBox::critical(this,
                QString::fromUtf8("\xd8\xae\xd8\xb7\xd8\xa3"),
                QString::fromUtf8("\xd9\x81\xd8\xb4\xd9\x84 \xd9\x81\xd9\x8a \xd8\xad\xd9\x81\xd8\xb8 \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xaa\xd8\xac"));
        }
    }
}

void ProductsWidget::onAddProduct()
{
    showProductDialog();
}

void ProductsWidget::onEditProduct()
{
    int row = m_table->currentRow();
    if (row < 0) return;
    int id = m_table->item(row, 0)->text().toInt();
    showProductDialog(id);
}

void ProductsWidget::onDeleteProduct()
{
    int row = m_table->currentRow();
    if (row < 0) return;

    QMessageBox::StandardButton reply = QMessageBox::question(this,
        QString::fromUtf8("\xd8\xaa\xd8\xa3\xd9\x83\xd9\x8a\xd8\xaf \xd8\xa7\xd9\x84\xd8\xad\xd8\xb0\xd9\x81"),
        QString::fromUtf8("\xd9\x87\xd9\x84 \xd8\xaa\xd8\xb1\xd9\x8a\xd8\xaf \xd8\xad\xd8\xb0\xd9\x81 \xd9\x87\xd8\xb0\xd8\xa7 \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xaa\xd8\xac\xd8\x9f"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        int id = m_table->item(row, 0)->text().toInt();
        Database::instance()->deleteProduct(id);
        loadProducts();
    }
}

void ProductsWidget::onSearchChanged(const QString &text)
{
    Q_UNUSED(text)
    loadProducts();
}

void ProductsWidget::onCategoryFilterChanged(int index)
{
    Q_UNUSED(index)
    loadProducts();
}
