#include "mainwindow.h"
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
    
    ModbusTimer = new QTimer;
    ModbusTimer->setInterval(ui->lineTimeLoop->text().toInt());

    // 初始化滤波器相关变量
    realTimer = new QElapsedTimer();
    realTimer->start();
    lastTimestamp = 0;
    filteredValues.clear();

    // 修改计时器超时处理，直接发送数据采集命令，而不是触发按钮点击
    // 注释掉此处的连接，使用下面的连接代替
    /*connect(ModbusTimer, &QTimer::timeout, this, [=](){
        emit sendModbusCommand(ui->lineServerAddress->text().toInt(),
                               ui->lineSegAddress->text().toInt(),
                               ui->lineSegNum->text().toInt());
    });*/

    // 添加WebSocket消息接收处理连接
    connect(wsTh, &WebSocketThread::messageReceived, this, &MainWindow::handleWebSocketMessage);
    
    // 添加WebSocket错误信号处理
    connect(wsTh, &WebSocketThread::serverError, this, [=](QString errorMsg) {
        ui->plainReceive->appendPlainText("WebSocket错误: " + errorMsg);
        qDebug() << "WebSocket错误: " + errorMsg;
    });
    
    // 添加测试WebSocket按钮
    // QPushButton* testWebSocketBtn = new QPushButton("测试WebSocket", this);
    // ui->statusbar->addPermanentWidget(testWebSocketBtn);
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

    // 创建DAQ定时器，用于更新图表
    daqUpdateTimer = new QTimer(this);
    connect(daqUpdateTimer, &QTimer::timeout, this, &MainWindow::updateDAQPlot);
    
    // 创建仪表盘更新定时器
    setupDashboardUpdateTimer();
    
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
                ModbusTimer->stop();
                ui->btnSend->setText("读取");
            }
        }
    });

    // 修改Modbus数据处理方式：
    // 1. MainWindow仍然接收数据用于UI显示
    connect(mbTh, &modbusThread::sendModbusResult, this, &MainWindow::showModbusResult);
    
    // 2. WebSocketThread直接接收Modbus数据，处理并转发
    connect(mbTh, &modbusThread::sendModbusResult, wsTh, &WebSocketThread::handleModbusRawData);
    
    // 连接重置计时器的信号与槽
    connect(this, &MainWindow::resetModbusTimer, mbTh, &modbusThread::resetTimer);

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

    // 创建实时计时器用于滤波
    realTimer = new QElapsedTimer();
    realTimer->start();
    lastTimestamp = 0;
    
    // 添加定时器信号槽连接
    connect(ModbusTimer, &QTimer::timeout, [=]() {
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
    
    // 移除重复的初始化代码
}

MainWindow::~MainWindow()
{
    // 保存仪表盘设置到配置文件
    QString settingsFile = QCoreApplication::applicationDirPath() + "/dashboard_settings.ini";
    QSettings settings(settingsFile, QSettings::IniFormat);
    saveDashboardMappings(settings);
    qDebug() << "已保存仪表盘设置到:" << settingsFile;
    
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

    // 清理定时器资源
    if (daqUpdateTimer) {
        daqUpdateTimer->stop();
        delete daqUpdateTimer;
        daqUpdateTimer = nullptr;
    }
    
    if (dashboardUpdateTimer) {
        dashboardUpdateTimer->stop();
        delete dashboardUpdateTimer;
        dashboardUpdateTimer = nullptr;
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

void MainWindow::updateDAQPlot()
{
    if (!daqIsAcquiring || daqTimeData.isEmpty() || daqNumChannels <= 0) {
        return;
    }
    
    // 确保GroupBox_6和其布局存在
    if (!ui->groupBox_6 || !ui->verticalLayout_7) {
        qDebug() << "错误: 无法找到DAQ图表容器或布局";
        return;
    }
    
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
    
    // 创建颜色列表
    QStringList colorNames = {"blue", "red", "green", "magenta", "cyan", "darkGreen", "darkRed", "darkBlue"};
    
    // 检查是否需要创建或更新图表
    if (plotsLayout->count() != daqNumChannels) {
        // 清除现有的图表
        while (plotsLayout->count() > 0) {
            QLayoutItem *item = plotsLayout->takeAt(0);
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
        
        // 为每个通道创建一个单独的图表
        for (int i = 0; i < daqNumChannels; ++i) {
            // 创建图表容器
            QWidget *plotContainer = new QWidget(scrollWidget);
            QHBoxLayout *containerLayout = new QHBoxLayout(plotContainer);
            containerLayout->setContentsMargins(5, 5, 5, 5);
            
            // 创建一个QCustomPlot实例
            QCustomPlot *plot = new QCustomPlot();
            plot->setMinimumHeight(150);
            
            // 设置轴标签
            plot->xAxis->setLabel("时间 (秒)");
            plot->yAxis->setLabel(QString("通道 %1").arg(i));
            
            // 添加图形
            plot->addGraph();
            plot->graph(0)->setPen(QPen(QColor(colorNames[i % colorNames.size()])));
            
            // 使能交互
            plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
            
            // 重要：防止闪烁
            plot->setNoAntialiasingOnDrag(true);
            plot->setNotAntialiasedElements(QCP::aeAll);
            
            // 创建游标容器 - 但不再在这里添加值标签
            QWidget *cursorContainer = new QWidget();
            cursorContainer->setFixedWidth(60);
            QVBoxLayout *cursorLayout = new QVBoxLayout(cursorContainer);
            cursorLayout->setContentsMargins(5, 0, 5, 0);
            
            // 创建一个标签显示当前值，直接添加到plotContainer
            QLabel *valueLabel = new QLabel("0.00", plotContainer);
            valueLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 14px; background-color: rgba(255, 255, 255, 150);").arg(colorNames[i % colorNames.size()]));
            valueLabel->setAlignment(Qt::AlignCenter);
            valueLabel->setObjectName(QString("daqValueLabel_%1").arg(i));
            valueLabel->move(plot->width() - 60, 10);
            valueLabel->setFixedSize(50, 20);
            valueLabel->show();
            
            // 箭头指示标签
            QLabel *arrowLabel = new QLabel("←", plotContainer);
            arrowLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 18px;").arg(colorNames[i % colorNames.size()]));
            arrowLabel->setAlignment(Qt::AlignCenter);
            arrowLabel->setObjectName(QString("daqArrowLabel_%1").arg(i));
            arrowLabel->move(plot->width() - 30, plot->height()/2 - arrowLabel->height()/2);
            arrowLabel->show();
            
            // 将图表和游标容器添加到容器布局
            containerLayout->addWidget(plot);
            containerLayout->addWidget(cursorContainer);
            
            // 将整个容器添加到滚动区域中
            plotsLayout->addWidget(plotContainer);
            
            // 设置对象名，以便后续查找
            plot->setObjectName(QString("daqPlot_%1").arg(i));
        }
    }
    
    // 更新图表数据
    // 获取要显示的数据范围，减少处理量
    int dataSize = daqTimeData.size();
    
    for (int i = 0; i < daqNumChannels && i < plotsLayout->count(); ++i) {
        // 获取图表容器和相关控件
        QWidget *plotContainer = plotsLayout->itemAt(i)->widget();
        if (!plotContainer) continue;
        
        QCustomPlot *plot = plotContainer->findChild<QCustomPlot*>(QString("daqPlot_%1").arg(i));
        QLabel *valueLabel = plotContainer->findChild<QLabel*>(QString("daqValueLabel_%1").arg(i));
        QLabel *arrowLabel = plotContainer->findChild<QLabel*>(QString("daqArrowLabel_%1").arg(i));
        
        if (!plot) continue;
        
        // 直接添加数据，不清空原有数据
        plot->graph(0)->setData(daqTimeData, daqChannelData[i], true);
        
        // 自动调整X轴范围，总是显示最新的数据
        double xMin = dataSize > 1000 ? daqTimeData[dataSize - 1000] : daqTimeData.first();
        double xMax = daqTimeData.last();
        plot->xAxis->setRange(xMin, xMax);
        
        // 修改Y轴自动缩放逻辑，每次检查最新点是否在范围内
        if (!daqChannelData[i].isEmpty()) {
            double latestValue = daqChannelData[i].last();
            
            // 获取当前Y轴范围
            double yMin = plot->yAxis->range().lower;
            double yMax = plot->yAxis->range().upper;
            
            // 检查最新值是否在当前范围内，如果不在则调整范围
            if (latestValue < yMin || latestValue > yMax) {
                // 计算最近100个点的范围（或全部点，如果点数少于100）
                int startIndex = qMax(0, daqChannelData[i].size() - 100);
                double dataMin = daqChannelData[i][startIndex];
                double dataMax = dataMin;
                
                for (int j = startIndex; j < daqChannelData[i].size(); ++j) {
                    dataMin = qMin(dataMin, daqChannelData[i][j]);
                    dataMax = qMax(dataMax, daqChannelData[i][j]);
                }
                
                // 确保范围不会太小
                double range = dataMax - dataMin;
                if (range < 0.1) range = 0.1;
                
                // 设置新的Y轴范围，增加一些边距
                plot->yAxis->setRange(dataMin - range * 0.1, dataMax + range * 0.1);
            }
        }
        
        // 更新游标显示 - 显示最新数据值
        if (valueLabel && !daqChannelData[i].isEmpty()) {
            double latestValue = daqChannelData[i].last();
            valueLabel->setText(QString::number(latestValue, 'f', 2));
            
            // 更新箭头位置
            if (arrowLabel) {
                // 获取当前值对应的Y坐标
                double pixelY = plot->yAxis->coordToPixel(latestValue);
                
                // 更新箭头位置 - 放置在图表右侧边线上，指向左侧的数据
                arrowLabel->setParent(plotContainer); // 确保箭头是plotContainer的子控件
                arrowLabel->setText("←"); // 使用左箭头
                arrowLabel->move(plot->width() - 30, pixelY - arrowLabel->height()/2);
                
                // 确保值标签固定在右上角，不随箭头移动
                valueLabel->setParent(plotContainer); // 确保值标签是plotContainer的子控件
                valueLabel->move(plot->width() - 60, 10); // 固定在右上角
                valueLabel->setFixedSize(50, 20); // 设置固定大小
                valueLabel->raise(); // 确保值标签在最上层
                valueLabel->show();
            }
        }
        
        // 重绘图表
        plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void MainWindow::setupDAQTable()
{
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
}

void MainWindow::updateDAQTable(const QVector<double> &timeData, const QVector<QVector<double>> &channelData, int numChannels)
{
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
    
    // 添加新列
    daqDataModel->insertColumn(currentColumn);
    
    // 添加时间值
    QStandardItem *timeItem = new QStandardItem(QString::number(timeData[lastIndex], 'f', 3));
    timeItem->setBackground(QBrush(QColor(240, 240, 255))); // 时间行使用浅蓝色背景
    daqDataModel->setItem(0, currentColumn - 1, timeItem);
    
    // 添加各通道数据
    for (int i = 0; i < numChannels; ++i) {
        if (i < channelData.size() && lastIndex < channelData[i].size()) {
            QStandardItem *dataItem = new QStandardItem(QString::number(channelData[i][lastIndex], 'f', 3));
            // 根据数据范围设置背景色
            if (channelData[i][lastIndex] > 100.0) {
                dataItem->setBackground(QBrush(QColor(255, 220, 220))); // 高值使用浅红色
            } else if (channelData[i][lastIndex] < 0.0) {
                dataItem->setBackground(QBrush(QColor(220, 255, 220))); // 负值使用浅绿色
            }
            daqDataModel->setItem(i + 1, currentColumn - 1, dataItem);
        }
    }
    
    // 如果超出最大列数，删除最旧的列
    const int maxColumns = 100;
    if (currentColumn > maxColumns) {
        daqDataModel->removeColumn(0);
    }
    
    // 滚动到最新列
    if (daqDataModel->columnCount() > 0) {
        ui->daqDataTableView->scrollTo(daqDataModel->index(0, daqDataModel->columnCount() - 1), QAbstractItemView::EnsureVisible);
    }
}

void MainWindow::handleDAQData(QVector<double> timeData, QVector<QVector<double>> channelData, int numChannels)
{
    // 打印调试信息以便跟踪性能
    if (!timeData.isEmpty() && numChannels > 0) {
        qDebug() << "接收到DAQ数据: 时间点数=" << timeData.size() 
                 << ", 通道数=" << numChannels
                 << ", 最新时间=" << timeData.last();
    }
    
    // 更新DAQ数据缓冲区
    daqTimeData = timeData;
    daqChannelData = channelData;
    daqNumChannels = numChannels;
    
    // 更新表格 - 实时更新，确保与图表同步
    updateDAQTable(timeData, channelData, numChannels);
    
    // 主动更新图表，不再等待定时器
    updateDAQPlot();
    
    // 不在每次数据到达时都更新仪表盘，可以考虑用另一个低频率定时器更新
    // 或在用户交互时更新，减轻主线程负担
    // 注释掉直接更新仪表盘的代码
    /*
    static ECUData emptyEcuData;
    static QVector<double> emptyModbusData;
    updateDashboardByMapping(emptyModbusData, channelData, emptyEcuData);
    */
}

void MainWindow::handleDAQStatus(bool isRunning, QString message)
{
    daqIsAcquiring = isRunning;
    
    // 更新UI状态
    ui->startDAQButton->setEnabled(!isRunning);
    ui->stopDAQButton->setEnabled(isRunning);
    ui->deviceNameEdit->setEnabled(!isRunning);
    ui->channelsEdit->setEnabled(!isRunning);
    ui->sampleRateEdit->setEnabled(!isRunning);
    
    // 显示状态信息
    statusBar()->showMessage(message, 3000);
    
    // 启动或停止定时器
    if (isRunning) {
        // 创建并启动仪表盘更新定时器 - 使用较低频率500ms
        if (!dashboardUpdateTimer) {
            dashboardUpdateTimer = new QTimer(this);
            connect(dashboardUpdateTimer, &QTimer::timeout, this, [this]() {
                if (daqIsAcquiring && !daqChannelData.isEmpty()) {
                    static ECUData emptyEcuData;
                    static QVector<double> emptyModbusData;
                    updateDashboardByMapping(emptyModbusData, daqChannelData, emptyEcuData);
                }
            });
        }
        dashboardUpdateTimer->start(100); // 改为100ms刷新率
        
        qDebug() << "DAQ采集已启动，正在等待数据...";
    } else {
        // 停止所有定时器
        if (daqUpdateTimer) {
            daqUpdateTimer->stop();
        }
        if (dashboardUpdateTimer) {
            dashboardUpdateTimer->stop();
        }
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
            // 停止定时器
            ModbusTimer->stop();
            
            // 改变按钮文字为"读取"
            ui->btnSend->setText("读取");
            ui->plainReceive->appendPlainText("数据采集已停止");
        }
        
        // 通过信号在子线程中关闭串口，而不是直接调用
        emit closeModbusConnection();
        
        // 注意：不在这里设置UI状态，而是在收到modbusConnectionStatus信号后设置
        // UI状态会通过modbusConnectionStatus信号更新
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
        
        // 重置ModBus线程中的计时器，确保时间同步
        emit resetModbusTimer();
        
        // 读取滤波器状态并更新UI
        filterEnabled = ui->filterEnabledCheckBox->isChecked();
        qDebug() << "滤波器状态: " << (filterEnabled ? "开启" : "关闭") 
                 << "，时间常数: " << ui->lineTimeLoop->text().toDouble() << "ms";
        
        // 设置定时器并发送命令
        ModbusTimer->start(ui->lineTimeLoop->text().toInt());
        emit sendModbusCommand(ui->lineServerAddress->text().toInt(),
                              ui->lineSegAddress->text().toInt(),
                              ui->lineSegNum->text().toInt());
        
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
        
        // 停止定时器
        ModbusTimer->stop();
        
        // 改变按钮文字为"读取"
        ui->btnSend->setText("读取");
        ui->plainReceive->appendPlainText("数据采集已停止");
    }
}





//初始化图表
void MainWindow::myplotInit(QCustomPlot *customPlot)
{
    // 清除主图表，创建滚动区域
    if (customPlot) {
        // 获取父级布局和控件
        QWidget* parentWidget = customPlot->parentWidget();
        QLayout* parentLayout = parentWidget ? parentWidget->layout() : nullptr;
        
        if (parentLayout) {
            // 移除并删除现有的customPlot
            parentLayout->removeWidget(customPlot);
            customPlot->deleteLater(); // 安全删除原始控件
            myPlot = nullptr; // 清空指针引用
            
            // 创建滚动区域
            QScrollArea *scrollArea = new QScrollArea(parentWidget);
            scrollArea->setWidgetResizable(true);
            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            
            QWidget *scrollWidget = new QWidget();
            QVBoxLayout *plotsLayout = new QVBoxLayout(scrollWidget);
            plotsLayout->setSpacing(10); // 设置图表间距
            scrollArea->setWidget(scrollWidget);
            
            // 添加滚动区域到父级布局
            parentLayout->addWidget(scrollArea);
            
            // 保存滚动区域引用
            myScrollArea = scrollArea;
        } else {
            qDebug() << "错误: 未找到父级布局，无法初始化Modbus图表";
            return;
        }
    } else {
        qDebug() << "错误: 传入的customPlot为空";
        
        // 尝试直接从UI中获取groupBox_4
        if (ui->groupBox_4 && ui->verticalLayout_6) {
            // 清空现有布局内容
            QLayoutItem *item;
            while ((item = ui->verticalLayout_6->takeAt(0)) != nullptr) {
                if (item->widget()) {
                    delete item->widget();
                }
                delete item;
            }
            
            // 创建滚动区域
            QScrollArea *scrollArea = new QScrollArea();
            scrollArea->setWidgetResizable(true);
            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
            scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            
            QWidget *scrollWidget = new QWidget();
            QVBoxLayout *plotsLayout = new QVBoxLayout(scrollWidget);
            plotsLayout->setSpacing(10); // 设置图表间距
            scrollArea->setWidget(scrollWidget);
            
            // 添加到UI布局
            ui->verticalLayout_6->addWidget(scrollArea);
            
            // 保存滚动区域引用
            myScrollArea = scrollArea;
        } else {
            qDebug() << "错误: 无法找到groupBox_4或其布局，Modbus图表初始化失败";
            return;
        }
    }
    
    // 初始化graphs列表
    graphs.clear();
    myPlots.clear();
    
    // 检查channelNum是否有效
    if (channelNum <= 0) {
        qDebug() << "警告: 通道数量无效，使用默认值";
        channelNum = 3; // 默认创建3个图表
    }
    
    // 获取滚动区域的内容部件和布局
    QWidget *scrollWidget = myScrollArea->widget();
    QVBoxLayout *plotsLayout = qobject_cast<QVBoxLayout*>(scrollWidget->layout());
    
    if (!plotsLayout) {
        qDebug() << "错误: 无法获取滚动区域布局";
        return;
    }
    
    // 创建颜色列表
    QStringList colorNames = {"blue", "red", "green", "magenta", "cyan", 
                             "darkGreen", "darkRed", "darkBlue"};
    
    // 为每个通道创建图表
    for (int i = 0; i < channelNum; ++i) {
        // 创建容器和布局
        QWidget *plotContainer = new QWidget(scrollWidget);
        QHBoxLayout *containerLayout = new QHBoxLayout(plotContainer);
        containerLayout->setContentsMargins(5, 5, 5, 5);
        
        // 创建自定义图表并设置
        QCustomPlot *plot = new QCustomPlot(plotContainer);
        // 不设置对象名，直接通过findChildren找到
        plot->setMinimumHeight(150);
        plot->xAxis->setLabel("时间 (秒)");
        plot->yAxis->setLabel(QString("通道 %1").arg(i));
        
        // 添加图形
        plot->addGraph();
        plot->graph(0)->setPen(QPen(QColor(colorNames[i % colorNames.size()])));
        
        // 使能交互
        plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        
        // 设置性能优化参数
        plot->setNoAntialiasingOnDrag(true);
        plot->setNotAntialiasedElements(QCP::aeAll);
        
        // 创建值标签容器
        QWidget *valueContainer = new QWidget(plotContainer);
        valueContainer->setFixedWidth(60);
        QVBoxLayout *valueLayout = new QVBoxLayout(valueContainer);
        valueLayout->setContentsMargins(5, 0, 5, 0);
        
        // 创建值标签 - 不设置对象名
        QLabel *valueLabel = new QLabel("0.00", valueContainer);
        valueLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 14px;")
                                 .arg(colorNames[i % colorNames.size()]));
        valueLabel->setAlignment(Qt::AlignCenter);
        
        // 创建箭头标签 - 初始时使用左箭头（指向右侧）
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
        containerLayout->addWidget(plot);
        containerLayout->addWidget(valueContainer);
        
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
}

//获取modbus结果
void MainWindow::showModbusResult(QVector<double> resultdata, qint64 readTimeInterval)
{
    // 更新通道数
    int newChannelNum = resultdata.count();
    
    // 确保通道数至少为1
    if (newChannelNum <= 0) {
        qDebug() << "警告: 接收到的通道数为0，跳过处理";
        return;
    }
    
    // 如果通道数发生变化，重新初始化图表
    if (newChannelNum != channelNum) {
        qDebug() << "通道数量变化: " << channelNum << " -> " << newChannelNum;
        channelNum = newChannelNum;
        myplotInit(myPlot);
    }
    
    // 如果滚动区域不存在，可能是初始化失败，尝试重新初始化
    if (!myScrollArea) {
        qDebug() << "警告: 滚动区域不存在，尝试重新初始化Modbus图表";
        myplotInit(ui->modbusCustomPlot);
        
        // 如果仍然无法初始化，则跳过处理
        if (!myScrollArea) {
            qDebug() << "错误: 无法初始化Modbus图表，跳过数据处理";
            return;
        }
    }
    
    // 清空显示区域
    ui->plainReceive->clear();
    
    // 应用滤波（如果启用）
    QVector<double> dataToShow;
    if (filterEnabled) {
        dataToShow = applyFilterToResults(resultdata, readTimeInterval);
    } else {
        dataToShow = resultdata;
    }
    
    // 更新仪表盘
    // 由于没有DAQ和ECU数据的获取方法，我们使用空数据
    static QVector<QVector<double>> emptyDaqData;
    static ECUData emptyEcuData;
    updateDashboardByMapping(dataToShow, emptyDaqData, emptyEcuData);
    
    // 实时曲线增加数据点
    realPlotTime += (double)readTimeInterval / 1000.0;
    
    ui->plainReceive->appendPlainText(QString("时间: %1 秒").arg(realPlotTime, 0, 'f', 1));
    
    // 获取寄存器起始地址
    int startAddress = ui->lineSegAddress->text().toInt();
    
    // 获取滚动区域布局，用于获取图表容器
    QWidget *scrollWidget = myScrollArea->widget();
    if (!scrollWidget) {
        qDebug() << "错误: 滚动区域部件不存在";
        return;
    }
    
    QVBoxLayout *plotsLayout = qobject_cast<QVBoxLayout*>(scrollWidget->layout());
    if (!plotsLayout) {
        qDebug() << "错误: 滚动区域布局不存在";
        return;
    }
    
    // 更新每个图表
    for (int i = 0; i < channelNum; ++i)
    {
        // 显示每个寄存器的值
        int registerAddress = i + startAddress;  // 使用起始地址计算实际寄存器地址
        ui->plainReceive->appendPlainText(QString("寄存器 %1: %2").arg(registerAddress).arg(dataToShow[i], 0, 'f', 1));
        
        // 找到对应的图表和标签
        QWidget *plotContainer = plotsLayout->itemAt(i)->widget();
        if (!plotContainer) continue;
        
        // 修正：不需要通过名称查找控件，直接从plotContainer中获取QCustomPlot
        QCustomPlot *plot = nullptr;
        QLabel *valueLabel = nullptr;
        QLabel *arrowLabel = nullptr;
        
        // 查找控件
        foreach(QCustomPlot *cp, plotContainer->findChildren<QCustomPlot*>()) {
            plot = cp;
            break;
        }
        
        // 查找值标签和箭头标签
        QList<QLabel*> labels = plotContainer->findChildren<QLabel*>();
        if (labels.size() >= 2) {
            foreach(QLabel *label, labels) {
                if (label->text() == "→" || label->text() == "←" || label->text().contains("←") || label->text().contains("→")) {
                    arrowLabel = label;
                    // 将箭头修改为向左指向
                    arrowLabel->setText("←");
                } else {
                    valueLabel = label;
                }
            }
        }
        
        if (!plot) {
            qDebug() << "错误: 无法找到通道" << i << "的图表";
            continue;
        }
        
        // 如果没有找到标签，创建默认标签
        if (!valueLabel) {
            qDebug() << "警告: 无法找到通道" << i << "的值标签，使用默认值";
            continue;
        }
        
        if (!arrowLabel) {
            qDebug() << "警告: 无法找到通道" << i << "的箭头标签，使用默认值";
            continue;
        }
        
        // 设置通道标题，包含寄存器地址信息
        plot->yAxis->setLabel(QString("通道 %1").arg(registerAddress));
        
        // 添加到图表 - 使用保存的graph对象添加数据
        if (i < graphs.size() && graphs[i] != nullptr) {
            graphs[i]->addData(realPlotTime, dataToShow[i]);
        }
    
        // 更新当前值标签 - 固定在右上角位置显示
        valueLabel->setText(QString::number(dataToShow[i], 'f', 2));
        valueLabel->move(plot->width() - 60, 10); // 固定在右上角
        valueLabel->show();
        
        // 调整X轴范围，显示最近的数据
        double visibleRange = 5.0; // 显示最近5秒的数据
        if(realPlotTime > visibleRange) {
            plot->xAxis->setRange(realPlotTime - visibleRange, realPlotTime);
        } else {
            plot->xAxis->setRange(0, visibleRange);
        }
        
        // 使用静态计数器减少Y轴缩放频率，避免频繁重绘导致闪烁
        static int yScaleCounter = 0;
        if (++yScaleCounter % 5 == 0) { // 每5次采样才重新计算Y轴范围
            double minY = graphs[i]->dataMainValue(0);
            double maxY = minY;
            // 查找最近100个点的范围
            int dataSize = graphs[i]->dataCount();
            int startIndex = dataSize > 100 ? dataSize - 100 : 0;
            for (int j = startIndex; j < dataSize; j++) {
                double value = graphs[i]->dataMainValue(j);
                minY = qMin(minY, value);
                maxY = qMax(maxY, value);
            }
            
            double range = maxY - minY;
            if (range < 0.1) range = 0.1;
            
            // 设置新的Y轴范围，增加一些边距
            plot->yAxis->setRange(minY - range * 0.1, maxY + range * 0.1);
        }
        
        // 更新箭头位置 - 将箭头放在右侧边线上，沿着曲线移动
        double pixelY = plot->yAxis->coordToPixel(dataToShow[i]);
        // 将箭头放在右侧边线上，箭头指向左侧
        arrowLabel->setText("←"); // 确保箭头指向左侧
        arrowLabel->move(plot->width() - 30, pixelY - arrowLabel->height()/2);
        
        // 使用QueuedReplot减少闪烁
        plot->replot(QCustomPlot::rpQueuedReplot);
    }
    
    // 更新表格数据
    updateTableData(realPlotTime, dataToShow);
    
    // 显示过滤信息和状态栏提示
    if (filterEnabled) {
        ui->plainReceive->appendPlainText(QString("滤波已启用，时间常数: %1 ms").arg(ui->lineTimeLoop->text()));
        sBar->showMessage(QString("已更新 %1 个通道的数据（已滤波）").arg(channelNum), 3000);
    } else {
        sBar->showMessage(QString("已更新 %1 个通道的数据").arg(channelNum), 3000);
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



// void MainWindow::on_btnPageModbus_clicked()
// {
//     switchPage();
// }


// void MainWindow::on_btnPageCan_clicked()
// {
//     switchPage();
// }


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
    
    // 添加新列
    tableModel->insertColumn(currentColumn);
    
    // 添加时间值
    QStandardItem *timeItem = new QStandardItem(QString::number(time, 'f', 1));
    timeItem->setBackground(QBrush(QColor(240, 240, 255))); // 时间行使用浅蓝色背景
    tableModel->setItem(0, currentColumn - 1, timeItem);
    
    // 添加各通道数据
    for (int i = 0; i < data.size(); ++i) {
        QStandardItem *dataItem = new QStandardItem(QString::number(data[i], 'f', 1));
        // 根据数据范围设置背景色
        if (data[i] > 100.0) {
            dataItem->setBackground(QBrush(QColor(255, 220, 220))); // 高值使用浅红色
        } else if (data[i] < 0.0) {
            dataItem->setBackground(QBrush(QColor(220, 255, 220))); // 负值使用浅绿色
        }
        tableModel->setItem(i + 1, currentColumn - 1, dataItem);
    }
    
    // 如果超出最大列数，删除最旧的列
    if (currentColumn > maxColumns) {
        tableModel->removeColumn(0);
    }
    
    // 滚动到最新列
    if (tableModel->columnCount() > 0) {
        ui->tableView->scrollTo(tableModel->index(0, tableModel->columnCount() - 1), QAbstractItemView::EnsureVisible);
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




// void MainWindow::on_btnPageECU_clicked()
// {
//       switchPage();
// }

// 设置ECU表格模型
void MainWindow::setupECUTable()
{
    // 如果模型已存在则删除
    if (ecuDataModel) {
        delete ecuDataModel;
        ecuDataModel = nullptr;
    }
    
    // 创建新模型
    ecuDataModel = new QStandardItemModel(this);
    
    // 设置行标签
    QStringList rowLabels;
    rowLabels << "时间(秒)"
              << "节气门开度(%)"
              << "发动机转速(rpm)"
              << "缸温(°C)"
              << "排温(°C)"
              << "轴温(°C)"
              << "燃油压力(kPa)"
              << "进气温度(°C)"
              << "大气压力(kPa)"
              << "飞行时间(min)"
              << "状态";
    
    ecuDataModel->setVerticalHeaderLabels(rowLabels);
    
    // 设置表格模型
    ui->tableViewECU->setModel(ecuDataModel);
    
    // 调整表格外观
    ui->tableViewECU->horizontalHeader()->setVisible(false); // 隐藏列头
    ui->tableViewECU->verticalHeader()->setDefaultSectionSize(30); // 行高
    ui->tableViewECU->horizontalHeader()->setDefaultSectionSize(70); // 设置默认列宽
    ui->tableViewECU->setAlternatingRowColors(true); // 交替行颜色
    ui->tableViewECU->setSelectionMode(QAbstractItemView::SingleSelection); // 单选模式
    
    // 设置表格为只读
    ui->tableViewECU->setEditTriggers(QAbstractItemView::NoEditTriggers);
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
    // 保存最新的ECU数据
    latestECUData = data;
    
    // 更新ECU图表
    updateECUPlot(data);
    
    // 如果表格模型为空，则初始化它
    if (!ecuDataModel) {
        setupECUTable();
    }
    
    // 确保模型存在
    if (!ecuDataModel) {
        sBar->showMessage("ECU数据模型初始化失败", 3000);
        return;
    }
    
    // 获取当前列数
    int currentColumn = ecuDataModel->columnCount();
    
    // 添加新列
    ecuDataModel->insertColumn(currentColumn);
    
    // 创建表格项目
    
    // 时间戳 - 第一行
    QStandardItem *timeItem = new QStandardItem(data.timestamp.toString("hh:mm:ss.zzz"));
    timeItem->setBackground(QBrush(QColor(240, 240, 255))); // 时间行使用浅蓝色背景
    ecuDataModel->setItem(0, currentColumn - 1, timeItem);
    
    // 节气门开度
    QStandardItem *throttleItem;
    if (data.throttleError) {
        throttleItem = new QStandardItem("故障");
        throttleItem->setBackground(QBrush(QColor(255, 200, 200))); // 故障使用浅红色背景
    } else {
        throttleItem = new QStandardItem(QString::number(data.throttle, 'f', 1));
    }
    ecuDataModel->setItem(1, currentColumn - 1, throttleItem);
    
    // 发动机转速
    QStandardItem *rpmItem;
    if (data.engineSpeedError) {
        rpmItem = new QStandardItem("故障");
        rpmItem->setBackground(QBrush(QColor(255, 200, 200)));
    } else {
        rpmItem = new QStandardItem(QString::number(data.engineSpeed, 'f', 0));
    }
    ecuDataModel->setItem(2, currentColumn - 1, rpmItem);
    
    // 缸温
    QStandardItem *cylinderTempItem;
    if (data.cylinderTempError) {
        cylinderTempItem = new QStandardItem("故障");
        cylinderTempItem->setBackground(QBrush(QColor(255, 200, 200)));
    } else {
        cylinderTempItem = new QStandardItem(QString::number(data.cylinderTemp, 'f', 1));
    }
    ecuDataModel->setItem(3, currentColumn - 1, cylinderTempItem);
    
    // 排温
    QStandardItem *exhaustTempItem;
    if (data.exhaustTempError) {
        exhaustTempItem = new QStandardItem("故障");
        exhaustTempItem->setBackground(QBrush(QColor(255, 200, 200)));
    } else {
        exhaustTempItem = new QStandardItem(QString::number(data.exhaustTemp, 'f', 1));
    }
    ecuDataModel->setItem(4, currentColumn - 1, exhaustTempItem);
    
    // 轴温
    QStandardItem *axleTempItem;
    if (data.axleTempError) {
        axleTempItem = new QStandardItem("故障");
        axleTempItem->setBackground(QBrush(QColor(255, 200, 200)));
    } else {
        axleTempItem = new QStandardItem(QString::number(data.axleTemp, 'f', 1));
    }
    ecuDataModel->setItem(5, currentColumn - 1, axleTempItem);
    
    // 燃油压力
    QStandardItem *fuelPressureItem;
    if (data.fuelPressureError) {
        fuelPressureItem = new QStandardItem("故障");
        fuelPressureItem->setBackground(QBrush(QColor(255, 200, 200)));
    } else {
        fuelPressureItem = new QStandardItem(QString::number(data.fuelPressure, 'f', 1));
    }
    ecuDataModel->setItem(6, currentColumn - 1, fuelPressureItem);
    
    // 进气温度
    QStandardItem *intakeTempItem;
    if (data.intakeTempError) {
        intakeTempItem = new QStandardItem("故障");
        intakeTempItem->setBackground(QBrush(QColor(255, 200, 200)));
    } else {
        intakeTempItem = new QStandardItem(QString::number(data.intakeTemp, 'f', 1));
    }
    ecuDataModel->setItem(7, currentColumn - 1, intakeTempItem);
    
    // 大气压力
    QStandardItem *atmPressureItem;
    if (data.atmPressureError) {
        atmPressureItem = new QStandardItem("故障");
        atmPressureItem->setBackground(QBrush(QColor(255, 200, 200)));
    } else {
        atmPressureItem = new QStandardItem(QString::number(data.atmPressure, 'f', 1));
    }
    ecuDataModel->setItem(8, currentColumn - 1, atmPressureItem);
    
    // 飞行时间
    QStandardItem *flightTimeItem;
    if (data.flightTimeError) {
        flightTimeItem = new QStandardItem("故障");
        flightTimeItem->setBackground(QBrush(QColor(255, 200, 200)));
    } else {
        flightTimeItem = new QStandardItem(QString::number(data.flightTime, 'f', 0));
    }
    ecuDataModel->setItem(9, currentColumn - 1, flightTimeItem);
    
    // 状态
    bool hasError = data.throttleError || data.engineSpeedError || data.cylinderTempError || 
                   data.exhaustTempError || data.axleTempError || data.fuelPressureError || 
                   data.intakeTempError || data.atmPressureError || data.flightTimeError;
    QStandardItem *statusItem = new QStandardItem(hasError ? "故障" : "正常");
    if (hasError) {
        statusItem->setBackground(QBrush(QColor(255, 200, 200)));
    }
    ecuDataModel->setItem(10, currentColumn - 1, statusItem);
    
    // 如果超出最大列数，删除最旧的列
    const int maxColumns = 100;
    if (currentColumn > maxColumns) {
        ecuDataModel->removeColumn(0);
    }
    
    // 滚动到最新列
    if (ecuDataModel->columnCount() > 0) {
        ui->tableViewECU->scrollTo(ecuDataModel->index(0, ecuDataModel->columnCount() - 1), QAbstractItemView::EnsureVisible);
    }
    
    // 显示状态栏信息
    sBar->showMessage("收到ECU数据 - " + data.timestamp.toString("hh:mm:ss.zzz"), 2000);
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
    
    // 保存其他通信设置
    // ...可以根据需要添加其他配置项
    
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

// 更新仪表盘的方法
// void MainWindow::updateDashboards(const QVector<double> &data)
// {
//     // 不再使用此方法，已被updateDashboardByMapping替代
// }

// 根据映射关系更新仪表盘显示
void MainWindow::updateDashboardByMapping(const QVector<double> &modbusData, 
                                       const QVector<QVector<double>> &daqData, 
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
    
    // 2. DAQ数据 (B_x)
    if (!daqData.isEmpty()) {
        for (int i = 0; i < daqData.size(); i++) {
            if (!daqData[i].isEmpty()) {
                // 使用最新的值
                QString varName = QString("B_%1").arg(i);
                currentVarMap[varName] = daqData[i].last();
                updatedVars.insert(varName);
            }
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
                    if (!daqData.isEmpty() && mapping.channelIndex < daqData.size() && !daqData[mapping.channelIndex].isEmpty()) {
                        currentValue = daqData[mapping.channelIndex].last();
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

// 设置仪表盘更新定时器
void MainWindow::setupDashboardUpdateTimer()
{
    // 创建仪表盘更新定时器
    dashboardUpdateTimer = new QTimer(this);
    
    // 使用Qt::QueuedConnection确保在主线程中更新UI
    connect(dashboardUpdateTimer, &QTimer::timeout, this, [this]() {
        if (!daqIsAcquiring || daqChannelData.isEmpty()) {
            return;
        }
        
        // 移除减少更新频率的静态计数器，确保每次都刷新
        qDebug() << "更新仪表盘: 时间=" << QTime::currentTime().toString();
        
        // 使用当前数据更新仪表盘
        static ECUData emptyEcuData;
        static QVector<double> emptyModbusData;
        updateDashboardByMapping(emptyModbusData, daqChannelData, emptyEcuData);
    }, Qt::QueuedConnection);
    
    // 不要立即启动定时器，将在handleDAQStatus中启动
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
    // 获取ECU图表容器
    QWidget *ecuPlotContainer = ui->groupBoxECUplot;
    if (!ecuPlotContainer) {
        qDebug() << "错误: 无法找到ECU图表容器";
        return;
    }
    
    // 清除现有的布局和控件
    if (ecuPlotContainer->layout()) {
        QLayoutItem *item;
        while ((item = ecuPlotContainer->layout()->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        delete ecuPlotContainer->layout();
    }
    
    // 创建新的滚动区域
    QVBoxLayout *containerLayout = new QVBoxLayout(ecuPlotContainer);
    containerLayout->setContentsMargins(5, 5, 5, 5);
    
    QScrollArea *scrollArea = new QScrollArea(ecuPlotContainer);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QWidget *scrollWidget = new QWidget();
    QVBoxLayout *plotsLayout = new QVBoxLayout(scrollWidget);
    plotsLayout->setSpacing(10);
    scrollArea->setWidget(scrollWidget);
    
    containerLayout->addWidget(scrollArea);
    
    // 创建颜色列表
    QStringList colorNames = {"blue", "red", "green", "magenta", "cyan", "darkGreen", "darkRed", "darkBlue"};
    
    // 为每个ECU参数创建图表
    QStringList ecuParams = {"节气门开度", "发动机转速", "缸温", "排温", "轴温", "燃油压力", "进气温度", "大气压力"};
    
    ecuPlots.clear();
    ecuArrows.clear();
    ecuValueLabels.clear();
    
    for (int i = 0; i < ecuParams.size(); ++i) {
        // 创建图表容器
        QWidget *plotContainer = new QWidget(scrollWidget);
        QHBoxLayout *plotLayout = new QHBoxLayout(plotContainer);
        plotLayout->setContentsMargins(5, 5, 5, 5);
        
        // 创建图表
        QCustomPlot *plot = new QCustomPlot(plotContainer);
        plot->setMinimumHeight(150);
        
        // 设置图表属性
        plot->xAxis->setLabel("时间 (秒)");
        plot->yAxis->setLabel(ecuParams[i]);
        
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
        
        // 将数值标签添加到容器
        valueLayout->addStretch();
        valueLayout->addWidget(valueLabel);
        valueLayout->addStretch();
        
        // 将图表和数值容器添加到布局
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
        
        // 保存图表和标签的引用
        ecuPlots.append(plot);
        ecuArrows.append(arrowLabel);
        ecuValueLabels.append(valueLabel);
        
        // 将容器添加到滚动区域
        plotsLayout->addWidget(plotContainer);
    }
    
    // 设置初始范围
    for (QCustomPlot *plot : ecuPlots) {
        plot->xAxis->setRange(0, 60);
        plot->yAxis->setRange(0, 100);
        plot->replot();
    }
    
    qDebug() << "ECU图表初始化完成，创建了" << ecuPlots.size() << "个参数的图表";
}

void MainWindow::updateECUPlot(const ECUData &data)
{
    if (ecuPlots.isEmpty() || !data.isValid) {
        return;
    }
    
    // 增加时间戳
    ecuTimeData.append(data.timer.elapsed() / 1000.0); // 转换为秒
    
    // 确保数据向量长度匹配
    if (ecuData.isEmpty()) {
        // 初始化数据向量
        for (int i = 0; i < 8; ++i) {
            ecuData.append(QVector<double>());
        }
    }
    
    // 添加新数据点
    ecuData[0].append(data.throttle);
    ecuData[1].append(data.engineSpeed);
    ecuData[2].append(data.cylinderTemp);
    ecuData[3].append(data.exhaustTemp);
    ecuData[4].append(data.axleTemp);
    ecuData[5].append(data.fuelPressure);
    ecuData[6].append(data.intakeTemp);
    ecuData[7].append(data.atmPressure);
    
    // 限制数据点数量，避免内存占用过多
    const int maxPoints = 3600; // 保存1小时的数据（每秒一个点）
    if (ecuTimeData.size() > maxPoints) {
        ecuTimeData.removeFirst();
        for (int i = 0; i < ecuData.size(); ++i) {
            if (ecuData[i].size() > maxPoints) {
                ecuData[i].removeFirst();
            }
        }
    }
    
    // 更新每个图表
    for (int i = 0; i < ecuPlots.size() && i < ecuData.size(); ++i) {
        QCustomPlot *plot = ecuPlots[i];
        if (!plot) continue;
        
        // 设置数据
        plot->graph(0)->setData(ecuTimeData, ecuData[i]);
        
        // 获取最新值
        double latestValue = ecuData[i].last();
        
        // 更新值标签
        if (i < ecuValueLabels.size() && ecuValueLabels[i]) {
            ecuValueLabels[i]->setText(QString::number(latestValue, 'f', 2));
            ecuValueLabels[i]->move(plot->width() - 60, 10); // 固定在右上角
            ecuValueLabels[i]->show();
        }
        
        // 更新箭头位置
        if (i < ecuArrows.size() && ecuArrows[i]) {
            // 计算Y坐标
            double pixelY = plot->yAxis->coordToPixel(latestValue);
            
            // 更新箭头位置 - 放置在图表右侧边线上，指向左侧
            ecuArrows[i]->setText("←"); // 确保箭头指向左侧
            ecuArrows[i]->move(plot->width() - 30, pixelY - ecuArrows[i]->height()/2);
        }
        
        // 更新X轴范围，显示最新的数据
        double latestTime = ecuTimeData.last();
        double timeWindow = 60.0; // 显示最近60秒的数据
        if (latestTime > timeWindow) {
            plot->xAxis->setRange(latestTime - timeWindow, latestTime);
        } else {
            plot->xAxis->setRange(0, timeWindow);
        }
        
        // 获取最新值和当前Y轴范围
        double yMin = plot->yAxis->range().lower;
        double yMax = plot->yAxis->range().upper;
        
        // 检查最新值是否在当前范围内，如果不在则调整范围
        if (latestValue < yMin || latestValue > yMax) {
            // 计算最近60个点的范围（或全部点，如果点数少于60）
            int startIndex = qMax(0, ecuData[i].size() - 60);
            double dataMin = ecuData[i][startIndex];
            double dataMax = dataMin;
            
            for (int j = startIndex; j < ecuData[i].size(); ++j) {
                dataMin = qMin(dataMin, ecuData[i][j]);
                dataMax = qMax(dataMax, ecuData[i][j]);
            }
            
            // 确保范围不会太小
            double range = dataMax - dataMin;
            if (range < 0.1) range = 0.1;
            
            // 设置新的Y轴范围，增加一些边距
            plot->yAxis->setRange(dataMin - range * 0.1, dataMax + range * 0.1);
        }
        
        // 重绘图表
        plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

// 添加新的ECU数据点
void MainWindow::addECUDataPoint(double time, const QVector<double> &values)
{
    // 添加时间点
    ecuTimeData.append(time);
    
    // 确保数据向量长度匹配
    if (ecuData.isEmpty()) {
        // 初始化数据向量
        for (int i = 0; i < 8; ++i) {
            ecuData.append(QVector<double>());
        }
    }
    
    // 添加各通道数据
    for (int i = 0; i < values.size() && i < ecuData.size(); ++i) {
        ecuData[i].append(values[i]);
    }
    
    // 限制数据点数量，防止内存占用过大
    int maxPoints = 10000;
    while (ecuTimeData.size() > maxPoints) {
        ecuTimeData.removeFirst();
        for (int i = 0; i < ecuData.size(); ++i) {
            if (!ecuData[i].isEmpty()) {
                ecuData[i].removeFirst();
            }
        }
    }
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



