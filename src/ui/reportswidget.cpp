#include "reportswidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QHeaderView>
#include <QSettings>
#include <QFont>
#include <QFrame>
#include <QDateTime>
#include "../core/database.h"
#include "../core/gdiprinter.h"

ReportsWidget::ReportsWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void ReportsWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // Title
    QLabel *title = new QLabel(
        QString::fromUtf8("\xf0\x9f\x93\x8a \xd8\xa7\xd9\x84\xd8\xaa\xd9\x82\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xb1 \xd9\x88\xd8\xa7\xd9\x84\xd8\xa3\xd8\xb1\xd8\xa8\xd8\xa7\xd8\xad"), this);
    title->setObjectName("pageTitle");
    QFont titleFont("Arial", 20, QFont::Bold);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignRight);
    mainLayout->addWidget(title);

    // Toolbar
    QHBoxLayout *toolbar = new QHBoxLayout();

    m_printBtn = new QPushButton(
        QString::fromUtf8("\xf0\x9f\x96\xa8 \xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xaa\xd9\x82\xd8\xb1\xd9\x8a\xd8\xb1"), this);
    m_printBtn->setObjectName("primaryButton");
    m_printBtn->setFixedHeight(38);
    m_printBtn->setCursor(Qt::PointingHandCursor);
    toolbar->addWidget(m_printBtn);

    m_refreshBtn = new QPushButton(
        QString::fromUtf8("\xd8\xaa\xd8\xad\xd8\xaf\xd9\x8a\xd8\xab"), this);
    m_refreshBtn->setObjectName("secondaryButton");
    m_refreshBtn->setFixedHeight(38);
    m_refreshBtn->setCursor(Qt::PointingHandCursor);
    toolbar->addWidget(m_refreshBtn);

    toolbar->addStretch();

    m_fromDate = new QDateEdit(QDate::currentDate().addDays(-30), this);
    m_fromDate->setCalendarPopup(true);
    m_fromDate->setFixedHeight(38);
    m_fromDate->setDisplayFormat("yyyy/MM/dd");
    toolbar->addWidget(m_fromDate);

    QLabel *toLabel = new QLabel(QString::fromUtf8("\xd8\xa5\xd9\x84\xd9\x89"), this);
    toolbar->addWidget(toLabel);

    m_toDate = new QDateEdit(QDate::currentDate(), this);
    m_toDate->setCalendarPopup(true);
    m_toDate->setFixedHeight(38);
    m_toDate->setDisplayFormat("yyyy/MM/dd");
    toolbar->addWidget(m_toDate);

    QLabel *fromLabel = new QLabel(QString::fromUtf8("\xd9\x85\xd9\x86"), this);
    toolbar->addWidget(fromLabel);

    m_reportType = new QComboBox(this);
    m_reportType->setFixedHeight(38);
    m_reportType->setMinimumWidth(160);
    m_reportType->addItem(QString::fromUtf8("\xd9\x85\xd9\x84\xd8\xae\xd8\xb5 \xd8\xa7\xd9\x84\xd9\x85\xd8\xa8\xd9\x8a\xd8\xb9\xd8\xa7\xd8\xaa"));
    m_reportType->addItem(QString::fromUtf8("\xd8\xb3\xd8\xac\xd9\x84 \xd8\xa7\xd9\x84\xd9\x85\xd8\xa8\xd9\x8a\xd8\xb9\xd8\xa7\xd8\xaa"));
    m_reportType->addItem(QString::fromUtf8("\xd8\xa3\xd9\x83\xd8\xab\xd8\xb1 \xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xaa\xd8\xac\xd8\xa7\xd8\xaa \xd9\x85\xd8\xa8\xd9\x8a\xd8\xb9\xd8\xa7\xd9\x8b"));
    m_reportType->addItem(QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd8\xa8\xd9\x8a\xd8\xb9\xd8\xa7\xd8\xaa \xd8\xad\xd8\xb3\xd8\xa8 \xd8\xa7\xd9\x84\xd9\x82\xd8\xb3\xd9\x85"));
    m_reportType->addItem(QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd8\xa8\xd9\x8a\xd8\xb9\xd8\xa7\xd8\xaa \xd8\xa7\xd9\x84\xd9\x8a\xd9\x88\xd9\x85\xd9\x8a\xd8\xa9"));
    toolbar->addWidget(m_reportType);

    mainLayout->addLayout(toolbar);

    // Summary cards
    QWidget *summaryWidget = new QWidget(this);
    summaryWidget->setObjectName("summaryWidget");
    QHBoxLayout *summaryLayout = new QHBoxLayout(summaryWidget);
    summaryLayout->setSpacing(12);

    auto createCard = [&](const QString &label, QLabel *&valueLabel, const QString &color) {
        QWidget *card = new QWidget(summaryWidget);
        card->setObjectName("summaryCard");
        card->setStyleSheet(QString("QWidget#summaryCard { border-left: 4px solid %1; }").arg(color));
        card->setFixedHeight(90);
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(12, 8, 12, 8);

        QLabel *lbl = new QLabel(label, card);
        lbl->setObjectName("summaryLabel");
        lbl->setAlignment(Qt::AlignRight);
        cardLayout->addWidget(lbl);

        valueLabel = new QLabel("0", card);
        valueLabel->setObjectName("summaryValue");
        valueLabel->setAlignment(Qt::AlignRight);
        QFont vf("Arial", 22, QFont::Bold);
        valueLabel->setFont(vf);
        valueLabel->setStyleSheet(QString("color: %1;").arg(color));
        cardLayout->addWidget(valueLabel);

        summaryLayout->addWidget(card);
    };

    createCard(QString::fromUtf8("\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a \xd8\xa7\xd9\x84\xd9\x85\xd8\xa8\xd9\x8a\xd8\xb9\xd8\xa7\xd8\xaa"), m_totalSalesLabel, "#3498db");
    createCard(QString::fromUtf8("\xd8\xb5\xd8\xa7\xd9\x81\xd9\x8a \xd8\xa7\xd9\x84\xd8\xb1\xd8\xa8\xd8\xad"), m_totalProfitLabel, "#2ecc71");
    createCard(QString::fromUtf8("\xd8\xb9\xd8\xaf\xd8\xaf \xd8\xa7\xd9\x84\xd9\x85\xd8\xb9\xd8\xa7\xd9\x85\xd9\x84\xd8\xa7\xd8\xaa"), m_totalTransLabel, "#f39c12");
    createCard(QString::fromUtf8("\xd9\x85\xd8\xaa\xd9\x88\xd8\xb3\xd8\xb7 \xd8\xa7\xd9\x84\xd9\x81\xd8\xa7\xd8\xaa\xd9\x88\xd8\xb1\xd8\xa9"), m_avgSaleLabel, "#9b59b6");

    mainLayout->addWidget(summaryWidget);

    // Data table
    m_table = new QTableWidget(this);
    m_table->setObjectName("dataTable");
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->hide();
    m_table->setAlternatingRowColors(true);
    m_table->setLayoutDirection(Qt::RightToLeft);
    mainLayout->addWidget(m_table, 1);

    // Connections
    connect(m_reportType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &ReportsWidget::onReportTypeChanged);
    connect(m_refreshBtn, &QPushButton::clicked, this, &ReportsWidget::refreshData);
    connect(m_fromDate, &QDateEdit::dateChanged, this, &ReportsWidget::onDateRangeChanged);
    connect(m_toDate, &QDateEdit::dateChanged, this, &ReportsWidget::onDateRangeChanged);
    connect(m_printBtn, &QPushButton::clicked, this, &ReportsWidget::onPrintReport);
}

void ReportsWidget::refreshData()
{
    loadSummary();
    onReportTypeChanged(m_reportType->currentIndex());
}

void ReportsWidget::loadSummary()
{
    QDateTime from(m_fromDate->date(), QTime(0, 0, 0));
    QDateTime to(m_toDate->date(), QTime(23, 59, 59));

    Database *db = Database::instance();
    double sales = db->getTotalSales(from, to);
    double profit = db->getTotalProfit(from, to);
    int trans = db->getTotalTransactions(from, to);
    double avg = trans > 0 ? sales / trans : 0;

    m_totalSalesLabel->setText(QString::number(sales, 'f', 2));
    m_totalProfitLabel->setText(QString::number(profit, 'f', 2));
    m_totalTransLabel->setText(QString::number(trans));
    m_avgSaleLabel->setText(QString::number(avg, 'f', 2));
}

void ReportsWidget::onDateRangeChanged()
{
    refreshData();
}

void ReportsWidget::onReportTypeChanged(int index)
{
    loadSummary();
    switch (index) {
        case 0: loadSalesHistory(); break;
        case 1: loadSalesHistory(); break;
        case 2: loadTopProducts(); break;
        case 3: loadSalesByCategory(); break;
        case 4: loadDailySales(); break;
    }
}

void ReportsWidget::loadSalesHistory()
{
    QDateTime from(m_fromDate->date(), QTime(0, 0, 0));
    QDateTime to(m_toDate->date(), QTime(23, 59, 59));

    m_table->clear();
    m_table->setColumnCount(7);
    QStringList headers;
    headers << QString::fromUtf8("\xd8\xb1\xd9\x82\xd9\x85 \xd8\xa7\xd9\x84\xd9\x81\xd8\xa7\xd8\xaa\xd9\x88\xd8\xb1\xd8\xa9")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb9\xd9\x85\xd9\x8a\xd9\x84")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x83\xd8\xa7\xd8\xb4\xd9\x8a\xd8\xb1")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xaf\xd9\x81\xd8\xb9")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xad\xd8\xa7\xd9\x84\xd8\xa9");
    m_table->setHorizontalHeaderLabels(headers);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    QList<Sale> sales = Database::instance()->getSalesByDateRange(from, to);
    m_table->setRowCount(sales.size());

    for (int i = 0; i < sales.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(sales[i].invoiceNumber));
        m_table->setItem(i, 1, new QTableWidgetItem(sales[i].createdAt.toString("yyyy/MM/dd hh:mm")));
        m_table->setItem(i, 2, new QTableWidgetItem(sales[i].customerName));
        m_table->setItem(i, 3, new QTableWidgetItem(sales[i].userName));
        m_table->setItem(i, 4, new QTableWidgetItem(QString::number(sales[i].total, 'f', 2)));
        m_table->setItem(i, 5, new QTableWidgetItem(sales[i].paymentMethod));
        m_table->setItem(i, 6, new QTableWidgetItem(
            QString::fromUtf8("\xd9\x85\xd9\x83\xd8\xaa\xd9\x85\xd9\x84")));
    }
}

void ReportsWidget::loadTopProducts()
{
    QDateTime from(m_fromDate->date(), QTime(0, 0, 0));
    QDateTime to(m_toDate->date(), QTime(23, 59, 59));

    m_table->clear();
    m_table->setColumnCount(3);
    QStringList headers;
    headers << "#"
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd9\x86\xd8\xaa\xd8\xac")
            << QString::fromUtf8("\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a \xd8\xa7\xd9\x84\xd9\x85\xd8\xa8\xd9\x8a\xd8\xb9\xd8\xa7\xd8\xaa");
    m_table->setHorizontalHeaderLabels(headers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    QList<QPair<QString, double>> data = Database::instance()->getTopProducts(from, to);
    m_table->setRowCount(data.size());

    for (int i = 0; i < data.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        m_table->setItem(i, 1, new QTableWidgetItem(data[i].first));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::number(data[i].second, 'f', 2)));
    }
}

void ReportsWidget::loadSalesByCategory()
{
    QDateTime from(m_fromDate->date(), QTime(0, 0, 0));
    QDateTime to(m_toDate->date(), QTime(23, 59, 59));

    m_table->clear();
    m_table->setColumnCount(2);
    QStringList headers;
    headers << QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x82\xd8\xb3\xd9\x85")
            << QString::fromUtf8("\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a \xd8\xa7\xd9\x84\xd9\x85\xd8\xa8\xd9\x8a\xd8\xb9\xd8\xa7\xd8\xaa");
    m_table->setHorizontalHeaderLabels(headers);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    QList<QPair<QString, double>> data = Database::instance()->getSalesByCategory(from, to);
    m_table->setRowCount(data.size());

    for (int i = 0; i < data.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(data[i].first));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::number(data[i].second, 'f', 2)));
    }
}

void ReportsWidget::loadDailySales()
{
    QDateTime from(m_fromDate->date(), QTime(0, 0, 0));
    QDateTime to(m_toDate->date(), QTime(23, 59, 59));

    m_table->clear();
    m_table->setColumnCount(2);
    QStringList headers;
    headers << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xaa\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xae")
            << QString::fromUtf8("\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a \xd8\xa7\xd9\x84\xd9\x85\xd8\xa8\xd9\x8a\xd8\xb9\xd8\xa7\xd8\xaa");
    m_table->setHorizontalHeaderLabels(headers);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    QList<QPair<QString, double>> data = Database::instance()->getDailySales(from, to);
    m_table->setRowCount(data.size());

    for (int i = 0; i < data.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(data[i].first));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::number(data[i].second, 'f', 2)));
    }
}

void ReportsWidget::onPrintReport()
{
    QDateTime from(m_fromDate->date(), QTime(0, 0, 0));
    QDateTime to(m_toDate->date(), QTime(23, 59, 59));

    QSettings settings("SmartPOS", "SmartPOS");
    GdiPrinter printer;
    QString gdiName = settings.value("printer/gdiName").toString();
    if (!gdiName.isEmpty()) printer.setPrinterName(gdiName);

    QString reportTitle = m_reportType->currentText();

    // Collect table data
    QStringList headers;
    QList<QStringList> rows;
    for (int c = 0; c < m_table->columnCount(); ++c) {
        if (m_table->horizontalHeaderItem(c))
            headers << m_table->horizontalHeaderItem(c)->text();
    }
    for (int r = 0; r < m_table->rowCount(); ++r) {
        QStringList row;
        for (int c = 0; c < m_table->columnCount(); ++c) {
            if (m_table->item(r, c))
                row << m_table->item(r, c)->text();
            else
                row << "";
        }
        rows << row;
    }

    double totalSales = Database::instance()->getTotalSales(from, to);
    QString summary = QString::fromUtf8("\xd8\xa5\xd8\xac\xd9\x85\xd8\xa7\xd9\x84\xd9\x8a \xd8\xa7\xd9\x84\xd9\x85\xd8\xa8\xd9\x8a\xd8\xb9\xd8\xa7\xd8\xaa: ") +
                       QString::number(totalSales, 'f', 2);

    printer.printReport(reportTitle + " - " + m_fromDate->text() + " ~ " + m_toDate->text(),
                        headers, rows, summary);
}

void ReportsWidget::onExportReport()
{
    // Future: Export to CSV/Excel
}
