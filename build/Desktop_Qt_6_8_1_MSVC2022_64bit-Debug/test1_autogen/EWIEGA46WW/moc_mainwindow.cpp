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
    "sendModbusCommand",
    "",
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
    "on_btnScanPort_clicked",
    "on_btnOpenPort_clicked",
    "on_btnClearReceiveArea_clicked",
    "on_btnClearSendArea_clicked",
    "on_btnSend_clicked",
    "handleModbusData",
    "QList<double>",
    "resultdata",
    "readTimeInterval",
    "on_btnCanOpenDevice_clicked",
    "on_btnCanSend_clicked",
    "on_btnCanStart_clicked",
    "on_btnCanReset_clicked",
    "on_btnCanInit_clicked",
    "on_btnPagePlot_clicked",
    "on_btnPageData_clicked",
    "on_btnSaveData_clicked",
    "on_btnReadData_clicked",
    "on_startDAQButton_clicked",
    "on_stopDAQButton_clicked",
    "handleDAQData",
    "timeData",
    "QList<QList<double>>",
    "channelData",
    "handleDAQStatus",
    "isRunning",
    "message",
    "handleDAQError",
    "errorMessage",
    "on_filterEnabledCheckBox_stateChanged",
    "state",
    "on_btnECUScan_clicked",
    "on_btnECUStart_clicked",
    "handleECUData",
    "ECUData",
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
    "modbusData",
    "daqData",
    "ecuData",
    "updateDashboardData",
    "DataSnapshot",
    "snapshot",
    "setupDAQPlot",
    "updateLayout",
    "ECUPlotInit",
    "setupDash1Plot",
    "updateDash1Plot",
    "value",
    "processDataSnapshots",
    "setupMasterTimer",
    "on_btnStartAll_clicked"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN10MainWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      65,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,  404,    2, 0x06,    1 /* Public */,
       6,    5,  411,    2, 0x06,    5 /* Public */,
      12,    0,  422,    2, 0x06,   11 /* Public */,
      13,    0,  423,    2, 0x06,   12 /* Public */,
      14,    1,  424,    2, 0x06,   13 /* Public */,
      15,    0,  427,    2, 0x06,   15 /* Public */,
      16,    2,  428,    2, 0x06,   16 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
      19,    0,  433,    2, 0x08,   19 /* Private */,
      20,    0,  434,    2, 0x08,   20 /* Private */,
      21,    0,  435,    2, 0x08,   21 /* Private */,
      22,    0,  436,    2, 0x08,   22 /* Private */,
      23,    0,  437,    2, 0x08,   23 /* Private */,
      24,    2,  438,    2, 0x08,   24 /* Private */,
      28,    0,  443,    2, 0x08,   27 /* Private */,
      29,    0,  444,    2, 0x08,   28 /* Private */,
      30,    0,  445,    2, 0x08,   29 /* Private */,
      31,    0,  446,    2, 0x08,   30 /* Private */,
      32,    0,  447,    2, 0x08,   31 /* Private */,
      33,    0,  448,    2, 0x08,   32 /* Private */,
      34,    0,  449,    2, 0x08,   33 /* Private */,
      35,    0,  450,    2, 0x08,   34 /* Private */,
      36,    0,  451,    2, 0x08,   35 /* Private */,
      37,    0,  452,    2, 0x08,   36 /* Private */,
      38,    0,  453,    2, 0x08,   37 /* Private */,
      39,    2,  454,    2, 0x08,   38 /* Private */,
      43,    2,  459,    2, 0x08,   41 /* Private */,
      46,    1,  464,    2, 0x08,   44 /* Private */,
      48,    1,  467,    2, 0x08,   46 /* Private */,
      50,    0,  470,    2, 0x08,   48 /* Private */,
      51,    0,  471,    2, 0x08,   49 /* Private */,
      52,    1,  472,    2, 0x08,   50 /* Private */,
      54,    2,  475,    2, 0x08,   52 /* Private */,
      56,    1,  480,    2, 0x08,   55 /* Private */,
      57,    0,  483,    2, 0x08,   57 /* Private */,
      58,    1,  484,    2, 0x08,   58 /* Private */,
      60,    1,  487,    2, 0x08,   60 /* Private */,
      62,    2,  490,    2, 0x08,   62 /* Private */,
      64,    1,  495,    2, 0x08,   65 /* Private */,
      66,    1,  498,    2, 0x08,   67 /* Private */,
      67,    1,  501,    2, 0x08,   69 /* Private */,
      68,    0,  504,    2, 0x08,   71 /* Private */,
      69,    0,  505,    2, 0x08,   72 /* Private */,
      70,    0,  506,    2, 0x08,   73 /* Private */,
      71,    0,  507,    2, 0x08,   74 /* Private */,
      72,    0,  508,    2, 0x08,   75 /* Private */,
      73,    0,  509,    2, 0x08,   76 /* Private */,
      74,    0,  510,    2, 0x08,   77 /* Private */,
      75,    2,  511,    2, 0x08,   78 /* Private */,
      79,    2,  516,    2, 0x08,   81 /* Private */,
      80,    0,  521,    2, 0x08,   84 /* Private */,
      81,    1,  522,    2, 0x08,   85 /* Private */,
      83,    1,  525,    2, 0x08,   87 /* Private */,
      84,    0,  528,    2, 0x08,   89 /* Private */,
      85,    1,  529,    2, 0x08,   90 /* Private */,
      87,    0,  532,    2, 0x08,   92 /* Private */,
      88,    3,  533,    2, 0x08,   93 /* Private */,
      92,    2,  540,    2, 0x08,   97 /* Private */,
      95,    0,  545,    2, 0x08,  100 /* Private */,
      96,    0,  546,    2, 0x08,  101 /* Private */,
      97,    0,  547,    2, 0x08,  102 /* Private */,
      98,    0,  548,    2, 0x08,  103 /* Private */,
      99,    1,  549,    2, 0x08,  104 /* Private */,
     101,    0,  552,    2, 0x08,  106 /* Private */,
     102,    0,  553,    2, 0x08,  107 /* Private */,
     103,    0,  554,    2, 0x08,  108 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,    3,    4,    5,
    QMetaType::Void, QMetaType::QString, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    7,    8,    9,   10,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonObject, QMetaType::Int,   17,   18,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 25, QMetaType::LongLong,   26,   27,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 25, 0x80000000 | 41,   40,   42,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   44,   45,
    QMetaType::Void, QMetaType::QString,   47,
    QMetaType::Void, QMetaType::Int,   49,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 53,   17,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   55,   45,
    QMetaType::Void, QMetaType::QString,   47,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   59,
    QMetaType::Void, QMetaType::Int,   61,
    QMetaType::Void, QMetaType::Bool, QMetaType::QString,   63,   45,
    QMetaType::Void, QMetaType::QString,   65,
    QMetaType::Void, QMetaType::QString,   65,
    QMetaType::Void, QMetaType::QString,   45,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 77,   76,   78,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 77,   76,   78,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 82,   78,
    QMetaType::Void, 0x80000000 | 82,   78,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   86,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 25, 0x80000000 | 25, 0x80000000 | 53,   89,   90,   91,
    QMetaType::Void, 0x80000000 | 25, 0x80000000 | 93,   40,   94,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Double,  100,
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
        // method 'handleModbusData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVector<double>, std::false_type>,
        QtPrivate::TypeAndForceComplete<qint64, std::false_type>,
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
        // method 'on_startDAQButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_stopDAQButton_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleDAQData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<double> &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<QVector<double>> &, std::false_type>,
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
        // method 'handleECUData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const ECUData &, std::false_type>,
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
        // method 'updateDashboardData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QVector<double> &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const DataSnapshot &, std::false_type>,
        // method 'setupDAQPlot'
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
        // method 'processDataSnapshots'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'setupMasterTimer'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'on_btnStartAll_clicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->sendModbusCommand((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3]))); break;
        case 1: _t->sendModbusInfo((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[5]))); break;
        case 2: _t->closeModbusConnection(); break;
        case 3: _t->resetModbusTimer(); break;
        case 4: _t->openECUPort((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->closeECUPort(); break;
        case 6: _t->sendModbusResultToWebSocket((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 7: _t->on_btnScanPort_clicked(); break;
        case 8: _t->on_btnOpenPort_clicked(); break;
        case 9: _t->on_btnClearReceiveArea_clicked(); break;
        case 10: _t->on_btnClearSendArea_clicked(); break;
        case 11: _t->on_btnSend_clicked(); break;
        case 12: _t->handleModbusData((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<qint64>>(_a[2]))); break;
        case 13: _t->on_btnCanOpenDevice_clicked(); break;
        case 14: _t->on_btnCanSend_clicked(); break;
        case 15: _t->on_btnCanStart_clicked(); break;
        case 16: _t->on_btnCanReset_clicked(); break;
        case 17: _t->on_btnCanInit_clicked(); break;
        case 18: _t->on_btnPagePlot_clicked(); break;
        case 19: _t->on_btnPageData_clicked(); break;
        case 20: _t->on_btnSaveData_clicked(); break;
        case 21: _t->on_btnReadData_clicked(); break;
        case 22: _t->on_startDAQButton_clicked(); break;
        case 23: _t->on_stopDAQButton_clicked(); break;
        case 24: _t->handleDAQData((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<QList<double>>>>(_a[2]))); break;
        case 25: _t->handleDAQStatus((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 26: _t->handleDAQError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 27: _t->on_filterEnabledCheckBox_stateChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 28: _t->on_btnECUScan_clicked(); break;
        case 29: _t->on_btnECUStart_clicked(); break;
        case 30: _t->handleECUData((*reinterpret_cast< std::add_pointer_t<ECUData>>(_a[1]))); break;
        case 31: _t->handleECUStatus((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 32: _t->handleECUError((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 33: _t->on_btnPageInitial_clicked(); break;
        case 34: _t->enableInitialPage((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 35: _t->onRandomNumberGenerated((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 36: _t->handleWebSocketServerStarted((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QString>>(_a[2]))); break;
        case 37: _t->handleWebSocketClientConnected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 38: _t->handleWebSocketClientDisconnected((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 39: _t->handleWebSocketMessage((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 40: _t->testWebSocketConnection(); break;
        case 41: _t->on_btnWebSocketControl_clicked(); break;
        case 42: _t->on_btnWebSocketTest_clicked(); break;
        case 43: _t->onMainTimerTimeout(); break;
        case 44: _t->on_actionSetupInitial_triggered(); break;
        case 45: _t->on_actionLoadInitial_triggered(); break;
        case 46: _t->on_actionSaveInitial_triggered(); break;
        case 47: _t->handleDashboardSettingsChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QMap<QString,QVariant>>>(_a[2]))); break;
        case 48: _t->handleDashForceSettingsChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QMap<QString,QVariant>>>(_a[2]))); break;
        case 49: _t->applyDashboardMappings(); break;
        case 50: _t->saveDashboardMappings((*reinterpret_cast< std::add_pointer_t<QSettings&>>(_a[1]))); break;
        case 51: _t->loadDashboardMappings((*reinterpret_cast< std::add_pointer_t<QSettings&>>(_a[1]))); break;
        case 52: _t->initDefaultDashboardMappings(); break;
        case 53: _t->updateDashboardDAQChannels((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 54: _t->updateModbusChannels(); break;
        case 55: _t->updateDashboardByMapping((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<ECUData>>(_a[3]))); break;
        case 56: _t->updateDashboardData((*reinterpret_cast< std::add_pointer_t<QList<double>>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<DataSnapshot>>(_a[2]))); break;
        case 57: _t->setupDAQPlot(); break;
        case 58: _t->updateLayout(); break;
        case 59: _t->ECUPlotInit(); break;
        case 60: _t->setupDash1Plot(); break;
        case 61: _t->updateDash1Plot((*reinterpret_cast< std::add_pointer_t<double>>(_a[1]))); break;
        case 62: _t->processDataSnapshots(); break;
        case 63: _t->setupMasterTimer(); break;
        case 64: _t->on_btnStartAll_clicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 24:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<QList<double>> >(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 55:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<double> >(); break;
            }
            break;
        case 56:
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
            using _q_method_type = void (MainWindow::*)(int , int , int );
            if (_q_method_type _q_method = &MainWindow::sendModbusCommand; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)(QString , int , int , int , int );
            if (_q_method_type _q_method = &MainWindow::sendModbusInfo; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 1;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)();
            if (_q_method_type _q_method = &MainWindow::closeModbusConnection; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 2;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)();
            if (_q_method_type _q_method = &MainWindow::resetModbusTimer; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 3;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)(const QString & );
            if (_q_method_type _q_method = &MainWindow::openECUPort; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 4;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)();
            if (_q_method_type _q_method = &MainWindow::closeECUPort; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 5;
                return;
            }
        }
        {
            using _q_method_type = void (MainWindow::*)(const QJsonObject & , int );
            if (_q_method_type _q_method = &MainWindow::sendModbusResultToWebSocket; *reinterpret_cast<_q_method_type *>(_a[1]) == _q_method) {
                *result = 6;
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
        if (_id < 65)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 65;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 65)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 65;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::sendModbusCommand(int _t1, int _t2, int _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::sendModbusInfo(QString _t1, int _t2, int _t3, int _t4, int _t5)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t5))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MainWindow::closeModbusConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void MainWindow::resetModbusTimer()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void MainWindow::openECUPort(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void MainWindow::closeECUPort()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void MainWindow::sendModbusResultToWebSocket(const QJsonObject & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_WARNING_POP
