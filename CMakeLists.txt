cmake_minimum_required(VERSION 3.19)
project(test1 VERSION 1.0 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets SerialPort SerialBus PrintSupport WebSockets Network)

qt_standard_project_setup()

# 添加资源文件
qt_add_resources(QRC_FILES
    logo.qrc
)

# 强制重新生成MOC文件
set(CMAKE_AUTOMOC_RELAXED_MODE TRUE)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# 确保所有依赖的头文件都被添加到CMake源文件列表中
file(GLOB HEADERS *.h)
file(GLOB FORMS *.ui)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        modbusthread.cpp
        modbusthread.h
        plotthread.cpp
        plotthread.h
        qcustomplot.cpp
        qcustomplot.h
        canthread.cpp
        canthread.h
        daqthread.cpp
        daqthread.h
        ecuthread.cpp
        ecuthread.h
        dashboard.cpp
        dashboard.h
        dashboardcalculator.cpp
        dashboardcalculator.h
        websocketthread.h
        websocketthread.cpp
        ${QRC_FILES}
        ${ART_DAQ_LIB_PATH}
        ${HEADERS}
        ${FORMS}
)

qt_add_executable(test1
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    modbusthread.cpp
    modbusthread.h
    plotthread.cpp
    plotthread.h
    qcustomplot.cpp
    qcustomplot.h
    canthread.cpp
    canthread.h
    daqthread.cpp
    daqthread.h
    ecuthread.cpp
    ecuthread.h
    dashboard.cpp
    dashboard.h
    dashboardcalculator.cpp
    dashboardcalculator.h
    websocketthread.h
    websocketthread.cpp
    ControlCAN.dll
    ControlCAN.h
    ControlCAN.lib
    Art_DAQ.dll Art_DAQ.h Art_DAQ.lib Art_DAQ_64.dll Art_DAQ_64.lib
    ${QRC_FILES}
    ${HEADERS}
    ${FORMS}
)

target_include_directories(test1 PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(test1 PRIVATE ${CMAKE_SOURCE_DIR}/ControlCAN.lib)
target_link_libraries(test1 PRIVATE ${CMAKE_SOURCE_DIR}/Art_DAQ.lib)
add_custom_command(TARGET test1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/ControlCAN.dll
    $<TARGET_FILE_DIR:test1>)
add_custom_command(TARGET test1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/Art_DAQ.dll
    $<TARGET_FILE_DIR:test1>)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    add_custom_command(TARGET test1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/Art_DAQ_64.dll
        $<TARGET_FILE_DIR:test1>/Art_DAQ_64.dll)
endif()

target_link_libraries(test1
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::SerialPort
        Qt::SerialBus
        Qt::PrintSupport
        Qt::WebSockets
        Qt::Network
)

include(GNUInstallDirs)

install(TARGETS test1
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET test1
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# 添加HTML文件操作
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/websocket_client.html")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/websocket_client.html DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/websocket_client.html DESTINATION ${CMAKE_INSTALL_BINDIR})
    message(STATUS "WebSocket客户端HTML文件已复制到构建目录: ${CMAKE_CURRENT_BINARY_DIR}/websocket_client.html")
else()
    message(WARNING "WebSocket客户端HTML文件不存在: ${CMAKE_CURRENT_SOURCE_DIR}/websocket_client.html")
endif()

# 确保在Debug和Release目录中都有HTML文件
add_custom_command(TARGET test1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/websocket_client.html
    $<TARGET_FILE_DIR:test1>/websocket_client.html)

# 设置应用程序属性
set_target_properties(test1 PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
