#ifndef REPORTSWIDGET_H
#define REPORTSWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QDateEdit>
#include <QComboBox>

class ReportsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsWidget(QWidget *parent = nullptr);
    void refreshData();

private slots:
    void onDateRangeChanged();
    void onReportTypeChanged(int index);
    void onPrintReport();
    void onExportReport();

private:
    void setupUI();
    void loadSummary();
    void loadSalesHistory();
    void loadTopProducts();
    void loadSalesByCategory();
    void loadDailySales();

    QComboBox *m_reportType;
    QDateEdit *m_fromDate;
    QDateEdit *m_toDate;
    QPushButton *m_refreshBtn;
    QPushButton *m_printBtn;

    // Summary cards
    QLabel *m_totalSalesLabel;
    QLabel *m_totalProfitLabel;
    QLabel *m_totalTransLabel;
    QLabel *m_avgSaleLabel;

    QTableWidget *m_table;
};

#endif // REPORTSWIDGET_H
