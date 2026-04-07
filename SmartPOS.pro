QT       += core gui widgets sql network printsupport serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

TARGET = SmartPOS
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    src/main.cpp \
    src/core/database.cpp \
    src/core/firebase.cpp \
    src/core/hardwareid.cpp \
    src/core/thermalprinter.cpp \
    src/core/gdiprinter.cpp \
    src/core/barcodehandler.cpp \
    src/ui/mainwindow.cpp \
    src/ui/loginwidget.cpp \
    src/ui/activationwidget.cpp \
    src/ui/poswidget.cpp \
    src/ui/productswidget.cpp \
    src/ui/customerswidget.cpp \
    src/ui/reportswidget.cpp \
    src/ui/settingswidget.cpp \
    src/ui/userswidget.cpp \
    src/ui/widgets/sidebar.cpp

HEADERS += \
    src/models/product.h \
    src/models/category.h \
    src/models/customer.h \
    src/models/sale.h \
    src/models/user.h \
    src/core/database.h \
    src/core/firebase.h \
    src/core/hardwareid.h \
    src/core/thermalprinter.h \
    src/core/gdiprinter.h \
    src/core/barcodehandler.h \
    src/ui/mainwindow.h \
    src/ui/loginwidget.h \
    src/ui/activationwidget.h \
    src/ui/poswidget.h \
    src/ui/productswidget.h \
    src/ui/customerswidget.h \
    src/ui/reportswidget.h \
    src/ui/settingswidget.h \
    src/ui/userswidget.h \
    src/ui/widgets/sidebar.h

RESOURCES += \
    resources/resources.qrc

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
