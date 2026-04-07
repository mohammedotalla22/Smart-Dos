#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);

signals:
    void themeChanged(const QString &theme);

private slots:
    void onSaveSettings();
    void onThemeSelected(int index);
    void onTestThermalPrinter();
    void onTestGdiPrinter();

private:
    void setupUI();
    void loadSettings();

    // Store info
    QLineEdit *m_storeNameEdit;
    QLineEdit *m_storePhoneEdit;
    QLineEdit *m_storeAddressEdit;
    QLineEdit *m_taxNumberEdit;

    // Printer settings
    QComboBox *m_printerTypeCombo;
    QComboBox *m_thermalPortCombo;
    QSpinBox *m_thermalBaudSpin;
    QComboBox *m_thermalSizeCombo;
    QComboBox *m_gdiPrinterCombo;
    QCheckBox *m_autoPrintCheck;

    // Barcode settings
    QCheckBox *m_barcodeEnabledCheck;
    QSpinBox *m_barcodeMinLenSpin;

    // Theme
    QComboBox *m_themeCombo;

    // Firebase
    QLineEdit *m_fbProjectEdit;
    QLineEdit *m_fbApiKeyEdit;
    QLineEdit *m_fbCollectionEdit;

    // Hardware ID
    QLabel *m_hwIdLabel;

    QPushButton *m_saveBtn;
};

#endif // SETTINGSWIDGET_H
