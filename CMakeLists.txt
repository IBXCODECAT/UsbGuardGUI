cmake_minimum_required(VERSION 3.16)

project(UsbGuardGUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS DBus Widgets Qml Quick )

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appUsbGuardGUI
    main.cpp
)

# Use standard CMake commands to gather source files from subdirectories

# 1. Gather all C++ sources (.cpp files) from the 'Models' subdirectory
file(GLOB MODEL_SOURCES
    "Models/*.cpp"
)

# 2. Gather all C++ sources (.cpp files) from the 'src' subdirectory (or wherever your manager/proxy are)
file(GLOB SRC_SOURCES
    "src/*.cpp"
    "*.cpp" # For any .cpp files in the root directory (like TableModelBase.cpp if it's there)
)

# 3. Gather all headers (.h files) from both directories
file(GLOB ALL_HEADERS
    "Models/*.h"
    "src/*.h"
    "*.h" # For any .h files in the root directory
)

# 4. Combine all source files into a single variable
set(ALL_CPP_SOURCES ${MODEL_SOURCES} ${SRC_SOURCES})
set(ALL_QML_FILES Main.qml) # Assuming only one QML file

# 5. Use the variables in qt_add_qml_module
# Note: qt_add_qml_module automatically handles adding headers to the target,
# but it's often cleaner to manage headers in a separate target_sources command
# or simply ensure they are in the project structure.
qt_add_qml_module(appUsbGuardGUI
    URI UsbGuardGUI
    VERSION 1.0
    # Pass the QML files
    QML_FILES ${ALL_QML_FILES}

    # Pass the collected C++ source files
    SOURCES ${ALL_CPP_SOURCES}
)

# OPTIONAL: Explicitly add all header files to the target (good practice for IDEs)
target_sources(appUsbGuardGUI
    PUBLIC
        ${ALL_HEADERS}
)

# OPTIONAL: Ensure the compiler can find the headers in the subdirectories
target_include_directories(appUsbGuardGUI
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Models
        ${CMAKE_CURRENT_SOURCE_DIR}/src
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
