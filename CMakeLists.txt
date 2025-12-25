cmake_minimum_required(VERSION 3.16)

project(Proyek_1_moduled VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_GENERATE_QMLLS_INI ON)
set(CMAKE_PREFIX_PATH "D:\\Program-Files\\Qt\\6.9.2\\mingw_64\\lib\\cmake" CACHE STRING "" FORCE)
set(QML_IMPORT_PATH "${CMAKE_BINARY_DIR}" CACHE STRING "" FORCE)
set(QML2_IMPORT_PATH "${CMAKE_BINARY_DIR}" CACHE STRING "" FORCE)

find_package(Qt6 REQUIRED COMPONENTS Quick Qml Sql)

qt_standard_project_setup(REQUIRES 6.8)


qt_add_executable(appProyek_1_moduled
    main.cpp
)

qt_add_qml_module(appProyek_1_moduled
    URI Proyek_1_moduled
    VERSION 1.0
    QML_FILES
        Main.qml
        components/qml/main/NavBar.qml
        components/qml/main/SideBar.qml
        components/qml/main/ContentContainer.qml
        components/qml/main/OpeningAnimation.qml
        components/qml/contents/HomePage.qml
        components/qml/contents/HomePageSections/Section1.qml
    SOURCES
        components/src/Backend.h
        components/src/Backend.cpp
        components/src/services/database/SqlDatabase.h
        components/src/services/database/SqlDatabase.cpp
        components/src/components/OpeningAnimation.h
        components/src/components/OpeningAnimation.cpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appProyek_1_moduled PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appProyek_1_moduled
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(appProyek_1_moduled
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/components/src
)

target_link_libraries(appProyek_1_moduled
    PRIVATE Qt6::Quick Qt6::Qml Qt6::Sql
)

include(GNUInstallDirs)
install(TARGETS appProyek_1_moduled
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
