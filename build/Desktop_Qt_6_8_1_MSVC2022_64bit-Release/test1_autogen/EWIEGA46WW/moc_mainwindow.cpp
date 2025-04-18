/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN10MainWindowE = QtMocHelpers::stringData(
    "MainWindow",
    "triggerProcessDataSnapshots",
    "",
    "sendModbusCommand",
    "serverAddress",
    "startAddress",
    "length",
    "sendModbusInfo",
    "portName",
    "baudRateIndex",
    "stopBitsIndex",
    "dataBitsIndex",
    "parityIndex",
    "closeModbusConnection",
    "resetModbusTimer",
    "openECUPort",
    "closeECUPort",
    "sendModbusResultToWebSocket",
    "data",
    "interval",
    "sendConfigCounts",
    "modbusCount",
    "daqCount",
    "createCalibrationMenu",
    "on_btnScanPort_clicked",
    "on_btnOpenPort_clicked",
    "on_btnClearReceiveArea_clicked",
    "on_btnClearSendArea_clicked",
    "on_btnSend_clicked",
    "handleSnapshotProcessed",
    "DataSnapshot",
    "snapshot",
    "snapshotCount",
    "on_btnCanOpenDevice_clicked",
    "on_btnCanSend_clicked",
    "on_btnCanStart_clicked",
    "on_btnCanReset_clicked",
    "on_btnCanInit_clicked",
    "on_btnPagePlot_clicked",
    "on_btnPageData_clicked",
    "on_btnSaveData_clicked",
    "on_btnReadData_clicked",
    "handleDAQStatus",
    "isRunning",
    "message",
    "handleDAQError",
    "errorMessage",
    "on_filterEnabledCheckBox_stateChanged",
    "state",
    "on_btnECUScan_clicked",
    "on_btnECUStart_clicked",
    "handleECUStatus",
    "connected",
    "handleECUError",
    "on_btnPageInitial_clicked",
    "enableInitialPage",
    "enable",
    "onRandomNumberGenerated",
    "number",
    "handleWebSocketServerStarted",
    "success",
    "handleWebSocketClientConnected",
    "clientInfo",
    "handleWebSocketClientDisconnected",
    "handleWebSocketMessage",
    "testWebSocketConnection",
    "on_btnWebSocketControl_clicked",
    "on_btnWebSocketTest_clicked",
    "onMainTimerTimeout",
    "on_actionSetupInitial_triggered",
    "on_actionLoadInitial_triggered",
    "on_actionSaveInitial_triggered",
    "handleDashboardSettingsChanged",
    "dashboardName",
    "QMap<QString,QVariant>",
    "settings",
    "handleDashForceSettingsChanged",
    "applyDashboardMappings",
    "saveDashboardMappings",
    "QSettings&",
    "loadDashboardMappings",
    "initDefaultDashboardMappings",
    "updateDashboardDAQChannels",
    "channelsStr",
    "updateModbusChannels",
    "updateDashboardByMapping",
    "QList<double>",
    "modbusData",
    "daqData",
    "ECUData",
    "ecuData",
    "updateDashboardData",
    "timeData",
    "setupDAQPlot",
    "on_startDAQButton_clicked",
    "on_stopDAQButton_clicked",
    "updateLayout",
    "ECUPlotInit",
    "setupDash1Plot",
    "updateDash1Plot",
    "value",
    "updateAllPlots",
    "on_btnStartAll_clicked",
    "switchPage",
    "on_actionCalibrateSensor_triggered"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN10MainWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      67,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  416,    2, 0x06,    1 /* Public */,
       3,    3,  417,    2, 0x06,    2 /* Public */,
       7,    5,  424,    2, 0x06,    6 /* Public */,
      13,    0,  435,    2, 0x06,   12 /* Public */,
      14,    0,  436,    2, 0x06,   13 /* Public */,
      15,    1,  437,    2, 0x06,   14 /* Public */,
      16,    0,  440,    2, 0x06,   16 /* Public */,
      17,    2,  441,    2, 0x06,   17 /* Public */,
      20,    2,  446,    2, 0x06,   20 /* Public */,
      23,    0,  451,    2, 0x06,   23 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      24,    0,  452,    2, 0x08,   24 /* Private */,
      25,    0,  453,    2, 0x08,   25 /* Private */,
      26,    0,  454,    2, 0x08,   26 /* Private */,
      27,    0,  455,    2, 0x08,   27 /* Private */,
      28,    0,  456,    2, 0x08,   28 /* Private */,
      29,    2,  457,    2, 0x08,   29 /* Private */,
      33,    0,  462,    2, 0x08,   32 /* Private */,
      34,    0,  463,    2, 0x08,   33 /* Private */,
      35,    0,  464,    2, 0x08,   34 /* Private */,
      36,    0,  465,    2, 0x08,   35 /* Private */,
      37,    0,  466,    2, 0x08,   36 /* Private */,
      38,    0,  467,    2, 0x08,   37 /* Private */,
      39,    0,  468,    2, 0x08,   38 /* Private */,
      40,    0,  469,    2, 0x08,   39 /* Private */,
      41,    0,  470,    2, 0x08,   40 /* Private */,
      42,    2,  471,    2, 0x08,   41 /* Private */,
      45,    1,  476,    2, 0x08,   44 /* Private */,
      47,    1,  479,    2, 0x08,   46 /* Private */,
      49,    0,  482,    2, 0x08,   48 /* Private */,
      50,    0,  483,    2, 0x08,   49 /* Private */,
      51,    2,  484,    2, 0x08,   50 /* Private */,
      53,    1,  489,    2, 0x08,   53 /* Private */,
      54,    0,  492,    2, 0x08,   55 /* Private */,
      55,    1,  493,    2, 0x08,   56 /* Private */,
      57,    1,  496,    2, 0x08,   58 /* Private */,
      59,    2,  499,    2, 0x08,   60 /* Private */,
      61,    1,  504,    2, 0x08,   63 /* Private */,
      63,    1,  507,    2, 0x08,   65 /* Private */,
      64,    1,  510,    2, 0x08,   67 /* Private */,
      65,    0,  513,    2, 0x08,   69 /* Private */,
      66,    0,  514,    2, 0x08,   70 /* Private */,
      67,    0,  515,    2, 0x08,   71 /* Private */,
      68,    0,  516,    2, 0x08,   72 /* Private */,
      69,    0,  517,    2, 0x08,   73 /* Private */,
      70,    0,  518,    2, 0x08,   74 /* Private */,
      71,    0,  519,    2, 0x08,   75 /* Private */,
      72,    2,  520,    2, 0x08,   76 /* Private */,
      76,    2,  525,    2, 0x08,   79 /* Private */,
      77,    0,  530,    2, 0x08,   82 /* Private */,
      78,    1,  531,    2, 0x08,   83 /* Private */,
      80,    1,  534,    2, 0x08,   85 /* Private */,
      81,    0,  537,    2, 0x08,   87 /* Private */,
      82,    1,  538,    2, 0x08,   88 /* Private */,
      84,    0,  541,    2, 0x08,   90 /* Private */,
      85,    4,  542,    2, 0x08,   91 /* Private */,
      91,    2,  551,    2, 0x08,   96 /* Private */,
      93,    0,  556,    2, 0x08,   99 /* Private */,
      94,    0,  557,    2, 0x08,  100 /* Private */,
      95,    0,  558,    2, 0x08,  101 /* Private */,
      96,    0,  559,    2, 0x08,  102 /* Private */,
      97,    0,  560,    2, 0x08,  103 /* Private */,
      98,    0,  561,    2, 0x08,  104 /* Private */,
      99,    1,  562,    2, 0x08,  105 /* Private */,
     101,    2,  565,    2, 0x08,  107 /* Private */,
     102,    0,  570,    2, 0x08,  110 /* Private */,
     103,    0,  571,    2, 0x08,  111 /* Private */,
     104,    0,  572,    2, 0x08,  112 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    4,    5,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    8,    9,   10,   11,   12,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonObject, QMetaType::Int,   18,   19,
    QMetaType::Void, QMetaType::Int, QMetaType::Int,   21,   22,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 30, QMetaType::Int,   31,   32,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   43,   44,
    QMetaType::Void, QMetaType::QString,   46,
    QMetaType::Void, QMetaType::Int,   48,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   52,   44,
    QMetaType::Void, QMetaType::QString,   46,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   56,
    QMetaType::Void, QMetaType::Int,   58,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   60,   44,
    QMetaType::Void, QMetaType::QString,   62,
    QMetaType::Void, QMetaType::QString,   62,
    QMetaType::Void, QMetaType::QString,   44,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 74,   73,   75,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 74,   73,   75,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 79,   75,
    QMetaType::Void, 0x80000000 | 79,   75,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   83,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 86, 0x80000000 | 86, 0x80000000 | 89, 0x80000000 | 30,   87,   88,   90,   31,
    QMetaType::Void, 0x80000000 | 86, 0x80000000 | 30,   92,   31,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,  100,
    QMetaType::Void, 0x80000000 | 30, QMetaType::Int,   31,   32,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ZN10MainWindowE.offsetsAndSizes,
    qt_meta_data_ZN10MainWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN10MainWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'triggerProcessDataSnapshots'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendModbusCommand'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'sendModbusInfo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'closeModbusConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'resetModbusTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openECUPort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'closeECUPort'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendModbusResultToWebSocket'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'sendConfigCounts'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'createCalibrationMenu'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnScanPort_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnOpenPort_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnClearReceiveArea_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnClearSendArea_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSend_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleSnapshotProcessed'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const DataSnapshot &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_btnCanOpenDevice_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnCanSend_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnCanStart_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnCanReset_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnCanInit_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnPagePlot_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnPageData_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnSaveData_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnReadData_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleDAQStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'handleDAQError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'on_filterEnabledCheckBox_stateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_btnECUScan_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnECUStart_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleECUStatus'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'handleECUError'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'on_btnPageInitial_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'enableInitialPage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'onRandomNumberGenerated'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'handleWebSocketServerStarted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'handleWebSocketClientConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'handleWebSocketClientDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'handleWebSocketMessage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'testWebSocketConnection'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnWebSocketControl_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnWebSocketTest_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onMainTimerTimeout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionSetupInitial_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionLoadInitial_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionSaveInitial_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleDashboardSettingsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QMap<QString,QVariant> &, std::false_type>,
        // method 'handleDashForceSettingsChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QMap<QString,QVariant> &, std::false_type>,
        // method 'applyDashboardMappings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'saveDashboardMappings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QSettings &, std::false_type>,
        // method 'loadDashboardMappings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QSettings &, std::false_type>,
        // method 'initDefaultDashboardMappings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateDashboardDAQChannels'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'updateModbusChannels'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateDashboardByMapping'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<double> &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<double> &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ECUData &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const DataSnapshot &, std::false_type>,
        // method 'updateDashboardData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<double> &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const DataSnapshot &, std::false_type>,
        // method 'setupDAQPlot'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_startDAQButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_stopDAQButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateLayout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'ECUPlotInit'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setupDash1Plot'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateDash1Plot'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        // method 'updateAllPlots'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const DataSnapshot &, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'on_btnStartAll_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'switchPage'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_actionCalibrateSensor_triggered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->triggerProcessDataSnapshots(); break;
        case 1: _t->sendModbusCommand((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 2: _t->sendModbusInfo((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[5]))); break;
        case 3: _t->closeModbusConnection(); break;
        case 4: _t->resetModbusTimer(); break;
        case 5: _t->openECUPort((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->closeECUPort(); break;
        case 7: _t->sendModbusResultToWebSocket((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 8: _t->sendConfigCounts((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 9: _t->createCalibrationMenu(); break;
        case 10: _t->on_btnScanPort_clicked(); break;
        case 11: _t->on_btnOpenPort_clicked(); break;
        case 12: _t->on_btnClearReceiveArea_clicked(); break;
        case 13: _t->on_btnClearSendArea_clicked(); break;
        case 14: _t->on_btnSend_clicked(); break;
        case 15: _t->handleSnapshotProcessed((*reinterpret_cast< std::add_pointer_t<DataSnapshot>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 16: _t->on_btnCanOpenDevice_clicked(); break;
        case 17: _t->on_btnCanSend_clicked(); break;
        case 18: _t->on_btnCanStart_clicked(); break;
        case 19: _t->on_btnCanReset_clicked(); break;
        case 20: _t->on_btnCanInit_clicked(); break;
        case 21: _t->on_btnPagePlot_clicked(); break;
        case 22: _t->on_btnPageData_clicked(); break;
        case 23: _t->on_btnSaveData_clicked(); break;
        case 24: _t->on_btnReadData_clicked(); break;
        case 25: _t->handleDAQStatus((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 26: _t->handleDAQError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 27: _t->on_filterEnabledCheckBox_stateChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 28: _t->on_btnECUScan_clicked(); break;
        case 29: _t->on_btnECUStart_clicked(); break;
        case 30: _t->handleECUStatus((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 31: _t->handleECUError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 32: _t->on_btnPageInitial_clicked(); break;
        case 33: _t->enableInitialPage((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 34: _t->onRandomNumberGenerated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 35: _t->handleWebSocketServerStarted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 36: _t->handleWebSocketClientConnected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 37: _t->handleWebSocketClientDisconnected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 38: _t->handleWebSocketMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 39: _t->testWebSocketConnection(); break;
        case 40: _t->on_btnWebSocketControl_clicked(); break;
        case 41: _t->on_btnWebSocketTest_clicked(); break;
        case 42: _t->onMainTimerTimeout(); break;
        case 43: _t->on_actionSetupInitial_triggered(); break;
        case 44: _t->on_actionLoadInitial_triggered(); break;
        case 45: _t->on_actionSaveInitial_triggered(); break;
        case 46: _t->handleDashboardSettingsChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QMap<QString,QVariant>>>(_a[2]))); break;
        case 47: _t->handleDashForceSettingsChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QMap<QString,QVariant>>>(_a[2]))); break;
        case 48: _t->applyDashboardMappings(); break;
        case 49: _t->saveDashboardMappings((*reinterpret_cast< std::add_pointer_t<QSettings&>>(_a[1]))); break;
        case 50: _t->loadDashboardMappings((*reinterpret_cast< std::add_pointer_t<QSettings&>>(_a[1]))); break;
        case 51: _t->initDefaultDashboardMappings(); break;
        case 52: _t->updateDashboardDAQChannels((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 53: _t->updateModbusChannels(); break;
        case 54: _t->updateDashboardByMapping((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<ECUData>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<DataSnapshot>>(_a[4]))); break;
        case 55: _t->updateDashboardData((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<DataSnapshot>>(_a[2]))); break;
        case 56: _t->setupDAQPlot(); break;
        case 57: _t->on_startDAQButton_clicked(); break;
        case 58: _t->on_stopDAQButton_clicked(); break;
        case 59: _t->updateLayout(); break;
        case 60: _t->ECUPlotInit(); break;
        case 61: _t->setupDash1Plot(); break;
        case 62: _t->updateDash1Plot((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 63: _t->updateAllPlots((*reinterpret_cast< std::add_pointer_t<DataSnapshot>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 64: _t->on_btnStartAll_clicked(); break;
        case 65: _t->switchPage(); break;
        case 66: _t->on_actionCalibrateSensor_triggered(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 54:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 55:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _q_method_type = void (MainWindow::*)();
            if (_q_method_type _q_method = &MainWindow::triggerProcessDataSnapshots; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)(int , int , int );
            if (_q_method_type _q_method = &MainWindow::sendModbusCommand; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)(QString , int , int , int , int );
            if (_q_method_type _q_method = &MainWindow::sendModbusInfo; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)();
            if (_q_method_type _q_method = &MainWindow::closeModbusConnection; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)();
            if (_q_method_type _q_method = &MainWindow::resetModbusTimer; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)(const QString & );
            if (_q_method_type _q_method = &MainWindow::openECUPort; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)();
            if (_q_method_type _q_method = &MainWindow::closeECUPort; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 6;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)(const QJsonObject & , int );
            if (_q_method_type _q_method = &MainWindow::sendModbusResultToWebSocket; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 7;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)(int , int );
            if (_q_method_type _q_method = &MainWindow::sendConfigCounts; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 8;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)();
            if (_q_method_type _q_method = &MainWindow::createCalibrationMenu; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 9;
                return;
            }
        }
    }
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN10MainWindowE.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 67)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 67;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 67)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 67;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::triggerProcessDataSnapshots()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MainWindow::sendModbusCommand(int _t1, int _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MainWindow::sendModbusInfo(QString _t1, int _t2, int _t3, int _t4, int _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void MainWindow::closeModbusConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void MainWindow::resetModbusTimer()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void MainWindow::openECUPort(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void MainWindow::closeECUPort()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void MainWindow::sendModbusResultToWebSocket(const QJsonObject & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void MainWindow::sendConfigCounts(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void MainWindow::createCalibrationMenu()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
QT_WARNING_POP
