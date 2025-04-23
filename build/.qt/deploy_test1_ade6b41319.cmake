include(C:/Users/80911/Documents/QT/vs_test/test1/build/.qt/QtDeploySupport.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/test1-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase;qtserialport;qtwebsockets;qtdeclarative;qtdeclarative;qtdeclarative")

qt6_deploy_runtime_dependencies(
    EXECUTABLE C:/Users/80911/Documents/QT/vs_test/test1/build/test1.exe
    GENERATE_QT_CONF
)
