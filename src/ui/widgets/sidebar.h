#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QList>

class SidebarButton : public QPushButton
{
    Q_OBJECT

public:
    explicit SidebarButton(const QString &text, const QString &iconChar, QWidget *parent = nullptr);
    void setActive(bool active);

private:
    bool m_active;

protected:
    void paintEvent(QPaintEvent *event) override;
};

class Sidebar : public QWidget
{
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);

    void setUserInfo(const QString &name, const QString &role);
    void setActiveButton(int index);
    int activeIndex() const { return m_activeIndex; }

signals:
    void menuItemClicked(int index);
    void logoutClicked();

private:
    QVBoxLayout *m_layout;
    QLabel *m_logoLabel;
    QLabel *m_userNameLabel;
    QLabel *m_userRoleLabel;
    QList<SidebarButton*> m_buttons;
    QPushButton *m_logoutBtn;
    int m_activeIndex;

    void setupUI();
    void addMenuButton(const QString &text, const QString &iconChar);
};

#endif // SIDEBAR_H
