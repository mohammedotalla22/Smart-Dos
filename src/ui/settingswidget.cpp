#include "settingswidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QSettings>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QFont>
#include "../core/thermalprinter.h"
#include "../core/gdiprinter.h"
#include "../core/hardwareid.h"
#include "../core/firebase.h"

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadSettings();
}

void SettingsWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    // Title
    QLabel *title = new QLabel(
        QString::fromUtf8("\xe2\x9a\x99 \xd8\xa7\xd9\x84\xd8\xa5\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa"), this);
    title->setObjectName("pageTitle");
    QFont titleFont("Arial", 20, QFont::Bold);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignRight);
    mainLayout->addWidget(title);

    // Scroll area
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setObjectName("settingsScroll");

    QWidget *content = new QWidget(scroll);
    content->setLayoutDirection(Qt::RightToLeft);
    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setSpacing(16);

    // ===== Store Info =====
    QGroupBox *storeGroup = new QGroupBox(
        QString::fromUtf8("\xd9\x85\xd8\xb9\xd9\x84\xd9\x88\xd9\x85\xd8\xa7\xd8\xaa \xd8\xa7\xd9\x84\xd9\x85\xd8\xaa\xd8\xac\xd8\xb1"), content);
    storeGroup->setObjectName("settingsGroup");
    QFormLayout *storeForm = new QFormLayout(storeGroup);

    m_storeNameEdit = new QLineEdit(storeGroup);
    m_storeNameEdit->setFixedHeight(36);
    storeForm->addRow(QString::fromUtf8("\xd8\xa7\xd8\xb3\xd9\x85 \xd8\xa7\xd9\x84\xd9\x85\xd8\xaa\xd8\xac\xd8\xb1:"), m_storeNameEdit);

    m_storePhoneEdit = new QLineEdit(storeGroup);
    m_storePhoneEdit->setFixedHeight(36);
    storeForm->addRow(QString::fromUtf8("\xd8\xb1\xd9\x82\xd9\x85 \xd8\xa7\xd9\x84\xd9\x87\xd8\xa7\xd8\xaa\xd9\x81:"), m_storePhoneEdit);

    m_storeAddressEdit = new QLineEdit(storeGroup);
    m_storeAddressEdit->setFixedHeight(36);
    storeForm->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86:"), m_storeAddressEdit);

    m_taxNumberEdit = new QLineEdit(storeGroup);
    m_taxNumberEdit->setFixedHeight(36);
    storeForm->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb1\xd9\x82\xd9\x85 \xd8\xa7\xd9\x84\xd8\xb6\xd8\xb1\xd9\x8a\xd8\xa8\xd9\x8a:"), m_taxNumberEdit);

    contentLayout->addWidget(storeGroup);

    // ===== Printer Settings =====
    QGroupBox *printerGroup = new QGroupBox(
        QString::fromUtf8("\xd8\xa5\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa \xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9"), content);
    printerGroup->setObjectName("settingsGroup");
    QFormLayout *printerForm = new QFormLayout(printerGroup);

    m_printerTypeCombo = new QComboBox(printerGroup);
    m_printerTypeCombo->setFixedHeight(36);
    m_printerTypeCombo->addItem(QString::fromUtf8("\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9 \xd8\xad\xd8\xb1\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xa9 (Thermal)"), "thermal");
    m_printerTypeCombo->addItem(QString::fromUtf8("\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9 \xd8\xb9\xd8\xa7\xd8\xaf\xd9\x8a\xd8\xa9 (GDI/A4)"), "gdi");
    printerForm->addRow(QString::fromUtf8("\xd9\x86\xd9\x88\xd8\xb9 \xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9:"), m_printerTypeCombo);

    // Thermal settings
    m_thermalPortCombo = new QComboBox(printerGroup);
    m_thermalPortCombo->setFixedHeight(36);
    m_thermalPortCombo->setEditable(true);
    QStringList ports = ThermalPrinter::availablePorts();
    for (int i = 0; i < ports.size(); ++i) {
        m_thermalPortCombo->addItem(ports[i]);
    }
    printerForm->addRow(QString::fromUtf8("\xd9\x85\xd9\x86\xd9\x81\xd8\xb0 \xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xad\xd8\xb1\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xa9:"), m_thermalPortCombo);

    m_thermalBaudSpin = new QSpinBox(printerGroup);
    m_thermalBaudSpin->setFixedHeight(36);
    m_thermalBaudSpin->setMinimum(1200);
    m_thermalBaudSpin->setMaximum(115200);
    m_thermalBaudSpin->setValue(9600);
    printerForm->addRow(QString::fromUtf8("\xd8\xb3\xd8\xb1\xd8\xb9\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xa7\xd8\xaa\xd8\xb5\xd8\xa7\xd9\x84 (Baud):"), m_thermalBaudSpin);

    m_thermalSizeCombo = new QComboBox(printerGroup);
    m_thermalSizeCombo->setFixedHeight(36);
    m_thermalSizeCombo->addItem("58mm", "58");
    m_thermalSizeCombo->addItem("80mm", "80");
    m_thermalSizeCombo->setCurrentIndex(1);
    printerForm->addRow(QString::fromUtf8("\xd8\xad\xd8\xac\xd9\x85 \xd8\xa7\xd9\x84\xd9\x88\xd8\xb1\xd9\x82:"), m_thermalSizeCombo);

    QPushButton *testThermalBtn = new QPushButton(
        QString::fromUtf8("\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xad\xd8\xb1\xd8\xa7\xd8\xb1\xd9\x8a\xd8\xa9"), printerGroup);
    testThermalBtn->setObjectName("secondaryButton");
    testThermalBtn->setFixedHeight(36);
    testThermalBtn->setCursor(Qt::PointingHandCursor);
    printerForm->addRow("", testThermalBtn);

    // GDI settings
    m_gdiPrinterCombo = new QComboBox(printerGroup);
    m_gdiPrinterCombo->setFixedHeight(36);
    QStringList printers = GdiPrinter::availablePrinters();
    for (int i = 0; i < printers.size(); ++i) {
        m_gdiPrinterCombo->addItem(printers[i]);
    }
    printerForm->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xb9\xd8\xa7\xd8\xaf\xd9\x8a\xd8\xa9 (GDI):"), m_gdiPrinterCombo);

    QPushButton *testGdiBtn = new QPushButton(
        QString::fromUtf8("\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xb9\xd8\xa7\xd8\xaf\xd9\x8a\xd8\xa9"), printerGroup);
    testGdiBtn->setObjectName("secondaryButton");
    testGdiBtn->setFixedHeight(36);
    testGdiBtn->setCursor(Qt::PointingHandCursor);
    printerForm->addRow("", testGdiBtn);

    m_autoPrintCheck = new QCheckBox(
        QString::fromUtf8("\xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9 \xd8\xaa\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x8a\xd8\xa9 \xd8\xa8\xd8\xb9\xd8\xaf \xd9\x83\xd9\x84 \xd8\xb9\xd9\x85\xd9\x84\xd9\x8a\xd8\xa9 \xd8\xa8\xd9\x8a\xd8\xb9"), printerGroup);
    printerForm->addRow("", m_autoPrintCheck);

    contentLayout->addWidget(printerGroup);

    // ===== Barcode Scanner =====
    QGroupBox *barcodeGroup = new QGroupBox(
        QString::fromUtf8("\xd8\xa5\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa \xd9\x82\xd8\xa7\xd8\xb1\xd8\xa6 \xd8\xa7\xd9\x84\xd8\xa8\xd8\xa7\xd8\xb1\xd9\x83\xd9\x88\xd8\xaf"), content);
    barcodeGroup->setObjectName("settingsGroup");
    QFormLayout *barcodeForm = new QFormLayout(barcodeGroup);

    m_barcodeEnabledCheck = new QCheckBox(
        QString::fromUtf8("\xd8\xaa\xd9\x81\xd8\xb9\xd9\x8a\xd9\x84 \xd9\x82\xd8\xa7\xd8\xb1\xd8\xa6 \xd8\xa7\xd9\x84\xd8\xa8\xd8\xa7\xd8\xb1\xd9\x83\xd9\x88\xd8\xaf"), barcodeGroup);
    m_barcodeEnabledCheck->setChecked(true);
    barcodeForm->addRow("", m_barcodeEnabledCheck);

    m_barcodeMinLenSpin = new QSpinBox(barcodeGroup);
    m_barcodeMinLenSpin->setFixedHeight(36);
    m_barcodeMinLenSpin->setMinimum(1);
    m_barcodeMinLenSpin->setMaximum(50);
    m_barcodeMinLenSpin->setValue(4);
    barcodeForm->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xad\xd8\xaf \xd8\xa7\xd9\x84\xd8\xa3\xd8\xaf\xd9\x86\xd9\x89 \xd9\x84\xd8\xb7\xd9\x88\xd9\x84 \xd8\xa7\xd9\x84\xd8\xa8\xd8\xa7\xd8\xb1\xd9\x83\xd9\x88\xd8\xaf:"), m_barcodeMinLenSpin);

    contentLayout->addWidget(barcodeGroup);

    // ===== Appearance =====
    QGroupBox *themeGroup = new QGroupBox(
        QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x85\xd8\xb8\xd9\x87\xd8\xb1"), content);
    themeGroup->setObjectName("settingsGroup");
    QFormLayout *themeForm = new QFormLayout(themeGroup);

    m_themeCombo = new QComboBox(themeGroup);
    m_themeCombo->setFixedHeight(36);
    m_themeCombo->addItem(QString::fromUtf8("\xd8\xaf\xd8\xa7\xd9\x83\xd9\x86 (Dark)"), "dark");
    m_themeCombo->addItem(QString::fromUtf8("\xd9\x81\xd8\xa7\xd8\xaa\xd8\xad (Light)"), "light");
    themeForm->addRow(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb3\xd9\x85\xd8\xa9:"), m_themeCombo);

    contentLayout->addWidget(themeGroup);

    // ===== Firebase =====
    QGroupBox *fbGroup = new QGroupBox(
        QString::fromUtf8("\xd8\xa5\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa Firebase"), content);
    fbGroup->setObjectName("settingsGroup");
    QFormLayout *fbForm = new QFormLayout(fbGroup);

    m_fbProjectEdit = new QLineEdit(fbGroup);
    m_fbProjectEdit->setFixedHeight(36);
    m_fbProjectEdit->setPlaceholderText("your-project-id");
    fbForm->addRow("Project ID:", m_fbProjectEdit);

    m_fbApiKeyEdit = new QLineEdit(fbGroup);
    m_fbApiKeyEdit->setFixedHeight(36);
    m_fbApiKeyEdit->setPlaceholderText("AIza...");
    m_fbApiKeyEdit->setEchoMode(QLineEdit::Password);
    fbForm->addRow("API Key:", m_fbApiKeyEdit);

    m_fbCollectionEdit = new QLineEdit(fbGroup);
    m_fbCollectionEdit->setFixedHeight(36);
    m_fbCollectionEdit->setPlaceholderText("devices");
    fbForm->addRow("Collection:", m_fbCollectionEdit);

    contentLayout->addWidget(fbGroup);

    // ===== Hardware ID =====
    QGroupBox *hwGroup = new QGroupBox(
        QString::fromUtf8("\xd9\x85\xd8\xb9\xd8\xb1\xd9\x81 \xd8\xa7\xd9\x84\xd8\xac\xd9\x87\xd8\xa7\xd8\xb2"), content);
    hwGroup->setObjectName("settingsGroup");
    QHBoxLayout *hwLayout = new QHBoxLayout(hwGroup);

    QPushButton *copyHwBtn = new QPushButton(
        QString::fromUtf8("\xd9\x86\xd8\xb3\xd8\xae"), hwGroup);
    copyHwBtn->setObjectName("secondaryButton");
    copyHwBtn->setFixedSize(60, 36);
    copyHwBtn->setCursor(Qt::PointingHandCursor);
    hwLayout->addWidget(copyHwBtn);

    m_hwIdLabel = new QLabel(HardwareId::generate(), hwGroup);
    m_hwIdLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    QFont monoFont("Courier New", 12);
    m_hwIdLabel->setFont(monoFont);
    m_hwIdLabel->setAlignment(Qt::AlignRight);
    hwLayout->addWidget(m_hwIdLabel, 1);

    connect(copyHwBtn, &QPushButton::clicked, this, [this]() {
        QApplication::clipboard()->setText(m_hwIdLabel->text());
        QMessageBox::information(this,
            QString::fromUtf8("\xd8\xaa\xd9\x85"),
            QString::fromUtf8("\xd8\xaa\xd9\x85 \xd9\x86\xd8\xb3\xd8\xae \xd9\x85\xd8\xb9\xd8\xb1\xd9\x81 \xd8\xa7\xd9\x84\xd8\xac\xd9\x87\xd8\xa7\xd8\xb2"));
    });

    contentLayout->addWidget(hwGroup);

    contentLayout->addStretch();
    scroll->setWidget(content);
    mainLayout->addWidget(scroll, 1);

    // Save button
    m_saveBtn = new QPushButton(
        QString::fromUtf8("\xd8\xad\xd9\x81\xd8\xb8 \xd8\xa7\xd9\x84\xd8\xa5\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa"), this);
    m_saveBtn->setObjectName("primaryButton");
    m_saveBtn->setFixedHeight(48);
    m_saveBtn->setCursor(Qt::PointingHandCursor);
    QFont saveFont("Arial", 14, QFont::Bold);
    m_saveBtn->setFont(saveFont);
    mainLayout->addWidget(m_saveBtn);

    // Connections
    connect(m_saveBtn, &QPushButton::clicked, this, &SettingsWidget::onSaveSettings);
    connect(m_themeCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &SettingsWidget::onThemeSelected);
    connect(testThermalBtn, &QPushButton::clicked, this, &SettingsWidget::onTestThermalPrinter);
    connect(testGdiBtn, &QPushButton::clicked, this, &SettingsWidget::onTestGdiPrinter);
}

void SettingsWidget::loadSettings()
{
    QSettings s("SmartPOS", "SmartPOS");

    m_storeNameEdit->setText(s.value("store/name", "Smart POS").toString());
    m_storePhoneEdit->setText(s.value("store/phone").toString());
    m_storeAddressEdit->setText(s.value("store/address").toString());
    m_taxNumberEdit->setText(s.value("store/taxNumber").toString());

    QString pType = s.value("printer/type", "thermal").toString();
    m_printerTypeCombo->setCurrentIndex(pType == "gdi" ? 1 : 0);
    m_thermalPortCombo->setCurrentText(s.value("printer/thermalPort").toString());
    m_thermalBaudSpin->setValue(s.value("printer/thermalBaud", 9600).toInt());
    QString pSize = s.value("printer/thermalSize", "80").toString();
    m_thermalSizeCombo->setCurrentIndex(pSize == "58" ? 0 : 1);
    m_gdiPrinterCombo->setCurrentText(s.value("printer/gdiName").toString());
    m_autoPrintCheck->setChecked(s.value("printer/autoPrint", false).toBool());

    m_barcodeEnabledCheck->setChecked(s.value("barcode/enabled", true).toBool());
    m_barcodeMinLenSpin->setValue(s.value("barcode/minLength", 4).toInt());

    QString theme = s.value("appearance/theme", "dark").toString();
    m_themeCombo->setCurrentIndex(theme == "light" ? 1 : 0);

    m_fbProjectEdit->setText(s.value("firebase/projectId").toString());
    m_fbApiKeyEdit->setText(s.value("firebase/apiKey").toString());
    m_fbCollectionEdit->setText(s.value("firebase/collection", "devices").toString());
}

void SettingsWidget::onSaveSettings()
{
    QSettings s("SmartPOS", "SmartPOS");

    s.setValue("store/name", m_storeNameEdit->text());
    s.setValue("store/phone", m_storePhoneEdit->text());
    s.setValue("store/address", m_storeAddressEdit->text());
    s.setValue("store/taxNumber", m_taxNumberEdit->text());

    s.setValue("printer/type", m_printerTypeCombo->currentData().toString());
    s.setValue("printer/thermalPort", m_thermalPortCombo->currentText());
    s.setValue("printer/thermalBaud", m_thermalBaudSpin->value());
    s.setValue("printer/thermalSize", m_thermalSizeCombo->currentData().toString());
    s.setValue("printer/gdiName", m_gdiPrinterCombo->currentText());
    s.setValue("printer/autoPrint", m_autoPrintCheck->isChecked());

    s.setValue("barcode/enabled", m_barcodeEnabledCheck->isChecked());
    s.setValue("barcode/minLength", m_barcodeMinLenSpin->value());

    s.setValue("appearance/theme", m_themeCombo->currentData().toString());

    s.setValue("firebase/projectId", m_fbProjectEdit->text());
    s.setValue("firebase/apiKey", m_fbApiKeyEdit->text());
    s.setValue("firebase/collection", m_fbCollectionEdit->text());

    // Reconfigure Firebase
    Firebase::instance()->setConfig(
        m_fbProjectEdit->text(),
        m_fbApiKeyEdit->text(),
        m_fbCollectionEdit->text()
    );

    QMessageBox::information(this,
        QString::fromUtf8("\xd8\xaa\xd9\x85"),
        QString::fromUtf8("\xd8\xaa\xd9\x85 \xd8\xad\xd9\x81\xd8\xb8 \xd8\xa7\xd9\x84\xd8\xa5\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf\xd8\xa7\xd8\xaa \xd8\xa8\xd9\x86\xd8\xac\xd8\xa7\xd8\xad"));
}

void SettingsWidget::onThemeSelected(int index)
{
    QString theme = m_themeCombo->itemData(index).toString();
    emit themeChanged(theme);
}

void SettingsWidget::onTestThermalPrinter()
{
    ThermalPrinter tp;
    QString port = m_thermalPortCombo->currentText().split(" - ").first();
    if (tp.openPort(port, m_thermalBaudSpin->value())) {
        tp.setPaperSize(m_thermalSizeCombo->currentData().toString() == "58" ?
            ThermalPrinter::Paper58mm : ThermalPrinter::Paper80mm);
        tp.setAlignment(1);
        tp.setBold(true);
        tp.printLine("Smart POS");
        tp.setBold(false);
        tp.printLine(QString::fromUtf8("\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9"));
        tp.printSeparator();
        tp.printLine(QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9 \xd8\xaa\xd8\xb9\xd9\x85\xd9\x84 \xd8\xa8\xd8\xb4\xd9\x83\xd9\x84 \xd8\xb5\xd8\xad\xd9\x8a\xd8\xad"));
        tp.cut();
        tp.closePort();
        QMessageBox::information(this,
            QString::fromUtf8("\xd9\x86\xd8\xac\xd8\xa7\xd8\xad"),
            QString::fromUtf8("\xd8\xaa\xd9\x85 \xd8\xa7\xd9\x84\xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9 \xd8\xa8\xd9\x86\xd8\xac\xd8\xa7\xd8\xad"));
    } else {
        QMessageBox::warning(this,
            QString::fromUtf8("\xd8\xae\xd8\xb7\xd8\xa3"),
            QString::fromUtf8("\xd9\x84\xd8\xa7 \xd9\x8a\xd9\x85\xd9\x83\xd9\x86 \xd8\xa7\xd9\x84\xd8\xa7\xd8\xaa\xd8\xb5\xd8\xa7\xd9\x84 \xd8\xa8\xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9"));
    }
}

void SettingsWidget::onTestGdiPrinter()
{
    GdiPrinter gp;
    if (!m_gdiPrinterCombo->currentText().isEmpty()) {
        gp.setPrinterName(m_gdiPrinterCombo->currentText());
    }

    QStringList headers;
    headers << QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xa8\xd9\x86\xd8\xaf")
            << QString::fromUtf8("\xd8\xa7\xd9\x84\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9");
    QList<QStringList> rows;
    QStringList row1; row1 << QString::fromUtf8("\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb1") << "100.00";
    rows << row1;

    gp.printReport(
        QString::fromUtf8("\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9"),
        headers, rows,
        QString::fromUtf8("\xd8\xa7\xd9\x84\xd8\xb7\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa9 \xd8\xaa\xd8\xb9\xd9\x85\xd9\x84 \xd8\xa8\xd9\x86\xd8\xac\xd8\xa7\xd8\xad"));
}
