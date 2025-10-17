cmake_minimum_required(VERSION 3.16)

project(UsbGuardGUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS DBus Widgets Qml Quick )

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appUsbGuardGUI
    main.cpp
)

qt_add_qml_module(appUsbGuardGUI
    URI UsbGuardGUI
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES TableModelBase.h TableModelBase.cpp
        SOURCES UsbGuardDevicesmodel.h UsbGuardDevicesmodel.cpp
        SOURCES dbususbproxy.h dbususbproxy.cpp
)

target_link_libraries(appUsbGuardGUI PRIVATE
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::DBus
)

include(GNUInstallDirs)
install(TARGETS appUsbGuardGUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
