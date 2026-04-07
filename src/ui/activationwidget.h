#ifndef ACTIVATIONWIDGET_H
#define ACTIVATIONWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class ActivationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ActivationWidget(QWidget *parent = nullptr);
    void setHardwareId(const QString &id);

signals:
    void activationSuccess();

private slots:
    void onActivateClicked();

private:
    void setupUI();

    QLabel *m_hwIdLabel;
    QLineEdit *m_licenseEdit;
    QPushButton *m_activateBtn;
    QPushButton *m_copyHwIdBtn;
    QLabel *m_statusLabel;
    QString m_hardwareId;
};

#endif // ACTIVATIONWIDGET_H
