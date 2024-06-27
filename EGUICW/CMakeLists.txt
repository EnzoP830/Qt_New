cmake_minimum_required(VERSION 3.16)
project(EGUICW LANGUAGES CXX)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/sql/books")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql Widgets)

qt_standard_project_setup()

qt_add_executable(EGUICW
    mainwindow.cpp mainwindow.h
    initdb.h
    main.cpp
    employeeWindow.h
    employeeWindow.cpp
    ticketWindow.h
    ticketWindow.cpp
    createEmployeeWindow.h
    createEmployeeWindow.cpp
    createTicketWindow.h
    createTicketWindow.cpp
    repairPartsWindow.h
    repairPartsWindow.cpp
    createRepairPartsWindow.h
    createRepairPartsWindow.cpp



)

set_target_properties(EGUICW PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(EGUICW PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    Qt6::Widgets
)
