cmake_minimum_required(VERSION 3.5)

project(SmartPOS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt5 packages
find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
    Network
    PrintSupport
    SerialPort
)

# Source files
set(SOURCES
    src/main.cpp
    src/core/database.cpp
    src/core/firebase.cpp
    src/core/hardwareid.cpp
    src/core/thermalprinter.cpp
    src/core/gdiprinter.cpp
    src/core/barcodehandler.cpp
    src/ui/mainwindow.cpp
    src/ui/loginwidget.cpp
    src/ui/activationwidget.cpp
    src/ui/poswidget.cpp
    src/ui/productswidget.cpp
    src/ui/customerswidget.cpp
    src/ui/reportswidget.cpp
    src/ui/settingswidget.cpp
    src/ui/userswidget.cpp
    src/ui/widgets/sidebar.cpp
)

# Header files
set(HEADERS
    src/models/product.h
    src/models/category.h
    src/models/customer.h
    src/models/sale.h
    src/models/user.h
    src/core/database.h
    src/core/firebase.h
    src/core/hardwareid.h
    src/core/thermalprinter.h
    src/core/gdiprinter.h
    src/core/barcodehandler.h
    src/ui/mainwindow.h
    src/ui/loginwidget.h
    src/ui/activationwidget.h
    src/ui/poswidget.h
    src/ui/productswidget.h
    src/ui/customerswidget.h
    src/ui/reportswidget.h
    src/ui/settingswidget.h
    src/ui/userswidget.h
    src/ui/widgets/sidebar.h
)

# Resource files
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link Qt5 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Sql
    Qt5::Network
    Qt5::PrintSupport
    Qt5::SerialPort
)

# Windows-specific settings
if(WIN32)
    # Link Windows libraries for hardware ID
    target_link_libraries(${PROJECT_NAME} PRIVATE
        wbemuuid
        ole32
        oleaut32
        iphlpapi
    )
    # Set Windows subsystem (GUI app, no console window)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Compiler warnings
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_DEPRECATED_WARNINGS
)

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
