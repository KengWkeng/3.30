#include "calibrationdialog.h"
#include <QApplication>
#include <QScreen>
#include <QCoreApplication>
#include <QTextCodec>
#include <vector>
#include <algorithm>
#include <cmath>

// Constructor
CalibrationDialog::CalibrationDialog(SnapshotThread *thread, QWidget *parent)
    : QDialog(parent), snapshotThread(thread),
      currentDeviceType("Temperature Sensor Calibration"), // 默认设备
      currentChannelIndex(0),      // 默认通道
      isCollectingPoints(false),
      latestRawValue(0.0),
      coef_a(0.0), coef_b(0.0), coef_c(1.0), coef_d(0.0), // Default y=x
      latestSnapshotIndex(-1),
      isCalibrationRunning(false), // Initialize state
      isBatchCalibration(false)    // 初始非批量校准模式
{
    // Ensure thread pointer is valid
    if (!snapshotThread) {
        qCritical() << "[CalibrationDialog] SnapshotThread pointer is null!";
        // Handle error appropriately, maybe throw or disable functionality
        QMessageBox::critical(this, "错误", "数据采集线程无效!");
        // We might need to disable UI elements here if the thread is null
    }

    // Set window properties
    setWindowTitle(tr("传感器校准"));
    setMinimumSize(800, 600);
    QScreen *primaryScreen = QGuiApplication::primaryScreen();
    if (primaryScreen) {
        QSize screenSize = primaryScreen->availableSize(); // Use available size
    resize(screenSize.width() * 0.8, screenSize.height() * 0.8);
    }

    // Initialize UI elements and layout
    setupUI();

    // Connect signals and slots
    connectSignalsSlots();

    // Initialize plot
    initializePlot();

    // Initialize calibration data storage
    calibrationPoints.clear();
    tempMeasurements.clear();
    channelCalibrationPoints.clear();
    channelCalibrationResults.clear();

    // Configure timers
    dataUpdateTimer.setInterval(100);  // Update UI every 100ms
    pointCollectionTimer.setInterval(5000); // 5-second collection period
    pointCollectionTimer.setSingleShot(true); // Timer only fires once per start()

    // Initialize channel colors for multi-channel display
    channelColors = {Qt::blue, Qt::red, Qt::green, Qt::magenta, Qt::cyan,
                     Qt::yellow, Qt::darkBlue, Qt::darkRed, Qt::darkGreen,
                     Qt::darkMagenta, Qt::darkCyan, Qt::darkYellow, Qt::black,
                     Qt::gray, Qt::lightGray, Qt::darkGray};

    // Set default selections in combo boxes
    deviceComboBox->setCurrentText("Temperature Sensor Calibration");
    // onDeviceChanged will be called automatically

    // Ensure we select "All Channels" option by default for Modbus
    // and initialize the batch calibration mode
    QTimer::singleShot(100, this, [this]() {
        // Force Modbus device selection and find valid channels
        if (getDeviceInternalName(currentDeviceType) == "Modbus") {
            // Find valid channels
            findValidChannels();

            // Initialize batch calibration mode
            isBatchCalibration = true;

            // Update the channel combo box
            updateChannelComboBox();

            // Select the "All Channels" option (index 0)
            if (channelComboBox->count() > 0) {
                channelComboBox->setCurrentIndex(0);
                qDebug() << "[CalibrationDialog] Auto-selected 'All Channels' option for Modbus on startup";
            }

            // Initialize plot for multi-channel display
            plot->clearGraphs();
            for (int i = 0; i < validChannels.size(); ++i) {
                int ch = validChannels[i];
                plot->addGraph();
                plot->graph(i)->setPen(QPen(channelColors[i % channelColors.size()]));
                plot->graph(i)->setName(QString(tr("通道 %1")).arg(ch));
            }
            plot->xAxis->setLabel(tr("时间 (s)"));
            plot->yAxis->setLabel(tr("温度值"));
            plot->legend->setVisible(true);
            plot->replot();
        }
    });

    // Start the UI update timer
    dataUpdateTimer.start();

    // Initial UI state (calibration not running)
    updateUiForCalibrationState(false);
}

// Destructor
CalibrationDialog::~CalibrationDialog()
{
    // Stop timers
    dataUpdateTimer.stop();
    pointCollectionTimer.stop();
    // UI elements are managed by Qt's parent-child system
}

// Public method to update channels when Modbus settings change
void CalibrationDialog::updateModbusChannels()
{
    // Only update if the current device is Modbus
    if (getDeviceInternalName(currentDeviceType) == "Modbus") {
        qDebug() << "[CalibrationDialog] Updating Modbus channels due to register settings change";

        // Store current selection state
        int currentIndex = channelComboBox->currentIndex();

        // Update the channel combo box
        updateChannelComboBox();

        // If we're in batch calibration mode, we need to update the plot
        if (isBatchCalibration) {
            // Clear existing graphs
            plot->clearGraphs();

            // Create a graph for each valid channel
            for (int i = 0; i < validChannels.size(); ++i) {
                int ch = validChannels[i];
                plot->addGraph();
                plot->graph(i)->setPen(QPen(channelColors[i % channelColors.size()]));
                plot->graph(i)->setName(QString(tr("通道 %1")).arg(ch));
            }

            // Update plot properties
            plot->legend->setVisible(true);
            plot->replot();
        }
    }
}

// Setup UI layout and controls
void CalibrationDialog::setupUI()
{
    // Main layout (vertical)
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Top area: Plot (left) and Controls (right)
    QHBoxLayout *topLayout = new QHBoxLayout();

    // --- Left Area (Plot and Table) - 3/4 of width ---
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // Plot - Upper section
    plot = new QCustomPlot();
    plot->setMinimumSize(400, 300); // Adjusted minimum size
    leftLayout->addWidget(plot, 3); // Plot takes more vertical space

    // Calibration Table - Lower section
    calibrationTable = new QTableWidget();
    calibrationTable->horizontalHeader()->setVisible(false); // Hide default horizontal header
    calibrationTable->verticalHeader()->setVisible(false); // Hide default vertical header
    calibrationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Stretch columns
    calibrationTable->setSelectionBehavior(QAbstractItemView::SelectItems); // Allow selecting individual cells
    calibrationTable->setSelectionMode(QAbstractItemView::SingleSelection); // Allow single selection
    calibrationTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Make table read-only
    calibrationTable->setMinimumHeight(200); // Increase minimum height for better visibility
    leftLayout->addWidget(calibrationTable, 1); // Table takes less space

    topLayout->addLayout(leftLayout, 3); // Left area takes 3/4 width

    // --- Right Area (Controls) - 1/4 of width ---
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // Upper Section - Device and Channel Selection
    QGroupBox *deviceGroupBox = new QGroupBox(tr("设备和通道选择"));
    QFormLayout *deviceLayout = new QFormLayout(); // Use QFormLayout for label-widget pairs
    deviceComboBox = new QComboBox();
    deviceComboBox->addItems({"Temperature Sensor Calibration", "Force Sensor Calibration", "ECU Data Correction", "CustomData Correction"});
    channelComboBox = new QComboBox();
    deviceLayout->addRow(tr("设备类型:"), deviceComboBox);
    deviceLayout->addRow(tr("通道:"), channelComboBox);
    deviceGroupBox->setLayout(deviceLayout);
    rightLayout->addWidget(deviceGroupBox);

    // Lower Section - Real-time values and calibration controls
    QGroupBox *valueGroupBox = new QGroupBox(tr("实时数据与校准输入"));
    QFormLayout *valueLayout = new QFormLayout();
    currentValueLabel = new QLabel("0.0"); // Label to display current raw value
    currentValueLabel->setMinimumWidth(80);
    currentValueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    calibrationInput = new QDoubleSpinBox(); // Input for standard calibration value
    calibrationInput->setRange(-99999.9999, 99999.9999);
    calibrationInput->setDecimals(4);
    calibrationInput->setSingleStep(0.1);
    valueLayout->addRow(tr("当前原始值:"), currentValueLabel);
    valueLayout->addRow(tr("输入标准值:"), calibrationInput);
    valueGroupBox->setLayout(valueLayout);
    rightLayout->addWidget(valueGroupBox);

    // Buttons section
    QVBoxLayout *buttonsLayout = new QVBoxLayout();

    // Start/Stop Calibration Button
    startStopButton = new QPushButton(tr("开始校准"));
    buttonsLayout->addWidget(startStopButton);

    // Add/Remove Point Buttons
    QHBoxLayout *pointButtonLayout = new QHBoxLayout();
    addPointButton = new QPushButton(tr("添加校准点"));
    removePointButton = new QPushButton(tr("移除选中点"));
    pointButtonLayout->addWidget(addPointButton);
    pointButtonLayout->addWidget(removePointButton);
    buttonsLayout->addLayout(pointButtonLayout);

    // Initialize Current Channel Button
    QPushButton *initChannelButton = new QPushButton(tr("初始化当前通道"));
    buttonsLayout->addWidget(initChannelButton);
    connect(initChannelButton, &QPushButton::clicked, [this]() {
        // Clear calibration points for current channel
        if (isBatchCalibration) {
            for (int ch : validChannels) {
                channelCalibrationPoints[ch].clear();
            }
            updateMultiChannelCalibrationTable();
        } else {
            calibrationPoints.clear();
            updateCalibrationTable();
        }
        checkCalibrationButtonState();
        QMessageBox::information(this, tr("通道初始化"), tr("当前通道的校准点已清除"));
    });

    // Initialize Calibration Parameters Button
    initializeButton = new QPushButton(tr("初始化校准参数"));
    buttonsLayout->addWidget(initializeButton);
    connect(initializeButton, &QPushButton::clicked, [this]() {
        // Confirm with user before initializing
        QMessageBox::StandardButton reply;
        if (isBatchCalibration && getDeviceInternalName(currentDeviceType) == "Modbus") {
            reply = QMessageBox::question(this, tr("确认初始化"),
                                        tr("是否要将所有有效通道的校准参数初始化为默认值 (y = x)？\n\n这将覆盖现有的校准参数。"),
                                        QMessageBox::Yes | QMessageBox::No);
        } else {
            reply = QMessageBox::question(this, tr("确认初始化"),
                                        tr("是否要将通道 %1 的校准参数初始化为默认值 (y = x)？\n\n这将覆盖现有的校准参数。")
                                        .arg(currentChannelIndex),
                                        QMessageBox::Yes | QMessageBox::No);
        }

        if (reply == QMessageBox::Yes) {
            initializeCalibrationParams();
        }
    });

    // Add buttons layout to right side
    rightLayout->addLayout(buttonsLayout);

    rightLayout->addStretch(); // Pushes buttons to the bottom

    // Bottom Buttons (Confirm, Clear, Close)
    QHBoxLayout *bottomButtonLayout = new QHBoxLayout();
    confirmButton = new QPushButton(tr("计算并保存系数")); // Renamed button
    confirmButton->setObjectName("confirmButton"); // Use new object name
    confirmButton->setEnabled(false); // Initially disabled
    clearButton = new QPushButton(tr("清除所有点"));
    closeButton = new QPushButton(tr("关闭"));
    bottomButtonLayout->addWidget(confirmButton);
    bottomButtonLayout->addWidget(clearButton);
    bottomButtonLayout->addStretch();
    bottomButtonLayout->addWidget(closeButton);
    rightLayout->addLayout(bottomButtonLayout);

    topLayout->addLayout(rightLayout, 1); // Right area takes 1/4 width
    mainLayout->addLayout(topLayout);
    setLayout(mainLayout);
}

// Initialize QCustomPlot settings
void CalibrationDialog::initializePlot()
{
    if (!plot) return;
    plot->clearGraphs();

    // Define a set of distinct colors for multiple channels
    channelColors = {
        Qt::blue, Qt::red, Qt::green, Qt::magenta, Qt::cyan,
        Qt::yellow, Qt::darkBlue, Qt::darkRed, Qt::darkGreen,
        Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow, Qt::black
    };

    // Check if we're in batch calibration mode for Modbus
    if (isBatchCalibration && getDeviceInternalName(currentDeviceType) == "Modbus") {
        // Create a graph for each valid channel
        for (int i = 0; i < validChannels.size(); ++i) {
            int ch = validChannels[i];
            plot->addGraph();
            plot->graph(i)->setPen(QPen(channelColors[i % channelColors.size()]));
            plot->graph(i)->setName(QString(tr("通道 %1")).arg(ch));
        }

        // Set plot properties for multi-channel display
        plot->xAxis->setLabel(tr("时间 (s)"));
        plot->yAxis->setLabel(tr("温度值"));
        plot->legend->setVisible(true);
    } else {
        // Single channel mode
        plot->addGraph(); // Add the main graph for the selected channel
        plot->graph(0)->setPen(QPen(Qt::blue));
        plot->graph(0)->setName("Selected Channel"); // Generic name initially

        plot->xAxis->setLabel(tr("时间 (s 相对于对话框打开)"));
        plot->yAxis->setLabel(tr("原始值"));
        plot->legend->setVisible(false);
    }

    // Common settings
    plot->xAxis->setRange(0, 20); // Initial 20-second view
    plot->yAxis->setRange(0, 100); // Default Y range
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    plot->replot();
}

// Connect signals and slots
void CalibrationDialog::connectSignalsSlots()
{
    // Device/Channel selection changes
    connect(deviceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CalibrationDialog::onDeviceChanged);
    connect(channelComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CalibrationDialog::onChannelChanged);

    // Button clicks
    connect(startStopButton, &QPushButton::clicked, this, &CalibrationDialog::onStartStopClicked); // +++ Connect Start/Stop +++
    connect(addPointButton, &QPushButton::clicked, this, &CalibrationDialog::onAddPointClicked);
    connect(removePointButton, &QPushButton::clicked, this, &CalibrationDialog::onRemovePointClicked);
    connect(confirmButton, &QPushButton::clicked, this, &CalibrationDialog::onConfirmClicked); // +++ Connect Confirm +++ (was onCalibrateClicked)
    connect(clearButton, &QPushButton::clicked, this, &CalibrationDialog::onClearClicked);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::reject); // Use reject to signal cancellation if needed

    // Timer timeouts
    connect(&dataUpdateTimer, &QTimer::timeout, this, &CalibrationDialog::onDataUpdateTimerTimeout);
    connect(&pointCollectionTimer, &QTimer::timeout, this, &CalibrationDialog::onPointCollectionTimerTimeout);

    // Connect to SnapshotThread's raw data signal
    if (snapshotThread) {
        connect(snapshotThread, &SnapshotThread::rawSnapshotReady, this, &CalibrationDialog::onRawSnapshotReceived);
    } else {
        qWarning() << "[CalibrationDialog] Cannot connect to SnapshotThread signals, thread is null!";
    }
}

// 查找有效的通道
void CalibrationDialog::findValidChannels()
{
    validChannels.clear();

    // 只有在温度传感器校准（Modbus）模式下才进行有效通道检测
    if (getDeviceInternalName(currentDeviceType) != "Modbus") {
        return;
    }

    // 获取主窗口实例
    QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parent());
    if (!mainWindow) {
        qWarning() << "[CalibrationDialog] Cannot find MainWindow parent!";
        // 如果无法获取主窗口，则使用默认方法
        useDefaultChannelDetection();
        return;
    }

    // 查找寄存器地址和数量输入框
    QLineEdit *lineSegAddress = mainWindow->findChild<QLineEdit*>("lineSegAddress");
    QLineEdit *lineSegNum = mainWindow->findChild<QLineEdit*>("lineSegNum");

    if (!lineSegAddress || !lineSegNum) {
        qWarning() << "[CalibrationDialog] Cannot find Modbus register input fields!";
        // 如果无法获取输入框，则使用默认方法
        useDefaultChannelDetection();
        return;
    }

    // 获取寄存器起始地址和数量
    int startAddress = lineSegAddress->text().toInt();
    int registerCount = lineSegNum->text().toInt();

    // 确保寄存器数量至少为1
    if (registerCount < 1) {
        registerCount = 1;
    }

    // 创建通道编号列表 - 总是包含所有配置的通道
    for (int i = 0; i < registerCount; i++) {
        int channelIndex = i; // 使用相对索引作为通道编号
        validChannels.append(channelIndex);
    }

    qDebug() << "[CalibrationDialog] Using all Modbus channels from register settings: Start=" << startAddress
             << ", Count=" << registerCount << ", Channels:" << validChannels;
}

// 使用默认方法检测有效通道
void CalibrationDialog::useDefaultChannelDetection()
{
    // 在默认情况下，添加所有可能的Modbus通道（最外16个）
    int defaultChannelCount = 16; // 默认支持16个Modbus通道

    // 如果有有效的快照数据，使用实际的数据大小
    if (latestRawSnapshot.modbusValid && !latestRawSnapshot.modbusData.isEmpty()) {
        defaultChannelCount = latestRawSnapshot.modbusData.size();
    }

    // 添加所有通道
    for (int i = 0; i < defaultChannelCount; ++i) {
        validChannels.append(i);
    }

    qDebug() << "[CalibrationDialog] Using all Modbus channels (default method):" << validChannels;
}

// Update channel combo box based on selected device
void CalibrationDialog::updateChannelComboBox()
{
    channelComboBox->blockSignals(true); // Prevent triggering onChannelChanged during update
    channelComboBox->clear();

    QString internalDeviceType = getDeviceInternalName(currentDeviceType);

    // 温度传感器校准（Modbus）特殊处理
    if (internalDeviceType == "Modbus") {
        // 查找有效通道
        findValidChannels();

        if (!validChannels.isEmpty()) {
            // 添加"所有通道"选项
            channelComboBox->addItem(
                QString("所有通道：%1~%2").arg(validChannels.first()).arg(validChannels.last()),
                -1); // -1 表示所有通道

            // 设置批量校准模式
            isBatchCalibration = true;
        } else {
            // 如果没有有效通道，显示提示信息
            channelComboBox->addItem("无有效通道", -2); // -2 表示无效状态
            isBatchCalibration = false;
        }
    } else {
        // 其他设备类型，按常规方式处理
        int count = getChannelCount(internalDeviceType);

        // 查找有效通道（只显示有效的）
        QVector<int> deviceValidChannels;
        if (internalDeviceType == "DAQ") {
            // For DAQ, always show all configured channels
            qDebug() << "[CalibrationDialog] DAQ channel count:" << count;
            qDebug() << "[CalibrationDialog] DAQ data valid:" << latestRawSnapshot.daqValid;
            if (latestRawSnapshot.daqValid) {
                qDebug() << "[CalibrationDialog] DAQ data size:" << latestRawSnapshot.daqData.size();
            }

            // Get DAQ channels from MainWindow
            QMainWindow *mainWindow = qobject_cast<QMainWindow*>(parent());
            if (mainWindow) {
                QLineEdit *channelsEdit = mainWindow->findChild<QLineEdit*>("channelsEdit");
                if (channelsEdit) {
                    QString channelStr = channelsEdit->text();
                    QStringList parts = channelStr.split('/', Qt::SkipEmptyParts);
                    count = parts.size();
                    qDebug() << "[CalibrationDialog] DAQ channels from MainWindow:" << channelStr << "Count:" << count;
                }
            }

            // Always add all channels
            for (int i = 0; i < count; ++i) {
                deviceValidChannels.append(i);
            }
        }
        else if (internalDeviceType == "ECU") {
            // For ECU, always show all 9 standard channels
            qDebug() << "[CalibrationDialog] ECU channel count:" << count;
            qDebug() << "[CalibrationDialog] ECU data valid:" << latestRawSnapshot.ecuValid;
            if (latestRawSnapshot.ecuValid) {
                qDebug() << "[CalibrationDialog] ECU data size:" << latestRawSnapshot.ecuData.size();
            }

            // Always add all ECU channels
            for (int i = 0; i < count; ++i) {
                deviceValidChannels.append(i);
            }
        }
        else if (internalDeviceType == "Custom") {
            for (int i = 0; i < qMin(count, latestRawSnapshot.customData.size()); ++i) {
                deviceValidChannels.append(i); // Custom数据通常是计算出来的，都视为有效
            }
        }

        // 如果没有找到有效通道，则使用全部通道
        if (deviceValidChannels.isEmpty()) {
            for (int i = 0; i < count; ++i) {
                deviceValidChannels.append(i);
            }
        }

        // 添加有效通道到下拉框
        for (int i : deviceValidChannels) {
            channelComboBox->addItem(getChannelName(internalDeviceType, i), i);
        }

        // 非批量校准模式
        isBatchCalibration = false;
    }

    channelComboBox->blockSignals(false);

    // Set current index after populating, triggering onChannelChanged if index changes
    if (channelComboBox->count() > 0) {
        channelComboBox->setCurrentIndex(0); // Default to first channel
    } else {
         onChannelChanged(-1); // Handle case with no channels
    }
}

// Get channel count for a device type
int CalibrationDialog::getChannelCount(const QString &deviceType)
{
    if (deviceType == "Modbus") return 16;
    if (deviceType == "DAQ") return 16; // Assuming 16 DAQ channels
    if (deviceType == "ECU") return 9;
    if (deviceType == "Custom") return 5;
    return 0;
}

// Get display name for a channel
QString CalibrationDialog::getChannelName(const QString &deviceType, int index)
{
    if (deviceType == "Modbus") {
        return QString(tr("通道 %1")).arg(index);
    }
    else if (deviceType == "DAQ") {
        return QString(tr("力传感器通道 %1")).arg(index);
    }
    else if (deviceType == "ECU") {
        QStringList ecuNames = {
            tr("节气门开度"),       // Throttle
            tr("发动机转速"),       // Engine Speed
            tr("缸温"),                   // Cylinder Temp
            tr("排气温度"),           // Exhaust Temp
            tr("轴温"),                   // Axle Temp
            tr("燃油压力"),           // Fuel Pressure
            tr("进气温度"),           // Intake Temp
            tr("大气压力"),           // Atm Pressure
            tr("飞行时间")            // Flight Time
        };

        if (index >= 0 && index < ecuNames.size()) {
            return ecuNames.at(index);
        }
    }
    else if (deviceType == "Custom") {
        return QString(tr("自定义数据 %1")).arg(index);
    }

    // Default name for other types or invalid index
    return QString(tr("通道 %1")).arg(index);
}

// Slot called when device selection changes
void CalibrationDialog::onDeviceChanged(int index)
{
    currentDeviceType = deviceComboBox->itemText(index);
    QString internalDeviceType = getDeviceInternalName(currentDeviceType);
    qDebug() << "[CalibrationDialog] Device changed to:" << currentDeviceType
             << "(internal name:" << internalDeviceType << ")";

    // Clear existing calibration points when device type changes
    calibrationPoints.clear();
    channelCalibrationPoints.clear();
    excludedColumns.clear();

    // For temperature sensors (Modbus), we need to handle batch calibration
    if (internalDeviceType == "Modbus") {
        // Find valid channels first
        findValidChannels();

        // Initialize empty point collections for each valid channel
        for (int ch : validChannels) {
            channelCalibrationPoints[ch] = QVector<QPair<double, double>>();
        }

        // Set batch calibration mode
        isBatchCalibration = true;

        // Update plot for multi-channel display
        plot->clearGraphs();

        // Create a graph for each valid channel with different colors
        for (int i = 0; i < validChannels.size(); ++i) {
            int ch = validChannels[i];
            plot->addGraph();
            plot->graph(i)->setPen(QPen(channelColors[i % channelColors.size()]));
            plot->graph(i)->setName(QString(tr("通道 %1")).arg(ch));
        }

        // Set plot properties
        plot->xAxis->setLabel(tr("时间 (s)"));
        plot->yAxis->setLabel(tr("温度值"));
        plot->legend->setVisible(true);
        plot->replot();
    }
    else if (internalDeviceType == "DAQ") {
        // For DAQ, use single channel mode by default
        isBatchCalibration = false;

        // Reset to single graph
        plot->clearGraphs();
        plot->addGraph();
        plot->graph(0)->setPen(QPen(Qt::blue));
        plot->graph(0)->setName(tr("力传感器"));
        plot->xAxis->setLabel(tr("时间 (s)"));
        plot->yAxis->setLabel(tr("力值"));
        plot->legend->setVisible(true);
        plot->replot();

        // Check if DAQ data is valid
        if (!latestRawSnapshot.daqValid) {
            qDebug() << "[CalibrationDialog] Warning: DAQ data is not valid in the latest snapshot";
        } else {
            qDebug() << "[CalibrationDialog] DAQ data is valid, channels:" << latestRawSnapshot.daqData.size();
        }
    }
    else if (internalDeviceType == "ECU") {
        // For ECU, use single channel mode by default
        isBatchCalibration = false;

        // Reset to single graph
        plot->clearGraphs();
        plot->addGraph();
        plot->graph(0)->setPen(QPen(Qt::green));
        plot->graph(0)->setName(tr("ECU数据"));
        plot->xAxis->setLabel(tr("时间 (s)"));
        plot->yAxis->setLabel(tr("ECU值"));
        plot->legend->setVisible(true);
        plot->replot();

        // Check if ECU data is valid
        if (!latestRawSnapshot.ecuValid) {
            qDebug() << "[CalibrationDialog] Warning: ECU data is not valid in the latest snapshot";
        } else {
            qDebug() << "[CalibrationDialog] ECU data is valid, channels:" << latestRawSnapshot.ecuData.size();
        }
    }
    else {
        // For other device types, use single channel mode
        isBatchCalibration = false;

        // Reset to single graph
        plot->clearGraphs();
        plot->addGraph();
        plot->graph(0)->setPen(QPen(Qt::blue));
        plot->graph(0)->setName(internalDeviceType);
        plot->legend->setVisible(false);
        plot->replot();
    }

    updateChannelComboBox();
    // onChannelChanged will be called automatically by setCurrentIndex in updateChannelComboBox
}

// Slot called when channel selection changes
void CalibrationDialog::onChannelChanged(int index)
{
    if (index < 0 || index >= channelComboBox->count()) {
        currentChannelIndex = -1;
        qWarning() << "[CalibrationDialog] Invalid channel index:" << index;
        // Clear plot and label if channel is invalid
        plot->graph(0)->data()->clear();
        plot->replot();
        currentValueLabel->setText("N/A");
        plot->yAxis->setLabel(tr("原始值"));
        plot->graph(0)->setName("Invalid Channel");
        return;
    }

    // 获取通道索引数据
    QVariant channelData = channelComboBox->itemData(index);
    int selectedChannel = channelData.toInt();

    // 检查是否是批量校准模式（所有通道）
    if (selectedChannel == -1 && isBatchCalibration) {
        currentChannelIndex = -1; // 表示所有通道
        qDebug() << "[CalibrationDialog] All channels selected for batch calibration";

        // 为所有有效通道初始化校准点结构
        channelCalibrationPoints.clear();
        channelCalibrationResults.clear();
        for (int ch : validChannels) {
            channelCalibrationPoints[ch] = QVector<QPair<double, double>>();
        }

        // 初始化多通道绘图
        plot->clearGraphs();
        QStringList lineColors = {"blue", "red", "green", "cyan", "magenta", "yellow", "black"};

        for (int i = 0; i < validChannels.size(); ++i) {
            int ch = validChannels[i];
            plot->addGraph();
            // 使用颜色循环
            QColor color(lineColors[i % lineColors.size()]);
            plot->graph(i)->setPen(QPen(color));
            QString chName = QString("通道 %1").arg(ch);
            plot->graph(i)->setName(chName);
        }

        // 更新绘图标签
        plot->xAxis->setLabel(tr("时间 (s 相对于对话框打开)"));
        plot->yAxis->setLabel(tr("温度传感器原始值"));
        plot->legend->setVisible(true); // 显示图例
        plot->xAxis->setRange(0, 20); // 初始显示20秒
        plot->yAxis->setRange(0, 100); // 默认Y范围
        plot->replot();

        // 重置当前值显示
        QVector<double> latestValues = getMultiChannelRawValues(latestRawSnapshot);
        updateMultiChannelValueLabel(latestValues);
        } else {
        currentChannelIndex = selectedChannel;
        qDebug() << "[CalibrationDialog] Single channel changed to index:" << currentChannelIndex;

        // 重置单通道绘图
        plot->clearGraphs();
        plot->addGraph();
        plot->graph(0)->setPen(QPen(Qt::blue));
        plot->graph(0)->setName(QString("%1 Ch %2")
                            .arg(getDeviceInternalName(currentDeviceType))
                            .arg(currentChannelIndex));

        // 更新绘图标签
        QString yAxisLabel = QString("%1 - %2 (%3)")
                           .arg(currentDeviceType)
                           .arg(getChannelName(getDeviceInternalName(currentDeviceType), currentChannelIndex))
                           .arg(tr("原始值"));
        plot->xAxis->setLabel(tr("时间 (s 相对于对话框打开)"));
        plot->yAxis->setLabel(yAxisLabel);
        plot->legend->setVisible(false); // 隐藏图例
        plot->xAxis->setRange(0, 20);
        plot->replot();

        // 重置当前值显示
        updateCurrentValueLabel(latestRawValue);
    }
}

// 获取多通道原始值
QVector<double> CalibrationDialog::getMultiChannelRawValues(const DataSnapshot& snapshot)
{
    QVector<double> values;
    QString internalDeviceType = getDeviceInternalName(currentDeviceType);

    // 只处理温度传感器（Modbus）批量校准
    if (internalDeviceType == "Modbus" && isBatchCalibration) {
        if (snapshot.modbusValid) {
            for (int ch : validChannels) {
                if (ch < snapshot.modbusData.size()) {
                    values.append(snapshot.modbusData[ch]);
    } else {
                    values.append(0.0); // 通道不存在时返回0
                }
            }
        } else {
            // 无效数据时返回0
            values.resize(validChannels.size(), 0.0);
        }
    }

    return values;
}

// 更新多通道当前值标签
void CalibrationDialog::updateMultiChannelValueLabel(const QVector<double>& values)
{
    if (values.isEmpty()) {
        currentValueLabel->setText("N/A");
        return;
    }

    // 显示第一个通道的值，其他通道会在图表中显示
    currentValueLabel->setText(QString::number(values.first(), 'f', 4));
}

// 向图表添加多通道数据点
void CalibrationDialog::updateMultiChannelPlot(double time, const QVector<double>& values)
{
    if (!plot || values.isEmpty() || !isBatchCalibration) return;

    // 要确保值的数量与有效通道数一致
    int count = qMin(values.size(), validChannels.size());

    for (int i = 0; i < count; ++i) {
        if (i < plot->graphCount()) {
            plot->graph(i)->addData(time, values[i]);
        }
    }

    // 调整X轴显示最近20秒
    if (time > 20) {
        plot->xAxis->setRange(time - 20, time);
    } else {
        plot->xAxis->setRange(0, 20);
    }

    // 自动调整Y轴
    bool foundValidData = false;
    for (int i = 0; i < plot->graphCount(); ++i) {
        if (plot->graph(i)->data()->size() > 0) {
            foundValidData = true;
            break;
        }
    }

    if (foundValidData) {
        plot->rescaleAxes(true); // 自动缩放所有轴
    }

    plot->replot(QCustomPlot::rpQueuedReplot); // 使用队列重绘提高性能
}

// Slot to receive raw snapshot data from SnapshotThread
void CalibrationDialog::onRawSnapshotReceived(const DataSnapshot &rawSnapshot)
{
    // Store the latest snapshot
    latestRawSnapshot = rawSnapshot;
    latestSnapshotIndex = rawSnapshot.snapshotIndex;

    // Get the raw value for the currently selected channel
    double currentRawValue = getRawValueFromSnapshot(rawSnapshot);
    latestRawValue = currentRawValue; // Update the latest known raw value

    // If collecting points for calibration, add to temporary list
    if (isCollectingPoints) {
        // For single channel mode, just store the current raw value
        tempMeasurements.append(currentRawValue);

        // For batch calibration mode, store the entire snapshot for later processing
        if (isBatchCalibration && getDeviceInternalName(currentDeviceType) == "Modbus") {
            tempSnapshots.append(rawSnapshot);
        }
    }

    // Note: UI updates (plot, label) are handled by onDataUpdateTimerTimeout
    // to control the update frequency.
}

// Get the raw value for the current device/channel from a snapshot
double CalibrationDialog::getRawValueFromSnapshot(const DataSnapshot& snapshot)
{
    if (currentChannelIndex < 0) return 0.0; // Return 0 if channel is invalid

    QString deviceType = getDeviceInternalName(currentDeviceType);

    if (deviceType == "Modbus" && snapshot.modbusValid && currentChannelIndex < snapshot.modbusData.size()) {
        return snapshot.modbusData.at(currentChannelIndex);
    }
    else if (deviceType == "DAQ" && snapshot.daqValid && currentChannelIndex < snapshot.daqData.size()) {
        qDebug() << "[CalibrationDialog] Getting DAQ raw value for channel" << currentChannelIndex
                 << "from snapshot:" << snapshot.daqData.at(currentChannelIndex);
        return snapshot.daqData.at(currentChannelIndex);
    }
    else if (deviceType == "ECU" && snapshot.ecuValid && currentChannelIndex < snapshot.ecuData.size()) {
        qDebug() << "[CalibrationDialog] Getting ECU raw value for channel" << currentChannelIndex
                 << "from snapshot:" << snapshot.ecuData.at(currentChannelIndex);
        return snapshot.ecuData.at(currentChannelIndex);
    }
     if (currentDeviceType == "Custom" && currentChannelIndex < snapshot.customData.size()) {
         return snapshot.customData.at(currentChannelIndex);
     }

    // Return 0 if data source is invalid or index out of bounds
    return 0.0;
}

// Timer slot to update UI elements periodically
void CalibrationDialog::onDataUpdateTimerTimeout()
{
    static QElapsedTimer timeSinceOpen;
    if (!timeSinceOpen.isValid()) {
        timeSinceOpen.start();
    }
    double elapsedSeconds = timeSinceOpen.elapsed() / 1000.0;

    if (isBatchCalibration) {
        // 批量校准模式 - 更新多通道数据
        QVector<double> values = getMultiChannelRawValues(latestRawSnapshot);
        updateMultiChannelValueLabel(values);
        updateMultiChannelPlot(elapsedSeconds, values);
    } else {
        // 单通道模式 - 更新单通道数据
        updateCurrentValueLabel(latestRawValue);
        updatePlotData(elapsedSeconds, latestRawValue);
    }
}

// Update the label displaying the current raw value
void CalibrationDialog::updateCurrentValueLabel(double value)
{
    if (currentChannelIndex >= 0) {
        currentValueLabel->setText(QString::number(value, 'f', 4));
    } else {
        currentValueLabel->setText("N/A");
    }
}

// Add data point to the plot
void CalibrationDialog::updatePlotData(double time, double value)
{
    if (!plot || currentChannelIndex < 0) return;

    plot->graph(0)->addData(time, value);

    // Adjust x-axis to show the last 20 seconds
    if (time > 20) {
        plot->xAxis->setRange(time - 20, time);
    } else {
        plot->xAxis->setRange(0, 20);
    }

    // Rescale y-axis based on visible data
    plot->graph(0)->rescaleValueAxis(true, true); // Autoscale based on visible data points

    plot->replot(QCustomPlot::rpQueuedReplot); // Use queued replot for performance
}

// +++ Implement Start/Stop Button Slot +++
void CalibrationDialog::onStartStopClicked()
{
    if (!isCalibrationRunning) {
        // Request MainWindow to start calibration run
        emit startCalibrationRequested();
        isCalibrationRunning = true;
        startStopButton->setText(tr("停止校准"));
        updateUiForCalibrationState(true);
        qDebug() << "[CalibrationDialog] Calibration Started (Requested).";
    } else {
        // Request MainWindow to stop calibration run
        emit stopCalibrationRequested();
        isCalibrationRunning = false;
        startStopButton->setText(tr("开始校准"));
        updateUiForCalibrationState(false);
        // Stop point collection if it was running
        if (isCollectingPoints) {
            pointCollectionTimer.stop();
            isCollectingPoints = false;
            addPointButton->setText(tr("添加校准点"));
            addPointButton->setEnabled(true); // Re-enable immediately
        }
        qDebug() << "[CalibrationDialog] Calibration Stopped (Requested).";
    }
}

// +++ Implement UI Update based on State +++
void CalibrationDialog::updateUiForCalibrationState(bool running)
{
    // Enable/disable controls based on whether calibration acquisition is running
    deviceComboBox->setEnabled(!running);
    channelComboBox->setEnabled(!running);
    calibrationInput->setEnabled(running);
    addPointButton->setEnabled(running && !isCollectingPoints); // Enable only if running and not collecting

    // Enable remove button if there are selected items and they're valid data cells
    bool canRemove = false;
    if (running && calibrationTable->rowCount() > 0 && !calibrationTable->selectedItems().isEmpty()) {
        QTableWidgetItem *selectedItem = calibrationTable->selectedItems().first();
        if (isBatchCalibration && getDeviceInternalName(currentDeviceType) == "Modbus") {
            // For batch calibration, only enable if a data cell (not header) is selected
            canRemove = (selectedItem->row() > 0 && selectedItem->column() > 0);
        } else {
            // For single channel calibration, any row can be removed
            canRemove = true;
        }
    }
    removePointButton->setEnabled(canRemove);

    // Enable clear button if there are any calibration points
    bool hasPoints = false;
    if (isBatchCalibration) {
        for (auto it = channelCalibrationPoints.begin(); it != channelCalibrationPoints.end(); ++it) {
            if (!it.value().isEmpty()) {
                hasPoints = true;
                break;
            }
        }
    } else {
        hasPoints = !calibrationPoints.isEmpty();
    }
    clearButton->setEnabled(running && hasPoints);

    // Enable confirm button if there are enough points for calibration
    bool hasEnoughPoints = false;
    if (isBatchCalibration) {
        for (auto it = channelCalibrationPoints.begin(); it != channelCalibrationPoints.end(); ++it) {
            if (it.value().size() >= 2) { // Need at least 2 points for calibration
                hasEnoughPoints = true;
                break;
            }
        }
    } else {
        hasEnoughPoints = (calibrationPoints.size() >= 2);
    }
    confirmButton->setEnabled(running && hasEnoughPoints);

    // Enable table selection when running
    calibrationTable->setEnabled(running);

    // Connect or disconnect table selection signal based on state
    if (running) {
        // Connect table selection signal if not already connected
        if (!tableSelectionConnected) {
            connect(calibrationTable, &QTableWidget::itemSelectionChanged, this, [this]() {
                updateUiForCalibrationState(isCalibrationRunning);
            });
            tableSelectionConnected = true;
        }
    } else if (tableSelectionConnected) {
        // Disconnect when not running to avoid unnecessary updates
        disconnect(calibrationTable, &QTableWidget::itemSelectionChanged, this, nullptr);
        tableSelectionConnected = false;
    }
}

// Slot called when "Add Calibration Point" is clicked
void CalibrationDialog::onAddPointClicked()
{
    if (!isCalibrationRunning) {
        QMessageBox::warning(this, tr("警告"), tr("请先点击 '开始校准' 按钮。"));
        return;
    }
    if (isCollectingPoints) return; // Already collecting

    // Get the standard value from the input field
    double standardValue = calibrationInput->value();

    // Check if this standard value already exists in the calibration data
    bool standardValueExists = false;

    if (isBatchCalibration) {
        // For batch calibration, check if any channel has this standard value
        for (auto it = channelCalibrationPoints.begin(); it != channelCalibrationPoints.end(); ++it) {
            for (const auto& point : it.value()) {
                if (qFuzzyCompare(point.second, standardValue)) {
                    standardValueExists = true;
                    break;
                }
            }
            if (standardValueExists) break;
        }

        // Ask user if they want to replace existing data for this standard value
        if (standardValueExists) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, tr("确认"),
                                        tr("标准值 %1 已存在。\n\n是否要替换现有数据？")
                                        .arg(standardValue),
                                        QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                // Remove existing points with this standard value
                for (auto it = channelCalibrationPoints.begin(); it != channelCalibrationPoints.end(); ++it) {
                    QVector<QPair<double, double>> &points = it.value();
                    for (int i = points.size() - 1; i >= 0; --i) {
                        if (qFuzzyCompare(points[i].second, standardValue)) {
                            points.remove(i);
                        }
                    }
                }
            } else {
                // User chose not to replace, so we'll add a slightly different value
                standardValue += 0.01; // Add a small offset to make it unique
                QMessageBox::information(this, tr("信息"),
                                        tr("将使用新的标准值: %1")
                                        .arg(standardValue));
            }
        }

        // 批量校准时检查是否有太多校准点
        bool tooManyPoints = false;
        for (auto it = channelCalibrationPoints.begin(); it != channelCalibrationPoints.end(); ++it) {
            if (it.value().size() >= 50) {
                tooManyPoints = true;
                break;
            }
        }

        if (tooManyPoints) {
            QMessageBox::warning(this, tr("警告"), tr("已达到最大校准点数 (50)!"));
            return;
        }
    } else {
        // For single channel calibration
        for (const auto& point : calibrationPoints) {
            if (qFuzzyCompare(point.second, standardValue)) {
                standardValueExists = true;
                break;
            }
        }

        if (standardValueExists) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, tr("确认"),
                                        tr("标准值 %1 已存在。\n\n是否要替换现有数据？")
                                        .arg(standardValue),
                                        QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                // Remove existing point with this standard value
                for (int i = calibrationPoints.size() - 1; i >= 0; --i) {
                    if (qFuzzyCompare(calibrationPoints[i].second, standardValue)) {
                        calibrationPoints.remove(i);
                    }
                }
            } else {
                // User chose not to replace, so we'll add a slightly different value
                standardValue += 0.01; // Add a small offset to make it unique
                QMessageBox::information(this, tr("信息"),
                                        tr("将使用新的标准值: %1")
                                        .arg(standardValue));
            }
        }

        // 单通道校准点数限制
        if (calibrationPoints.size() >= 50) {
            QMessageBox::warning(this, tr("警告"), tr("已达到最大校准点数 (50)!"));
            return;
        }
    }

    // Update the input field with the potentially modified standard value
    calibrationInput->setValue(standardValue);

    // Clear temporary measurements and snapshots
    tempMeasurements.clear();
    tempSnapshots.clear();

    // Start the 5-second collection period
    isCollectingPoints = true;
    addPointButton->setText(tr("采集中 (5s)..."));
    addPointButton->setEnabled(false);
    // Disable other buttons during collection
    removePointButton->setEnabled(false);
    clearButton->setEnabled(false);
    confirmButton->setEnabled(false);

    pointCollectionTimer.start(); // Start the 5-second timer
    qDebug() << "[CalibrationDialog] Started 5-second point collection.";
}

// Slot called when the 5-second point collection timer finishes
void CalibrationDialog::onPointCollectionTimerTimeout()
{
    qDebug() << "[CalibrationDialog] 5-second point collection finished.";
    isCollectingPoints = false;
    addPointButton->setText(tr("添加校准点"));
    // Re-enable UI based on current state
    updateUiForCalibrationState(isCalibrationRunning);

    double standardValue = calibrationInput->value();

    if (isBatchCalibration && getDeviceInternalName(currentDeviceType) == "Modbus") {
        // Temperature sensor batch calibration mode
        // For temperature sensors, we collect data for all valid channels simultaneously

        // Get the raw values for all valid channels
        if (latestRawSnapshot.modbusValid) {
            // We need to analyze the collected data from all snapshots
            // Each snapshot contains data for all channels

            // Create a map to store measurements for each channel
            QMap<int, QVector<double>> channelMeasurements;

            // Initialize the map for all valid channels
            for (int ch : validChannels) {
                channelMeasurements[ch] = QVector<double>();
            }

            // Process all collected snapshots
            for (int i = 0; i < tempSnapshots.size(); i++) {
                const DataSnapshot& snapshot = tempSnapshots[i];
                if (snapshot.modbusValid) {
                    // Extract values for each valid channel from this snapshot
                    for (int ch : validChannels) {
                        if (ch < snapshot.modbusData.size()) {
                            double val = snapshot.modbusData[ch];
                            channelMeasurements[ch].append(val);
                        }
                    }
                }
            }

            // Now calculate the average for each channel
            for (int ch : validChannels) {
                if (!channelMeasurements[ch].isEmpty()) {
                    double sum = 0.0;
                    for (double val : channelMeasurements[ch]) {
                        sum += val;
                    }
                    double average = sum / channelMeasurements[ch].size();

                    // Add calibration point for this channel
                    channelCalibrationPoints[ch].append(qMakePair(average, standardValue));
                    qDebug() << "[CalibrationDialog] Added calibration point for channel" << ch
                            << "average:" << average << "standard:" << standardValue
                            << "(from" << channelMeasurements[ch].size() << "samples)";
                } else {
                    // If no measurements for this channel, use the latest raw value
                    if (ch < latestRawSnapshot.modbusData.size()) {
                        double latestVal = latestRawSnapshot.modbusData[ch];
                        channelCalibrationPoints[ch].append(qMakePair(latestVal, standardValue));
                        qDebug() << "[CalibrationDialog] Added calibration point for channel" << ch
                                << "using latest value:" << latestVal << "standard:" << standardValue
                                << "(no samples collected)";
                    }
                }
            }
        } else {
            // If no valid Modbus data, use zeros
            for (int ch : validChannels) {
                channelCalibrationPoints[ch].append(qMakePair(0.0, standardValue));
                qDebug() << "[CalibrationDialog] Added zero calibration point for channel" << ch
                        << "(no valid data) standard:" << standardValue;
            }
        }

        // 更新校准表格
        updateMultiChannelCalibrationTable();
    } else {
        // 单通道校准模式
        double averageRawValue = 0.0;
    if (!tempMeasurements.isEmpty()) {
        double sum = 0.0;
            for (double val : tempMeasurements) {
                sum += val;
        }
            averageRawValue = sum / tempMeasurements.size();
            qDebug() << "[CalibrationDialog] Collected" << tempMeasurements.size() << "points, Average raw value:" << averageRawValue;
    } else {
            averageRawValue = latestRawValue;
            qDebug() << "[CalibrationDialog] No points collected in 5s, using last raw value:" << averageRawValue;
        }

        calibrationPoints.append(qMakePair(averageRawValue, standardValue));
        updateCalibrationTable();
    }

    checkCalibrationButtonState(); // Check confirm button state again
}

// Update the QTableWidget with current calibration points
void CalibrationDialog::updateCalibrationTable()
{
    calibrationTable->setRowCount(0); // Clear existing rows
    calibrationTable->setRowCount(calibrationPoints.size());

    for (int i = 0; i < calibrationPoints.size(); ++i) {
        double rawVal = calibrationPoints[i].first;
        double stdVal = calibrationPoints[i].second;
        double error = stdVal - rawVal;

        QTableWidgetItem *itemRaw = new QTableWidgetItem(QString::number(rawVal, 'f', 4));
        QTableWidgetItem *itemStd = new QTableWidgetItem(QString::number(stdVal, 'f', 4));
        QTableWidgetItem *itemError = new QTableWidgetItem(QString::number(error, 'f', 4));

        // Center align items
        itemRaw->setTextAlignment(Qt::AlignCenter);
        itemStd->setTextAlignment(Qt::AlignCenter);
        itemError->setTextAlignment(Qt::AlignCenter);

        calibrationTable->setItem(i, 0, itemRaw);
        calibrationTable->setItem(i, 1, itemStd);
        calibrationTable->setItem(i, 2, itemError);
    }
}

// Check and update the enabled state of the confirm button
void CalibrationDialog::checkCalibrationButtonState()
{
    if (isBatchCalibration) {
        // 批量校准模式 - 检查任意通道是否有足够的校准点
        bool hasEnoughPoints = false;
        for (auto it = channelCalibrationPoints.begin(); it != channelCalibrationPoints.end(); ++it) {
            if (it.value().size() >= 2) {
                hasEnoughPoints = true;
                break;
            }
        }

        confirmButton->setEnabled(isCalibrationRunning && hasEnoughPoints);
    } else {
        // 单通道校准模式
        confirmButton->setEnabled(isCalibrationRunning && calibrationPoints.size() >= 2);
    }
}

// Slot called when "Remove Selected Point" is clicked
void CalibrationDialog::onRemovePointClicked()
{
    QList<QTableWidgetItem*> selectedItems = calibrationTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先在表格中选择要移除的数据点!"));
        return;
    }

    if (isBatchCalibration && getDeviceInternalName(currentDeviceType) == "Modbus") {
        // 批量校准模式 - 对于模块化的表格布局
        QTableWidgetItem *selectedItem = selectedItems.first();
        int row = selectedItem->row();
        int col = selectedItem->column();

        // 如果选中的是通道列（第一列），则不允许删除
        if (col == 0) {
            QMessageBox::warning(this, tr("警告"), tr("请选择数据列！不能删除通道列。"));
            return;
        }

        // 获取标准值（从标准值行）
        QTableWidgetItem *stdValueItem = calibrationTable->item(1, col);
        if (!stdValueItem) {
            QMessageBox::warning(this, tr("警告"), tr("无法获取标准值信息!"));
            return;
        }

        double standardValue = stdValueItem->data(Qt::UserRole).toDouble();

        // 确认删除整列
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("确认"),
                                    tr("是否要删除标准值 %1 的所有校准点？\n\n这将删除所有通道在该标准值下的数据。")
                                    .arg(standardValue),
                                    QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            // 从所有通道的数据结构中移除该标准值的校准点
            for (auto it = channelCalibrationPoints.begin(); it != channelCalibrationPoints.end(); ++it) {
                int channelIndex = it.key();
                QVector<QPair<double, double>> &points = it.value();
                for (int i = points.size() - 1; i >= 0; --i) {
                    if (qFuzzyCompare(points[i].second, standardValue)) {
                        points.remove(i);
                        qDebug() << "[CalibrationDialog] Removed calibration point for channel" << channelIndex
                                << "with standard value" << standardValue;
                    }
                }
            }

            // 从排除列表中移除该列（如果存在）
            if (excludedColumns.contains(col)) {
                excludedColumns.remove(col);
                qDebug() << "[CalibrationDialog] Removed column" << col << "from excluded columns list";
            }

            // 完全重建表格
            updateMultiChannelCalibrationTable();
        }
    } else {
        // 单通道校准模式
        int rowToRemove = selectedItems.first()->row();

        if (rowToRemove >= 0 && rowToRemove < calibrationPoints.size()) {
            // 移除数据结构中的点
            calibrationPoints.remove(rowToRemove);

            // 移除表格中的行
            calibrationTable->removeRow(rowToRemove);

            qDebug() << "[CalibrationDialog] Removed calibration point at row:" << rowToRemove;
        }
    }

    // 更新按钮状态
    checkCalibrationButtonState();
    // 确保根据选择正确启用/禁用移除按钮
    updateUiForCalibrationState(isCalibrationRunning);
}

// Slot called when "Clear All Points" is clicked
void CalibrationDialog::onClearClicked()
{
    if (QMessageBox::question(this, tr("确认"), tr("确定要清除所有校准点吗?"),
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        if (isBatchCalibration) {
            // 批量校准模式 - 清除所有通道的校准点
            channelCalibrationPoints.clear();

            // 重新初始化空的点集合
            for (int ch : validChannels) {
                channelCalibrationPoints[ch] = QVector<QPair<double, double>>();
            }

            // 更新表格
            updateMultiChannelCalibrationTable();
        } else {
            // 单通道校准模式
    calibrationPoints.clear();
            updateCalibrationTable();
        }

        qDebug() << "[CalibrationDialog] Cleared all calibration points.";
    }

    // 更新按钮状态
    checkCalibrationButtonState();
}

// 保存多通道校准结果到配置文件
bool CalibrationDialog::saveMultiChannelCalibration()
{
    if (!isBatchCalibration || validChannels.isEmpty() || channelCalibrationResults.isEmpty()) return false;

    QString calibFilePath = QCoreApplication::applicationDirPath() + "/calibration.ini";
    qDebug() << "[CalibrationDialog] Attempting to save batch calibration to:" << calibFilePath;

    // 首先读取现有的配置文件，以保留其他通道的设置
    QSettings settings(calibFilePath, QSettings::IniFormat);

    // 检查文件是否可写
    if (!settings.isWritable()) {
        qWarning() << "[CalibrationDialog] Calibration file is not writable:" << calibFilePath << "Status:" << settings.status();
        // 如果文件不存在，尝试创建
        QFile file(calibFilePath);
        if (!file.exists()) {
            if (!file.open(QIODevice::WriteOnly)) {
                qWarning() << "[CalibrationDialog] Could not create calibration file.";
                return false;
            }
            file.close();
            // 重新打开设置
            settings.sync();
            if (!settings.isWritable()) {
                qWarning() << "[CalibrationDialog] Still cannot write to calibration file after creation attempt.";
                return false;
            }
        } else {
            qWarning() << "[CalibrationDialog] Calibration file exists but is not writable.";
            return false;
        }
    }

    // 获取当前时间戳
    QString timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    // 写入每个通道的校准系数
    QString internalDeviceType = getDeviceInternalName(currentDeviceType);

    // 只更新当前校准的通道，而不是整个INI文件
    for (auto it = channelCalibrationResults.begin(); it != channelCalibrationResults.end(); ++it) {
        int ch = it.key();
        const CalibrationParams &params = it.value();

        // 构造值字符串
        QString coeffsString = QString("%1, %2, %3, %4 ; Timestamp: %5")
                                  .arg(params.a, 0, 'g', 8)
                                  .arg(params.b, 0, 'g', 8)
                                  .arg(params.c, 0, 'g', 8)
                                  .arg(params.d, 0, 'g', 8)
                                  .arg(timeStamp);

        // 构造键（例如：Modbus/Channel_0）
        QString key = QString("%1/Channel_%2").arg(internalDeviceType).arg(ch);

        qDebug() << "[CalibrationDialog] Saving calibration for channel" << ch << "- Key:" << key << "Value:" << coeffsString;

        // 只更新当前通道的值
        settings.setValue(key, coeffsString);
    }

    // 确保数据写入磁盘
    settings.sync();

    if (settings.status() != QSettings::NoError) {
        qCritical() << "[CalibrationDialog] Error writing to QSettings:" << settings.status();
        return false;
    }

    // 触发SnapshotThread重新加载校准文件
    if (snapshotThread) {
        QMetaObject::invokeMethod(snapshotThread, "reloadCalibrationSettings", Qt::QueuedConnection,
                                  Q_ARG(QString, calibFilePath));
        qDebug() << "[CalibrationDialog] Requested SnapshotThread to reload calibration settings after save.";
    } else {
        qWarning() << "[CalibrationDialog] SnapshotThread is null, cannot trigger settings reload!";
    }

    return true;
}

// Slot called when "Calculate & Save Coefficients" is clicked
void CalibrationDialog::onConfirmClicked()
{
    if (!isCalibrationRunning) {
         QMessageBox::warning(this, "警告", "校准流程尚未开始或已停止。");
         return;
    }

    if (isBatchCalibration) {
        // 批量校准模式
        bool hasEnoughPoints = false;
        for (auto it = channelCalibrationPoints.begin(); it != channelCalibrationPoints.end(); ++it) {
            if (it.value().size() >= 2) {
                hasEnoughPoints = true;
                break;
            }
        }

        if (!hasEnoughPoints) {
            QMessageBox::warning(this, "警告", "至少需要一个通道有2个或更多校准点才能执行计算!");
            return;
        }

        // 执行多通道拟合
        performMultiChannelCubicFit();

        // 构建结果消息
        QString resultMsg = tr("多通道校准系数计算完成:\n\n");

        for (int ch : validChannels) {
            if (channelCalibrationResults.contains(ch)) {
                const CalibrationParams &params = channelCalibrationResults[ch];
                resultMsg += QString(tr("通道 %1: 标准值 = %2*(原始值)³ + %3*(原始值)² + %4*(原始值) + %5\n"))
                                .arg(ch)
                                .arg(params.a, 0, 'g', 8)
                                .arg(params.b, 0, 'g', 8)
                                .arg(params.c, 0, 'g', 8)
                                .arg(params.d, 0, 'g', 8);
            }
        }

        resultMsg += tr("\n是否保存这些系数到配置文件?");

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("确认批量保存"), resultMsg,
                                     QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            if (saveMultiChannelCalibration()) {
                QMessageBox::information(this, tr("成功"), tr("多通道校准系数已成功保存并应用。"));
            } else {
                QMessageBox::critical(this, tr("错误"), tr("保存多通道校准系数到文件时出错!"));
            }
        }
    } else {
        // 单通道校准模式
        if (calibrationPoints.size() < 2) {
            QMessageBox::warning(this, "警告", "至少需要2个校准点才能执行计算!");
            return;
        }

        // 执行三次函数拟合
        performCubicFit();

        // 显示结果并请求确认保存
        QString resultMsg = QString(tr("校准系数计算完成:\n\n"
                                     "标准值 = %1 * (原始值)³ + \n"
                                     "         %2 * (原始值)² + \n"
                                     "         %3 * (原始值) + \n"
                                     "         %4\n\n"
                                     "是否保存这些系数到配置文件?"))
                              .arg(coef_a, 0, 'g', 8)
                              .arg(coef_b, 0, 'g', 8)
                              .arg(coef_c, 0, 'g', 8)
                              .arg(coef_d, 0, 'g', 8);

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("确认保存"), resultMsg,
                                     QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            if (saveCalibrationToFile()) {
                QMessageBox::information(this, tr("成功"), tr("校准系数已成功保存并应用。"));
    } else {
                QMessageBox::critical(this, tr("错误"), tr("保存校准系数到文件时出错!"));
            }
        }
    }
}

// Perform cubic fit (y = ax^3 + bx^2 + cx + d)
// Where y = standard value, x = average raw value
void CalibrationDialog::performCubicFit()
{
    int n = calibrationPoints.size();
    if (n < 2) {
        // Should not happen due to button state, but check anyway
        coef_a = 0.0; coef_b = 0.0; coef_c = 1.0; coef_d = 0.0; // Default
        return;
    }

    // --- Gaussian Elimination for Polynomial Regression --- //
    // We want to find coefficients a, b, c, d for y = ax^3 + bx^2 + cx + d
    // System of normal equations: X^T * X * A = X^T * Y
    // where X is the design matrix, Y is the vector of standard values,
    // and A is the vector of coefficients [d, c, b, a]^T (or [a, b, c, d]^T depends on matrix setup)

    int degree = 3; // Cubic fit
    if (n == 3) degree = 2; // Force quadratic if only 3 points
    if (n == 2) degree = 1; // Force linear if only 2 points

    int numCoeffs = degree + 1;

    // Build the sums required for the normal equations matrix (X^T * X)
    // Matrix size will be numCoeffs x numCoeffs
    std::vector<std::vector<double>> XT_X(numCoeffs, std::vector<double>(numCoeffs, 0.0));
    std::vector<double> XT_Y(numCoeffs, 0.0);

    // Calculate sums of powers of x (raw values)
    std::vector<double> sumXPower(2 * degree + 1, 0.0);
    for (const auto& point : calibrationPoints) {
        double x = point.first; // Raw average value
        double x_pow = 1.0;
        for (int k = 0; k <= 2 * degree; ++k) {
            sumXPower[k] += x_pow;
            x_pow *= x;
        }
    }

    // Populate the X^T * X matrix (symmetric)
    for (int i = 0; i < numCoeffs; ++i) {
        for (int j = 0; j < numCoeffs; ++j) {
            XT_X[i][j] = sumXPower[i + j];
        }
    }

    // Calculate the sums for the X^T * Y vector
    for (const auto& point : calibrationPoints) {
        double x = point.first;  // Raw average value
        double y = point.second; // Standard value
        double x_pow = 1.0;
        for (int k = 0; k < numCoeffs; ++k) {
            XT_Y[k] += y * x_pow;
            x_pow *= x;
        }
    }

    // --- Solve the system XT_X * A = XT_Y using Gaussian Elimination --- //

    // Augment the matrix [XT_X | XT_Y]
    for (int i = 0; i < numCoeffs; ++i) {
        XT_X[i].push_back(XT_Y[i]);
    }

    // Forward Elimination
    for (int i = 0; i < numCoeffs; ++i) {
        // Find pivot
        int pivot = i;
        for (int k = i + 1; k < numCoeffs; ++k) {
            if (std::fabs(XT_X[k][i]) > std::fabs(XT_X[pivot][i])) {
                pivot = k;
            }
        }
        // Swap rows
        std::swap(XT_X[i], XT_X[pivot]);

        // Check for singularity (or near singularity)
        if (std::fabs(XT_X[i][i]) < 1e-10) {
             qWarning() << "[CalibrationDialog] Matrix is singular or near-singular during fitting. Using default coefficients.";
             coef_a = 0.0; coef_b = 0.0; coef_c = 1.0; coef_d = 0.0;
             return;
        }

        // Normalize row i
        double div = XT_X[i][i];
        for (int j = i; j < numCoeffs + 1; ++j) {
            XT_X[i][j] /= div;
        }

        // Eliminate column i in rows below i
        for (int k = i + 1; k < numCoeffs; ++k) {
            double factor = XT_X[k][i];
            for (int j = i; j < numCoeffs + 1; ++j) {
                XT_X[k][j] -= factor * XT_X[i][j];
            }
        }
    }

    // Back Substitution
    std::vector<double> coeffs(numCoeffs);
    for (int i = numCoeffs - 1; i >= 0; --i) {
        coeffs[i] = XT_X[i][numCoeffs]; // Start with the constant term
        for (int j = i + 1; j < numCoeffs; ++j) {
            coeffs[i] -= XT_X[i][j] * coeffs[j];
        }
    }

    // Assign coefficients based on the degree
    coef_d = (numCoeffs > 0) ? coeffs[0] : 0.0;
    coef_c = (numCoeffs > 1) ? coeffs[1] : 0.0;
    coef_b = (numCoeffs > 2) ? coeffs[2] : 0.0;
    coef_a = (numCoeffs > 3) ? coeffs[3] : 0.0;

    // If degree was forced lower, zero out higher-order coefficients explicitly
    if (degree < 3) coef_a = 0.0;
    if (degree < 2) coef_b = 0.0;
    // If linear fit (degree 1), c and d are calculated, a and b are 0
    // If quadratic fit (degree 2), b, c, d are calculated, a is 0

    qDebug() << "[CalibrationDialog] Fit Coefficients (degree=" << degree << "): a=" << coef_a
             << ", b=" << coef_b << ", c=" << coef_c << ", d=" << coef_d;

}

// 初始化校准参数为默认值 (y = x)
bool CalibrationDialog::initializeCalibrationParams()
{
    QString calibFilePath = QCoreApplication::applicationDirPath() + "/calibration.ini";
    qDebug() << "[CalibrationDialog] Initializing calibration parameters to default values (y = x)";

    QSettings settings(calibFilePath, QSettings::IniFormat);

    // 检查文件是否可写
    if (!settings.isWritable()) {
        qWarning() << "[CalibrationDialog] Calibration file is not writable:" << calibFilePath << "Status:" << settings.status();
        // 如果文件不存在，尝试创建
        QFile file(calibFilePath);
        if (!file.exists()) {
            if (!file.open(QIODevice::WriteOnly)) {
                qWarning() << "[CalibrationDialog] Could not create calibration file.";
                return false;
            }
            file.close();
            // 重新打开设置
            settings.sync();
            if (!settings.isWritable()) {
                qWarning() << "[CalibrationDialog] Still cannot write to calibration file after creation attempt.";
                return false;
            }
        } else {
            qWarning() << "[CalibrationDialog] Calibration file exists but is not writable.";
            return false;
        }
    }

    // 获取当前时间戳
    QString timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    // 默认参数字符串 (a=0, b=0, c=1, d=0 => y = x)
    QString defaultCoeffsString = QString("0, 0, 1, 0 ; Timestamp: %1").arg(timeStamp);

    if (isBatchCalibration && getDeviceInternalName(currentDeviceType) == "Modbus") {
        // 批量模式 - 为所有有效通道初始化
        QString internalDeviceType = getDeviceInternalName(currentDeviceType);

        for (int ch : validChannels) {
            // 构造键（例如：Modbus/Channel_0）
            QString key = QString("%1/Channel_%2").arg(internalDeviceType).arg(ch);

            qDebug() << "[CalibrationDialog] Initializing Key:" << key << "Value:" << defaultCoeffsString;

            // 写入值
            settings.setValue(key, defaultCoeffsString);
        }
    } else {
        // 单通道模式
        QString internalDeviceType = getDeviceInternalName(currentDeviceType);
        QString key = QString("%1/Channel_%2").arg(internalDeviceType).arg(currentChannelIndex);

        qDebug() << "[CalibrationDialog] Initializing Key:" << key << "Value:" << defaultCoeffsString;

        // 写入值
        settings.setValue(key, defaultCoeffsString);
    }

    // 确保数据写入磁盘
    settings.sync();

    if (settings.status() != QSettings::NoError) {
        qCritical() << "[CalibrationDialog] Error writing to QSettings:" << settings.status();
        return false;
    }

    // 触发SnapshotThread重新加载校准文件
    if (snapshotThread) {
        QMetaObject::invokeMethod(snapshotThread, "reloadCalibrationSettings", Qt::QueuedConnection,
                                  Q_ARG(QString, calibFilePath));
        qDebug() << "[CalibrationDialog] Requested SnapshotThread to reload calibration settings after initialization.";
    } else {
        qWarning() << "[CalibrationDialog] SnapshotThread is null, cannot trigger settings reload!";
    }

    // 显示成功消息
    QMessageBox::information(this, tr("成功"), tr("校准参数已成功初始化为默认值 (y = x)。"));

    return true;
}

// Save calibration coefficients to the INI file
bool CalibrationDialog::saveCalibrationToFile()
{
    QString calibFilePath = QCoreApplication::applicationDirPath() + "/calibration.ini";
    qDebug() << "[CalibrationDialog] Attempting to save calibration to:" << calibFilePath;

    // 首先读取现有的配置文件，以保留其他通道的设置
    QSettings settings(calibFilePath, QSettings::IniFormat);

    // Check if the file could be opened for writing
    if (!settings.isWritable()) {
         qWarning() << "[CalibrationDialog] Calibration file is not writable:" << calibFilePath << "Status:" << settings.status();
         // Try to create the file if it doesn't exist (QSettings doesn't do this automatically on read failure)
         QFile file(calibFilePath);
         if (!file.exists()) {
             if (!file.open(QIODevice::WriteOnly)) {
                 qWarning() << "[CalibrationDialog] Could not create calibration file.";
                 return false;
             }
             file.close();
             // Retry opening with QSettings
             settings.sync(); // Force reload
             if (!settings.isWritable()) {
                 qWarning() << "[CalibrationDialog] Still cannot write to calibration file after creation attempt.";
                 return false;
             }
         } else {
              qWarning() << "[CalibrationDialog] Calibration file exists but is not writable.";
              return false; // File exists but isn't writable
         }
    }

    // 获取当前时间戳
    QString timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    // Construct the value string with timestamp
    QString coeffsString = QString("%1, %2, %3, %4 ; Timestamp: %5")
                              .arg(coef_a, 0, 'g', 8)
                              .arg(coef_b, 0, 'g', 8)
                              .arg(coef_c, 0, 'g', 8)
                              .arg(coef_d, 0, 'g', 8)
                              .arg(timeStamp);

    // Construct the key (e.g., Modbus/Channel_0)
    QString key = QString("%1/Channel_%2").arg(getDeviceInternalName(currentDeviceType)).arg(currentChannelIndex);

    qDebug() << "[CalibrationDialog] Saving calibration for channel" << currentChannelIndex << "- Key:" << key << "Value:" << coeffsString;

    // 只更新当前通道的值，而不是整个INI文件
    settings.setValue(key, coeffsString);

    // Ensure data is written to disk
    settings.sync();

    if (settings.status() != QSettings::NoError) {
        qCritical() << "[CalibrationDialog] Error writing to QSettings:" << settings.status();
        return false;
    }

    // Trigger SnapshotThread to reload the calibration file
    if (snapshotThread) {
        // Use QMetaObject::invokeMethod to ensure it's called in the SnapshotThread's context if needed
        QMetaObject::invokeMethod(snapshotThread, "reloadCalibrationSettings", Qt::QueuedConnection,
                                  Q_ARG(QString, calibFilePath));
        qDebug() << "[CalibrationDialog] Requested SnapshotThread to reload calibration settings after save.";
    } else {
         qWarning() << "[CalibrationDialog] SnapshotThread is null, cannot trigger settings reload!";
    }

    return true;
}

// 获取设备显示名称
QString CalibrationDialog::getDeviceDisplayName(const QString &internalName)
{
    if (internalName == "Modbus") return "Temperature Sensor Calibration";
    if (internalName == "DAQ") return "Force Sensor Calibration";
    if (internalName == "ECU") return "ECU Data Correction";
    if (internalName == "Custom") return "CustomData Correction";
    return internalName; // 默认返回原始名称
}

// 获取设备内部名称
QString CalibrationDialog::getDeviceInternalName(const QString &displayName)
{
    if (displayName == "Temperature Sensor Calibration" || displayName == "温度传感器校准") return "Modbus";
    if (displayName == "Force Sensor Calibration" || displayName == "力传感器校准") return "DAQ";
    if (displayName == "ECU Data Correction" || displayName == "ECU数据修正") return "ECU";
    if (displayName == "CustomData Correction" || displayName == "CustomData数据修正") return "Custom";
    return displayName; // 默认返回原始名称
}

// 更新多通道校准表格
void CalibrationDialog::updateMultiChannelCalibrationTable()
{
    if (!isBatchCalibration) return;

    // 如果是首次显示，尝试重新查找有效通道
    if (validChannels.isEmpty() || (validChannels.size() == 1 && channelCalibrationPoints.isEmpty())) {
        findValidChannels();
        qDebug() << "[CalibrationDialog] Re-finding valid channels for table display:" << validChannels.size() << "channels found";
    }

    if (validChannels.isEmpty()) {
        calibrationTable->setRowCount(1);
        calibrationTable->setColumnCount(1);
        QTableWidgetItem *noChannelsItem = new QTableWidgetItem(tr("没有有效的通道"));
        noChannelsItem->setTextAlignment(Qt::AlignCenter);
        calibrationTable->setItem(0, 0, noChannelsItem);
        return;
    }

    // Store the current selection if any
    int currentRow = calibrationTable->currentRow();
    int currentCol = calibrationTable->currentColumn();

    // Get all unique calibration values across all channels
    QSet<double> calibrationValues;
    for (int ch : validChannels) {
        auto points = channelCalibrationPoints.value(ch);
        for (const auto& point : points) {
            calibrationValues.insert(point.second); // Add standard value
        }
    }

    // Convert to sorted list
    QList<double> sortedCalibValues;
    for (const double& value : calibrationValues) {
        sortedCalibValues.append(value);
    }
    std::sort(sortedCalibValues.begin(), sortedCalibValues.end());

    // Clear the table completely and reset it
    calibrationTable->clear();
    calibrationTable->setRowCount(0);
    calibrationTable->setColumnCount(0);
    calibrationTable->clearContents();
    calibrationTable->clearSpans();
    calibrationTable->clearSelection();
    calibrationTable->clearFocus();

    // If no calibration values, return after clearing
    if (sortedCalibValues.isEmpty()) {
        return;
    }

    // Set up table with rows: Header row, Standard values row, and Channel rows
    int totalRows = validChannels.size() + 2; // +2 for header row and standard values row
    int totalCols = sortedCalibValues.size() + 1; // +1 for channel ID column

    calibrationTable->setRowCount(totalRows);
    calibrationTable->setColumnCount(totalCols);

    // Get the appropriate device name
    QString deviceName = getDeviceInternalName(currentDeviceType);

    // Set the top-left corner cell with the device name
    QTableWidgetItem *cornerItem = new QTableWidgetItem(QString("(%1) 通道/采集序列").arg(deviceName));
    cornerItem->setTextAlignment(Qt::AlignCenter);
    cornerItem->setBackground(QBrush(QColor(240, 240, 240)));
    calibrationTable->setItem(0, 0, cornerItem);

    // Set column headers with sequence numbers (first row)
    for (int col = 0; col < sortedCalibValues.size(); ++col) {
        QTableWidgetItem *headerItem = new QTableWidgetItem(QString::number(col + 1));
        headerItem->setTextAlignment(Qt::AlignCenter);
        headerItem->setBackground(QBrush(QColor(240, 240, 240)));
        // Store the column index for reference
        headerItem->setData(Qt::UserRole + 1, col + 1);
        calibrationTable->setItem(0, col + 1, headerItem);
    }

    // Set standard values row (second row)
    QTableWidgetItem *stdRowLabel = new QTableWidgetItem(tr("校准值"));
    stdRowLabel->setTextAlignment(Qt::AlignCenter);
    stdRowLabel->setBackground(QBrush(QColor(240, 240, 240)));
    calibrationTable->setItem(1, 0, stdRowLabel);

    // Add standard values in the second row
    for (int col = 0; col < sortedCalibValues.size(); ++col) {
        double calibValue = sortedCalibValues[col];
        QTableWidgetItem *stdValueItem = new QTableWidgetItem(QString::number(calibValue, 'f', 2));
        stdValueItem->setTextAlignment(Qt::AlignCenter);
        stdValueItem->setBackground(QBrush(QColor(240, 240, 240)));
        // Store the standard value as user data for easy access
        stdValueItem->setData(Qt::UserRole, calibValue);
        calibrationTable->setItem(1, col + 1, stdValueItem);
    }

    // Add a row for each channel
    for (int rowIdx = 0; rowIdx < validChannels.size(); ++rowIdx) {
        int ch = validChannels[rowIdx];
        int tableRow = rowIdx + 2; // +2 to account for header and standard value rows

        // Set channel ID in first column
        QTableWidgetItem *channelItem = new QTableWidgetItem(tr("通道%1").arg(ch));
        channelItem->setTextAlignment(Qt::AlignCenter);
        channelItem->setBackground(QBrush(QColor(240, 240, 240)));
        // Store the channel ID as user data for easy access
        channelItem->setData(Qt::UserRole, ch);
        calibrationTable->setItem(tableRow, 0, channelItem);

        // Add raw values for each standard value column
        for (int colIdx = 0; colIdx < sortedCalibValues.size(); ++colIdx) {
            double calibValue = sortedCalibValues[colIdx];

            // Find if this channel has a raw value for this calibration value
            bool found = false;
            double rawValue = 0.0;
            auto points = channelCalibrationPoints.value(ch);
            for (const auto& point : points) {
                if (qFuzzyCompare(point.second, calibValue)) {
                    rawValue = point.first;
                    found = true;
                    break;
                }
            }

            // Create table item
            QTableWidgetItem *rawItem;
            if (found) {
                rawItem = new QTableWidgetItem(QString::number(rawValue, 'f', 4));
                rawItem->setForeground(QBrush(QColor(0, 0, 0))); // Black text for valid values
                // Store both raw and standard values as user data
                QVariantMap itemData;
                itemData["raw"] = rawValue;
                itemData["standard"] = calibValue;
                itemData["channel"] = ch;
                itemData["columnIndex"] = colIdx + 1; // Store 1-based column index
                rawItem->setData(Qt::UserRole, QVariant::fromValue(itemData));
            } else {
                rawItem = new QTableWidgetItem("--"); // No data for this channel at this calib value
                rawItem->setForeground(QBrush(QColor(150, 150, 150))); // Gray text for missing values
            }
            rawItem->setTextAlignment(Qt::AlignCenter);
            calibrationTable->setItem(tableRow, colIdx + 1, rawItem);
        }
    }

    // Restore selection if possible
    if (currentRow >= 0 && currentRow < calibrationTable->rowCount() &&
        currentCol >= 0 && currentCol < calibrationTable->columnCount()) {
        calibrationTable->setCurrentCell(currentRow, currentCol);
    }

    // Resize columns to content
    calibrationTable->resizeColumnsToContents();
    calibrationTable->resizeRowsToContents();

    // Force a complete repaint to eliminate any visual artifacts
    calibrationTable->repaint();
}

// 为每个通道执行三次函数拟合
void CalibrationDialog::performMultiChannelCubicFit()
{
    if (!isBatchCalibration || validChannels.isEmpty()) return;

    channelCalibrationResults.clear();

    // 检查是否有被排除的列
    QSet<double> excludedStandardValues;
    if (!excludedColumns.isEmpty()) {
        // 获取被排除列的标准值
        for (int col : excludedColumns) {
            QTableWidgetItem *stdValueItem = calibrationTable->item(1, col);
            if (stdValueItem) {
                double standardValue = stdValueItem->data(Qt::UserRole).toDouble();
                excludedStandardValues.insert(standardValue);
                qDebug() << "[CalibrationDialog] Excluding standard value" << standardValue << "from calculations";
            }
        }
    }

    // 对每个通道进行拟合
    bool anyChannelFailed = false;
    QStringList failedChannels;

    for (int ch : validChannels) {
        // 获取该通道的校准点
        QVector<QPair<double, double>> allPoints = channelCalibrationPoints.value(ch);

        // 过滤掉被排除的标准值点
        QVector<QPair<double, double>> points;
        for (const auto& point : allPoints) {
            if (!excludedStandardValues.contains(point.second)) {
                points.append(point);
            }
        }

        // 检查是否有足够的点进行三次函数拟合（至少4个点）
        if (points.size() < 4) {
            qDebug() << "[CalibrationDialog] Not enough points for channel" << ch
                     << "- need at least 4 points for cubic fit, only have" << points.size();
            failedChannels.append(QString::number(ch));
            anyChannelFailed = true;

            // 使用默认校准参数
            CalibrationParams defaultParams;
            defaultParams.a = 0.0;
            defaultParams.b = 0.0;
            defaultParams.c = 1.0;
            defaultParams.d = 0.0;
            channelCalibrationResults[ch] = defaultParams;
            continue;
        }

        // --- 执行拟合计算 --- //
        int n = points.size();

        // 确定多项式次数
        int degree = 3; // 默认为三次
        if (n == 3) degree = 2; // 3个点使用二次
        if (n == 2) degree = 1; // 2个点使用一次

        int numCoeffs = degree + 1;

        // 构建法方程矩阵 (X^T * X)
        std::vector<std::vector<double>> XT_X(numCoeffs, std::vector<double>(numCoeffs, 0.0));
        std::vector<double> XT_Y(numCoeffs, 0.0);

        // 计算x的幂次和
        std::vector<double> sumXPower(2 * degree + 1, 0.0);
        for (const auto& point : points) {
            double x = point.first; // 原始平均值
            double x_pow = 1.0;
            for (int k = 0; k <= 2 * degree; ++k) {
                sumXPower[k] += x_pow;
                x_pow *= x;
            }
        }

        // 填充 X^T * X 矩阵
        for (int i = 0; i < numCoeffs; ++i) {
            for (int j = 0; j < numCoeffs; ++j) {
                XT_X[i][j] = sumXPower[i + j];
            }
        }

        // 计算 X^T * Y 向量
        for (const auto& point : points) {
            double x = point.first;  // 原始平均值
            double y = point.second; // 标准值
            double x_pow = 1.0;
            for (int k = 0; k < numCoeffs; ++k) {
                XT_Y[k] += y * x_pow;
                x_pow *= x;
            }
        }

        // --- 使用高斯消元法解方程组 XT_X * A = XT_Y --- //

        // 增广矩阵 [XT_X | XT_Y]
        for (int i = 0; i < numCoeffs; ++i) {
            XT_X[i].push_back(XT_Y[i]);
        }

        // 前向消元
        for (int i = 0; i < numCoeffs; ++i) {
            // 寻找主元
            int pivot = i;
            for (int k = i + 1; k < numCoeffs; ++k) {
                if (std::fabs(XT_X[k][i]) > std::fabs(XT_X[pivot][i])) {
                    pivot = k;
                }
            }
            // 交换行
            std::swap(XT_X[i], XT_X[pivot]);

            // 检查奇异性
            if (std::fabs(XT_X[i][i]) < 1e-10) {
                qWarning() << "[CalibrationDialog] Matrix is singular for channel" << ch << ". Using default coefficients.";
                CalibrationParams defaultParams;
                defaultParams.a = 0.0;
                defaultParams.b = 0.0;
                defaultParams.c = 1.0;
                defaultParams.d = 0.0;
                channelCalibrationResults[ch] = defaultParams;
                continue; // 跳过此通道
            }

            // 归一化第i行
            double div = XT_X[i][i];
            for (int j = i; j < numCoeffs + 1; ++j) {
                XT_X[i][j] /= div;
            }

            // 消去i列在i以下的元素
            for (int k = i + 1; k < numCoeffs; ++k) {
                double factor = XT_X[k][i];
                for (int j = i; j < numCoeffs + 1; ++j) {
                    XT_X[k][j] -= factor * XT_X[i][j];
                }
            }
        }

        // 回代
        std::vector<double> coeffs(numCoeffs);
        for (int i = numCoeffs - 1; i >= 0; --i) {
            coeffs[i] = XT_X[i][numCoeffs]; // 初始值为常数项
            for (int j = i + 1; j < numCoeffs; ++j) {
                coeffs[i] -= XT_X[i][j] * coeffs[j];
            }
        }

        // 根据次数分配系数
        CalibrationParams params;
        params.d = (numCoeffs > 0) ? coeffs[0] : 0.0;
        params.c = (numCoeffs > 1) ? coeffs[1] : 1.0;
        params.b = (numCoeffs > 2) ? coeffs[2] : 0.0;
        params.a = (numCoeffs > 3) ? coeffs[3] : 0.0;

        // 如果度数较低，显式将高次系数设为0
        if (degree < 3) params.a = 0.0;
        if (degree < 2) params.b = 0.0;

        channelCalibrationResults[ch] = params;

        qDebug() << "[CalibrationDialog] Fit Coefficients for channel" << ch
                << "(degree=" << degree << "): a=" << params.a
                << ", b=" << params.b << ", c=" << params.c << ", d=" << params.d;
    }

    // 如果有通道校准失败，显示警告
    if (anyChannelFailed) {
        QMessageBox::warning(this, tr("校准失败"),
                           tr("以下通道的校准失败，因为数据点不足（三次函数拟合需要至少4个点）：\n%1")
                           .arg(failedChannels.join(", ")));
    }
}