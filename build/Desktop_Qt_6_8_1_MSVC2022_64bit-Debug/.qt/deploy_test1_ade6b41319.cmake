include(C:/Users/80911/Documents/QT/vs_test/test1/build/Desktop_Qt_6_8_1_MSVC2022_64bit-Debug/.qt/QtDeploySupport.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/test1-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase;qtserialport;qtwebsockets")

qt6_deploy_runtime_dependencies(
    EXECUTABLE C:/Users/80911/Documents/QT/vs_test/test1/build/Desktop_Qt_6_8_1_MSVC2022_64bit-Debug/test1.exe
    GENERATE_QT_CONF
)
