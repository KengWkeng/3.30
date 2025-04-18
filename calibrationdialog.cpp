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
      currentDeviceType("温度传感器校准"), // 默认设备
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

    // Populate channel combo box based on the default device
    updateChannelComboBox();
    
    // Set default selections in combo boxes
    deviceComboBox->setCurrentText("温度传感器校准");
    // onDeviceChanged will be called automatically, which calls onChannelChanged

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

// Setup UI layout and controls
void CalibrationDialog::setupUI()
{
    // Main layout (vertical)
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Top area: Plot (left) and Controls (right)
    QHBoxLayout *topLayout = new QHBoxLayout();
    
    // --- Left Area (Plot and Table) ---
    QVBoxLayout *leftLayout = new QVBoxLayout();
    
    // Plot
    plot = new QCustomPlot();
    plot->setMinimumSize(400, 300); // Adjusted minimum size
    leftLayout->addWidget(plot, 3); // Plot takes more vertical space

    // Calibration Table
    calibrationTable = new QTableWidget();
    calibrationTable->setColumnCount(3);
    calibrationTable->setHorizontalHeaderLabels({tr("原始平均值"), tr("校准标准值"), tr("误差")});
    calibrationTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Stretch columns
    calibrationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    calibrationTable->setSelectionMode(QAbstractItemView::SingleSelection); // Allow single row selection
    calibrationTable->setEditTriggers(QAbstractItemView::NoEditTriggers); // Make table read-only
    calibrationTable->setMinimumHeight(150);
    leftLayout->addWidget(calibrationTable, 1); // Table takes less space
    
    topLayout->addLayout(leftLayout, 3); // Left area takes 3/4 width
    
    // --- Right Area (Controls) ---
    QVBoxLayout *rightLayout = new QVBoxLayout();
    
    // +++ Start/Stop Calibration Button +++
    startStopButton = new QPushButton(tr("开始校准"));
    rightLayout->addWidget(startStopButton);
    rightLayout->addSpacing(10); // Add some space

    // Device and Channel Selection GroupBox
    QGroupBox *deviceGroupBox = new QGroupBox(tr("设备和通道选择"));
    QFormLayout *deviceLayout = new QFormLayout(); // Use QFormLayout for label-widget pairs
    deviceComboBox = new QComboBox();
    deviceComboBox->addItems({"温度传感器校准", "力传感器校准", "ECU数据修正", "CustomData数据修正"});
    channelComboBox = new QComboBox();
    deviceLayout->addRow(tr("设备类型:"), deviceComboBox);
    deviceLayout->addRow(tr("通道:"), channelComboBox);
    deviceGroupBox->setLayout(deviceLayout);
    rightLayout->addWidget(deviceGroupBox);
    
    // Measurement and Calibration Input GroupBox
    QGroupBox *valueGroupBox = new QGroupBox(tr("测量与校准输入"));
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
    
    // Add/Remove Point Buttons
    QHBoxLayout *pointButtonLayout = new QHBoxLayout();
    addPointButton = new QPushButton(tr("添加校准点"));
    removePointButton = new QPushButton(tr("移除选中点"));
    pointButtonLayout->addWidget(addPointButton);
    pointButtonLayout->addWidget(removePointButton);
    rightLayout->addLayout(pointButtonLayout);

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

    topLayout->addLayout(rightLayout, 1);
    mainLayout->addLayout(topLayout);
    setLayout(mainLayout);
}

// Initialize QCustomPlot settings
void CalibrationDialog::initializePlot()
{
    if (!plot) return;
    plot->clearGraphs();
    plot->addGraph(); // Add the main graph for the selected channel
    plot->graph(0)->setPen(QPen(Qt::blue));
    plot->graph(0)->setName("Selected Channel"); // Generic name initially

    plot->xAxis->setLabel(tr("时间 (s 相对于对话框打开)"));
    plot->yAxis->setLabel(tr("原始值"));
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
    
    // 检查最近的快照数据，查找有效的Modbus通道
    if (latestRawSnapshot.modbusValid) {
        for (int i = 0; i < latestRawSnapshot.modbusData.size(); ++i) {
            // 如果通道值不为0或无效值，则认为通道有效
            // 注意：这只是一个简单的示例，您可能需要更复杂的逻辑来检测有效通道
            double val = latestRawSnapshot.modbusData[i];
            if (val != 0.0 && !std::isnan(val) && !std::isinf(val)) {
                validChannels.append(i);
            }
        }
    }
    
    // 如果没有检测到有效通道，至少包含通道0
    if (validChannels.isEmpty()) {
        validChannels.append(0);
    }
    
    qDebug() << "[CalibrationDialog] Valid Modbus channels detected:" << validChannels;
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
        if (internalDeviceType == "DAQ" && latestRawSnapshot.daqValid) {
            for (int i = 0; i < qMin(count, latestRawSnapshot.daqData.size()); ++i) {
                double val = latestRawSnapshot.daqData[i];
                if (val != 0.0 && !std::isnan(val) && !std::isinf(val)) {
                    deviceValidChannels.append(i);
                }
            }
        } else if (internalDeviceType == "ECU" && latestRawSnapshot.ecuValid) {
            for (int i = 0; i < qMin(count, latestRawSnapshot.ecuData.size()); ++i) {
                double val = latestRawSnapshot.ecuData[i];
                if (val != 0.0 && !std::isnan(val) && !std::isinf(val)) {
                    deviceValidChannels.append(i);
                }
            }
        } else if (internalDeviceType == "Custom") {
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
    if (deviceType == "ECU") {
            QStringList ecuNames = {"Throttle(节气门)", "EngineSpeed(转速)", "CylinderTemp(缸温)", 
                                   "ExhaustTemp(排气温度)", "AxleTemp(轴温)", "FuelPressure(油压)", 
                                   "IntakeTemp(进气温度)", "AtmPressure(大气压)", "FlightTime(飞行时间)"};
        if (index >= 0 && index < ecuNames.size()) {
            return ecuNames.at(index);
        }
    }
    // Default name for other types or invalid ECU index
    return QString("Channel %1").arg(index);
}

// Slot called when device selection changes
void CalibrationDialog::onDeviceChanged(int index)
{
    currentDeviceType = deviceComboBox->itemText(index);
    qDebug() << "[CalibrationDialog] Device changed to:" << currentDeviceType;
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
        tempMeasurements.append(currentRawValue);
    }

    // Note: UI updates (plot, label) are handled by onDataUpdateTimerTimeout
    // to control the update frequency.
}

// Get the raw value for the current device/channel from a snapshot
double CalibrationDialog::getRawValueFromSnapshot(const DataSnapshot& snapshot)
{
    if (currentChannelIndex < 0) return 0.0; // Return 0 if channel is invalid

    if (currentDeviceType == "Modbus" && snapshot.modbusValid && currentChannelIndex < snapshot.modbusData.size()) {
        return snapshot.modbusData.at(currentChannelIndex);
    }
    if (currentDeviceType == "DAQ" && snapshot.daqValid && currentChannelIndex < snapshot.daqData.size()) {
        return snapshot.daqData.at(currentChannelIndex);
    }
    if (currentDeviceType == "ECU" && snapshot.ecuValid && currentChannelIndex < snapshot.ecuData.size()) {
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
    removePointButton->setEnabled(running && calibrationTable->rowCount() > 0 && calibrationTable->selectedItems().count() > 0);
    clearButton->setEnabled(running && calibrationPoints.size() > 0);
    confirmButton->setEnabled(running && calibrationPoints.size() >= 2); // Enable confirm if running and enough points

    // Optionally disable table selection when not running?
    calibrationTable->setEnabled(running);
}

// Slot called when "Add Calibration Point" is clicked
void CalibrationDialog::onAddPointClicked()
{
    if (!isCalibrationRunning) {
        QMessageBox::warning(this, tr("警告"), tr("请先点击 '开始校准' 按钮。"));
        return;
    }
    if (isCollectingPoints) return; // Already collecting

    // 批量校准时检查是否有太多校准点
    if (isBatchCalibration) {
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
    } else if (calibrationPoints.size() >= 50) {
        // 单通道校准点数限制
        QMessageBox::warning(this, tr("警告"), tr("已达到最大校准点数 (50)!"));
        return;
    }

    // Clear temporary measurements
    tempMeasurements.clear();
    
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
    
    if (isBatchCalibration) {
        // 批量校准模式 - 为每个通道计算平均值
        QMap<int, double> channelAverages;
        
        for (int ch : validChannels) {
            double sum = 0.0;
            int count = 0;
            
            // 计算每个通道的平均值
            for (int i = 0; i < latestRawSnapshot.modbusData.size() && i < validChannels.size(); ++i) {
                if (validChannels[i] == ch) {
                    if (!tempMeasurements.isEmpty() && i < tempMeasurements.size()) {
                        sum += tempMeasurements[i];
                        count++;
                    }
                }
            }
            
            double average = (count > 0) ? (sum / count) : latestRawValue;
            channelAverages[ch] = average;
            
            // 添加校准点
            channelCalibrationPoints[ch].append(qMakePair(average, standardValue));
            qDebug() << "[CalibrationDialog] Added calibration point for channel" << ch 
                    << "average:" << average << "standard:" << standardValue;
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
        QMessageBox::warning(this, tr("警告"), tr("请先在表格中选择要移除的行!"));
        return;
    }
    
    // 获取选中的行索引
    int rowToRemove = selectedItems.first()->row();
    
    if (isBatchCalibration) {
        // 批量校准模式 - 需要确定选中的是哪个通道的点
        if (rowToRemove >= 0 && rowToRemove < calibrationTable->rowCount()) {
            QTableWidgetItem *channelItem = calibrationTable->item(rowToRemove, 0);
            if (channelItem) {
                bool ok;
                int ch = channelItem->text().toInt(&ok);
                
                if (ok && channelCalibrationPoints.contains(ch)) {
                    // 查找这是该通道的第几个点
                    int pointCount = 0;
                    int pointIndex = -1;
                    
                    for (int r = 0; r < rowToRemove; ++r) {
                        QTableWidgetItem *rChannelItem = calibrationTable->item(r, 0);
                        if (rChannelItem && rChannelItem->text().toInt() == ch) {
                            pointCount++;
                        }
                    }
                    
                    // 点的索引应该等于之前相同通道的点数
                    pointIndex = pointCount;
                    
                    // 检查索引是否有效
                    if (pointIndex >= 0 && pointIndex < channelCalibrationPoints[ch].size()) {
                        // 从数据结构中移除点
                        channelCalibrationPoints[ch].remove(pointIndex);
                        
                        // 从表格中移除行
                        calibrationTable->removeRow(rowToRemove);
                        
                        qDebug() << "[CalibrationDialog] Removed calibration point at row:" << rowToRemove
                                << "for channel:" << ch << "point index:" << pointIndex;
                    }
                }
            }
        }
        
        // 更新表格以保持一致
        updateMultiChannelCalibrationTable();
    } else {
        // 单通道校准模式
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
    QString timeStamp = QDateTime::currentDateTime().toString("|yyyy-MM-dd HH:mm:ss|");
    
    // 写入每个通道的校准系数
    QString internalDeviceType = getDeviceInternalName(currentDeviceType);
    
    for (auto it = channelCalibrationResults.begin(); it != channelCalibrationResults.end(); ++it) {
        int ch = it.key();
        const CalibrationParams &params = it.value();
        
        // 构造值字符串
        QString coeffsString = QString("%1, %2, %3, %4 %5")
                                  .arg(params.a, 0, 'g', 8)
                                  .arg(params.b, 0, 'g', 8)
                                  .arg(params.c, 0, 'g', 8)
                                  .arg(params.d, 0, 'g', 8)
                                  .arg(timeStamp);
        
        // 构造键（例如：Modbus/Channel_0）
        QString key = QString("%1/Channel_%2").arg(internalDeviceType).arg(ch);
        
        qDebug() << "[CalibrationDialog] Saving batch calibration - Key:" << key << "Value:" << coeffsString;
        
        // 写入值
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
        qDebug() << "[CalibrationDialog] Requested SnapshotThread to reload calibration settings after batch save.";
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

// Save calibration coefficients to the INI file
bool CalibrationDialog::saveCalibrationToFile()
{
    QString calibFilePath = QCoreApplication::applicationDirPath() + "/calibration.ini";
    qDebug() << "[CalibrationDialog] Attempting to save calibration to:" << calibFilePath;

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
    QString timeStamp = QDateTime::currentDateTime().toString("|yyyy-MM-dd HH:mm:ss|");

    // Construct the value string with timestamp
    QString coeffsString = QString("%1, %2, %3, %4 %5")
                              .arg(coef_a, 0, 'g', 8)
                              .arg(coef_b, 0, 'g', 8)
                              .arg(coef_c, 0, 'g', 8)
                              .arg(coef_d, 0, 'g', 8)
                              .arg(timeStamp);

    // Construct the key (e.g., Modbus/Channel_0)
    QString key = QString("%1/Channel_%2").arg(getDeviceInternalName(currentDeviceType)).arg(currentChannelIndex);

    qDebug() << "[CalibrationDialog] Saving Key:" << key << "Value:" << coeffsString;

    // Write the value
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
        qDebug() << "[CalibrationDialog] Requested SnapshotThread to reload calibration settings.";
    } else {
         qWarning() << "[CalibrationDialog] SnapshotThread is null, cannot trigger settings reload!";
    }
    
    return true;
}

// 获取设备显示名称
QString CalibrationDialog::getDeviceDisplayName(const QString &internalName)
{
    if (internalName == "Modbus") return "温度传感器校准";
    if (internalName == "DAQ") return "力传感器校准";
    if (internalName == "ECU") return "ECU数据修正";
    if (internalName == "Custom") return "CustomData数据修正";
    return internalName; // 默认返回原始名称
}

// 获取设备内部名称
QString CalibrationDialog::getDeviceInternalName(const QString &displayName)
{
    if (displayName == "温度传感器校准") return "Modbus";
    if (displayName == "力传感器校准") return "DAQ";
    if (displayName == "ECU数据修正") return "ECU";
    if (displayName == "CustomData数据修正") return "Custom";
    return displayName; // 默认返回原始名称
}

// 更新多通道校准表格
void CalibrationDialog::updateMultiChannelCalibrationTable()
{
    if (!isBatchCalibration || validChannels.isEmpty()) return;
    
    // 获取所有通道的校准点总数
    int maxPoints = 0;
    for (auto it = channelCalibrationPoints.begin(); it != channelCalibrationPoints.end(); ++it) {
        maxPoints = qMax(maxPoints, it.value().size());
    }
    
    calibrationTable->setRowCount(0); // 清除现有行
    
    if (maxPoints == 0) return; // 没有数据
    
    // 设置列数：通道ID + 原始值 + 标准值 + 误差
    calibrationTable->setColumnCount(4);
    calibrationTable->setHorizontalHeaderLabels({tr("通道"), tr("原始平均值"), tr("校准标准值"), tr("误差")});
    
    int rowIndex = 0;
    
    // 遍历每个有效通道的校准点
    for (int ch : validChannels) {
        auto points = channelCalibrationPoints.value(ch);
        
        if (points.isEmpty()) continue;
        
        // 对于每个校准点添加一行
        for (const auto &point : points) {
            double rawVal = point.first;
            double stdVal = point.second;
            double error = stdVal - rawVal;
            
            calibrationTable->insertRow(rowIndex);
            
            QTableWidgetItem *itemChannel = new QTableWidgetItem(QString::number(ch));
            QTableWidgetItem *itemRaw = new QTableWidgetItem(QString::number(rawVal, 'f', 4));
            QTableWidgetItem *itemStd = new QTableWidgetItem(QString::number(stdVal, 'f', 4));
            QTableWidgetItem *itemError = new QTableWidgetItem(QString::number(error, 'f', 4));
            
            // 居中对齐
            itemChannel->setTextAlignment(Qt::AlignCenter);
            itemRaw->setTextAlignment(Qt::AlignCenter);
            itemStd->setTextAlignment(Qt::AlignCenter);
            itemError->setTextAlignment(Qt::AlignCenter);
            
            calibrationTable->setItem(rowIndex, 0, itemChannel);
            calibrationTable->setItem(rowIndex, 1, itemRaw);
            calibrationTable->setItem(rowIndex, 2, itemStd);
            calibrationTable->setItem(rowIndex, 3, itemError);
            
            rowIndex++;
        }
    }
}

// 为每个通道执行三次函数拟合
void CalibrationDialog::performMultiChannelCubicFit()
{
    if (!isBatchCalibration || validChannels.isEmpty()) return;
    
    channelCalibrationResults.clear();
    
    for (int ch : validChannels) {
        auto points = channelCalibrationPoints.value(ch);
        
        if (points.size() < 2) {
            // 至少需要2个点，否则使用默认校准参数
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
} 