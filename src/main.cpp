#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QLocale>
#include <QFont>
#include <QFile>
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application info
    app.setApplicationName("SmartPOS");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("SmartPOS");

    // Set UTF-8 encoding
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // Set RTL layout for Arabic
    app.setLayoutDirection(Qt::RightToLeft);

    // Set default font
    QFont defaultFont("Arial", 12);
    defaultFont.setStyleHint(QFont::SansSerif);
    app.setFont(defaultFont);

    // Load default dark theme
    QFile styleFile(":/styles/dark.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString stylesheet = styleFile.readAll();
        app.setStyleSheet(stylesheet);
        styleFile.close();
    }

    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
