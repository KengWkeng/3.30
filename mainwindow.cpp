﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGlobal>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkInterface>
#include <QMessageBox> // 添加QMessageBox头文件
#include <QSettings>
#include <QCoreApplication>
#include <QScrollBar> // 添加QScrollBar头文件
#include "dashboard.h"
#include "dashboardcalculator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , channelNum(7)
    , geometriesInitialized(false)
    , filterEnabled(false)  // 默认不启用滤波器
    , ecuIsConnected(false) // 默认ECU未连接
    , myScrollArea(nullptr) // 初始化滚动区域指针
    , myPlot(nullptr)       // 初始化图表指针为空
{
    ui->setupUi(this);
    
    // 初始状态：禁用初始页上的所有控件，等待用户点击通信初始化按钮
    enableInitialPage(false);
    
    // 记录初始大小
    initialSize = size();
    
    // 初始状态下只启用初始化按钮，其他功能按钮禁用
    ui->btnPagePlot->setEnabled(false);
    ui->btnPageData->setEnabled(false);
    ui->btnSaveData->setEnabled(false);
    ui->btnReadData->setEnabled(false);
    
    // 设置窗口标题
    setWindowTitle("工业数据采集与监控系统");
    
    // 设置默认状态栏信息
    statusBar()->showMessage("欢迎使用工业数据采集与监控系统，请点击通信初始化按钮加载配置", 5000);
    
    // 设置初始尺寸
    initialSize = QSize(1280, 800);
    resize(initialSize);
    setMinimumSize(1024, 768);  // 设置最小窗口尺寸
    
    // 手动设置主布局的拉伸因子
    QHBoxLayout* contentLayout = qobject_cast<QHBoxLayout*>(ui->centralwidget->layout()->itemAt(1)->layout());
    if (contentLayout) {
        contentLayout->setStretch(0, 1);  // 左侧导航区域
        contentLayout->setStretch(1, 10); // 中部仪表盘区域
        contentLayout->setStretch(2, 10); // 右侧功能区域
        
        // 设置布局间距
        contentLayout->setSpacing(8);
        contentLayout->setContentsMargins(8, 8, 8, 8);
    }
    
    
    // 拉力仪表盘
    ui->dashForce->setTitle("拉力");
    ui->dashForce->setUnit("N");
    ui->dashForce->setMinValue(0);
    ui->dashForce->setMaxValue(5000);
    ui->dashForce->setPrecision(0);
    ui->dashForce->setPointerStyle(PointerStyle_Triangle);
    ui->dashForce->setPointerColor(QColor(24, 189, 155));
    ui->dashForce->setValue(0);
    
    // 扭矩仪表盘
    ui->dashTorque->setTitle("扭矩");
    ui->dashTorque->setUnit("N·m");
    ui->dashTorque->setMinValue(0);
    ui->dashTorque->setMaxValue(500);
    ui->dashTorque->setPrecision(1);
    ui->dashTorque->setPointerStyle(PointerStyle_Indicator);
    ui->dashTorque->setPointerColor(QColor(217, 86, 86));
    ui->dashTorque->setValue(0);
    
    // 转速仪表盘
    ui->dashRPM->setTitle("转速");
    ui->dashRPM->setUnit("RPM");
    ui->dashRPM->setMinValue(0);
    ui->dashRPM->setMaxValue(12000);
    ui->dashRPM->setPrecision(0);
    ui->dashRPM->setPointerStyle(PointerStyle_Indicator);
    ui->dashRPM->setPointerColor(QColor(255, 100, 0));
    ui->dashRPM->setValue(0);
    
    // 推力仪表盘
    ui->dashThrust->setTitle("推力");
    ui->dashThrust->setUnit("N");
    ui->dashThrust->setMinValue(0);
    ui->dashThrust->setMaxValue(8000);
    ui->dashThrust->setPrecision(0);
    ui->dashThrust->setPointerStyle(PointerStyle_Triangle);
    ui->dashThrust->setPointerColor(QColor(0, 100, 200));
    ui->dashThrust->setValue(0);
    
    // 油耗仪表盘
    ui->dashFuelConsumption->setTitle("油耗");
    ui->dashFuelConsumption->setUnit("ml/min");
    ui->dashFuelConsumption->setMinValue(0);
    ui->dashFuelConsumption->setMaxValue(1000);
    ui->dashFuelConsumption->setPrecision(0);
    ui->dashFuelConsumption->setPointerStyle(PointerStyle_Circle);
    ui->dashFuelConsumption->setPointerColor(QColor(200, 180, 0));
    ui->dashFuelConsumption->setValue(0);
    
    // 火花塞下温度仪表盘
    ui->dashSparkPlugTemp->setTitle("火花塞温度");
    ui->dashSparkPlugTemp->setUnit("°C");
    ui->dashSparkPlugTemp->setMinValue(0);
    ui->dashSparkPlugTemp->setMaxValue(900);
    ui->dashSparkPlugTemp->setPrecision(0);
    ui->dashSparkPlugTemp->setPointerStyle(PointerStyle_Indicator);
    ui->dashSparkPlugTemp->setPointerColor(QColor(250, 50, 50));
    ui->dashSparkPlugTemp->setValue(0);
    
    // 功率仪表盘
    ui->dashPower->setTitle("功率");
    ui->dashPower->setUnit("kW");
    ui->dashPower->setMinValue(0);
    ui->dashPower->setMaxValue(100);
    ui->dashPower->setPrecision(1);
    ui->dashPower->setPointerStyle(PointerStyle_Triangle);
    ui->dashPower->setPointerColor(QColor(100, 50, 200));
    ui->dashPower->setValue(0);
    
    // 初始化LCD显示
    ui->lcdForce->setDigitCount(5);
    ui->lcdForce->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdForce->setStyleSheet("background-color: black; color: #24bd9b;");
    ui->lcdForce->display(0);
    
    ui->lcdTorque->setDigitCount(5);
    ui->lcdTorque->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdTorque->setStyleSheet("background-color: black; color: #d95656;");
    ui->lcdTorque->display(0);
    
    ui->lcdRPM->setDigitCount(5);
    ui->lcdRPM->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdRPM->setStyleSheet("background-color: black; color: #ff6400;");
    ui->lcdRPM->display(0);
    
    ui->lcdThrust->setDigitCount(5);
    ui->lcdThrust->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdThrust->setStyleSheet("background-color: black; color: #0064c8;");
    ui->lcdThrust->display(0);
    
    ui->lcdFuelConsumption->setDigitCount(5);
    ui->lcdFuelConsumption->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdFuelConsumption->setStyleSheet("background-color: black; color: #c8b400;");
    ui->lcdFuelConsumption->display(0);
    
    ui->lcdSparkPlugTemp->setDigitCount(5);
    ui->lcdSparkPlugTemp->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdSparkPlugTemp->setStyleSheet("background-color: black; color: #fa3232;");
    ui->lcdSparkPlugTemp->display(0);
    
    ui->lcdPower->setDigitCount(5);
    ui->lcdPower->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdPower->setStyleSheet("background-color: black; color: #6432c8;");
    ui->lcdPower->display(0);
    
    // 设置logo图片
    QPixmap pixmap(":/logo.png");
    if (!pixmap.isNull()) {
        pixmap = pixmap.scaled(ui->labelLogo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labelLogo->setScaledContents(true);
        ui->labelLogo->setPixmap(pixmap);
        // qDebug() << "Logo加载成功，大小:" << pixmap.size();
    } else {
        qDebug() << "Logo加载失败，请检查资源文件";
    }
    
    // 初始化UI元素
    if (ui->lineServerAddress != nullptr) {
        ui->lineServerAddress->setText("1");
    } else if (ui->lineServerAddress != nullptr) {
        // 如果UI还没更新，使用原有控件
        ui->lineServerAddress->setText("1");
    }
    
    ui->lineSegAddress->setText("0");
    ui->lineSegNum->setText("1");
    
    // 设置第一个导航按钮为默认选中状态
    ui->btnPageInitial->setChecked(true);
    
    // 设置StackedWidget默认显示第一页
    ui->stackedWidget->setCurrentIndex(0);
    
    // 设置按钮初始状态
    ui->btnPageInitial->setEnabled(true);
    ui->btnReadData->setEnabled(true);
    ui->btnPagePlot->setEnabled(false);
    ui->btnPageData->setEnabled(false);
    ui->btnSaveData->setEnabled(false);
    
    // 默认禁用读取按钮，直到串口成功打开
    ui->btnSend->setEnabled(false);

    //创建子线程
    SubThread_Modbus= new QThread;
    SubThread_Can = new QThread;
    daqThread = new QThread; // 创建DAQ子线程
    ecuThread = new QThread; // 创建ECU子线程
    
    // 创建仪表盘计算线程（现在DashboardCalculator直接继承自QThread）
    dashboardCalculator = new DashboardCalculator(this);
    // 启动线程会在需要计算时自动进行
    
    //子线程对象
    mbTh = new modbusThread;
    mbTh->moveToThread(SubThread_Modbus);

    canTh = new CANThread;
    canTh->moveToThread(SubThread_Can);
    
    // 创建DAQ子线程对象
    daqTh = new DAQThread;
    daqTh->moveToThread(daqThread);
    
    // 创建ECU子线程对象
    ecuTh = new ECUThread;
    ecuTh->moveToThread(ecuThread);
    
    // 创建WebSocket对象 - 直接在主线程中使用，不要移动到其他线程
    wsTh = new WebSocketThread(this);
    
    // 添加调试信息
    qDebug() << "WebSocketThread对象创建成功";
    connect(wsTh, &WebSocketThread::serverStarted, this, [=](bool success, QString message) {
        qDebug() << "WebSocket服务器启动状态:" << (success ? "成功" : "失败") << message;
    });
    
    // 初始化滤波器相关变量
    realTimer = new QElapsedTimer();
    realTimer->start();
    lastTimestamp = 0;
    filteredValues.clear();



    // 初始化主定时器
    masterTimer = new QElapsedTimer();
    masterTimer->start();
    
    // 创建统一主定时器，取代之前的snapshotTimer和ModbusTimer
    mainTimer = new QTimer(this);
    mainTimer->setInterval(10); // 设置基本间隔为10ms
    connect(mainTimer, &QTimer::timeout, this, &MainWindow::onMainTimerTimeout);
    mainTimer->start();
    
    // 初始化定时器相关变量
    modbusReadRequested = false;
    modbusReading = false;
    lastModbusReadTime = 0;
    lastSnapshotTime = 0;
    lastPlotUpdateTime = 0;
    
    // 初始化当前数据快照
    currentSnapshot = DataSnapshot();
    
    // 添加WebSocket消息接收处理连接
    connect(wsTh, &WebSocketThread::messageReceived, this, &MainWindow::handleWebSocketMessage);
    
    // 添加WebSocket错误信号处理
    connect(wsTh, &WebSocketThread::serverError, this, [=](QString errorMsg) {
        ui->plainReceive->appendPlainText("WebSocket错误: " + errorMsg);
        qDebug() << "WebSocket错误: " + errorMsg;
    });
    

    connect(ui->btnTestWebSocket, &QPushButton::clicked, this, &MainWindow::testWebSocketConnection);

    //状态栏指针
    sBar=statusBar();
    
    // 非常重要：先保存myPlot指针，然后再初始化它
    // 这样可以确保myplotInit函数能够完全替换原始控件
    myPlot = ui->modbusCustomPlot;
    
    // 初始化ModBus图表
    myplotInit(myPlot);
    
    // 初始化其他变量
    realPlotTime=0;
    
    // 初始化表格模型为空指针，这样setupXXXTable函数会正确创建它们
    tableModel = nullptr;
    daqDataModel = nullptr;
    ecuDataModel = nullptr;
    
    // 初始化DAQ相关参数
    daqIsAcquiring = false;
    daqNumChannels = 0;
    daqSampleRate = 10000;

    
    //发送modbus命令
    connect(this,&MainWindow::sendModbusCommand,mbTh,&modbusThread::getModbusResult);

    // 添加串口参数设置的信号与槽连接
    connect(this,&MainWindow::sendModbusInfo,mbTh,&modbusThread::setModbusPortInfo);
    
    // 添加关闭modbus连接的信号与槽连接
    connect(this,&MainWindow::closeModbusConnection,mbTh,&modbusThread::closeModbusConnection);
    
    // 连接初始化信号，确保modbusClient在子线程中创建
    connect(SubThread_Modbus, &QThread::started, mbTh, &modbusThread::initModbusClient);
    
    // 接收串口状态变化的信号 - 使用子线程的信号，而不是直接在lambda中操作UI
    connect(mbTh, &modbusThread::modbusConnectionStatus, this, [=](bool connected, QString message) {
        ui->plainReceive->setPlainText(message);
        ui->radioButton->setChecked(connected);
        if (connected) {
            ui->btnOpenPort->setText("关闭串口");
            // 启用读取按钮
            ui->btnSend->setEnabled(true);
        } else {
            ui->btnOpenPort->setText("打开串口");
            // 禁用读取按钮
            ui->btnSend->setEnabled(false);
            // 如果正在读取数据，设置为"读取"状态
            if (ui->btnSend->text() == "结束") {
                // 停止定时器
                // ModbusTimer->stop();
                ui->btnSend->setText("读取");
            }
        }
    });

    // 修改Modbus数据处理方式：
    // 1. MainWindow仍然接收数据用于UI显示
    connect(mbTh, &modbusThread::sendModbusResult, this, &MainWindow::showModbusResult);
    
    // 2. WebSocketThread直接接收Modbus数据，处理并转发
    connect(mbTh, &modbusThread::sendModbusResult, wsTh, &WebSocketThread::handleModbusRawData);
    
    // // 连接重置计时器的信号与槽
    // connect(this, &MainWindow::resetModbusTimer, mbTh, &modbusThread::resetTimer);

    // 连接DAQ信号与槽
    connect(daqTh, &DAQThread::dataReady, this, &MainWindow::handleDAQData);
    connect(daqTh, &DAQThread::acquisitionStatus, this, &MainWindow::handleDAQStatus);
    connect(daqTh, &DAQThread::error, this, &MainWindow::handleDAQError);

    // 连接ECU信号与槽
    connect(ecuTh, &ECUThread::ecuDataReady, this, &MainWindow::handleECUData);
    connect(ecuTh, &ECUThread::ecuConnectionStatus, this, &MainWindow::handleECUStatus);
    connect(ecuTh, &ECUThread::ecuError, this, &MainWindow::handleECUError);
    connect(this, &MainWindow::openECUPort, ecuTh, &ECUThread::openECUPort);
    connect(this, &MainWindow::closeECUPort, ecuTh, &ECUThread::closeECUPort);
    
    // 连接初始化信号
    connect(ecuThread, &QThread::started, ecuTh, &ECUThread::initSerialPort);

    // 连接WebSocket信号与槽
    connect(this, &MainWindow::sendModbusResultToWebSocket, wsTh, &WebSocketThread::handleModbusData);
    connect(wsTh, &WebSocketThread::serverStarted, this, &MainWindow::handleWebSocketServerStarted);
    connect(wsTh, &WebSocketThread::clientConnected, this, &MainWindow::handleWebSocketClientConnected);
    connect(wsTh, &WebSocketThread::clientDisconnected, this, &MainWindow::handleWebSocketClientDisconnected);
    
    // 启动子线程
    SubThread_Modbus->start();
    daqThread->start();
    ecuThread->start(); // 启动ECU线程
    
    // WebSocket不使用单独线程，直接在主线程启动服务器
    // wsTh->startServer(8080); // 移除自动启动代码，保持默认关闭状态
    qDebug() << "WebSocket服务器默认处于关闭状态，请使用按钮手动启动";
    
    //连接清理线程槽函数
    connect(SubThread_Modbus,&QThread::finished,mbTh,&modbusThread::deleteLater);
    connect(daqThread, &QThread::finished, daqTh, &DAQThread::deleteLater);
    connect(ecuThread, &QThread::finished, ecuTh, &ECUThread::deleteLater); // 清理ECU线程
    
    // WebSocket在主线程，不需要特殊的清理逻辑
    
    ui->lineTimeLoop->setText("1000");
    
    // 设置DAQ界面
    setupDAQPlot();
    
    // 在UI完全初始化后再设置表格
    // 避免在构造函数中过早初始化表格
    QTimer::singleShot(0, this, [this]() {
        setupDAQTable();
        setupECUTable();
    });

    // 添加滤波器控件的信号连接
    connect(ui->filterEnabledCheckBox, &QCheckBox::stateChanged, 
            this, &MainWindow::on_filterEnabledCheckBox_stateChanged);

    // 记录初始窗口大小
    initialSize = size();

    // 添加按钮的逻辑控制 - 修改这部分代码
    connect(ui->btnPageInitial, &QPushButton::clicked, this, &MainWindow::on_btnPageInitial_clicked);
    
    // 手动创建菜单动作并连接它们
    QMenu *initMenu = new QMenu("初始化", this);
    menuBar()->addMenu(initMenu);
    
    QAction *actionSetupInitial = new QAction("设置初始化", this);
    QAction *actionLoadInitial = new QAction("读取初始化文件", this);
    QAction *actionSaveInitial = new QAction("保存初始化文件", this);
    
    initMenu->addAction(actionSetupInitial);
    initMenu->addAction(actionLoadInitial);
    initMenu->addAction(actionSaveInitial);
    
    // 连接菜单动作信号
    connect(actionSetupInitial, &QAction::triggered, this, &MainWindow::on_actionSetupInitial_triggered);
    connect(actionLoadInitial, &QAction::triggered, this, &MainWindow::on_actionLoadInitial_triggered);
    connect(actionSaveInitial, &QAction::triggered, this, &MainWindow::on_actionSaveInitial_triggered);
    
    // 连接Dashboard双击信号
    QList<Dashboard*> dashboards = this->findChildren<Dashboard*>();
    for (Dashboard* dashboard : dashboards) {
        connect(dashboard, &Dashboard::dashboardDoubleClicked, this, [=]() {
            qDebug() << "Dashboard双击信号已接收: " << dashboard->objectName();
        });
    }
    
    connect(ui->btnReadData, &QPushButton::clicked, this, [=]() {
        // 读取数据点击后，除btnSaveData外所有按钮启用
        ui->btnPageInitial->setEnabled(true);
        ui->btnPagePlot->setEnabled(true);
        ui->btnPageData->setEnabled(true);
        ui->btnSaveData->setEnabled(false);
    });


    
    // 添加定时器信号槽连接
    connect(mainTimer, &QTimer::timeout, [=]() {
        // 确保只在"结束"状态下发送命令（即正在循环读取状态）
        if (ui->btnSend->text() == "结束") {
            emit sendModbusCommand(ui->lineServerAddress->text().toInt(),
                                  ui->lineSegAddress->text().toInt(),
                                  ui->lineSegNum->text().toInt());
        }
    });
    
    // 初始化筛选器列表
    filteredValues.clear();
    
    // 初始化复选框内容
    on_btnScanPort_clicked();
    on_btnECUScan_clicked();
    
    // 连接所有仪表盘的设置变更信号
    QList<Dashboard*> allDashboards = getAllDashboards();
    for (Dashboard* dashboard : allDashboards) {
        connect(dashboard, &Dashboard::dashboardSettingsChanged, 
                this, &MainWindow::handleDashboardSettingsChanged);
        qDebug() << "Connected dashboard:" << dashboard->objectName();
    }
    
    // 特别为dashForce添加设置变更信号连接，更新dash1plot
    // 断开任何可能已存在的连接
    disconnect(ui->dashForce, &Dashboard::dashboardSettingsChanged, this, nullptr);
    
    // 重新连接信号，使用专用处理函数
    // 使用成员函数指针而非lambda表达式，避免潜在问题
    connect(ui->dashForce, &Dashboard::dashboardSettingsChanged, 
            this, &MainWindow::handleDashForceSettingsChanged);
    
    // 初始化默认的仪表盘映射关系
    initDefaultDashboardMappings();
    
    // 初始化仪表盘映射
    // 先尝试从保存的设置中加载，如果没有则使用默认设置
    QString settingsFile = QCoreApplication::applicationDirPath() + "/dashboard_settings.ini";
    QSettings settings(settingsFile, QSettings::IniFormat);
    
    // 检查文件是否存在并包含仪表盘映射
    if (QFile::exists(settingsFile) && settings.childGroups().contains("DashboardMappings")) {
        qDebug() << "从文件加载仪表盘映射: " << settingsFile;
        loadDashboardMappings(settings);
    } else {
        qDebug() << "使用默认仪表盘映射";
        initDefaultDashboardMappings();
    }

    // 初始化监控表格
    initMonitorTable();

    // 连接DAQ通道编辑框的信号
    connect(ui->channelsEdit, &QLineEdit::textChanged, this, &MainWindow::updateDashboardDAQChannels);
    
    // 连接Modbus寄存器地址和数量的信号
    connect(ui->lineSegAddress, &QLineEdit::textChanged, this, &MainWindow::updateModbusChannels);
    connect(ui->lineSegNum, &QLineEdit::textChanged, this, &MainWindow::updateModbusChannels);
    
    // 初始化仪表盘DAQ通道设置
    updateDashboardDAQChannels(ui->channelsEdit->text());
    // 初始化Modbus通道设置
    updateModbusChannels();
    
    // 初始化界面后，为所有仪表盘设置计算器并初始化dash1plot
    QTimer::singleShot(500, this, [this]() {
        try {
            // 设置仪表盘计算器
        QList<Dashboard*> dashboards = getAllDashboards();
        foreach(Dashboard* dashboard, dashboards) {
            dashboard->setCalculator(dashboardCalculator);
            }
            
            // 安全地初始化dash1plot绘图
            dashPlotTimeCounter = 0;
            dashForceValueLabel = nullptr;
            dashForceArrowLabel = nullptr;
            dashForceGraph = nullptr;
            
            if (ui->dash1plot && ui->dashForce) {
                setupDash1Plot();
                qDebug() << "dash1plot初始化完成";
            } else {
                qDebug() << "无法初始化dash1plot：控件不存在";
            }
        } catch (const std::exception &e) {
            qDebug() << "初始化仪表盘或dash1plot时出错:" << e.what();
        } catch (...) {
            qDebug() << "初始化仪表盘或dash1plot时出现未知错误";
        }
    });
    


    // 初始化数据快照相关成员
    masterTimer = new QElapsedTimer();
    masterTimer->start();

    // 初始化当前数据快照
    currentSnapshot = DataSnapshot();

}

MainWindow::~MainWindow()
{
    // 保存仪表盘设置到配置文件
    QString settingsFile = QCoreApplication::applicationDirPath() + "/dashboard_settings.ini";
    QSettings settings(settingsFile, QSettings::IniFormat);
    saveDashboardMappings(settings);
    qDebug() << "已保存仪表盘设置到:" << settingsFile;
    
    // 停止主定时器
    if (mainTimer) {
        mainTimer->stop();
        delete mainTimer;
        mainTimer = nullptr;
    }
    
    delete ui;
    //析构时结束子线程
    SubThread_Modbus->quit();
    SubThread_Modbus->wait();
    SubThread_Modbus->deleteLater();
    
    daqThread->quit();
    daqThread->wait();
    daqThread->deleteLater();
    
    ecuThread->quit();
    ecuThread->wait();
    ecuThread->deleteLater(); // 清理ECU线程
    
    // 清理滤波器相关资源
    if (realTimer) {
        delete realTimer;
        realTimer = nullptr;
    }
    
    // 清理主计时器资源
    if (masterTimer) {
        delete masterTimer;
        masterTimer = nullptr;
    }

    // 停止WebSocket服务器
    if (wsTh) {
        wsTh->stopServer();
        delete wsTh;
        wsTh = nullptr;
    }
    
    // 停止并清理计算线程
    if (dashboardCalculator) {
        dashboardCalculator->stopThread();
        // 等待线程完成
        dashboardCalculator->wait();
        delete dashboardCalculator;
    }
}

// DAQ相关实现
void MainWindow::setupDAQPlot()
{
    // 获取DAQ图表的组容器和布局
    if (!ui->groupBox_6 || !ui->verticalLayout_7) {
        qDebug() << "错误: 无法找到DAQ图表容器或布局";
        return;
    }
    
    // 清空现有布局内容
    QLayoutItem *item;
    while ((item = ui->verticalLayout_7->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
    
    // 创建滚动区域和容器部件
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *plotsLayout = new QVBoxLayout(scrollWidget);
    plotsLayout->setSpacing(10); // 设置图表间距
    scrollArea->setWidget(scrollWidget);
    
    // 添加滚动区域到布局
    ui->verticalLayout_7->addWidget(scrollArea);
    
    qDebug() << "DAQ图表区域初始化完成";
}

void MainWindow::updateDAQPlot(const QVector<double> &timeData, const DataSnapshot &snapshot)
{
    // 如果快照中无有效DAQ数据，则不更新
    if (!snapshot.daqValid || snapshot.daqData.isEmpty() || !snapshot.daqRunning) {
        return;
    }
    
    // 确保必要的组件存在
    if (!ui->groupBox_6 || !ui->verticalLayout_7 || !masterTimer) {
        qDebug() << "错误: 更新DAQ图表失败 - 缺少必要组件";
        return;
    }
    
    try {
    // 获取滚动区域和其内容
    QScrollArea *scrollArea = qobject_cast<QScrollArea*>(ui->verticalLayout_7->itemAt(0)->widget());
    if (!scrollArea) {
        qDebug() << "错误: 无法找到DAQ滚动区域";
        return;
    }
    
    QWidget *scrollWidget = scrollArea->widget();
    if (!scrollWidget) {
        qDebug() << "错误: 无法找到DAQ滚动区域的内容部件";
        return;
    }
    
    QVBoxLayout *plotsLayout = qobject_cast<QVBoxLayout*>(scrollWidget->layout());
    if (!plotsLayout) {
        qDebug() << "错误: 无法找到DAQ图表的布局";
        return;
    }
        
        // 获取通道数
        int numChannels = snapshot.daqData.size();
        if (numChannels <= 0) {
            return;
        }
    
    // 创建颜色列表
    QStringList colorNames = {"blue", "red", "green", "magenta", "cyan", "darkGreen", "darkRed", "darkBlue"};
    
        // 初始化图表(如果通道数发生变化)
        if (plotsLayout->count() != numChannels) {
            // 清空现有布局内容
            QLayoutItem *item;
            while ((item = plotsLayout->takeAt(0)) != nullptr) {
                if (item->widget()) {
                    delete item->widget();
                }
                delete item;
            }
            
            // 为每个通道创建图表
            for (int i = 0; i < numChannels; ++i) {
                // 创建图表容器，以支持标签和游标
                QWidget *plotContainer = new QWidget();
                plotContainer->setObjectName(QString("daqPlotContainer_%1").arg(i));
                QHBoxLayout *containerLayout = new QHBoxLayout(plotContainer);
                containerLayout->setContentsMargins(2, 2, 2, 2);
                
                // 创建图表
                QCustomPlot *plot = new QCustomPlot();
                plot->setObjectName(QString("daqPlot_%1").arg(i));
                plot->setMinimumHeight(100);
                plot->setInteraction(QCP::iRangeDrag, true);
                plot->setInteraction(QCP::iRangeZoom, true);
                
                // 设置图表样式
                plot->setBackground(QBrush(QColor(240, 240, 240)));
                
                // 设置时间轴
                plot->xAxis->setLabel("时间(秒)");
                plot->xAxis->setLabelFont(QFont("Arial", 9));
                
                // 设置数据轴
                plot->yAxis->setLabel(QString("通道 %1").arg(i));
                plot->yAxis->setLabelFont(QFont("Arial", 9));
                
                // 禁用图例
                plot->legend->setVisible(false);
                
                // 添加通道曲线
                plot->addGraph();
                
                // 设置曲线样式
                QPen pen;
                pen.setColor(QColor(colorNames[i % colorNames.size()]));
                pen.setWidth(1);
                plot->graph(0)->setPen(pen);
                
                // 启用抗锯齿
                plot->graph(0)->setAntialiased(true);
                
                // 创建数值标签
                QLabel *valueLabel = new QLabel("0.000", plotContainer);
                valueLabel->setObjectName(QString("daqValueLabel_%1").arg(i));
                valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
                valueLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 10pt;").arg(colorNames[i % colorNames.size()]));
                valueLabel->setFixedWidth(60);
                
                // 创建游标箭头
                QLabel *arrowLabel = new QLabel("←", plotContainer);
                arrowLabel->setObjectName(QString("daqArrowLabel_%1").arg(i));
            arrowLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 18px;").arg(colorNames[i % colorNames.size()]));
            arrowLabel->setAlignment(Qt::AlignCenter);
                arrowLabel->setFixedSize(20, 20);
                arrowLabel->move(plot->width() - 30, plot->height() / 2);
            
                // 将图表添加到容器
            containerLayout->addWidget(plot);
            
                // 将容器添加到布局
            plotsLayout->addWidget(plotContainer);
            }
        }
        
        // 使用快照的时间戳
        double currentTime = snapshot.timestamp;
        
        // 设置时间窗口和显示范围
        double maxTimeWindow = 30.0; // 30秒数据窗口
        double startTime = currentTime - maxTimeWindow;
        double endTime = currentTime;
        
        // 获取或创建时间数据
        QVector<double> daqTimeWindow;
        
        // 将快照时间点添加到时间窗口
        // 如果队列中存在快照，则使用队列中的时间点
        if (!snapshotQueue.isEmpty()) {
            // 从队列中提取时间点
            for (int i = 0; i < snapshotQueue.size(); ++i) {
                const DataSnapshot &snap = snapshotQueue.at(i);
                double snapTime = snap.timestamp;
                
                // 只添加在时间窗口内的点
                if (snapTime >= startTime && snapTime <= endTime) {
                    daqTimeWindow.append(snapTime);
                }
            }
        }
        
        // 如果没有足够的时间点，则使用当前时间点
        if (daqTimeWindow.isEmpty()) {
            daqTimeWindow.append(currentTime);
        }
        
        // 为每个通道更新图表
        for (int i = 0; i < numChannels && i < plotsLayout->count(); ++i) {
            // 获取图表容器和控件
        QWidget *plotContainer = plotsLayout->itemAt(i)->widget();
        if (!plotContainer) continue;
        
        QCustomPlot *plot = plotContainer->findChild<QCustomPlot*>(QString("daqPlot_%1").arg(i));
        QLabel *valueLabel = plotContainer->findChild<QLabel*>(QString("daqValueLabel_%1").arg(i));
        QLabel *arrowLabel = plotContainer->findChild<QLabel*>(QString("daqArrowLabel_%1").arg(i));
        
        if (!plot) continue;
        
            // 收集此通道的数据
            QVector<double> channelData;
            
            // 使用直接从快照队列中提取的数据
            if (!snapshotQueue.isEmpty()) {
                // 创建与时间点对应的数据点
                channelData.resize(daqTimeWindow.size());
                
                // 填充数据点
                for (int j = 0; j < snapshotQueue.size(); ++j) {
                    const DataSnapshot &snap = snapshotQueue.at(j);
                    double snapTime = snap.timestamp;
                    
                    // 找到时间点在daqTimeWindow中的索引
                    int timeIndex = daqTimeWindow.indexOf(snapTime);
                    
                    // 如果找到时间点，并且快照中有此通道的数据
                    if (timeIndex >= 0 && snap.daqValid && i < snap.daqData.size()) {
                        // 从一维数组中直接获取值
                        channelData[timeIndex] = snap.daqData[i];
                    }
                }
            }
            
            // 如果直接从队列中没有足够的数据，使用当前快照
            if (channelData.isEmpty() && i < snapshot.daqData.size()) {
                // 从一维数组中直接获取当前值
                channelData.append(snapshot.daqData[i]);
                daqTimeWindow = timeData; // 使用传入的时间数据
            }
            
            // 确保有数据可显示
            if (!channelData.isEmpty() && channelData.size() == daqTimeWindow.size()) {
                // 设置图表数据
                plot->graph(0)->setData(daqTimeWindow, channelData, true);
                
                // 设置X轴范围，显示固定时间窗口的数据
                plot->xAxis->setRange(startTime, endTime);
                
                // 更新值标签
                if (valueLabel && !channelData.isEmpty()) {
                    double latestValue = channelData.last();
                    valueLabel->setText(QString::number(latestValue, 'f', 3));
                }
                
                // 更新游标位置
                if (arrowLabel && !channelData.isEmpty()) {
                    double latestValue = channelData.last();
                    double pixelY = plot->yAxis->coordToPixel(latestValue);
                    pixelY = qBound(0.0, pixelY, (double)plot->height());
                    arrowLabel->move(plot->width() - 30, pixelY - arrowLabel->height()/2);
                }
                
                // 自动调整Y轴范围
                if (!channelData.isEmpty()) {
                    double yMin = *std::min_element(channelData.begin(), channelData.end());
                    double yMax = *std::max_element(channelData.begin(), channelData.end());
                    
                    // 确保范围不会太小
                    double range = yMax - yMin;
                    if (range < 0.1) range = 0.1;
                    
                    // 设置Y轴范围，添加10%的边距
                    plot->yAxis->setRange(yMin - range * 0.1, yMax + range * 0.1);
                }
                
                // 立即重绘图表
                plot->replot();
            }
        }
    } catch (const std::exception& e) {
        qDebug() << "更新DAQ图表时出错:" << e.what();
    } catch (...) {
        qDebug() << "更新DAQ图表时发生未知错误";
    }
}

void MainWindow::setupDAQTable()
{
    /*
    // 初始化数据模型
    if (daqDataModel) {
        delete daqDataModel;
    }
    
    // 创建新模型
    daqDataModel = new QStandardItemModel(this);
    
    // 设置行标签 - 不依赖于UI元素的具体值
    QStringList rowLabels;
    rowLabels << "时间(秒)";
    
    // DAQ通道通常有固定数量，这里使用2个通道作为默认值
    // 后续在真正添加数据时会根据实际通道数调整
    rowLabels << "通道 0" << "通道 1";
    
    daqDataModel->setVerticalHeaderLabels(rowLabels);
    
    // 设置表格模型
    ui->daqDataTableView->setModel(daqDataModel);
    
    // 调整表格外观
    ui->daqDataTableView->horizontalHeader()->setVisible(false); // 隐藏列头
    ui->daqDataTableView->verticalHeader()->setDefaultSectionSize(30); // 行高
    ui->daqDataTableView->horizontalHeader()->setDefaultSectionSize(60); // 设置默认列宽
    ui->daqDataTableView->setAlternatingRowColors(true); // 交替行颜色
    ui->daqDataTableView->setSelectionMode(QAbstractItemView::SingleSelection); // 单选模式
    
    // 设置表格为只读
    ui->daqDataTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    */
}

void MainWindow::updateDAQTable(const QVector<double> &timeData, const QVector<QVector<double>> &channelData, int numChannels)
{
    /*
    // 检查数据有效性
    if (numChannels <= 0 || timeData.isEmpty() || channelData.isEmpty()) {
        return;
    }
    
    // 如果表格模型为空，则初始化它
    if (!daqDataModel) {
        setupDAQTable();
    }
    
    // 检查并调整行数是否与通道数匹配
    if (daqDataModel->rowCount() != numChannels + 1) {
        // 保留当前列数据
        int currentColumns = daqDataModel->columnCount();
        
        // 重新设置行数
        daqDataModel->setRowCount(numChannels + 1);
        
        // 设置行标签
        QStringList rowLabels;
        rowLabels << "时间(秒)";
        for (int i = 0; i < numChannels; ++i) {
            rowLabels << QString("通道 %1").arg(i);
        }
        daqDataModel->setVerticalHeaderLabels(rowLabels);
    }
    
    // 获取最新的数据点
    int lastIndex = timeData.size() - 1;
    if (lastIndex < 0) {
        return;
    }
    
    // 获取当前列数
    int currentColumn = daqDataModel->columnCount();
    
    // 优化：一次性预分配列，避免多次单独的setItem调用
    QList<QStandardItem*> columnItems;
    columnItems.reserve(numChannels + 1);
    
    // 添加时间值（移除背景色设置）
    QStandardItem *timeItem = new QStandardItem(QString::number(timeData[lastIndex], 'f', 3));
    columnItems.append(timeItem);
    
    // 添加各通道数据（移除背景色设置）
    for (int i = 0; i < numChannels; ++i) {
        if (i < channelData.size() && lastIndex < channelData[i].size()) {
            QStandardItem *dataItem = new QStandardItem(QString::number(channelData[i][lastIndex], 'f', 3));
            columnItems.append(dataItem);
        } else {
            QStandardItem *emptyItem = new QStandardItem("N/A");
            columnItems.append(emptyItem);
        }
    }
    
    // 一次性添加新列
    daqDataModel->insertColumn(currentColumn, columnItems);
    
    // 如果超出最大列数，删除最旧的列
    const int maxColumns = 100; // 限制为100列，防止表格过大
    if (currentColumn > maxColumns) {
        daqDataModel->removeColumn(0);
    }
    
    // 优化：减少滚动频率，避免频繁UI更新
    static int scrollCounter = 0;
    if (++scrollCounter % 5 == 0) { // 每5次更新才滚动一次
        if (daqDataModel->columnCount() > 0) {
            ui->daqDataTableView->scrollTo(daqDataModel->index(0, daqDataModel->columnCount() - 1), QAbstractItemView::EnsureVisible);
        }
        scrollCounter = 0;
    }
    */
}

void MainWindow::handleDAQData(const QVector<double> &timeData, const QVector<QVector<double>> &channelData)
{
    // 检查数据有效性
    if (timeData.isEmpty() || channelData.isEmpty() || !daqIsAcquiring) {
        return;
    }
    
    try {
        // 如果主时间戳未初始化，则不处理数据
        if (!masterTimer) {
            qDebug() << "警告: 主时间戳未初始化，无法处理DAQ数据";
            return;
        }
        
        // 计算相对于主时间戳的时间
        static qint64 baseTimeMs = masterTimer->elapsed(); // 记录第一次数据到达时的时间
        
        // 获取实际通道数量
        int numChannels = channelData.size();
        
        // 初始化数据缓冲区（如果尚未初始化）
        if (daqTimeData.isEmpty() || daqChannelData.isEmpty() || daqChannelData.size() != numChannels) {
            daqTimeData.clear();
            daqChannelData.clear();
            daqChannelData.resize(numChannels);
            daqNumChannels = numChannels;
        }
        
        // 限制缓冲区大小常量 - 显著减小为1万个数据点
        const int maxDataPoints = 10000; // 减小为1万个数据点，足够显示而不会占用过多内存
        
        // 计算保留的数据点数量，确保足够空间添加新数据
        int newPointsCount = timeData.size();
        int currentSize = daqTimeData.size();
        
        // 预先检查并清理数据 - 如果添加新数据后会超出最大限制，先删除旧数据腾出空间
        if (currentSize + newPointsCount > maxDataPoints) {
            int pointsToRemove = (currentSize + newPointsCount) - maxDataPoints;
            
            // 从缓冲区开头删除多余数据点
            if (pointsToRemove > 0 && pointsToRemove <= daqTimeData.size()) {
                daqTimeData.remove(0, pointsToRemove);
                
                for (int ch = 0; ch < daqChannelData.size(); ++ch) {
                    if (daqChannelData[ch].size() > pointsToRemove) {
                        daqChannelData[ch].remove(0, pointsToRemove);
                    }
                }
            }
        }
        
        // 将新数据添加到缓冲区
        for (int i = 0; i < timeData.size(); ++i) {
            // 使用相对于主时间戳的时间（用于时间同步）
            double relTime = ((masterTimer->elapsed() - baseTimeMs) / 1000.0) + timeData[i] - timeData.first();
            daqTimeData.append(relTime);
            
            // 添加每个通道的数据
            for (int ch = 0; ch < numChannels && ch < channelData.size(); ++ch) {
                if (i < channelData[ch].size()) {
                    daqChannelData[ch].append(channelData[ch][i]);
                } else {
                    // 如果没有数据，填充最后一个值或0
                    double lastValue = daqChannelData[ch].isEmpty() ? 0.0 : daqChannelData[ch].last();
                    daqChannelData[ch].append(lastValue);
                }
            }
        }
        
        // 更新快照中的DAQ数据状态
        if (!daqChannelData.isEmpty() && !daqChannelData[0].isEmpty()) {
            currentSnapshot.daqValid = true;
            
            // 修改：使用一维数组保存每个通道的最新数据点
            currentSnapshot.daqData.resize(daqNumChannels);
            
            // 添加每个通道的最新数据点
            for (int ch = 0; ch < daqNumChannels && ch < daqChannelData.size(); ++ch) {
                if (!daqChannelData[ch].isEmpty()) {
                    // 直接保存最新的数据点
                    currentSnapshot.daqData[ch] = daqChannelData[ch].last();
                } else {
                    currentSnapshot.daqData[ch] = 0.0;
                }
            }
        }

    } catch (const std::exception& e) {
        qDebug() << "处理DAQ数据时出错:" << e.what();
    } catch (...) {
        qDebug() << "处理DAQ数据时发生未知错误";
    }
}

void MainWindow::handleDAQStatus(bool isRunning, QString message)
{
    daqIsAcquiring = isRunning;
    
    // 如果停止采集，清除数据快照中的DAQ数据有效标志
    if (!isRunning) {
        currentSnapshot.daqValid = false;
    }
    
    // 更新UI状态
    ui->startDAQButton->setEnabled(!isRunning);
    ui->stopDAQButton->setEnabled(isRunning);
    ui->deviceNameEdit->setEnabled(!isRunning);
    ui->channelsEdit->setEnabled(!isRunning);
    ui->sampleRateEdit->setEnabled(!isRunning);
    
    // 显示状态信息
    statusBar()->showMessage(message, 3000);
    
    if (isRunning) {
        qDebug() << "DAQ采集已启动，正在等待数据...";
    } else {
        qDebug() << "DAQ采集已停止";
    }
}

void MainWindow::handleDAQError(QString errorMessage)
{
    // 显示错误信息
    QMessageBox::critical(this, "数据采集错误", errorMessage);
}

void MainWindow::on_startDAQButton_clicked()
{
    // 获取参数
    QString deviceName = ui->deviceNameEdit->text().trimmed();
    QString channelStr = ui->channelsEdit->text().trimmed();
    
    // 添加日志输出，方便调试
    qDebug() << "开始数据采集: 设备=" << deviceName << ", 通道=" << channelStr;
    
    bool ok;
    double sampleRate = ui->sampleRateEdit->text().toDouble(&ok);
    if (!ok || sampleRate <= 0) {
        QMessageBox::warning(this, "错误", "采样率必须是正数");
        return;
    }
    
    daqSampleRate = sampleRate;
    
    // 解析通道
    QStringList parts = channelStr.split("/", Qt::SkipEmptyParts);
    int numChannels = parts.size();
    
    if (numChannels == 0) {
        QMessageBox::warning(this, "错误", "请至少指定一个通道");
        return;
    }
    
    // 提前禁用UI控件，防止用户重复点击
    ui->startDAQButton->setEnabled(false);
    ui->deviceNameEdit->setEnabled(false);
    ui->channelsEdit->setEnabled(false);
    ui->sampleRateEdit->setEnabled(false);
    ui->stopDAQButton->setEnabled(true);
    
    // 清空数据缓冲区，准备新的采集
    daqTimeData.clear();
    daqChannelData.clear();
    
    // 先初始化图表，避免采集开始后再创建UI元素
    updateDAQPlot();
    
    // 使用QTimer延迟初始化DAQ任务，让UI有时间更新
    QTimer::singleShot(100, this, [=]() {
        // 初始化DAQ任务
        daqTh->initDAQ(deviceName, channelStr, sampleRate, 1000);
        
        // 使用QTimer确保initDAQ完成后再开始采集
        QTimer::singleShot(50, this, [=]() {
            // 开始采集
            daqTh->startAcquisition();
        });
    });
    
    // 显示提示信息
    statusBar()->showMessage("正在启动DAQ采集...", 3000);
}

void MainWindow::on_stopDAQButton_clicked()
{
    daqTh->stopAcquisition();
}

// void MainWindow::on_btnPageDaq_clicked()
// {
//     switchPage();
// }

//切换页面
void MainWindow::switchPage(){
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    
    // 重置所有导航按钮的选中状态
    ui->btnPageInitial->setChecked(false);
    //ui->btnPageCan->setChecked(false);
    ui->btnPagePlot->setChecked(false);
    ui->btnPageData->setChecked(false);
    //ui->btnPageDaq->setChecked(false);
    
    // 设置当前点击的按钮为选中状态
    if (button) {
        button->setChecked(true);
    }
    
    // 切换到对应页面
    if(button==ui->btnPageInitial)
        ui->stackedWidget->setCurrentIndex(0);
    else if(button==ui->btnPagePlot)
        ui->stackedWidget->setCurrentIndex(1);
    else if(button==ui->btnPageData)
        ui->stackedWidget->setCurrentIndex(2);
    int i = 0;
    ui->stackedWidget->widget(i);
}


//扫描串口
void MainWindow::on_btnScanPort_clicked()
{
     ui->comboPort->clear();

     foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
     {
         ui->comboPort->addItem(info.portName());
     }

}


//设置串口信息并打开串口
void MainWindow::on_btnOpenPort_clicked()
{
    // 判断当前是打开还是关闭串口的操作
    if (ui->btnOpenPort->text() == "打开串口")
    {
        // 发送串口参数到子线程
        emit sendModbusInfo(ui->comboPort->currentText(),
                            ui->comboBaudRate->currentIndex(),
                            ui->comboStopBits->currentIndex(),
                            ui->comboDataBits->currentIndex(),
                            ui->comboParity->currentIndex()
                            );

        // 更新界面状态
        ui->plainReceive->setPlainText("正在连接串口...");
        ui->btnOpenPort->setText("关闭串口");
    }
    else
    {
        // 检查是否正在读取数据，如果是，先停止读取
        if (ui->btnSend->text() == "结束") {
            // 改变按钮文字为"读取"
            ui->btnSend->setText("读取");
            ui->plainReceive->appendPlainText("数据采集已停止");
        }
        
        // 通过信号在子线程中关闭串口，而不是直接调用
        emit closeModbusConnection();
        
    }
}




//清空接收区
void MainWindow::on_btnClearReceiveArea_clicked()
{
    ui->plainReceive->clear();
}



//清空发送区
void MainWindow::on_btnClearSendArea_clicked()
{
    ui->lineServerAddress->setText("1");
    ui->comboAction->setCurrentText("3");      //设置命令
    ui->lineSegAddress->setText("0");
    ui->lineSegNum->setText("1");
}


//发送modbus指令参数
void MainWindow::on_btnSend_clicked()
{
    //qDebug() << "on_btnSend_clicked from thread:" << QThread::currentThread();
    
    // 检查串口是否已连接，如果未连接，提示用户并返回
    if (!ui->radioButton->isChecked()) {
        QMessageBox::warning(this, "错误", "串口未连接，请先打开串口！");
        return;
    }
    
    // 检查按钮当前状态
    if (ui->btnSend->text() == "读取") {
        // 当前状态为"读取"，要开始数据采集
        
        // 获取寄存器数量
        int registerCount = ui->lineSegNum->text().toInt();
        if (registerCount < 1) {
            ui->plainReceive->appendPlainText("错误：寄存器数量必须大于0");
            return;
        }
        
        // 重置滤波器状态
        filteredValues.clear();
        lastTimestamp = 0;
        realTimer->restart();

        
        // 读取滤波器状态并更新UI
        filterEnabled = ui->filterEnabledCheckBox->isChecked();
        qDebug() << "滤波器状态: " << (filterEnabled ? "开启" : "关闭") 
                 << "，时间常数: " << ui->lineTimeLoop->text().toDouble() << "ms";
        
        // 设置Modbus读取标志，之后的读取将由主定时器触发
        modbusReadRequested = true;
        
        // 计算下一个整百+50的时间点，等待第一次触发
        qint64 currentTime = masterTimer->elapsed();
        qint64 nextReadTime = ((currentTime / 100) * 100) + 150; // 下一个整百+50
        qDebug() << "Modbus读取将在下一个整百+50毫秒时间点触发: " << nextReadTime << "ms";
        
        // 改变按钮文字为"结束"
        ui->btnSend->setText("结束");
        ui->plainReceive->appendPlainText("开始数据采集...");
        
        // 添加滤波状态信息
        if (filterEnabled) {
            ui->plainReceive->appendPlainText(QString("已启用低通滤波，时间常数: %1 ms").arg(ui->lineTimeLoop->text()));
        } else {
            ui->plainReceive->appendPlainText("低通滤波未启用，显示原始数据");
        }
    } else {
        // 当前状态为"结束"，要停止数据采集
        
        // 清除Modbus读取标志
        modbusReadRequested = false;
        modbusReading = false;
        
        // 改变按钮文字为"读取"
        ui->btnSend->setText("读取");
        ui->plainReceive->appendPlainText("数据采集已停止");
    }
}





//初始化图表
void MainWindow::myplotInit(QCustomPlot *customPlot)
{
    // 清理和初始化图表容器
    graphs.clear();
    myPlots.clear();
    
    // 获取包含Modbus图表的容器
    QWidget *container = ui->groupBox_4;
    if (!container) {
        qDebug() << "错误: 无法找到Modbus图表容器";
            return;
        }
    
    // 获取容器的布局
    QVBoxLayout *containerLayout = qobject_cast<QVBoxLayout*>(ui->groupBox_4->layout());
    if (!containerLayout) {
        containerLayout = new QVBoxLayout(container);
        containerLayout->setContentsMargins(5, 5, 5, 5);
        containerLayout->setSpacing(5);
    } else {
            // 清空现有布局内容
            QLayoutItem *item;
        while ((item = containerLayout->takeAt(0)) != nullptr) {
                if (item->widget()) {
                    delete item->widget();
                }
                delete item;
        }
            }
            
            // 创建滚动区域
    QScrollArea *scrollArea = new QScrollArea(container);
            scrollArea->setWidgetResizable(true);
            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            
            QWidget *scrollWidget = new QWidget();
            QVBoxLayout *plotsLayout = new QVBoxLayout(scrollWidget);
            plotsLayout->setSpacing(10); // 设置图表间距
            scrollArea->setWidget(scrollWidget);
            
    // 添加滚动区域到容器布局
    containerLayout->addWidget(scrollArea);
            
            // 保存滚动区域引用
            myScrollArea = scrollArea;
    
    // 检查channelNum是否有效
    if (channelNum <= 0) {
        qDebug() << "警告: 通道数量无效，使用默认值";
        channelNum = 3; // 默认创建3个图表
    }
    
    // 创建颜色列表
    QStringList colorNames = {"blue", "red", "green", "magenta", "cyan", "darkGreen", "darkRed", "darkBlue"};
    
    // 为每个通道创建图表
    for (int i = 0; i < channelNum; ++i) {
        // 创建图表容器
        QWidget *plotContainer = new QWidget(scrollWidget);
        QHBoxLayout *plotLayout = new QHBoxLayout(plotContainer);
        plotLayout->setContentsMargins(5, 5, 5, 5);
        
        // 创建图表
        QCustomPlot *plot = new QCustomPlot(plotContainer);
        plot->setMinimumHeight(150);
        
        // 设置图表属性
        plot->xAxis->setLabel("时间 (秒)");
        plot->yAxis->setLabel(QString("通道 %1").arg(i));
        
        // 添加图形
        plot->addGraph();
        plot->graph(0)->setPen(QPen(QColor(colorNames[i % colorNames.size()])));
        
        // 设置交互
        plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        
        // 性能优化
        plot->setNoAntialiasingOnDrag(true);
        plot->setNotAntialiasedElements(QCP::aeAll);
        
        // 创建数值标签容器
        QWidget *valueContainer = new QWidget(plotContainer);
        valueContainer->setFixedWidth(60);
        QVBoxLayout *valueLayout = new QVBoxLayout(valueContainer);
        valueLayout->setContentsMargins(5, 0, 5, 0);
        
        // 创建数值标签
        QLabel *valueLabel = new QLabel("0.00", valueContainer);
        valueLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 14px;").arg(colorNames[i % colorNames.size()]));
        valueLabel->setAlignment(Qt::AlignCenter);
        
        // 创建箭头标签 - 初始时使用左箭头
        QLabel *arrowLabel = new QLabel("←", plotContainer);
        arrowLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 18px;")
                                 .arg(colorNames[i % colorNames.size()]));
        arrowLabel->setAlignment(Qt::AlignCenter);
        arrowLabel->show();
        
        // 将标签添加到值容器
        valueLayout->addStretch();
        valueLayout->addWidget(valueLabel);
        valueLayout->addStretch();
        
        // 将组件添加到容器
        plotLayout->addWidget(plot);
        plotLayout->addWidget(valueContainer);
        
        // 设置箭头和值标签的初始位置
        // 箭头放在右侧边线上
        arrowLabel->setParent(plotContainer);
        arrowLabel->move(plot->width() - 30, plot->height()/2 - arrowLabel->height()/2);
        
        // 值标签固定在右上角，不随箭头移动
        valueLabel->setParent(plotContainer);
        valueLabel->move(plot->width() - 60, 10);
        valueLabel->setFixedSize(50, 20);
        
        // 保存对图表的引用
        myPlots.append(plot);
        graphs.append(plot->graph(0));
        
        // 将容器添加到滚动区域布局
        plotsLayout->addWidget(plotContainer);
    }
    
    // 设置图表初始范围
    for (QCustomPlot *plot : myPlots) {
        plot->xAxis->setRange(0, 5);
        plot->yAxis->setRange(0, 5);
        plot->replot();
    }
    
    qDebug() << "Modbus图表初始化完成，创建了" << channelNum << "个通道的图表";
    
    // 保存主图表引用
    myPlot = myPlots.isEmpty() ? nullptr : myPlots.first();
}

//获取modbus结果
void MainWindow::showModbusResult(QVector<double> resultdata, qint64 readTimeInterval)
{
    // 如果没有主计时器，则不处理
    if (!masterTimer) {
        qDebug() << "警告: 主计时器未初始化，无法处理Modbus数据";
        return;
    }
    
    try {
        // 检查数据是否有效
        if (resultdata.isEmpty()) {
            qDebug() << "警告: 收到空的Modbus数据";
            return;
        }
        
        qDebug() << "收到Modbus数据: " << resultdata;
        
        // 确保modbusData已经初始化为正确大小
        if (modbusData.size() != resultdata.size()) {
            modbusData.resize(resultdata.size());
            for (int i = 0; i < resultdata.size(); i++) {
                modbusData[i].clear();
            }
            modbusNumRegs = resultdata.size();
            
            // 确保图表也是正确的数量
            if (myPlots.size() != resultdata.size()) {
                channelNum = resultdata.size();
                myplotInit(myPlot);
            }
        }
        
        // 获取当前时间作为X轴值
        double currentTime = masterTimer->elapsed() / 1000.0; // 转换为秒
        
        // 将新数据添加到数据缓冲区并更新图表
        for (int i = 0; i < resultdata.size() && i < myPlots.size(); i++) {
            // 添加数据到存储
            modbusData[i].append(resultdata[i]);
            
            // 限制数据点数量，避免内存占用过多
            while (modbusData[i].size() > 10000) {
                modbusData[i].removeFirst();
            }
            
            // 更新图表
            if (i < graphs.size() && graphs[i]) {
                // 添加数据点到图表
                graphs[i]->addData(currentTime, resultdata[i]);
                
                // 找到对应的值标签和箭头标签
                if (i < myPlots.size() && myPlots[i]) {
                    QCustomPlot *plot = myPlots[i];
                    
                    // 更新箭头位置
                    QList<QLabel*> labels = plot->parentWidget()->findChildren<QLabel*>();
                    if (labels.size() >= 2) {
                        // 假设第一个是值标签，第二个是箭头标签
                        QLabel *valueLabel = labels.at(0);
                        QLabel *arrowLabel = labels.at(1);
                        
                        // 更新值
                        valueLabel->setText(QString::number(resultdata[i], 'f', 2));
                        
                        // 更新箭头位置
                        double yCoord = plot->yAxis->coordToPixel(resultdata[i]);
                        arrowLabel->move(plot->width() - 30, qRound(yCoord) - arrowLabel->height() / 2);
                    }
                    
                    // 设置X轴范围为最近60秒
                    double xMin = qMax(0.0, currentTime - 60.0);
                    plot->xAxis->setRange(xMin, currentTime);
                    
                    // 自动设置Y轴范围
                    if (graphs[i]->dataCount() > 0) {
                        // 查找可见范围内的最大最小值
                        double yMin = resultdata[i];
                        double yMax = resultdata[i];
                        
                        // 遍历最近添加的数据点
                        // 由于QCustomPlot数据结构的限制，使用简单的方法查找可见范围内的数据
                        const int visiblePointCount = qMin(graphs[i]->dataCount(), 600); // 最多查看最近600个点
                        for (int j = 0; j < visiblePointCount; j++) {
                            double key = graphs[i]->data()->at(graphs[i]->dataCount() - 1 - j)->key;
                            double value = graphs[i]->data()->at(graphs[i]->dataCount() - 1 - j)->value;
                            
                            if (key >= xMin && key <= currentTime) {
                                yMin = qMin(yMin, value);
                                yMax = qMax(yMax, value);
                            }
                        }
                        
                        // 添加一些边距
                        double range = yMax - yMin;
                        if (range < 0.1) range = 0.1; // 防止范围太小
                        yMin -= range * 0.1;
                        yMax += range * 0.1;
                        
                        // 设置Y轴范围
                        plot->yAxis->setRange(yMin, yMax);
        } else {
                        // 如果没有数据点，设置默认范围
                        plot->yAxis->setRange(0, 5);
                    }
                    
                    // 重绘图表
                    plot->replot(QCustomPlot::rpQueuedReplot);
                }
            }
        }
        
        // 标记Modbus数据有效
        modbusDataValid = true;
        
        // 更新数据快照
        currentSnapshot.modbusValid = true;
        currentSnapshot.modbusData = resultdata;
        
        // 直接在UI上显示最新值（用于调试）
        if (ui->plainReceive) {
            QString debugText = "Modbus最新值:\n";
            for (int i = 0; i < resultdata.size(); i++) {
                debugText += QString("通道 %1: %2\n").arg(i).arg(resultdata[i], 0, 'f', 2);
            }
            ui->plainReceive->setPlainText(debugText);
        }
        
    } catch (const std::exception& e) {
        qDebug() << "处理Modbus数据时出错: " << e.what();
    } catch (...) {
        qDebug() << "处理Modbus数据时发生未知错误";
    }
}

/*————————————————————————————————————————————————————CanBus————————————————————————————————————————————————————————————————————————*/

//打开can设备
void MainWindow::on_btnCanOpenDevice_clicked()
{
    if(ui->btnCanOpenDevice->text() == tr("打开设备"))
    {
        UINT baundRate = 0;
        if(ui->comboCanBaud->currentText().indexOf("Kbps") != -1)
            baundRate = ui->comboCanBaud->currentText().remove("Kbps").toUInt();
        else
            baundRate = QVariant(ui->comboCanBaud->currentText().remove("Mbps").toFloat()).toUInt();
        bool dev = canTh->openDevice(4,//QVariant(ui->comboBox->currentIndex()).toUInt(),
                                         QVariant(ui->comboCanID->currentIndex()).toUInt(),
                                         baundRate);
        if(dev == true)
        {
            ui->comboBox->setEnabled(false);
            ui->comboCanID->setEnabled(false);
            ui->comboCanBaud->setEnabled(false);
            ui->btnCanInit->setEnabled(true);
            ui->btnCanOpenDevice->setText(tr("关闭设备"));
        }
        else
            QMessageBox::warning(this,"警告","打开设备失败！");
    }
    else if(ui->btnCanOpenDevice->text() == tr("关闭设备"))
    {
        ui->comboBox->setEnabled(true);
        ui->comboCanID->setEnabled(true);
        ui->comboCanBaud->setEnabled(true);
        ui->comboChannel->setEnabled(true);
        ui->btnCanSend->setEnabled(false);
        ui->btnCanInit->setEnabled(false);
        ui->btnCanStart->setEnabled(false);
        ui->btnCanReset->setEnabled(false);
        ui->btnCanOpenDevice->setText(tr("打开设备"));
        canTh->stop();
        canTh->closeDevice();
    }
}


void MainWindow::on_btnCanStart_clicked()
{
    if(canTh->startCAN())
    {
        ui->btnCanStart->setEnabled(false);
        ui->btnCanReset->setEnabled(true);
        ui->btnCanSend->setEnabled(true);
        canTh->start();
    }
    else
        QMessageBox::warning(this,"警告","CAN启动失败！");
}

void MainWindow::on_btnCanSend_clicked()
{
    if(ui->sendIDEdit->text().toInt(Q_NULLPTR,16) > 0x7FF)
    {
        QMessageBox::warning(this,"警告","发送失败，标准帧ID范围为0~0x7FF！");
        return;
    }

    QStringList strList = ui->sendDataEdit->text().split(" ");
    unsigned char data[8];
    memset(data,0,8);
    UINT dlc = 0;
    dlc = strList.count() > 8 ? 8 : strList.count();
    for(int i = 0;i < dlc;i ++)
        data[i] = strList.at(i).toInt(Q_NULLPTR,16);
    if(canTh->sendData(QVariant(ui->comboChannel->currentIndex()).toUInt(),
                            QVariant(ui->sendIDEdit->text().toInt(Q_NULLPTR,16)).toUInt(),
                            ui->comboDataFrameType->currentIndex(),
                            ui->comboFrameType->currentIndex(),
                            data,dlc))
    {//发送成功，打印数据
        QStringList messageList;

        messageList.clear();
        messageList << QTime::currentTime().toString("hh:mm:ss zzz");//时间
        messageList << "无";//时间
        messageList << "无";//时间
        messageList << "CH" + QString::number(QVariant(ui->comboChannel->currentIndex()).toUInt());
        messageList << "发送";//收发
        messageList << "0x" + ui->sendIDEdit->text().toUpper();//ID
        messageList << ((ui->comboDataFrameType->currentIndex() == 0) ? "数据帧" : "远程帧");//类型
        messageList << ((ui->comboFrameType->currentIndex() == 0) ? "标准帧" : "扩展帧");//Frame
        QString str = "";
        if(ui->comboDataFrameType->currentIndex() == 0)//数据帧显示数据
        {
            messageList << "0x" + QString::number(dlc,16).toUpper();//长度
            str = "x| ";
            for(int j = 0;j < dlc;j ++)
                str += QString("%1 ").arg((unsigned char)data[j],2,16,QChar('0')).toUpper();//QString::number((unsigned char)data[j],16) + " ";
        }
        else
            messageList << "0x0";//长度
        messageList << str;//数据
        // AddDataToList(messageList);
        
        // 在状态栏上显示发送成功的消息
        statusBar()->showMessage("CAN数据发送成功", 3000);
    }
    else
        QMessageBox::warning(this,"警告","数据发送失败！");

}

void MainWindow::on_btnCanReset_clicked()
{
    if(canTh->reSetCAN())
    {
        ui->btnCanReset->setEnabled(false);
        ui->btnCanSend->setEnabled(false);
        ui->btnCanStart->setEnabled(true);
    }
    else
        QMessageBox::warning(this,"警告","CAN复位失败！");
}


void MainWindow::on_btnCanInit_clicked()
{
    if(canTh->initCAN())
    {
        ui->btnCanStart->setEnabled(true);
        ui->btnCanInit->setEnabled(false);
    }
    else
        QMessageBox::warning(this,"警告","CAN初始化失败！");
}


void MainWindow::on_btnPagePlot_clicked()
{
    switchPage();
}


void MainWindow::on_btnPageData_clicked()
{
    // 切换到数据页
    switchPage();
    
    // 确保表格已初始化
    if (!tableModel && channelNum > 0) {
        initTableView();
    }
}

// 初始化表格视图
void MainWindow::initTableView()
{
    /*
    // 如果模型已存在则删除
    if (tableModel) {
        delete tableModel;
    }
    
    // 创建新模型
    tableModel = new QStandardItemModel(this);
    
    // 获取寄存器起始地址
    int startAddress = ui->lineSegAddress->text().toInt();
    
    // 设置行标签
    QStringList rowLabels;
    rowLabels << "时间(秒)";
    
    // 添加每个通道的行标签
    for (int i = 0; i < channelNum; ++i) {
        int registerAddress = i + startAddress;
        rowLabels << QString("通道 %1").arg(registerAddress);
    }
    
    tableModel->setVerticalHeaderLabels(rowLabels);
    
    // 设置表格模型
    ui->tableView->setModel(tableModel);
    
    // 调整表格外观
    ui->tableView->horizontalHeader()->setVisible(false); // 隐藏列头
    ui->tableView->verticalHeader()->setDefaultSectionSize(30); // 行高
    ui->tableView->horizontalHeader()->setDefaultSectionSize(60); // 设置默认列宽
    ui->tableView->setAlternatingRowColors(true); // 交替行颜色
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection); // 单选模式
    
    // 设置表格为只读
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    */
}

// 更新表格数据
void MainWindow::updateTableData(double time, const QVector<double> &data)
{
    // 如果表格模型为空，则初始化它
    if (!tableModel) {
        initTableView();
    }
    
    // 如果通道数不匹配，重新初始化表格
    if (tableModel->rowCount() != data.size() + 1) {
        initTableView();
    }
    
    // 获取当前列数
    int currentColumn = tableModel->columnCount();
    
    // 优化：一次性预分配列，避免多次单独的setItem调用
    QList<QStandardItem*> columnItems;
    columnItems.reserve(data.size() + 1);
    
    // 添加时间值（移除背景色设置）
    QStandardItem *timeItem = new QStandardItem(QString::number(time, 'f', 1));
    columnItems.append(timeItem);
    
    // 添加各通道数据（移除背景色设置）
    for (int i = 0; i < data.size(); ++i) {
        QStandardItem *dataItem = new QStandardItem(QString::number(data[i], 'f', 1));
        columnItems.append(dataItem);
    }
    
    // 一次性添加新列
    tableModel->insertColumn(currentColumn, columnItems);
    
    // 如果超出最大列数，删除最旧的列
    int maxColumns = 1000; // 设置合理的最大列数
    if (currentColumn > maxColumns) {
        tableModel->removeColumn(0);
    }
    
    // 仅在新列被添加到视图末尾时滚动到最新列
    static int scrollCounter = 0;
    if (++scrollCounter % 10 == 0) { // 每10次更新才滚动一次，减少UI重绘
        if (tableModel->columnCount() > 0) {
            ui->tableView->scrollTo(tableModel->index(0, tableModel->columnCount() - 1), QAbstractItemView::EnsureVisible);
        }
        scrollCounter = 0;
    }
}

// 保存数据按钮点击事件
void MainWindow::on_btnSaveData_clicked()
{
    // 检查是否有数据可保存
    if (!tableModel || tableModel->columnCount() == 0) {
        QMessageBox::warning(this, "保存失败", "没有数据可以保存！");
        return;
    }
    
    // 打开文件对话框选择保存路径
    QString folderPath = QFileDialog::getExistingDirectory(this, "选择保存目录", QDir::homePath());
    if (folderPath.isEmpty()) {
        return; // 用户取消了选择
    }
    
    // 生成文件名（使用当前时间）
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    
    // 使用QDir对象正确处理路径，避免路径分隔符问题
    QDir dir(folderPath);
    QString csvFilePath = dir.absoluteFilePath("数据_" + timestamp + ".csv");  // 改为.csv扩展名
    QString imagePath = dir.absoluteFilePath("曲线图_" + timestamp + ".png");
    
    qDebug() << "准备保存CSV文件到：" << csvFilePath;
    
    // 保存表格数据到CSV
    bool success = exportTableToCSV(csvFilePath);
    
    if (!success) {
        QMessageBox::warning(this, "保存失败", "无法保存表格数据！");
        return;
    }
    
    // 保存曲线图为PNG
    qDebug() << "准备保存PNG图片到：" << imagePath;
    success = myPlot->savePng(imagePath, 0, 0, 1.0, -1);
    
    // 显示结果
    if (success) {
        QMessageBox::information(this, "保存成功", 
            QString("数据已保存到：\n表格(CSV): %1\n曲线图: %2").arg(csvFilePath).arg(imagePath));
    } else {
        QMessageBox::warning(this, "保存失败", "保存曲线图时发生错误！");
    }
}

// 读取数据按钮点击事件
void MainWindow::on_btnReadData_clicked()
{
    // 打开文件对话框选择要读取的CSV文件
    QString filePath = QFileDialog::getOpenFileName(this, "选择CSV文件", 
                                                   QDir::homePath(), 
                                                   "CSV文件 (*.csv)");
    if (filePath.isEmpty()) {
        return; // 用户取消了选择
    }
    
    qDebug() << "准备读取CSV文件：" << filePath;
    
    // 从CSV文件导入数据
    bool success = importDataFromCSV(filePath);
    
    // 显示结果
    if (success) {
        QMessageBox::information(this, "读取成功", "数据已成功读取并显示！");
    } else {
        QMessageBox::warning(this, "读取失败", "读取数据时发生错误！");
    }
}

// 导出表格数据到CSV文件
bool MainWindow::exportTableToCSV(const QString &filePath)
{
    if (!tableModel || tableModel->columnCount() == 0) {
        return false;
    }
    
    // 创建CSV文件
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法创建文件：" << filePath << "，错误：" << file.errorString();
        return false;
    }
    
    // 写入UTF-8 BOM，帮助Excel正确识别中文
    file.write("\xEF\xBB\xBF");
    
    QTextStream stream(&file);
    
    // 写入表头
    for (int col = 0; col < tableModel->columnCount(); ++col) {
        if (col > 0) stream << ",";
        stream << QString("列%1").arg(col + 1);
    }
    stream << "\r\n";  // 使用CRLF换行符，兼容Windows
    
    // 写入行标签和数据
    for (int row = 0; row < tableModel->rowCount(); ++row) {
        // 添加行标签
        QStandardItem* headerItem = tableModel->verticalHeaderItem(row);
        if (headerItem) {
            stream << headerItem->text();
        } else {
            stream << QString("行%1").arg(row + 1);
        }
        
        // 添加各列数据
        for (int col = 0; col < tableModel->columnCount(); ++col) {
            stream << ",";
            QStandardItem *item = tableModel->item(row, col);
            if (item) {
                // 处理可能包含逗号的文本
                QString text = item->text();
                if (text.contains(",") || text.contains("\"") || text.contains("\n")) {
                    text.replace("\"", "\"\""); // 转义双引号
                    stream << "\"" << text << "\"";
                } else {
                    stream << text;
                }
            }
        }
        stream << "\r\n";  // 使用CRLF换行符，兼容Windows
    }
    
    file.close();
    
    return true; // 如果能执行到这里，说明文件操作成功
}

// 从CSV文件导入数据
bool MainWindow::importDataFromCSV(const QString &filePath)
{
    // 直接尝试打开源文件
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件：" << filePath << "，错误：" << file.errorString();
        return false;
    }
    
    // 检测并跳过UTF-8 BOM（如果存在）
    QByteArray possibleBOM = file.peek(3);
    if (possibleBOM.startsWith("\xEF\xBB\xBF")) {
        file.read(3); // 跳过BOM
    }
    
    QTextStream stream(&file);
    
    // 读取表头行（忽略）
    QString headerLine = stream.readLine();
    
    // 清除现有数据
    if (tableModel) {
        tableModel->clear();
    } else {
        tableModel = new QStandardItemModel(this);
    }
    
    // 预处理数据，获取行数和列数
    QList<QStringList> allData;
    QStringList rowLabels;
    
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList fields = parseCSVLine(line);
        
        if (!fields.isEmpty()) {
            rowLabels.append(fields.first());
            fields.removeFirst(); // 移除行标签
            allData.append(fields);
        }
    }
    
    // 设置行标签
    tableModel->setVerticalHeaderLabels(rowLabels);
    
    // 设置表格大小
    if (!allData.isEmpty()) {
        int rows = allData.size();
        int cols = 0;
        for (const QStringList &row : allData) {
            cols = qMax(cols, row.size());
        }
        
        // 确保表格有足够的行列
        tableModel->setRowCount(rows);
        tableModel->setColumnCount(cols);
        
        // 填充数据
        for (int row = 0; row < rows; ++row) {
            const QStringList &rowData = allData[row];
            for (int col = 0; col < rowData.size(); ++col) {
                tableModel->setItem(row, col, new QStandardItem(rowData[col]));
            }
        }
    }
    
    file.close();
    
    // 应用表格模型到视图
    ui->tableView->setModel(tableModel);
    ui->tableView->horizontalHeader()->setVisible(false);
    ui->tableView->verticalHeader()->setDefaultSectionSize(30);
    ui->tableView->horizontalHeader()->setDefaultSectionSize(60);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 根据导入的数据重绘曲线
    redrawPlotsFromTableData();
    
    return true;
}

// 解析CSV行（处理引号内的逗号等）
QStringList MainWindow::parseCSVLine(const QString &line)
{
    QStringList result;
    bool inQuotes = false;
    QString field;
    
    for (int i = 0; i < line.length(); ++i) {
        QChar c = line[i];
        
        if (c == '\"') {
            if (i < line.length() - 1 && line[i+1] == '\"') {
                // 处理两个连续的双引号
                field += '\"';
                ++i; // 跳过下一个引号
            } else {
                // 切换引号状态
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            // 字段结束
            result.append(field);
            field.clear();
        } else {
            field += c;
        }
    }
    
    // 添加最后一个字段
    result.append(field);
    
    return result;
}

// 根据表格数据重绘曲线
void MainWindow::redrawPlotsFromTableData()
{
    if (!tableModel || tableModel->rowCount() < 2) {
        return; // 至少需要一行时间和一行数据
    }
    
    // 清除现有曲线
    myPlot->clearGraphs();
    
    // 获取通道数（行数-1，第一行是时间）
    channelNum = tableModel->rowCount() - 1;
    graphs.resize(channelNum);
    
    // 创建通道曲线
    for (int i = 0; i < channelNum; ++i) {
        graphs[i] = myPlot->addGraph();
        graphs[i]->setPen(QPen(QColor::fromHsv((i * 255) / channelNum, 255, 200)));
        graphs[i]->setName(tableModel->verticalHeaderItem(i + 1)->text());
    }
    
    // 添加数据点
    for (int col = 0; col < tableModel->columnCount(); ++col) {
        // 获取时间值
        double time = 0.0;
        QStandardItem *timeItem = tableModel->item(0, col);
        if (timeItem) {
            time = timeItem->text().toDouble();
        }
        
        // 获取各通道数据
        for (int i = 0; i < channelNum; ++i) {
            double value = 0.0;
            QStandardItem *dataItem = tableModel->item(i + 1, col);
            if (dataItem) {
                value = dataItem->text().toDouble();
            }
            graphs[i]->addData(time, value);
        }
    }
    
    // 重新设置坐标轴范围
    myPlot->rescaleAxes();
    myPlot->replot();
}

// 添加resizeEvent实现
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    
    // 保存初始尺寸和布局
    if (!geometriesInitialized && isVisible()) {
        initialSize = event->oldSize().isValid() ? event->oldSize() : size();
        
        // 保存各主要控件的初始几何信息
        initialGeometries[ui->widgetDashBoard] = ui->widgetDashBoard->geometry();
        initialGeometries[ui->stackedWidget] = ui->stackedWidget->geometry();
        
        geometriesInitialized = true;
    }
    
    // 更新布局
    updateLayout();
}

// 添加updateLayout函数实现
void MainWindow::updateLayout()
{
    if (!geometriesInitialized || initialSize.isEmpty())
        return;
    
    // 计算当前窗口与初始窗口的比例
    double widthRatio = (double)width() / initialSize.width();
    double heightRatio = (double)height() / initialSize.height();
    
    // 确保中央容器水平布局的比例正确
    QHBoxLayout* hLayout = qobject_cast<QHBoxLayout*>(ui->centralwidget->layout()->itemAt(1)->layout());
    if (hLayout) {
        // 确保布局拉伸因子设置正确
        if (hLayout->count() >= 3) {
            hLayout->setStretch(0, 1);  // 左侧导航区域
            hLayout->setStretch(1, 10); // 中部仪表盘区域
            hLayout->setStretch(2, 10); // 右侧功能区域
        }
    }
    
    // 处理内部QHBoxLayout的spacing
    int spacing = 6 * widthRatio;
    if (spacing < 3) spacing = 3;
    
    if (hLayout) {
        hLayout->setSpacing(spacing);
    }
    
    // 防止控件在缩放时太小导致内容看不清
    QSize minDashSize(300, 400);
    QSize minStackSize(600, 400);
    
    if (ui->widgetDashBoard->width() < minDashSize.width() || ui->widgetDashBoard->height() < minDashSize.height()) {
        ui->widgetDashBoard->setMinimumSize(minDashSize);
    }
    
    if (ui->stackedWidget->width() < minStackSize.width() || ui->stackedWidget->height() < minStackSize.height()) {
        ui->stackedWidget->setMinimumSize(minStackSize);
    }
    
    // 更新状态栏信息
    statusBar()->showMessage(QString("窗口大小: %1 x %2").arg(width()).arg(height()));
}

// 一阶低通滤波函数实现
double MainWindow::applyLowPassFilter(double input, double prevOutput, double timeConstant, double deltaT)
{
    // 计算滤波系数 alpha = deltaT / (timeConstant + deltaT)
    double alpha = deltaT / (timeConstant + deltaT);
    
    // 限制alpha在0到1之间，防止计算错误
    alpha = qBound(0.0, alpha, 1.0);
    
    // 应用滤波公式: y[n] = alpha * x[n] + (1 - alpha) * y[n-1]
    double result = alpha * input + (1.0 - alpha) * prevOutput;
    
    // qDebug() << "滤波计算: alpha=" << alpha << ", input=" << input << ", prevOutput=" << prevOutput << ", result=" << result;
    
    return result;
}

// 对所有通道应用滤波
QVector<double> MainWindow::applyFilterToResults(const QVector<double> &rawData, qint64 deltaT)
{
    // 如果滤波禁用，直接返回原始数据
    if (!filterEnabled) {
        return rawData;
    }
    
    // 如果原始数据为空，返回空数组
    if (rawData.isEmpty()) {
        qDebug() << "警告: 原始数据为空，无法应用滤波";
        return rawData;
    }
    
    // 确保filteredValues的大小与当前通道数匹配
    if (filteredValues.size() != rawData.size()) {
        qDebug() << "重置滤波缓存，通道数量变化: " << filteredValues.size() << " -> " << rawData.size();
        
        // 保存旧值以便重用
        QVector<double> oldValues = filteredValues;
        filteredValues.resize(rawData.size());
        
        // 初始化新的通道值
        for (int i = 0; i < rawData.size(); ++i) {
            // 如果是原有通道，保留原来的值
            if (i < oldValues.size()) {
                filteredValues[i] = oldValues[i];
            } else {
                // 新通道使用原始值初始化
                filteredValues[i] = rawData[i];
            }
        }
        
        // 第一次处理新通道数据时，直接返回原始数据，让滤波从下一轮开始生效
        return rawData;
    }
    
    // 获取时间常数，单位为毫秒
    double timeConstant = ui->lineTimeLoop->text().toDouble();
    
    // 确保时间常数和deltaT有效
    if (timeConstant <= 0) {
        timeConstant = 1000; // 默认使用1000ms作为时间常数
        qDebug() << "警告: 时间常数无效，使用默认值1000ms";
    }
    
    if (deltaT <= 0) {
        deltaT = 100; // 默认使用100ms
        qDebug() << "警告: deltaT无效，使用默认值100ms";
    }
    
    QVector<double> filteredData(rawData.size());
    
    // 对每个通道应用滤波
    for (int i = 0; i < rawData.size(); ++i) {
        // 计算滤波结果
        filteredData[i] = applyLowPassFilter(rawData[i], filteredValues[i], timeConstant, deltaT);
        // 更新上一次的滤波值，用于下次计算
        filteredValues[i] = filteredData[i];
    }
    
    return filteredData;
}

// 滤波开关状态改变槽函数
void MainWindow::on_filterEnabledCheckBox_stateChanged(int state)
{
    filterEnabled = (state == Qt::Checked);
    
    // 如果禁用滤波器，则清除之前的滤波数据
    if (!filterEnabled) {
        filteredValues.clear();
    }
    
    // 更新状态栏提示
    QString message = filterEnabled ? 
                     "已启用低通滤波，使用循环时间作为时间常数" : 
                     "已禁用低通滤波，显示原始数据";
    sBar->showMessage(message, 3000);
    
    qDebug() << "低通滤波状态: " << (filterEnabled ? "开启" : "关闭") 
             << "，时间常数: " << ui->lineTimeLoop->text().toDouble() << "ms";
}

// 设置ECU表格模型
void MainWindow::setupECUTable()
{
    /*
    // 检查表格对象是否存在
    if (!ui->tableViewECU) {
        qDebug() << "错误: ECU表格控件不存在";
        return;
    }
    
    // 创建表格模型
    ecuDataModel = new QStandardItemModel(this);
    
    // 设置水平表头（时间和各个参数）
    QStringList headers;
    headers << "时间(秒)"
            << "喷头(%)" 
              << "发动机转速(rpm)"
            << "缸温(℃)" 
            << "排温(℃)" 
            << "轴温(℃)" 
              << "燃油压力(kPa)"
            << "进气温度(℃)" 
              << "大气压力(kPa)"
            << "飞行时间(s)";
    
    ecuDataModel->setHorizontalHeaderLabels(headers);
    
    // 设置表格模型
    ui->tableViewECU->setModel(ecuDataModel);
    
    // 设置表格属性
    ui->tableViewECU->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableViewECU->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewECU->setAlternatingRowColors(true);
    ui->tableViewECU->horizontalHeader()->setStretchLastSection(true);
    ui->tableViewECU->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    
    // 设置表格列宽
    for (int i = 0; i < headers.size(); ++i) {
        ui->tableViewECU->setColumnWidth(i, 120);
    }
    
    // 设置时间列宽度小一些
    ui->tableViewECU->setColumnWidth(0, 80);
    */
}

void MainWindow::on_btnECUScan_clicked()
{
    // 清空串口列表
    ui->comboSerialECU->clear();
    
    // 扫描可用串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->comboSerialECU->addItem(info.portName());
    }
    
    // 如果有可用串口，选择第一个
    if (ui->comboSerialECU->count() > 0) {
        ui->comboSerialECU->setCurrentIndex(0);
        ui->btnECUStart->setEnabled(true);
    } else {
        // 没有可用串口
        ui->btnECUStart->setEnabled(false);
        QMessageBox::warning(this, "警告", "未找到可用串口!");
    }
}

void MainWindow::on_btnECUStart_clicked()
{
    // 检查是否已有串口连接
    if (ecuIsConnected) {
        // 如果已连接，则关闭
        emit closeECUPort();
        ui->btnECUStart->setText("开始采集");
    } else {
        // 如果未连接，则打开
        if (ui->comboSerialECU->currentText().isEmpty()) {
            QMessageBox::warning(this, "错误", "请先选择串口!");
            return;
        }
        
        // 清空表格
        ecuDataModel->removeRows(0, ecuDataModel->rowCount());
        
        // 打开选中的串口
        emit openECUPort(ui->comboSerialECU->currentText());
        ui->btnECUStart->setText("停止采集");
    }
}

void MainWindow::handleECUData(const ECUData &data)
{
    // 仅在ECU数据有效时处理
    if (!data.isValid) {
        return;
    }
    
    // 如果没有主计时器，则不处理
    if (!masterTimer) {
        qDebug() << "警告: 主计时器未初始化，无法处理ECU数据";
        return;
    }
    
    try {
        // 保存最新的ECU数据
        latestECUData = data;
        
        // 准备ECU数据向量 - 保存成向量格式方便处理
        QVector<double> ecuValues(9);
        ecuValues[0] = data.throttle;
        ecuValues[1] = data.engineSpeed;
        ecuValues[2] = data.cylinderTemp;
        ecuValues[3] = data.exhaustTemp;
        ecuValues[4] = data.axleTemp;
        ecuValues[5] = data.fuelPressure;
        ecuValues[6] = data.intakeTemp;
        ecuValues[7] = data.atmPressure;
        ecuValues[8] = data.flightTime;
        
        // 更新全局ECU数据映射
        ecudataMap = ecuValues;
        ecuDataValid = true;
        
        // 强制初始化ECU表格和图表，如果它们尚未初始化
        if (!ui->ECUCustomPlot->graphCount() || ecuData.isEmpty()) {
            ECUPlotInit();
        }
        
        if (!ecuDataModel || ecuDataModel->columnCount() == 0) {
            setupECUDataTable();
        }
        
        // 直接添加数据点到时间序列，不依赖于快照
        double currentTime = masterTimer->elapsed() / 1000.0;
        addECUDataPoint(currentTime, ecuValues);
        
        // 直接更新ECU图表和表格
        updateECUPlot(data);
        
        // 添加到当前快照
        currentSnapshot.ecuValid = true;
        currentSnapshot.ecuData = ecuValues;
        
    } catch (const std::exception& e) {
        qDebug() << "处理ECU数据时出错: " << e.what();
    } catch (...) {
        qDebug() << "处理ECU数据时发生未知错误";
    }
}

void MainWindow::handleECUStatus(bool connected, QString message)
{
    // 更新连接状态
    ecuIsConnected = connected;
    
    // 更新状态栏
    sBar->showMessage(message, 3000);
    
    // 更新UI状态
    if (connected) {
        ui->btnECUStart->setText("停止采集");
        ui->comboSerialECU->setEnabled(false);
        ui->btnECUScan->setEnabled(false);
    } else {
        ui->btnECUStart->setText("开始采集");
        ui->comboSerialECU->setEnabled(true);
        ui->btnECUScan->setEnabled(true);
    }
}

void MainWindow::handleECUError(QString errorMessage)
{
    // 显示错误消息
    QMessageBox::critical(this, "ECU通信错误", errorMessage);
    
    // 如果串口出错，可能需要重置连接状态
    if (ecuIsConnected) {
        ecuIsConnected = false;
        ui->btnECUStart->setText("开始采集");
        ui->comboSerialECU->setEnabled(true);
        ui->btnECUScan->setEnabled(true);
    }
}


void MainWindow::on_btnPageInitial_clicked()
{
    // 获取当前页面索引
    int currentIndex = ui->stackedWidget->currentIndex();
    
    // 只有当stackedWidget存在并且初始化页面已启用时才执行
    if (ui->stackedWidget && ui->stackedWidget->widget(0) && !ui->stackedWidget->widget(0)->isEnabled()) {
        QMessageBox::warning(this, "提示", "请先通过菜单中的【初始化】->【设置初始化】或【读取初始化文件】启用初始化页面");
        return;
    }
    
    // 切换到初始化页面 (索引0)
    ui->stackedWidget->setCurrentIndex(0);
    
    // 更新按钮选中状态
    ui->btnPageInitial->setChecked(true);
    ui->btnPagePlot->setChecked(false);
    ui->btnPageData->setChecked(false);
    
    // 更新状态栏
    statusBar()->showMessage("已切换到通信初始化页面", 2000);
}

// 新增：设置初始化菜单项槽函数
void MainWindow::on_actionSetupInitial_triggered()
{
    // 启用初始化页上的控件
    enableInitialPage(true);
    
    // 切换到初始化页面
    ui->stackedWidget->setCurrentIndex(0);
    
    // 更新按钮选中状态
    ui->btnPageInitial->setChecked(true);
    ui->btnPagePlot->setChecked(false);
    ui->btnPageData->setChecked(false);
    
    // 更新可用状态
    ui->btnPagePlot->setEnabled(true);
    ui->btnPageData->setEnabled(true);
    ui->btnSaveData->setEnabled(true);
    ui->btnReadData->setEnabled(true);
    
    // 显示提示消息
    QMessageBox::information(this, "初始化已启用", "初始化页面已启用，您可以进行通信设置");
    
    // 更新状态栏
    statusBar()->showMessage("初始化页面已启用", 2000);
}

// 新增：读取初始化文件菜单项槽函数
void MainWindow::on_actionLoadInitial_triggered()
{
    // 打开文件对话框，选择初始化配置文件
    QString filePath = QFileDialog::getOpenFileName(this, "选择初始化文件", 
                                                  QDir::homePath(), 
                                                  "INI文件 (*.ini)");
    if (filePath.isEmpty()) {
        return; // 用户取消了选择
    }
    
    // 加载初始化设置
    if (loadInitialSettings(filePath)) {
        // 启用初始化页上的控件
        enableInitialPage(true);
        
        // 切换到初始化页面
        ui->stackedWidget->setCurrentIndex(0);
        
        // 更新按钮选中状态
        ui->btnPageInitial->setChecked(true);
        ui->btnPagePlot->setChecked(false);
        ui->btnPageData->setChecked(false);
        
        // 更新可用状态
        ui->btnPagePlot->setEnabled(true);
        ui->btnPageData->setEnabled(true);
        ui->btnSaveData->setEnabled(true);
        ui->btnReadData->setEnabled(true);
        
        // 显示提示消息
        QMessageBox::information(this, "读取成功", "已成功从文件加载初始化设置");
        
        // 更新状态栏
        statusBar()->showMessage("初始化设置已从文件加载", 2000);
    } else {
        QMessageBox::warning(this, "读取失败", "无法从文件加载初始化设置");
    }
}

// 新增：保存初始化文件菜单项槽函数
void MainWindow::on_actionSaveInitial_triggered()
{
    // 检查初始化页面是否已启用
    if (!ui->page->isEnabled()) {
        QMessageBox::warning(this, "保存失败", "请先启用初始化页面或加载初始化设置");
        return;
    }
    
    // 打开文件对话框，选择保存路径
    QString filePath = QFileDialog::getSaveFileName(this, "保存初始化文件", 
                                                  QDir::homePath(), 
                                                  "INI文件 (*.ini)");
    if (filePath.isEmpty()) {
        return; // 用户取消了选择
    }
    
    // 确保文件名以.ini结尾
    if (!filePath.endsWith(".ini", Qt::CaseInsensitive)) {
        filePath += ".ini";
    }
    
    // 保存初始化设置
    if (saveInitialSettings(filePath)) {
        QMessageBox::information(this, "保存成功", "初始化设置已成功保存到文件");
        statusBar()->showMessage("初始化设置已保存到文件", 2000);
    } else {
        QMessageBox::warning(this, "保存失败", "无法保存初始化设置到文件");
    }
}

// 新增：保存初始化设置到文件
bool MainWindow::saveInitialSettings(const QString &filename)
{
    QSettings settings(filename, QSettings::IniFormat);
    
    // 保存Modbus设置
    settings.beginGroup("Modbus");
    if (ui->comboPort) settings.setValue("PortName", ui->comboPort->currentText());
    if (ui->comboBaudRate) settings.setValue("BaudRate", ui->comboBaudRate->currentIndex());
    if (ui->comboStopBits) settings.setValue("StopBits", ui->comboStopBits->currentIndex());
    if (ui->comboDataBits) settings.setValue("DataBits", ui->comboDataBits->currentIndex());
    if (ui->comboParity) settings.setValue("Parity", ui->comboParity->currentIndex());
    if (ui->lineServerAddress) settings.setValue("ServerAddress", ui->lineServerAddress->text());
    if (ui->comboAction) settings.setValue("ActionIndex", ui->comboAction->currentIndex()); // 添加保存comboAction的值
    if (ui->lineSegAddress) settings.setValue("SegAddress", ui->lineSegAddress->text());
    if (ui->lineSegNum) settings.setValue("SegNum", ui->lineSegNum->text());
    if (ui->lineTimeLoop) settings.setValue("TimeLoop", ui->lineTimeLoop->text());
    settings.endGroup();
    
    // 保存CAN设置
    settings.beginGroup("CAN");
    if (ui->comboBox) settings.setValue("DeviceType", ui->comboBox->currentText());
    if (ui->comboCanID) settings.setValue("DeviceIndex", ui->comboCanID->currentIndex());
    if (ui->comboCanBaud) settings.setValue("BaudRate", ui->comboCanBaud->currentIndex());
    // 数据发送部分
    if (ui->sendIDEdit) settings.setValue("SendID", ui->sendIDEdit->text());
    if (ui->comboChannel) settings.setValue("Channel", ui->comboChannel->currentIndex());
    if (ui->comboFrameType) settings.setValue("FrameType", ui->comboFrameType->currentIndex());
    if (ui->comboDataFrameType) settings.setValue("DataFrameType", ui->comboDataFrameType->currentIndex());
    if (ui->sendDataEdit) settings.setValue("SendData", ui->sendDataEdit->text());
    settings.endGroup();
    
    // 保存ECU设置
    settings.beginGroup("ECU");
    if (ui->comboSerialECU) settings.setValue("PortName", ui->comboSerialECU->currentText());
    settings.endGroup();
    
    // 保存DAQ设置
    settings.beginGroup("DAQ");
    if (ui->deviceNameEdit) settings.setValue("DeviceName", ui->deviceNameEdit->text());
    if (ui->channelsEdit) settings.setValue("Channels", ui->channelsEdit->text());
    if (ui->sampleRateEdit) settings.setValue("SampleRate", ui->sampleRateEdit->text());
    settings.endGroup();
    
    // 保存滤波器设置
    settings.beginGroup("Filter");
    if (ui->filterEnabledCheckBox) settings.setValue("Enabled", ui->filterEnabledCheckBox->isChecked());
    settings.endGroup();
    
    // 保存仪表盘映射关系
    saveDashboardMappings(settings);
    
    settings.sync();
    return (settings.status() == QSettings::NoError);
}

// 新增：从文件加载初始化设置
bool MainWindow::loadInitialSettings(const QString &filename)
{
    QSettings settings(filename, QSettings::IniFormat);
    
    // 检查文件是否存在且可读
    if (settings.status() != QSettings::NoError) {
        return false;
    }
    
    // 加载Modbus设置
    settings.beginGroup("Modbus");
    
    // 更新串口设置（需要先加载可用串口列表）
    if (ui->comboPort) {
        QString portName = settings.value("PortName", "").toString();
        int index = ui->comboPort->findText(portName);
        if (index >= 0) {
            ui->comboPort->setCurrentIndex(index);
        }
    }
    
    // 更新其他下拉框
    if (ui->comboBaudRate) ui->comboBaudRate->setCurrentIndex(settings.value("BaudRate", 0).toInt());
    if (ui->comboStopBits) ui->comboStopBits->setCurrentIndex(settings.value("StopBits", 0).toInt());
    if (ui->comboDataBits) ui->comboDataBits->setCurrentIndex(settings.value("DataBits", 0).toInt());
    if (ui->comboParity) ui->comboParity->setCurrentIndex(settings.value("Parity", 0).toInt());
    
    // 更新文本框
    if (ui->lineServerAddress) ui->lineServerAddress->setText(settings.value("ServerAddress", "1").toString());
    if (ui->comboAction) ui->comboAction->setCurrentIndex(settings.value("ActionIndex", 0).toInt()); // 添加读取comboAction的值
    if (ui->lineSegAddress) ui->lineSegAddress->setText(settings.value("SegAddress", "0").toString());
    if (ui->lineSegNum) ui->lineSegNum->setText(settings.value("SegNum", "1").toString());
    if (ui->lineTimeLoop) ui->lineTimeLoop->setText(settings.value("TimeLoop", "1000").toString());
    
    settings.endGroup();
    
    // 加载CAN设置
    settings.beginGroup("CAN");
    // CAN设备设置
    if (ui->comboBox) {
        QString deviceType = settings.value("DeviceType", "CANalyst-II").toString();
        int index = ui->comboBox->findText(deviceType);
        if (index >= 0) {
            ui->comboBox->setCurrentIndex(index);
        }
    }
    if (ui->comboCanID) ui->comboCanID->setCurrentIndex(settings.value("DeviceIndex", 0).toInt());
    if (ui->comboCanBaud) ui->comboCanBaud->setCurrentIndex(settings.value("BaudRate", 0).toInt());
    
    // 数据发送设置
    if (ui->sendIDEdit) ui->sendIDEdit->setText(settings.value("SendID", "00000000").toString());
    if (ui->comboChannel) ui->comboChannel->setCurrentIndex(settings.value("Channel", 0).toInt());
    if (ui->comboFrameType) ui->comboFrameType->setCurrentIndex(settings.value("FrameType", 0).toInt());
    if (ui->comboDataFrameType) ui->comboDataFrameType->setCurrentIndex(settings.value("DataFrameType", 0).toInt());
    if (ui->sendDataEdit) ui->sendDataEdit->setText(settings.value("SendData", "00 11 22 33 44 55 66 77").toString());
    settings.endGroup();
    
    // 加载ECU设置
    settings.beginGroup("ECU");
    if (ui->comboSerialECU) {
        QString portName = settings.value("PortName", "").toString();
        int index = ui->comboSerialECU->findText(portName);
        if (index >= 0) {
            ui->comboSerialECU->setCurrentIndex(index);
        }
    }
    settings.endGroup();
    
    // 加载DAQ设置
    settings.beginGroup("DAQ");
    if (ui->deviceNameEdit) ui->deviceNameEdit->setText(settings.value("DeviceName", "Dev1").toString());
    if (ui->channelsEdit) {
        QString channels = settings.value("Channels", "0/1").toString();
        ui->channelsEdit->setText(channels);
        // 确保通道列表更新到仪表盘
        updateDashboardDAQChannels(channels);
    }
    if (ui->sampleRateEdit) ui->sampleRateEdit->setText(settings.value("SampleRate", "10000").toString());
    settings.endGroup();
    
    // 加载滤波器设置
    settings.beginGroup("Filter");
    if (ui->filterEnabledCheckBox) {
        ui->filterEnabledCheckBox->setChecked(settings.value("Enabled", false).toBool());
        // 确保滤波器状态一致
        on_filterEnabledCheckBox_stateChanged(ui->filterEnabledCheckBox->checkState());
    }
    settings.endGroup();
    
    // 加载仪表盘映射关系
    loadDashboardMappings(settings);
    
    // 更新监控表格
    updateMonitorTable();
    
    return true;
}

// 添加辅助函数，用于启用/禁用初始页上的控件
void MainWindow::enableInitialPage(bool enable)
{
    // 启用/禁用第一页所有控件 - 使用更通用的方法
    QList<QWidget*> allWidgets = ui->page->findChildren<QWidget*>();
    
    foreach(QWidget* widget, allWidgets) {
        // 根据控件类型设置启用状态
        if (qobject_cast<QLineEdit*>(widget) || 
            qobject_cast<QComboBox*>(widget) || 
            qobject_cast<QCheckBox*>(widget) || 
            qobject_cast<QPushButton*>(widget) || 
            qobject_cast<QRadioButton*>(widget)) {
            // 检查控件名称确保不是不需要禁用的控件
            QString widgetName = widget->objectName();
            if (!widgetName.contains("btnPageInitial")) {
                widget->setEnabled(enable);
            }
        }
    }
    
    // 单独处理已知存在的特定控件
    if (ui->comboPort) ui->comboPort->setEnabled(enable);
    if (ui->comboBaudRate) ui->comboBaudRate->setEnabled(enable);
    if (ui->comboStopBits) ui->comboStopBits->setEnabled(enable);
    if (ui->comboDataBits) ui->comboDataBits->setEnabled(enable);
    if (ui->comboParity) ui->comboParity->setEnabled(enable);
    if (ui->lineServerAddress) ui->lineServerAddress->setEnabled(enable);
    if (ui->comboAction) ui->comboAction->setEnabled(enable);
    if (ui->lineSegAddress) ui->lineSegAddress->setEnabled(enable);
    if (ui->lineSegNum) ui->lineSegNum->setEnabled(enable);
    if (ui->lineTimeLoop) ui->lineTimeLoop->setEnabled(enable);
    
    // 特别确保滤波控制器的状态符合要求
    if (ui->filterEnabledCheckBox) {
        ui->filterEnabledCheckBox->setEnabled(enable);
        if (!enable) {
            ui->filterEnabledCheckBox->setChecked(false);
        }
    }
    
    if (ui->btnScanPort) ui->btnScanPort->setEnabled(enable);
    if (ui->btnOpenPort) ui->btnOpenPort->setEnabled(enable);
    
    // 特殊处理特定控件组
    if (ui->groupBoxCAN) ui->groupBoxCAN->setEnabled(enable);
    if (ui->groupBox_CAN2) ui->groupBox_CAN2->setEnabled(enable);
    if (ui->groupBoxECU) ui->groupBoxECU->setEnabled(enable);
    if (ui->groupBoxDAQ) ui->groupBoxDAQ->setEnabled(enable);
    
    // 如果启用了控件，则确保channelsEdit控件内容生效
    if (enable && ui->channelsEdit) {
        // 确保DAQ通道设置生效
        updateDashboardDAQChannels(ui->channelsEdit->text());
    }
}

// 实现WebSocket相关的槽函数
void MainWindow::handleWebSocketServerStarted(bool success, QString message)
{
    if (success) {
        ui->plainReceive->appendPlainText("WebSocket服务器已启动: " + message);
        qDebug() << "WebSocket服务器已启动: " + message;
        
        // 更新状态标签
        ui->labelWebSocketStatus->setText("WS: 在线");
        ui->labelWebSocketStatus->setStyleSheet("color: green; font-weight: bold;");
    } else {
        ui->plainReceive->appendPlainText("WebSocket服务器启动失败: " + message);
        qDebug() << "WebSocket服务器启动失败: " + message;
        
        // 更新状态标签
        ui->labelWebSocketStatus->setText("WS: 离线");
        ui->labelWebSocketStatus->setStyleSheet("color: red; font-weight: bold;");
    }
}

void MainWindow::handleWebSocketClientConnected(const QString &clientInfo)
{
    ui->plainReceive->appendPlainText("WebSocket客户端连接: " + clientInfo);
    qDebug() << "WebSocket客户端连接: " + clientInfo;
}

void MainWindow::handleWebSocketClientDisconnected(const QString &clientInfo)
{
    ui->plainReceive->appendPlainText("WebSocket客户端断开连接: " + clientInfo);
    qDebug() << "WebSocket客户端断开连接: " + clientInfo;
}

void MainWindow::handleWebSocketMessage(QString message)
{
    // 增强消息处理，包含来源信息
    ui->plainReceive->appendPlainText("收到WebSocket消息: " + message);
    qDebug() << "收到WebSocket消息: " << message;
    
    // 尝试解析JSON
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isNull() && doc.isObject()) {
        QJsonObject jsonObj = doc.object();
        
        // 处理不同类型的消息
        QString type = jsonObj["type"].toString();
        if (type == "command") {
            QString command = jsonObj["command"].toString();
            
            // 响应命令
            QJsonObject response;
            response["type"] = "response";
            response["command"] = command;
            response["status"] = "success";
            response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
            
            // 发送响应
            QJsonDocument responseDoc(response);
            QString responseStr = responseDoc.toJson(QJsonDocument::Compact);
            
            QMetaObject::invokeMethod(wsTh, "sendMessageToAllClients", 
                                     Qt::QueuedConnection, 
                                     Q_ARG(QString, responseStr));
                                     
            ui->plainReceive->appendPlainText("发送WebSocket响应: " + responseStr);
        }
    }
}

// 添加WebSocket测试函数
void MainWindow::testWebSocketConnection()
{
    if (!wsTh) {
        QMessageBox::warning(this, "错误", "WebSocket对象未初始化！");
        return;
    }
    
    if (!wsTh->isRunning()) {
        QMessageBox::warning(this, "错误", "WebSocket服务器未启动，请先启动服务器！");
        return;
    }
    
    qDebug() << "WebSocket服务器运行中，开始测试连接...";
    
    // 直接调用testConnection方法
    wsTh->testConnection();
    
    // 在状态栏中显示测试信息
    statusBar()->showMessage("WebSocket测试连接已发送", 3000);
    
    // 显示详细的URL信息
    QString message = "WebSocket服务器运行中\n\n";
    message += "WebSocket URL: ws://localhost:8080\n";
    message += "HTML客户端: http://localhost:8081/websocket_client.html\n\n";
    message += "请打开浏览器访问HTML客户端页面测试连接";
    
    QMessageBox::information(this, "测试连接", message);
}

// 实现缺少的onRandomNumberGenerated函数
void MainWindow::onRandomNumberGenerated(int number)
{
    // 简单的实现，仅记录收到的随机数
    qDebug() << "收到随机数: " << number;
}

// 根据映射关系更新仪表盘显示
void MainWindow::updateDashboardByMapping(const QVector<double> &modbusData, 
                                       const QVector<double> &daqData, 
                                       const ECUData &ecuData)
{
    // 遍历所有仪表盘
    QList<Dashboard*> dashboards = getAllDashboards();
    
    // 创建变量表，用于计算公式
    static QMap<QString, double> persistentVarMap; // 使用静态变量保持数据持久性
    QMap<QString, double> currentVarMap;
    
    // 临时变量，标记哪些变量有更新
    QSet<QString> updatedVars;
    
    // 将所有数据源的数据添加到变量表
    // 1. Modbus数据 (A_x)
    for (int i = 0; i < modbusData.size(); i++) {
        QString varName = QString("A_%1").arg(i);
        currentVarMap[varName] = modbusData[i];
        updatedVars.insert(varName);
    }
    
    // 2. DAQ数据 (B_x) - 修改为使用一维数组
    if (!daqData.isEmpty()) {
        for (int i = 0; i < daqData.size(); i++) {
            QString varName = QString("B_%1").arg(i);
            currentVarMap[varName] = daqData[i];
            updatedVars.insert(varName);
        }
    }
    
    // 3. ECU数据 (C_x) 
    currentVarMap["C_0"] = ecuData.throttle;
    currentVarMap["C_1"] = ecuData.engineSpeed;
    currentVarMap["C_2"] = ecuData.cylinderTemp;
    currentVarMap["C_3"] = ecuData.exhaustTemp;
    currentVarMap["C_4"] = ecuData.axleTemp;
    currentVarMap["C_5"] = ecuData.fuelPressure;
    currentVarMap["C_6"] = ecuData.intakeTemp;
    currentVarMap["C_7"] = ecuData.atmPressure;
    currentVarMap["C_8"] = ecuData.flightTime;
    
    updatedVars.insert("C_0");
    updatedVars.insert("C_1");
    updatedVars.insert("C_2");
    updatedVars.insert("C_3");
    updatedVars.insert("C_4");
    updatedVars.insert("C_5");
    updatedVars.insert("C_6");
    updatedVars.insert("C_7");
    updatedVars.insert("C_8");
    
    // 更新持久性变量表，只更新有变化的变量
    for (auto it = currentVarMap.begin(); it != currentVarMap.end(); ++it) {
        if (!persistentVarMap.contains(it.key()) || persistentVarMap[it.key()] != it.value()) {
            persistentVarMap[it.key()] = it.value();
        }
    }
    
    // 保存dashForce的值，用于稍后更新dash1plot
    double dashForceValue = 0.0;
    bool dashForceUpdated = false;
    
    // 处理仪表盘更新
    for (Dashboard* dashboard : dashboards) {
        // 如果仪表盘名称不在映射中，跳过
        if (!dashboardMappings.contains(dashboard->objectName()))
            continue;
            
        DashboardMapping& mapping = dashboardMappings[dashboard->objectName()];
        double currentValue = 0.0;
        bool valueUpdated = false;
        
        // 如果是自定义变量，设置变量值后让仪表盘自行计算公式
        if (mapping.sourceType == DataSource_Custom) {
            dashboard->setVariableValues(persistentVarMap);
            currentValue = dashboard->getValue();
            valueUpdated = true;
        } else {
            // 根据数据源类型更新仪表盘值
            switch (mapping.sourceType) {
                case DataSource_Modbus:
                    // 只有当提供了Modbus数据且数据源类型为Modbus时才更新
                    if (!modbusData.isEmpty() && mapping.channelIndex < modbusData.size()) {
                        currentValue = modbusData[mapping.channelIndex];
                        dashboard->setValue(currentValue);
                        valueUpdated = true;
                    }
                    break;
                    
                case DataSource_DAQ:
                    // 只有当提供了DAQ数据且数据源类型为DAQ时才更新
                    if (!daqData.isEmpty() && mapping.channelIndex < daqData.size()) {
                        currentValue = daqData[mapping.channelIndex];
                        dashboard->setValue(currentValue);
                        valueUpdated = true;
                    }
                    break;
                    
                case DataSource_ECU:
                    // 只有当数据源类型为ECU时才更新
                    switch (mapping.channelIndex) {
                        case 0: currentValue = ecuData.throttle; dashboard->setValue(currentValue); valueUpdated = true; break;
                        case 1: currentValue = ecuData.engineSpeed; dashboard->setValue(currentValue); valueUpdated = true; break;
                        case 2: currentValue = ecuData.cylinderTemp; dashboard->setValue(currentValue); valueUpdated = true; break;
                        case 3: currentValue = ecuData.exhaustTemp; dashboard->setValue(currentValue); valueUpdated = true; break;
                        case 4: currentValue = ecuData.axleTemp; dashboard->setValue(currentValue); valueUpdated = true; break;
                        case 5: currentValue = ecuData.fuelPressure; dashboard->setValue(currentValue); valueUpdated = true; break;
                        case 6: currentValue = ecuData.intakeTemp; dashboard->setValue(currentValue); valueUpdated = true; break;
                        case 7: currentValue = ecuData.atmPressure; dashboard->setValue(currentValue); valueUpdated = true; break;
                        case 8: currentValue = ecuData.flightTime; dashboard->setValue(currentValue); valueUpdated = true; break;
                    }
                    break;
                    
                default:
                    break;
            }
        }
        
        // 保存dashForce的值，用于更新dash1plot
        if (dashboard->objectName() == "dashForce" && valueUpdated) {
            dashForceValue = currentValue;
            dashForceUpdated = true;
        }
        
        // 更新对应的LCD显示
        if (valueUpdated) {
            QString lcdName = "lcd" + dashboard->objectName().mid(4); // 例如 dashForce -> lcdForce
            QLCDNumber* lcd = findChild<QLCDNumber*>(lcdName);
            if (lcd) {
                // 保留三位小数显示到LCD
                lcd->display(QString::number(currentValue, 'f', 3));
            }
        }
    }
    
    // 处理自定义变量仪表盘的公式计算
    // 两轮迭代，确保所有自定义变量都能正确计算
    // 第一轮：计算所有直接基于物理设备变量的自定义变量
    for (Dashboard* dashboard : dashboards) {
        if (!dashboardMappings.contains(dashboard->objectName()))
            continue;
            
        DashboardMapping& mapping = dashboardMappings[dashboard->objectName()];
        
        // 如果是自定义变量且有公式，计算并更新值
        if (mapping.sourceType == DataSource_Custom && dashboard->isCustomVariable() && 
            !dashboard->getFormula().isEmpty() && !dashboard->getVariableName().isEmpty()) {
            // 计算公式并更新
            dashboard->setVariableValues(persistentVarMap);
            
            // 将计算结果添加到变量表中，供其他公式使用
            QString varName = dashboard->getVariableName();
            persistentVarMap[varName] = dashboard->getValue();
            updatedVars.insert(varName);
            
            // 检查是否是dashForce，如果是，保存值用于更新dash1plot
            if (dashboard->objectName() == "dashForce") {
                dashForceValue = dashboard->getValue();
                dashForceUpdated = true;
            }
            
            // 更新对应的LCD显示
            QString lcdName = "lcd" + dashboard->objectName().mid(4);
            QLCDNumber* lcd = findChild<QLCDNumber*>(lcdName);
            if (lcd) {
                lcd->display(QString::number(dashboard->getValue(), 'f', 3));
            }
        }
    }
    
    // 第二轮：重新计算，确保依赖于其他自定义变量的公式也能正确计算
    for (Dashboard* dashboard : dashboards) {
        if (!dashboardMappings.contains(dashboard->objectName()))
            continue;
            
        DashboardMapping& mapping = dashboardMappings[dashboard->objectName()];
        
        // 再次更新所有自定义变量
        if (mapping.sourceType == DataSource_Custom && dashboard->isCustomVariable()) {
            dashboard->setVariableValues(persistentVarMap);
            
            // 再次更新变量表
            QString varName = dashboard->getVariableName();
            persistentVarMap[varName] = dashboard->getValue();
            
            // 检查是否是dashForce，如果是，保存值用于更新dash1plot
            if (dashboard->objectName() == "dashForce") {
                dashForceValue = dashboard->getValue();
                dashForceUpdated = true;
            }
            
            // 更新对应的LCD显示
            QString lcdName = "lcd" + dashboard->objectName().mid(4);
            QLCDNumber* lcd = findChild<QLCDNumber*>(lcdName);
            if (lcd) {
                lcd->display(QString::number(dashboard->getValue(), 'f', 3));
            }
        }
    }
    
    // 更新dash1plot图表(如果dashForce值有更新)
    if (dashForceUpdated && ui->dash1plot && dashForceGraph) {
        updateDash1Plot(dashForceValue);
    }
}

// 添加WebSocket按钮的点击处理函数
void MainWindow::on_btnWebSocketControl_clicked()
{
    if (!wsTh) {
        QMessageBox::warning(this, "错误", "WebSocket对象未初始化！");
        return;
    }
    
    // 检查当前服务器状态
    if (!wsTh->isRunning()) {
        // 如果当前未运行，则启动服务器
        qDebug() << "启动WebSocket服务器...";
        wsTh->startServer(8080);
        QThread::msleep(100); // 等待服务器启动
        
        bool isRunning = wsTh->isRunning();
        qDebug() << "WebSocket服务器状态: " << (isRunning ? "运行中" : "未运行");
        
        if (isRunning) {
            ui->btnWebSocketControl->setText("停止WebSocket服务器");
            ui->btnWebSocketControl->setStyleSheet("background-color: #d9534f;"); // 红色
            ui->labelWebSocketStatus->setText("WS: 在线");
            ui->labelWebSocketStatus->setStyleSheet("color: green; font-weight: bold;");
            QMessageBox::information(this, "启动成功", "WebSocket服务器已启动！");
        } else {
            QMessageBox::warning(this, "启动失败", "WebSocket服务器启动失败！");
        }
    } else {
        // 如果当前正在运行，则停止服务器
        qDebug() << "停止WebSocket服务器...";
        wsTh->stopServer();
        
        ui->btnWebSocketControl->setText("启动WebSocket服务器");
        ui->btnWebSocketControl->setStyleSheet("background-color: #2d904c;"); // 绿色
        ui->labelWebSocketStatus->setText("WS: 离线");
        ui->labelWebSocketStatus->setStyleSheet("color: red; font-weight: bold;");
        QMessageBox::information(this, "已停止", "WebSocket服务器已停止！");
    }
}

// 添加WebSocket测试按钮的槽函数实现
void MainWindow::on_btnWebSocketTest_clicked()
{
    // 调用现有的测试连接方法
    testWebSocketConnection();
}

// 初始化默认的仪表盘映射关系
void MainWindow::initDefaultDashboardMappings()
{
    // 如果已有映射，则不初始化
    if (!dashboardMappings.isEmpty()) {
        return;
    }
    
    // 初始化拉力仪表盘
    DashboardMapping forceMapping;
    forceMapping.dashboardName = "dashForce";
    forceMapping.sourceType = DataSource_Modbus;
    forceMapping.channelIndex = 0;
    forceMapping.labelText = "拉力";
    forceMapping.unit = "N";
    forceMapping.minValue = 0;
    forceMapping.maxValue = 1000;
    forceMapping.pointerColor = QColor("#ff6b6b");
    forceMapping.pointerStyle = PointerStyle_Indicator;
    dashboardMappings["dashForce"] = forceMapping;
    
    // 初始化转速仪表盘
    DashboardMapping rpmMapping;
    rpmMapping.dashboardName = "dashRPM";
    rpmMapping.sourceType = DataSource_Modbus;
    rpmMapping.channelIndex = 1;
    rpmMapping.labelText = "转速";
    rpmMapping.unit = "rpm";
    rpmMapping.minValue = 0;
    rpmMapping.maxValue = 10000;
    rpmMapping.pointerColor = QColor("#4CAF50");
    rpmMapping.pointerStyle = PointerStyle_Indicator;
    dashboardMappings["dashRPM"] = rpmMapping;
    
    // 初始化油耗仪表盘
    DashboardMapping fuelMapping;
    fuelMapping.dashboardName = "dashFuelConsumption";
    fuelMapping.sourceType = DataSource_Modbus;
    fuelMapping.channelIndex = 2;
    fuelMapping.labelText = "油耗";
    fuelMapping.unit = "ml/min";
    fuelMapping.minValue = 0;
    fuelMapping.maxValue = 1000;
    fuelMapping.pointerColor = QColor("#ffd700");
    fuelMapping.pointerStyle = PointerStyle_Circle;
    dashboardMappings["dashFuelConsumption"] = fuelMapping;
    
    // 初始化火花塞下温度仪表盘
    DashboardMapping tempMapping;
    tempMapping.dashboardName = "dashSparkPlugTemp";
    tempMapping.sourceType = DataSource_Modbus;
    tempMapping.channelIndex = 3;
    tempMapping.labelText = "火花塞温度";
    tempMapping.unit = "°C";
    tempMapping.minValue = 0;
    tempMapping.maxValue = 900;
    tempMapping.pointerColor = QColor("#fa3232");
    tempMapping.pointerStyle = PointerStyle_Indicator;
    dashboardMappings["dashSparkPlugTemp"] = tempMapping;
    
    // 初始化功率仪表盘
    DashboardMapping powerMapping;
    powerMapping.dashboardName = "dashPower";
    powerMapping.sourceType = DataSource_Modbus;
    powerMapping.channelIndex = 4;
    powerMapping.labelText = "功率";
    powerMapping.unit = "kW";
    powerMapping.minValue = 0;
    powerMapping.maxValue = 1000;
    powerMapping.pointerColor = QColor("#2196f3");
    powerMapping.pointerStyle = PointerStyle_Indicator;
    dashboardMappings["dashPower"] = powerMapping;
    
    // 初始化扭矩仪表盘
    DashboardMapping torqueMapping;
    torqueMapping.dashboardName = "dashTorque";
    torqueMapping.sourceType = DataSource_Modbus;
    torqueMapping.channelIndex = 5;
    torqueMapping.labelText = "扭矩";
    torqueMapping.unit = "N·m";
    torqueMapping.minValue = 0;
    torqueMapping.maxValue = 500;
    torqueMapping.pointerColor = QColor("#9c27b0");
    torqueMapping.pointerStyle = PointerStyle_Indicator;
    dashboardMappings["dashTorque"] = torqueMapping;
    
    // 初始化推力仪表盘
    DashboardMapping thrustMapping;
    thrustMapping.dashboardName = "dashThrust";
    thrustMapping.sourceType = DataSource_Modbus;
    thrustMapping.channelIndex = 6;
    thrustMapping.labelText = "推力";
    thrustMapping.unit = "N";
    thrustMapping.minValue = 0;
    thrustMapping.maxValue = 1000;
    thrustMapping.pointerColor = QColor("#ff9800");
    thrustMapping.pointerStyle = PointerStyle_Indicator;
    dashboardMappings["dashThrust"] = thrustMapping;
    
    // 应用初始映射设置到仪表盘
    applyDashboardMappings();
}

// 修改数据监控表格初始化和更新的函数
void MainWindow::initMonitorTable()
{
    // 监控表格已被删除，保留空实现
}

// 更新监控表格数据
void MainWindow::updateMonitorTable()
{
    // 监控表格已被删除，保留空实现
}

// 应用加载的仪表盘映射关系到UI
void MainWindow::applyDashboardMappings()
{
    // 遍历所有仪表盘映射
    foreach (const QString &dashboardName, dashboardMappings.keys()) {
        const DashboardMapping &mapping = dashboardMappings[dashboardName];
        
        // 查找仪表盘对象
        Dashboard* dashboard = findChild<Dashboard*>(dashboardName);
        if (!dashboard) {
            qDebug() << "找不到仪表盘对象:" << dashboardName;
            continue;
        }
        
        // 更新仪表盘属性
        dashboard->setTitle(mapping.labelText);
        dashboard->setUnit(mapping.unit);
        dashboard->setMinValue(mapping.minValue);
        dashboard->setMaxValue(mapping.maxValue);
        dashboard->setPointerColor(mapping.pointerColor);
        dashboard->setPointerStyle((PointerStyle)mapping.pointerStyle);
        
        // 设置变量名和公式
        dashboard->setVariableName(mapping.variableName);
        dashboard->setFormula(mapping.formula);
        
        // 如果是自定义变量，设置自定义变量标志
        if (mapping.sourceType == DataSource_Custom) {
            dashboard->setCustomVariable(true);
        }
        
        // 如果是DAQ类型，设置DAQ通道列表
        if (mapping.sourceType == DataSource_DAQ) {
            // 检查是否已经有通道列表
            QVariant channelsVar = dashboard->property("daqChannels");
            if (!channelsVar.isValid() || channelsVar.value<QVector<int>>().isEmpty()) {
                // 没有通道列表，创建默认的
                QVector<int> channelList;
                channelList.append(mapping.channelIndex);
                dashboard->setProperty("daqChannels", QVariant::fromValue(channelList));
            }
        }
        
        // 新增: 更新对应的QLabel标签文本
        QString labelName = "label" + dashboardName.mid(4); // 例如dashForce -> labelForce
        QLabel* label = findChild<QLabel*>(labelName);
        if (label) {
            label->setText(mapping.labelText);
        } else {
            qDebug() << "找不到标签对象:" << labelName;
        }
        
        // 新增: 更新对应的QLCDNumber设置
        QString lcdName = "lcd" + dashboardName.mid(4); // 例如dashForce -> lcdForce
        QLCDNumber* lcd = findChild<QLCDNumber*>(lcdName);
        if (lcd) {
            // 设置LCD显示白色背景
            lcd->setStyleSheet(QString("background-color: white; color: %1;").arg(mapping.pointerColor.name()));
            
            // 设置小数位数为3
            lcd->setDigitCount(8); // 设置足够显示带3位小数的数字
            lcd->setSmallDecimalPoint(true);
            lcd->setSegmentStyle(QLCDNumber::Flat); // 使用扁平风格，更清晰
        } else {
            qDebug() << "找不到LCD对象:" << lcdName;
        }
    }
}

// 新增：根据DAQ通道设置更新仪表盘通道选项
void MainWindow::updateDashboardDAQChannels(const QString &channelsStr)
{
    // 解析通道字符串 (例如 "0/2/3/4/8")
    QStringList parts = channelsStr.split('/', Qt::SkipEmptyParts);
    QVector<int> channelNumbers;
    
    foreach (const QString &part, parts) {
        bool ok;
        int channelNum = part.trimmed().toInt(&ok);
        if (ok) {
            channelNumbers.append(channelNum);
        }
    }
    
    if (channelNumbers.isEmpty()) {
        qDebug() << "没有有效的DAQ通道号";
        return;
    }
    
    qDebug() << "解析到的DAQ通道:" << channelNumbers;
    
    // 更新所有仪表盘的DAQ通道属性
    QList<Dashboard*> dashboards = getAllDashboards();
    foreach (Dashboard* dashboard, dashboards) {
        // 为所有仪表盘设置DAQ通道列表
        dashboard->setProperty("daqChannels", QVariant::fromValue(channelNumbers));
        
        // 检查是否是DAQ数据源，如果是则更新变量名以匹配新的通道列表
        QString variableName = dashboard->getVariableName();
        if (variableName.startsWith("B_")) {
            bool ok;
            int currentChannel = variableName.mid(2).toInt(&ok);
            if (ok) {
                // 检查当前通道是否在新的通道列表中
                if (!channelNumbers.contains(currentChannel) && !channelNumbers.isEmpty()) {
                    // 如果不在，则使用新列表中的第一个通道
                    dashboard->setVariableName(QString("B_%1").arg(channelNumbers.first()));
                    
                    // 更新映射
                    if (dashboardMappings.contains(dashboard->objectName())) {
                        DashboardMapping &mapping = dashboardMappings[dashboard->objectName()];
                        mapping.variableName = QString("B_%1").arg(channelNumbers.first());
                        mapping.channelIndex = 0; // 重置为首个索引
                    }
                }
            }
        }
    }
    
    // 保存设置到INI文件
    QSettings settings("./settings.ini", QSettings::IniFormat);
    saveDashboardMappings(settings);
    
    qDebug() << "已更新所有仪表盘的DAQ通道列表";
}

// 处理仪表盘设置变更
void MainWindow::handleDashboardSettingsChanged(const QString &dashboardName, const QMap<QString, QVariant> &settings)
{
    qDebug() << "仪表盘设置变更:" << dashboardName;
    
    if (!dashboardMappings.contains(dashboardName)) {
        qDebug() << "仪表盘映射不存在:" << dashboardName;
        return;
    }
    
    DashboardMapping &mapping = dashboardMappings[dashboardName];
    bool needSave = false;
    
    // 处理标题/标签变更
    if (settings.contains("title")) {
        mapping.labelText = settings["title"].toString();
        needSave = true;
        
        // 更新对应的QLabel
        QString labelName = "label" + dashboardName.mid(4); // 例如 dashForce -> labelForce
        QLabel* label = findChild<QLabel*>(labelName);
        if (label) {
            label->setText(mapping.labelText);
        }
    }
    
    // 处理单位变更
    if (settings.contains("unit")) {
        mapping.unit = settings["unit"].toString();
        needSave = true;
    }
    
    // 处理最小值变更
    if (settings.contains("minValue")) {
        mapping.minValue = settings["minValue"].toDouble();
        needSave = true;
    }
    
    // 处理最大值变更
    if (settings.contains("maxValue")) {
        mapping.maxValue = settings["maxValue"].toDouble();
        needSave = true;
    }
    
    // 处理指针颜色变更
    if (settings.contains("pointerColor")) {
        mapping.pointerColor = settings["pointerColor"].value<QColor>();
        needSave = true;
        
        // 更新对应的LCD数字颜色
        QString lcdName = "lcd" + dashboardName.mid(4); // 例如 dashForce -> lcdForce
        QLCDNumber* lcd = findChild<QLCDNumber*>(lcdName);
        if (lcd) {
            lcd->setStyleSheet(QString("background-color: white; color: %1;").arg(mapping.pointerColor.name()));
        }
    }
    
    // 处理指针样式变更
    if (settings.contains("pointerStyle")) {
        mapping.pointerStyle = settings["pointerStyle"].toInt();
        needSave = true;
    }
    
    // 处理数据源类型变更
    if (settings.contains("sourceType")) {
        mapping.sourceType = (DataSourceType)settings["sourceType"].toInt();
        needSave = true;
    }
    
    // 处理通道索引变更
    if (settings.contains("channelIndex")) {
        mapping.channelIndex = settings["channelIndex"].toInt();
        needSave = true;
    }
    
    // 处理变量名变更
    if (settings.contains("variableName")) {
        mapping.variableName = settings["variableName"].toString();
        needSave = true;
    }
    
    // 处理公式变更
    if (settings.contains("formula")) {
        mapping.formula = settings["formula"].toString();
        needSave = true;
    }
    
    // 如果有设置变更，保存到配置文件
    if (needSave) {
        QString settingsFile = QCoreApplication::applicationDirPath() + "/dashboard_settings.ini";
        QSettings settings(settingsFile, QSettings::IniFormat);
        saveDashboardMappings(settings);
        qDebug() << "已保存仪表盘设置变更到:" << settingsFile;
    }
}

// 保存仪表盘映射关系到配置文件
void MainWindow::saveDashboardMappings(QSettings &settings)
{
    // 清除旧的映射组
    settings.remove("DashboardMappings");
    
    // 设置组
    settings.beginGroup("DashboardMappings");
    
    // 保存映射关系数量
    settings.setValue("Count", dashboardMappings.size());
    
    // 保存每个映射关系
    int index = 0;
    for (auto it = dashboardMappings.begin(); it != dashboardMappings.end(); ++it, ++index) {
        QString prefix = QString("Mapping%1_").arg(index);
        const DashboardMapping &mapping = it.value();
        
        settings.setValue(prefix + "DashboardName", mapping.dashboardName);
        settings.setValue(prefix + "SourceType", (int)mapping.sourceType);
        settings.setValue(prefix + "ChannelIndex", mapping.channelIndex);
        settings.setValue(prefix + "LabelText", mapping.labelText);
        settings.setValue(prefix + "Unit", mapping.unit);
        settings.setValue(prefix + "MinValue", mapping.minValue);
        settings.setValue(prefix + "MaxValue", mapping.maxValue);
        settings.setValue(prefix + "PointerColor", mapping.pointerColor.name());
        settings.setValue(prefix + "PointerStyle", mapping.pointerStyle);
        
        // 保存变量名和公式
        settings.setValue(prefix + "VariableName", mapping.variableName);
        settings.setValue(prefix + "Formula", mapping.formula);
    }
    
    settings.endGroup();
    settings.sync();
    
    qDebug() << "已保存" << dashboardMappings.size() << "个仪表盘映射关系到配置文件";
}

// 从配置文件加载仪表盘映射关系
void MainWindow::loadDashboardMappings(QSettings &settings)
{
    // 清空当前映射
    dashboardMappings.clear();
    
    // 设置组
    settings.beginGroup("DashboardMappings");
    
    // 读取映射关系数量
    int count = settings.value("Count", 0).toInt();
    
    // 读取每个映射关系
    for (int index = 0; index < count; ++index) {
        QString prefix = QString("Mapping%1_").arg(index);
        
        DashboardMapping mapping;
        mapping.dashboardName = settings.value(prefix + "DashboardName").toString();
        mapping.sourceType = (DataSourceType)settings.value(prefix + "SourceType").toInt();
        mapping.channelIndex = settings.value(prefix + "ChannelIndex").toInt();
        mapping.labelText = settings.value(prefix + "LabelText").toString();
        mapping.unit = settings.value(prefix + "Unit").toString();
        mapping.minValue = settings.value(prefix + "MinValue").toDouble();
        mapping.maxValue = settings.value(prefix + "MaxValue").toDouble();
        mapping.pointerColor = QColor(settings.value(prefix + "PointerColor").toString());
        mapping.pointerStyle = settings.value(prefix + "PointerStyle").toInt();
        
        // 加载变量名和公式
        mapping.variableName = settings.value(prefix + "VariableName").toString();
        mapping.formula = settings.value(prefix + "Formula").toString();
        
        // 添加到映射中
        dashboardMappings[mapping.dashboardName] = mapping;
    }
    
    settings.endGroup();
    
    // 应用加载的设置
    applyDashboardMappings();
    
    qDebug() << "已加载" << dashboardMappings.size() << "个仪表盘映射关系";
    
    // 添加详细调试信息
    foreach (const QString &dashboardName, dashboardMappings.keys()) {
        const DashboardMapping &mapping = dashboardMappings[dashboardName];
        qDebug() << "仪表盘:" << dashboardName 
                 << "标签:" << mapping.labelText
                 << "数据源:" << mapping.sourceType
                 << "通道:" << mapping.channelIndex
                 << "颜色:" << mapping.pointerColor.name();
    }
}

// 获取所有仪表盘对象
QList<Dashboard*> MainWindow::getAllDashboards()
{
    // 使用findChildren查找所有Dashboard类型的子控件
    return this->findChildren<Dashboard*>();
}


// 新增：根据Modbus寄存器地址和数量更新Modbus通道
void MainWindow::updateModbusChannels()
{
    // 获取寄存器起始地址和数量
    int startAddress = ui->lineSegAddress->text().toInt();
    int registerCount = ui->lineSegNum->text().toInt();
    
    // 确保寄存器数量至少为1
    if (registerCount < 1) {
        registerCount = 1;
    }
    
    // 创建通道编号列表
    QVector<int> channelNumbers;
    for (int i = 0; i < registerCount; i++) {
        channelNumbers.append(startAddress + i);
    }
    
    qDebug() << "Modbus通道更新: 起始地址=" << startAddress << ", 数量=" << registerCount;
    
    // 更新所有仪表盘的Modbus通道属性
    QList<Dashboard*> dashboards = getAllDashboards();
    foreach (Dashboard* dashboard, dashboards) {
        // 为所有仪表盘设置Modbus通道列表
        dashboard->setProperty("modbusChannels", QVariant::fromValue(channelNumbers));
        
        // 检查是否是Modbus数据源，如果是则更新变量名以匹配新的通道列表
        QString variableName = dashboard->getVariableName();
        if (variableName.startsWith("A_")) {
            bool ok;
            int currentChannel = variableName.mid(2).toInt(&ok);
            if (ok) {
                // 检查当前通道是否在新的通道列表中
                if (!channelNumbers.contains(currentChannel) && !channelNumbers.isEmpty()) {
                    // 如果不在，则使用新列表中的第一个通道
                    dashboard->setVariableName(QString("A_%1").arg(channelNumbers.first()));
                    
                    // 更新映射
                    if (dashboardMappings.contains(dashboard->objectName())) {
                        DashboardMapping &mapping = dashboardMappings[dashboard->objectName()];
                        mapping.variableName = QString("A_%1").arg(channelNumbers.first());
                        mapping.channelIndex = channelNumbers.first() - startAddress; // 更新索引
                    }
                }
            }
        }
    }
    
    // 保存设置到INI文件
    QSettings settings("./settings.ini", QSettings::IniFormat);
    saveDashboardMappings(settings);
    
    qDebug() << "已更新所有仪表盘的Modbus通道列表";
    
    // 如果当前正在采集数据，可能需要重新初始化图表
    if (ui->btnSend->text() == "结束") {
        channelNum = registerCount;
        myplotInit(myPlot);
    }
}

// 前向声明，以便在ui_mainwindow.h中找不到这些元素时使用
static QCustomPlot* getECUCustomPlot(Ui::MainWindow* ui) {
    // 尝试通过名称查找控件
    QCustomPlot* plot = ui->centralwidget->findChild<QCustomPlot*>("ECUCustomPlot");
    if (!plot) {
        qDebug() << "警告: 无法通过名称找到ECUCustomPlot控件";
    }
    return plot;
}

static QGroupBox* getGroupBoxECUplot(Ui::MainWindow* ui) {
    // 尝试通过名称查找控件
    QGroupBox* groupBox = ui->centralwidget->findChild<QGroupBox*>("groupBoxECUplot");
    if (!groupBox) {
        qDebug() << "警告: 无法通过名称找到groupBoxECUplot控件";
    }
    return groupBox;
}

static QVBoxLayout* getVerticalLayout_8(Ui::MainWindow* ui) {
    // 尝试通过名称查找布局
    QGroupBox* groupBox = getGroupBoxECUplot(ui);
    if (groupBox && groupBox->layout()) {
        return qobject_cast<QVBoxLayout*>(groupBox->layout());
    }
    return nullptr;
}

void MainWindow::ECUPlotInit()
{
    qDebug() << "开始初始化ECU图表";
    
    // 使用ui中定义的ECUCustomPlot控件
    QCustomPlot *ecuPlot = ui->ECUCustomPlot;
    if (!ecuPlot) {
        qDebug() << "错误: ECUCustomPlot控件不存在";
        return;
    }
    
    // 清除现有的图表
    ecuPlot->clearGraphs();
    ecuPlot->clearItems();
    
    // 设置图表标题
    ecuPlot->plotLayout()->insertRow(0);
    QCPTextElement *title = new QCPTextElement(ecuPlot, "ECU数据监控", QFont("sans", 12, QFont::Bold));
    ecuPlot->plotLayout()->addElement(0, 0, title);
    
    // 创建参数名称和颜色映射
    QStringList names = {
        "喷头(%)", 
        "发动机转速(rpm)", 
        "缸温(℃)", 
        "排温(℃)", 
        "轴温(℃)", 
        "燃油压力(kPa)", 
        "进气温度(℃)", 
        "大气压力(kPa)", 
        "飞行时间(s)"
    };
    
    QStringList colors = {
        "blue", "red", "green", "magenta", "cyan", 
        "darkBlue", "darkRed", "darkGreen", "darkCyan"
    };
    
    // 为每个参数创建一个图表线条
    for (int i = 0; i < names.size(); i++) {
        ecuPlot->addGraph();
        ecuPlot->graph(i)->setPen(QPen(QColor(colors[i % colors.size()]), 2)); // 增加线条宽度
        ecuPlot->graph(i)->setName(names[i]); // 设置图例名称
    }
    
    // 设置图表样式
    ecuPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    ecuPlot->axisRect()->setupFullAxesBox(true);
    ecuPlot->xAxis->setLabel("时间(秒)");
    ecuPlot->yAxis->setLabel("数值");
    
    // 创建图例
    ecuPlot->legend->setVisible(true);
    ecuPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 200)));
    ecuPlot->legend->setBorderPen(Qt::NoPen);
    ecuPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignTop);
    
    // 初始化数据容器
    ecuTimeData.clear();
    ecuData.clear();
    ecuData.resize(names.size());
    
    // 设置初始轴范围
    ecuPlot->xAxis->setRange(0, 60); // 初始显示60秒
    ecuPlot->yAxis->setRange(0, 100); // 初始范围0-100
    ecuPlot->replot();
    
    qDebug() << "ECU图表初始化完成，通道数: " << ecuPlot->graphCount();
}

void MainWindow::updateECUPlot(const ECUData &data)
{
    try {
        // 确保图表已初始化 - 如果未初始化则强制初始化
        QCustomPlot *ecuPlot = ui->ECUCustomPlot;
        if (!ecuPlot || ecuPlot->graphCount() < 9) {
            ECUPlotInit();
            ecuPlot = ui->ECUCustomPlot; // 重新获取指针，以防初始化改变了它
            if (!ecuPlot || ecuPlot->graphCount() < 9) {
                qDebug() << "ECU图表未正确初始化";
                return;
            }
        }
        
        // 改变条件，即使没有数据也允许初始设置图表
        // 这样可以保证图表框架是可见的，即使暂时没有数据点
        if (ecuTimeData.isEmpty()) {
            double currentTime = masterTimer ? (masterTimer->elapsed() / 1000.0) : 0.0;
            ecuTimeData.append(currentTime);
            
            // 初始化数据向量，确保它们存在
            if (ecuData.isEmpty()) {
                for (int i = 0; i < 9; ++i) {
                    QVector<double> dataVector;
                    dataVector.append(0.0); // 添加初始0值
                    ecuData.append(dataVector);
                }
            }
        }
        
        // 将新数据点添加到图表
        QVector<double> values(9);
        values[0] = data.throttle;
        values[1] = data.engineSpeed;
        values[2] = data.cylinderTemp;
        values[3] = data.exhaustTemp;
        values[4] = data.axleTemp;
        values[5] = data.fuelPressure;
        values[6] = data.intakeTemp;
        values[7] = data.atmPressure;
        values[8] = data.flightTime;
        
        // 获取最新的时间范围
        double latestTime = ecuTimeData.last();
        double timeRange = 30.0; // 显示最近30秒的数据
        double minX = qMax(0.0, latestTime - timeRange);
        
        // 更新每个通道的数据
        for (int i = 0; i < qMin(ecuData.size(), 9); ++i) {
            if (i < ecuPlot->graphCount() && i < ecuData.size()) {
                // 确保时间向量和数据向量长度一致
                int dataSize = qMin(ecuTimeData.size(), ecuData[i].size());
                if (dataSize > 0) {
                    QVector<double> timeVector = ecuTimeData.mid(ecuTimeData.size() - dataSize);
                    QVector<double> dataVector = ecuData[i].mid(ecuData[i].size() - dataSize);
                    
                    // 更新图表数据
                    ecuPlot->graph(i)->setData(timeVector, dataVector);
                    
                    // 调试信息
                    qDebug() << "ECU图表" << i << "更新: 时间点数=" << timeVector.size() 
                             << " 数据点数=" << dataVector.size()
                             << " 最新值=" << (dataVector.isEmpty() ? 0.0 : dataVector.last());
                }
            }
        }
        
        // 设置X轴范围以显示最近的数据
        ecuPlot->xAxis->setRange(minX, latestTime);
        
        // 自动调整Y轴范围以适应所有可见数据
        ecuPlot->rescaleAxes();
        
        // 确保Y轴有合理的范围
        double yMin = ecuPlot->yAxis->range().lower;
        double yMax = ecuPlot->yAxis->range().upper;
        if (qAbs(yMax - yMin) < 0.1) {
            // 如果范围太小，设置一个默认范围
            ecuPlot->yAxis->setRange(yMin - 1.0, yMax + 1.0);
        }
        
        // 刷新图表 - 使用立即重绘，而不是队列化重绘
        ecuPlot->replot(QCustomPlot::rpImmediateRefresh);
    } catch (const std::exception& e) {
        qDebug() << "更新ECU图表时出错: " << e.what();
    } catch (...) {
        qDebug() << "更新ECU图表时发生未知错误";
    }
}

// 添加新的ECU数据点
void MainWindow::addECUDataPoint(double timePoint, const QVector<double> &values)
{
    // 确保数据向量大小一致
    if (values.size() != 9) {
        qDebug() << "ECU数据点大小错误: " << values.size();
        return;
    }
    
    // 添加调试信息
    qDebug() << "添加ECU数据点: 时间=" << timePoint << ", 数据点=" << values;
    
    // 使用统一的时间点，确保与masterTimer同步
    ecuTimeData.append(timePoint);
    
    // 限制数据点数量，防止内存过大
    const int maxDataPoints = 1000;
    while (ecuTimeData.size() > maxDataPoints) {
        ecuTimeData.removeFirst();
    }
    
    // 确保所有ECU数据向量已初始化
    if (ecuData.isEmpty()) {
        for (int i = 0; i < 9; ++i) {
            QVector<double> dataVector;
            ecuData.append(dataVector);
        }
    }
    
    // 确保ecuData大小正确 - 如果大小不一致，重置它
    if (ecuData.size() != 9) {
        ecuData.clear();
        for (int i = 0; i < 9; ++i) {
            QVector<double> dataVector;
            ecuData.append(dataVector);
        }
    }
    
    // 添加每个ECU数据点
    for (int i = 0; i < values.size() && i < ecuData.size(); ++i) {
        ecuData[i].append(values[i]);
        
        // 限制数据点数量
        while (ecuData[i].size() > maxDataPoints) {
                ecuData[i].removeFirst();
            }
        }
    }
    
void MainWindow::updateECUDataDisplay(const QVector<double> &timeData, const DataSnapshot &snapshot)
{
    try {
        if (!snapshot.ecuValid || snapshot.ecuData.isEmpty() || snapshot.ecuData.size() < 9) {
            return;
        }
        
        // 确保ECU表格已初始化
        if (!ecuDataModel || ecuDataModel->columnCount() == 0) {
            setupECUDataTable();
            if (!ecuDataModel) {
                qDebug() << "ECU表格初始化失败";
                return;
            }
        }
        
        // 转换为ECU数据结构
        ECUData ecuData;
        ecuData.isValid = true;
        ecuData.throttle = snapshot.ecuData[0];
        ecuData.engineSpeed = snapshot.ecuData[1];
        ecuData.cylinderTemp = snapshot.ecuData[2];
        ecuData.exhaustTemp = snapshot.ecuData[3];
        ecuData.axleTemp = snapshot.ecuData[4];
        ecuData.fuelPressure = snapshot.ecuData[5];
        ecuData.intakeTemp = snapshot.ecuData[6];
        ecuData.atmPressure = snapshot.ecuData[7];
        ecuData.flightTime = snapshot.ecuData[8];
        
        // 使用快照的时间戳确保与其他数据源同步
        double snapshotTime = timeData.isEmpty() ? snapshot.timestamp : timeData.first();
        
        // 调试输出添加的数据
        qDebug() << "更新ECU表格: 时间=" << snapshotTime 
                 << ", 喷头=" << ecuData.throttle
                 << ", 转速=" << ecuData.engineSpeed;
        
        // 更新ECU表格 - 使用横向表格格式(类似DAQ表格)
        QList<QStandardItem*> rowItems;
        
        // 添加时间戳 - 使用快照的统一时间戳
        rowItems.append(new QStandardItem(QString::number(snapshotTime, 'f', 3)));
        
        // 添加各项数据
        rowItems.append(new QStandardItem(QString::number(ecuData.throttle, 'f', 1)));
        rowItems.append(new QStandardItem(QString::number(ecuData.engineSpeed, 'f', 0)));
        rowItems.append(new QStandardItem(QString::number(ecuData.cylinderTemp, 'f', 0)));
        rowItems.append(new QStandardItem(QString::number(ecuData.exhaustTemp, 'f', 0)));
        rowItems.append(new QStandardItem(QString::number(ecuData.axleTemp, 'f', 0)));
        rowItems.append(new QStandardItem(QString::number(ecuData.fuelPressure, 'f', 1)));
        rowItems.append(new QStandardItem(QString::number(ecuData.intakeTemp, 'f', 1)));
        rowItems.append(new QStandardItem(QString::number(ecuData.atmPressure, 'f', 1)));
        rowItems.append(new QStandardItem(QString::number(ecuData.flightTime, 'f', 1)));
        
        // 添加新行
        ecuDataModel->appendRow(rowItems);
        
        // 限制行数，防止表格过大
        while (ecuDataModel->rowCount() > 100) {
            ecuDataModel->removeRow(0);
        }
        
        // 直接将快照数据和时间戳添加到ecuData和ecuTimeData中
        QVector<double> ecuValues = snapshot.ecuData;
        addECUDataPoint(snapshotTime, ecuValues);
        
        // 更新ECU图表 - 使用快照的统一时间戳
        updateECUPlot(ecuData);
    } catch (const std::exception& e) {
        qDebug() << "更新ECU数据显示时出错: " << e.what();
    } catch (...) {
        qDebug() << "更新ECU数据显示时发生未知错误";
    }
}

void MainWindow::setupECUDataTable()
{
    /*
    qDebug() << "开始设置ECU数据表格";
    
    // 初始化ECU数据模型
    if (ecuDataModel) {
        delete ecuDataModel;
    }
    
    // 创建新模型
    ecuDataModel = new QStandardItemModel(this);
    
    // 设置列标签
    QStringList columnLabels;
    columnLabels << "时间(秒)" << "喷头(%)" << "发动机转速(rpm)" << "缸温(℃)" 
                 << "排温(℃)" << "轴温(℃)" << "燃油压力(kPa)" << "进气温度(℃)" 
                 << "大气压力(kPa)" << "飞行时间(s)";
    
    ecuDataModel->setHorizontalHeaderLabels(columnLabels);
    
    // 确保表格控件存在
    if (!ui->tableViewECU) {
        qDebug() << "错误: tableViewECU控件不存在";
        return;
    }
    
    // 设置表格模型
    ui->tableViewECU->setModel(ecuDataModel);
    
    // 调整表格外观
    ui->tableViewECU->verticalHeader()->setVisible(false); // 隐藏行头
    ui->tableViewECU->verticalHeader()->setDefaultSectionSize(25); // 行高
    ui->tableViewECU->horizontalHeader()->setDefaultSectionSize(80); // 设置默认列宽
    ui->tableViewECU->setAlternatingRowColors(true); // 交替行颜色
    ui->tableViewECU->setSelectionMode(QAbstractItemView::SingleSelection); // 单选模式
    
    // 设置表格为只读
    ui->tableViewECU->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 自动调整列宽以适应内容
    ui->tableViewECU->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    
    qDebug() << "ECU数据表格设置完成";
    */
}

// 初始化dash1plot绘图区域
void MainWindow::setupDash1Plot()
{
    // 首先断开之前的所有连接，防止重复连接
    disconnect(ui->dashForce, &Dashboard::valueChanged, this, &MainWindow::updateDash1Plot);
    disconnect(ui->dashForce, &Dashboard::rangeChanged, this, nullptr);
    
    // 清空数据
    dashForceTimeData.clear();
    dashForceValueData.clear();
    
    // 获取自定义图表控件
    QCustomPlot *plot = ui->dash1plot;
    if (!plot) {
        qDebug() << "错误：dash1plot控件未找到";
        return;
    }
    
    // 确保dashForce仪表盘存在
    if (!ui->dashForce) {
        qDebug() << "错误：dashForce仪表盘不存在";
        return;
    }
    
    // 移除之前的事件过滤器
    plot->removeEventFilter(this);
    
    // 设置图表样式
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plot->setNoAntialiasingOnDrag(true);
    plot->setNotAntialiasedElements(QCP::aeAll);
    
    // 获取dashForce仪表盘的属性
    QString title = ui->dashForce->getTitle();
    QString unit = ui->dashForce->getUnit();
    if (unit.isEmpty()) {
        unit = "N"; // 默认单位
    }
    
    // 获取dashForce在映射中的信息
    QString customTitle = title;
    if (dashboardMappings.contains("dashForce")) {
        DashboardMapping mapping = dashboardMappings["dashForce"];
        if (!mapping.unit.isEmpty()) {
            unit = mapping.unit;
        }
        if (!mapping.labelText.isEmpty()) {
            customTitle = mapping.labelText;
        }
    }
    
    // 检查是否已有标题元素
    QCPTextElement *titleElement = nullptr;
    if (plot->plotLayout()->rowCount() > 0) {
        titleElement = qobject_cast<QCPTextElement*>(plot->plotLayout()->element(0, 0));
    }
    
    // 如果没有标题元素，则添加一个
    if (!titleElement) {
        // 添加标题行
        plot->plotLayout()->insertRow(0);
        titleElement = new QCPTextElement(plot, customTitle, QFont("sans", 12, QFont::Bold));
        plot->plotLayout()->addElement(0, 0, titleElement);
    } else {
        // 如果已有标题元素，直接更新文本
        titleElement->setText(customTitle);
    }
    
    // 设置坐标轴
    plot->xAxis->setLabel("时间 (秒)");
    plot->yAxis->setLabel(customTitle + " (" + unit + ")");
    plot->xAxis->setRange(0, 60);
    
    // 获取dashForce的范围
    double minValue = ui->dashForce->getMinValue();
    double maxValue = ui->dashForce->getMaxValue();
    plot->yAxis->setRange(minValue, maxValue);
    
    // 创建图表（先清除旧图表）
    plot->clearGraphs();
    dashForceGraph = plot->addGraph();
    
    // 使用dashForce的指针颜色
    QColor pointerColor = ui->dashForce->getPointerColor();
    dashForceGraph->setPen(QPen(pointerColor, 2));
    
    // 创建右上角的值标签
    if (dashForceValueLabel) {
        delete dashForceValueLabel;
        dashForceValueLabel = nullptr;
    }
    dashForceValueLabel = new QLabel("0.00 " + unit, plot);
    dashForceValueLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 14px; background-color: rgba(255, 255, 255, 180);").arg(pointerColor.name()));
    dashForceValueLabel->setAlignment(Qt::AlignCenter);
    dashForceValueLabel->setFixedSize(100, 25);
    dashForceValueLabel->move(plot->width() - 110, 10);
    
    // 创建右侧游标标签
    if (dashForceArrowLabel) {
        delete dashForceArrowLabel;
        dashForceArrowLabel = nullptr;
    }
    dashForceArrowLabel = new QLabel("←", plot);
    dashForceArrowLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 18px;").arg(pointerColor.name()));
    dashForceArrowLabel->setAlignment(Qt::AlignCenter);
    dashForceArrowLabel->setFixedSize(20, 20);
    
    // 设置值标签和游标可见
    dashForceValueLabel->show();
    dashForceArrowLabel->show();
    
    // 添加事件过滤器以处理窗口大小变化
    plot->installEventFilter(this);
    
    // 连接仪表盘的信号
    // 1. 值变化信号 - 更新图表
    connect(ui->dashForce, &Dashboard::valueChanged, this, &MainWindow::updateDash1Plot);
    
    // 2. 范围变化信号 - 调整Y轴
    connect(ui->dashForce, &Dashboard::rangeChanged, this, [this]() {
        QCustomPlot *plot = ui->dash1plot;
        if (plot) {
            double minValue = ui->dashForce->getMinValue();
            double maxValue = ui->dashForce->getMaxValue();
            plot->yAxis->setRange(minValue, maxValue);
            plot->replot();
        }
    });
    
    // 更新图表显示
    plot->replot();
    
    qDebug() << "成功初始化dash1plot图表，标题:" << customTitle << "，单位:" << unit;
}

// 更新dash1plot数据和显示
void MainWindow::updateDash1Plot(double value)
{
    // 获取自定义图表控件
    QCustomPlot *plot = ui->dash1plot;
    if (!plot || !dashForceGraph) {
        qDebug() << "错误：dash1plot或dashForceGraph无效";
        return;
    }
    
    // 确保图表有有效的axisRect
    if (!plot->axisRect()) {
        qDebug() << "错误：dash1plot的axisRect无效";
        return;
    }
    
    // 更新时间计数器（每次增加0.1秒，与100ms刷新率匹配）
    dashPlotTimeCounter += 0.1;
    
    // 添加新数据点
    dashForceTimeData.append(dashPlotTimeCounter);
    dashForceValueData.append(value);
    
    // 限制数据点数量（保留最近600个点，相当于60秒数据）
    const int maxPoints = 600;
    while (dashForceTimeData.size() > maxPoints) {
        dashForceTimeData.removeFirst();
        dashForceValueData.removeFirst();
    }
    
    // 更新图表数据
    dashForceGraph->setData(dashForceTimeData, dashForceValueData);
    
    // 更新X轴范围，始终显示最近60秒的数据
    double maxTime = dashPlotTimeCounter;
    double minTime = qMax(0.0, maxTime - 60.0);
    plot->xAxis->setRange(minTime, maxTime);
    
    // 获取当前仪表盘的单位、标题和范围
    if (!ui->dashForce) {
        qDebug() << "错误：dashForce仪表盘不存在";
        return;
    }
    
    QString unit = ui->dashForce->getUnit();
    QString customTitle = ui->dashForce->getTitle();
    double dashMinValue = ui->dashForce->getMinValue();
    double dashMaxValue = ui->dashForce->getMaxValue();
    
    // 从映射中获取更详细的信息
    if (dashboardMappings.contains("dashForce")) {
        DashboardMapping mapping = dashboardMappings["dashForce"];
        if (!mapping.unit.isEmpty()) {
            unit = mapping.unit;
        }
        if (!mapping.labelText.isEmpty()) {
            customTitle = mapping.labelText;
        }
    }
    
    // 更新Y轴标签（如果单位或标题变化）
    QString currentYLabel = customTitle + " (" + unit + ")";
    if (plot->yAxis->label() != currentYLabel) {
        plot->yAxis->setLabel(currentYLabel);
    }
    
    // 自动调整Y轴范围（在数据范围基础上增加10%余量）
    if (!dashForceValueData.isEmpty()) {
        double minValue = *std::min_element(dashForceValueData.begin(), dashForceValueData.end());
        double maxValue = *std::max_element(dashForceValueData.begin(), dashForceValueData.end());
        double margin = (maxValue - minValue) * 0.1;
        if (margin < 0.1) margin = 0.1; // 确保至少有一定的边距
        
        // 确保最小值不小于仪表盘最小值
        minValue = qMax(dashMinValue, minValue - margin);
        // 确保最大值不超过仪表盘最大值
        maxValue = qMin(dashMaxValue, maxValue + margin);
        
        // 设置Y轴范围
        plot->yAxis->setRange(minValue, maxValue);
    }
    
    // 检查标签是否存在
    if (!dashForceValueLabel || !dashForceArrowLabel) {
        qDebug() << "错误：dash1plot的值标签或游标标签不存在";
        return;
    }
    
    // 更新右上角显示的数值（带单位）
    dashForceValueLabel->setText(QString("%1 %2").arg(value, 0, 'f', 2).arg(unit));
    
    // 确保游标和值标签颜色与仪表盘指针颜色一致
    QColor pointerColor = ui->dashForce->getPointerColor();
    dashForceValueLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 14px; background-color: rgba(255, 255, 255, 180);").arg(pointerColor.name()));
    dashForceArrowLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 18px;").arg(pointerColor.name()));
    
    // 确保图表线条颜色与仪表盘指针颜色一致
    QPen graphPen = dashForceGraph->pen();
    if (graphPen.color() != pointerColor) {
        graphPen.setColor(pointerColor);
        dashForceGraph->setPen(graphPen);
    }
    
    // 更新游标位置 - 固定在右侧，指向左边
    if (!dashForceValueData.isEmpty()) {
        // 获取最新数据点的值
        double lastValue = dashForceValueData.last();
        
        // 将数据坐标转换为像素坐标（只需要Y坐标）
        double pixelY = plot->yAxis->coordToPixel(lastValue);
        
        // 固定X位置在图表最右侧，Y位置根据数据点调整
        int arrowX = plot->width() - dashForceArrowLabel->width() - 5; // 距离右边缘5像素
        
        // 边界检查，确保游标在可见区域内
        pixelY = qBound(10.0, pixelY, (double)(plot->height() - 30.0));
        
        // 移动游标到最右侧，高度对应最新数据点
        dashForceArrowLabel->move(arrowX, pixelY - 10);
    }
    
    // 使用QueuedReplot模式避免频繁刷新带来的性能问题
    plot->replot(QCustomPlot::rpQueuedReplot);
}

// 添加事件过滤器，处理图表大小变化
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // 检查是否为dash1plot的大小变化事件
    if (watched == ui->dash1plot && event->type() == QEvent::Resize) {
        QCustomPlot *plot = ui->dash1plot;
        
        // 更新标签位置
        if (dashForceValueLabel) {
            dashForceValueLabel->move(plot->width() - 110, 10);
        }
        
        // 更新游标位置 - 保持在右侧
        if (dashForceArrowLabel && !dashForceValueData.isEmpty()) {
            // 获取最新数据点的值
            double lastValue = dashForceValueData.last();
            
            // 计算新的游标Y位置
            double pixelY = plot->yAxis->coordToPixel(lastValue);
            
            // 固定X位置在图表最右侧
            int arrowX = plot->width() - dashForceArrowLabel->width() - 5;
            
            // 边界检查
            pixelY = qBound(10.0, pixelY, (double)(plot->height() - 30.0));
            
            // 移动游标
            dashForceArrowLabel->move(arrowX, pixelY - 10);
        }
        
        // 重绘图表
        plot->replot();
        return false; // 继续处理事件
    }
    
    // 调用基类方法处理其他事件
    return QMainWindow::eventFilter(watched, event);
}

// 专门处理dashForce仪表盘设置变更
void MainWindow::handleDashForceSettingsChanged(const QString &dashboardName, const QMap<QString, QVariant> &settings)
{
    // 只处理dashForce仪表盘
    if (dashboardName != "dashForce") {
        return;
    }
    
    qDebug() << "dashForce设置已变更，更新相关组件...";
    
    // 确保dashForce仪表盘存在
    if (!ui->dashForce) {
        qDebug() << "错误：dashForce仪表盘不存在";
        return;
    }
    
    // 1. 更新映射
    if (dashboardMappings.contains("dashForce")) {
        DashboardMapping &mapping = dashboardMappings["dashForce"];
        
        // 更新数据源类型
        if (settings.contains("deviceType")) {
            mapping.sourceType = (DataSourceType)settings["deviceType"].toInt();
        }
        
        // 更新通道索引
        if (settings.contains("channelIndex")) {
            mapping.channelIndex = settings["channelIndex"].toInt();
        }
        
        // 更新标签文本
        if (settings.contains("title")) {
            mapping.labelText = settings["title"].toString();
        }
        
        // 更新单位
        if (settings.contains("unit")) {
            mapping.unit = settings["unit"].toString();
            
            // 安全地尝试更新图表标签
            QCustomPlot *plot = ui->dash1plot;
            if (plot && plot->yAxis && !mapping.unit.isEmpty()) {
                try {
                    QString currentYLabel = mapping.labelText + " (" + mapping.unit + ")";
                    plot->yAxis->setLabel(currentYLabel);
                    
                    // 安全地更新值标签
                    if (dashForceValueLabel) {
                        double currentValue = ui->dashForce->getValue();
                        dashForceValueLabel->setText(QString("%1 %2").arg(currentValue, 0, 'f', 2).arg(mapping.unit));
                    }
                    
                    // 只在确认安全的情况下重绘
                    if (plot->axisRect()) {
                        // 确保游标位置正确
                        if (dashForceArrowLabel && !dashForceValueData.isEmpty()) {
                            // 获取最新数据点的值
                            double lastValue = dashForceValueData.last();
                            
                            // 计算游标位置
                            double pixelY = plot->yAxis->coordToPixel(lastValue);
                            int arrowX = plot->width() - dashForceArrowLabel->width() - 5;
                            
                            // 边界检查
                            pixelY = qBound(10.0, pixelY, (double)(plot->height() - 30.0));
                            
                            // 更新游标位置
                            dashForceArrowLabel->move(arrowX, pixelY - 10);
                        }
                        
                        plot->replot();
                    }
                } catch (...) {
                    qDebug() << "更新dash1plot标签时发生错误";
                }
            }
        }
        
        // 更新取值范围
        if (settings.contains("minValue")) {
            mapping.minValue = settings["minValue"].toDouble();
        }
        if (settings.contains("maxValue")) {
            mapping.maxValue = settings["maxValue"].toDouble();
        }
        
        // 更新指针颜色
        if (settings.contains("pointerColor")) {
            mapping.pointerColor = QColor(settings["pointerColor"].toString());
        }
        
        // 更新指针样式
        if (settings.contains("pointerStyle")) {
            mapping.pointerStyle = (PointerStyle)settings["pointerStyle"].toInt();
        }
        
        // 更新变量名称（对自定义变量很重要）
        if (settings.contains("variableName")) {
            mapping.variableName = settings["variableName"].toString();
        }
        
        // 更新公式（对自定义变量很重要）
        if (settings.contains("formula")) {
            mapping.formula = settings["formula"].toString();
        }
        
        qDebug() << "dashForce映射已更新: 数据源=" << mapping.sourceType
                 << ", 通道=" << mapping.channelIndex
                 << ", 变量=" << mapping.variableName;
    }
    
    // 2. 保存映射到配置文件
    QString settingsFile = QCoreApplication::applicationDirPath() + "/dashboard_settings.ini";
    QSettings configSettings(settingsFile, QSettings::IniFormat);
    saveDashboardMappings(configSettings);
    
    // 3. 尝试安全地更新dash1plot
    try {
        setupDash1Plot();
    } catch (const std::exception &e) {
        qDebug() << "更新dash1plot时发生错误:" << e.what();
    } catch (...) {
        qDebug() << "更新dash1plot时发生未知错误";
    }
    
    // 4. 确保重新初始化dashboard计算器
    if (dashboardCalculator && ui->dashForce) {
        ui->dashForce->setCalculator(dashboardCalculator);
    }
}

// 添加处理数据快照的槽函数实现
void MainWindow::processDataSnapshots()
{
    static int snapshotCount = 0;
    
    try {
        // 确保主时间戳已初始化
        if (!masterTimer) {
            qDebug() << "警告: 主时间戳未初始化，无法处理数据快照";
            // 创建主时间戳
            masterTimer = new QElapsedTimer();
            masterTimer->start();
            return;
        }
        
        // 获取当前基于主时间戳的时间(秒)及精确到整百毫秒的时间
        double currentTime = (masterTimer->elapsed() / 1000.0);
        double roundedTime = round(currentTime * 10.0) / 10.0; // 精确到100ms的整数倍
        
        // 创建当前快照
        DataSnapshot snapshot;
        snapshot.timestamp = roundedTime; // 使用整百毫秒的时间
        
        // 获取当前的Modbus数据
        snapshot.modbusValid = modbusDataValid;
        if (modbusDataValid && !modbusData.isEmpty()) {
            // 将二维数据转为一维，取每个通道的最后一个值
            snapshot.modbusData.resize(modbusNumRegs);
            for (int i = 0; i < modbusNumRegs && i < modbusData.size(); ++i) {
                if (!modbusData[i].isEmpty()) {
                    snapshot.modbusData[i] = modbusData[i].last();
                } else {
                    snapshot.modbusData[i] = 0.0;
                }
            }
        }
        
        // 获取当前的DAQ数据
        snapshot.daqValid = daqIsAcquiring && daqNumChannels > 0;
        snapshot.daqRunning = daqIsAcquiring;
        
        if (snapshot.daqValid && !daqChannelData.isEmpty()) {
            // 修改：为每个通道保存最新的数据点
            snapshot.daqData.resize(daqNumChannels);
            for (int i = 0; i < daqNumChannels && i < daqChannelData.size(); ++i) {
                if (!daqChannelData[i].isEmpty()) {
                    // 只保存最新的数据点
                    snapshot.daqData[i] = daqChannelData[i].last();
                } else {
                    snapshot.daqData[i] = 0.0;
                }
            }
        }
        
        // 获取当前的ECU数据
        snapshot.ecuValid = ecuDataValid;
        if (ecuDataValid && !ecuData.isEmpty() && ecuData.size() == 9) {
            snapshot.ecuData.resize(9);
            for (int i = 0; i < 9; ++i) {
                if (!ecuData[i].isEmpty()) {
                    snapshot.ecuData[i] = ecuData[i].last();
                } else {
                    snapshot.ecuData[i] = 0.0;
                }
            }
        }
        
        // 增加快照计数
        snapshotCount++;
        snapshot.snapshotIndex = snapshotCount;
        
        // 将当前快照添加到队列中
        snapshotQueue.enqueue(snapshot);
        
        // 限制队列大小，保留最新的300个快照(约30秒数据)
        while (snapshotQueue.size() > 300) {
            snapshotQueue.dequeue();
        }
        
        // 创建时间向量
        QVector<double> timeData;
        timeData.append(roundedTime);
        
        // 仅更新仪表盘数据，绘图会在main timer的50ms点进行
        if (snapshot.modbusValid || snapshot.daqValid || snapshot.ecuValid) {
            updateDashboardData(timeData, snapshot);
        }
    } catch (const std::exception& e) {
        qDebug() << "处理数据快照时出错: " << e.what();
    } catch (...) {
        qDebug() << "处理数据快照时发生未知错误";
    }
}

// 更新仪表盘数据（从快照）
void MainWindow::updateDashboardData(const QVector<double> &timeData, const DataSnapshot &snapshot)
{
    try {
        // 确保有必要的数据才更新仪表盘
        if (snapshot.modbusValid || snapshot.daqValid || snapshot.ecuValid) {
            // 转换ECU数据
            ECUData ecuData;
            ecuData.isValid = snapshot.ecuValid;
            
            // 确保ecuData向量大小符合要求
            if (snapshot.ecuValid && snapshot.ecuData.size() >= 9) {
                // 从快照数据中提取ECU数据
                ecuData.throttle = snapshot.ecuData[0];
                ecuData.engineSpeed = snapshot.ecuData[1];
                ecuData.cylinderTemp = snapshot.ecuData[2];
                ecuData.exhaustTemp = snapshot.ecuData[3];
                ecuData.axleTemp = snapshot.ecuData[4];
                ecuData.fuelPressure = snapshot.ecuData[5];
                ecuData.intakeTemp = snapshot.ecuData[6];
                ecuData.atmPressure = snapshot.ecuData[7];
                ecuData.flightTime = snapshot.ecuData[8];
            }

            // 更新全局最新ECU数据
            if (ecuData.isValid) {
                latestECUData = ecuData;
            }
            
            // 更新仪表盘 - 使用当前映射关系
            // 确保有数据可用
            updateDashboardByMapping(
                snapshot.modbusData, 
                snapshot.daqData, 
                ecuData);
        }
    } catch (const std::exception& e) {
        qDebug() << "更新仪表盘数据时出错:" << e.what();
    } catch (...) {
        qDebug() << "更新仪表盘数据时发生未知错误";
    }
}

// 更新Modbus数据表格（从快照）
void MainWindow::updateModbusTable(const QVector<double> &timeData, const QVector<QVector<double>> &modbusData, int numRegs)
{
    /*
    try {
        // 确保表格模型和数据有效
        if (!tableModel || modbusData.isEmpty() || timeData.isEmpty() || numRegs <= 0) {
            return;
        }
        
        // 性能优化：限制更新频率，避免频繁更新UI
        static QElapsedTimer updateTimer;
        static bool timerInitialized = false;
        
        if (!timerInitialized) {
            updateTimer.start();
            timerInitialized = true;
        } else if (updateTimer.elapsed() < 100) { // 限制为100ms更新一次
            return; // 如果距离上次更新不到100ms，则跳过此次更新
        }
        
        updateTimer.restart(); // 重置计时器
        
        // 获取最新的时间点 - 使用DAQ相同的时间格式
        double latestTime = timeData.last();
        
        // 创建要插入的数据列
        QVector<double> dataColumn;
        dataColumn.reserve(numRegs); // 预分配空间避免多次内存分配
        
        for (int i = 0; i < numRegs && i < modbusData.size(); ++i) {
            if (!modbusData[i].isEmpty()) {
                dataColumn.append(modbusData[i].first());
            } else {
                dataColumn.append(0.0); // 如果没有数据，则填充0
            }
        }
        
        // 更新表格数据 - 使用与DAQ相同的时间基准
        updateTableData(latestTime, dataColumn);
    } catch (const std::exception& e) {
        qDebug() << "更新Modbus表格时出错:" << e.what();
    } catch (...) {
        qDebug() << "更新Modbus表格时发生未知错误";
    }
    */
}

// 实现setupMasterTimer函数
void MainWindow::setupMasterTimer()
{
    // 如果主计时器已经存在，则先删除
    if (masterTimer) {
        delete masterTimer;
    }
    
    // 创建新的主计时器并立即启动
    masterTimer = new QElapsedTimer();
    masterTimer->start();
    
    qDebug() << "主计时器已初始化并启动";
}

// 保留原有的无参数版本的updateDAQPlot，调用新版本来确保向后兼容
void MainWindow::updateDAQPlot()
{
    // 如果没在采集中或没有数据，则不更新
    if (!daqIsAcquiring || daqChannelData.isEmpty() || daqNumChannels <= 0 || !masterTimer) {
        return;
    }
    
    // 创建一个基于当前时间的快照
    DataSnapshot snapshot;
    snapshot.timestamp = masterTimer->elapsed() / 1000.0;
    snapshot.daqValid = true;
    snapshot.daqRunning = daqIsAcquiring;
    
    // 修改：从二维数组中只保存每个通道的最新数据点
    snapshot.daqData.resize(daqNumChannels);
    for (int ch = 0; ch < daqNumChannels && ch < daqChannelData.size(); ++ch) {
        if (!daqChannelData[ch].isEmpty()) {
            // 只保存最新的一个数据点
            snapshot.daqData[ch] = daqChannelData[ch].last();
        } else {
            snapshot.daqData[ch] = 0.0;
        }
    }
    
    // 创建时间向量
    QVector<double> timeData;
    timeData.append(snapshot.timestamp);
    
    // 调用带参数的版本
    updateDAQPlot(timeData, snapshot);
}

// 实现主定时器的超时处理函数
void MainWindow::onMainTimerTimeout()
{
    // 获取当前时间(毫秒)
    qint64 currentTime = masterTimer->elapsed();
    
    // 计算当前时间对应的整百毫秒和整百+50毫秒时间点
    qint64 currentHundred = (currentTime / 100) * 100;
    qint64 currentHundredPlus50 = currentHundred + 50;
    
    // 检查是否达到了整百毫秒时间点(用于快照)
    if (currentTime >= currentHundred && lastSnapshotTime < currentHundred) {
        // 更新上次快照时间
        lastSnapshotTime = currentHundred;
        
        // 创建和处理数据快照 - 在整百毫秒触发
        processDataSnapshots();
    }
    
    // 检查是否达到了整百+50毫秒时间点(用于Modbus数据读取)
    if (currentTime >= currentHundredPlus50 && lastModbusReadTime < currentHundredPlus50) {
        // 更新上次Modbus读取时间
        lastModbusReadTime = currentHundredPlus50;
        
        // 如果请求了Modbus读取且按钮处于"结束"状态，执行Modbus命令
        if (modbusReadRequested && ui->btnSend->text() == "结束") {
            emit sendModbusCommand(ui->lineServerAddress->text().toInt(),
                                  ui->lineSegAddress->text().toInt(),
                                  ui->lineSegNum->text().toInt());
        }
        
        // 如果ECU连接有效，也在同一时间执行ECU数据读取
        if (ecuIsConnected) {
            // ECU数据读取通常由硬件触发，这里我们可以处理接收到的数据
            // 根据实际情况补充ECU数据处理代码
        }
    }
    
    // 检查是否需要更新绘图 - 在快照时间之后的50ms触发
    if (currentTime >= currentHundredPlus50 && lastPlotUpdateTime < currentHundredPlus50) {
        // 更新上次绘图更新时间
        lastPlotUpdateTime = currentHundredPlus50;
        
        // 如果队列中有快照数据，更新绘图
        if (!snapshotQueue.isEmpty()) {
            // 获取队列中的最新快照
            DataSnapshot latestSnapshot = snapshotQueue.last();
            
            // 创建时间向量
            QVector<double> timeData;
            timeData.append(latestSnapshot.timestamp);
            
            // 更新DAQ图表
            if (latestSnapshot.daqValid) {
                updateDAQPlot(timeData, latestSnapshot);
            }
        }
    }
}