cmake_minimum_required(VERSION 3.16)

project(Proyek_1_moduled VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appProyek_1_moduled
    main.cpp
)

qt_add_qml_module(appProyek_1_moduled
    URI Proyek_1_moduled
    VERSION 1.0
    QML_FILES
        Main.qml
        QML_FILES components/main/NavBar.qml
        QML_FILES components/main/SideBar.qml
        QML_FILES components/main/ContentContainer.qml
        QML_FILES components/contents/HomePage.qml
        QML_FILES components/contents/HomePageSections/Section1.qml
        RESOURCES key
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

target_link_libraries(appProyek_1_moduled
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appProyek_1_moduled
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
