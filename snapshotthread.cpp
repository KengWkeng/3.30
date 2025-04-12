#include "snapshotthread.h"
#include <QDir> // Include for QDir::currentPath()

SnapshotThread::SnapshotThread(QObject *parent) : QObject(parent),
    logFile(nullptr), logStream(nullptr), snapshotsSinceLastWrite(0) // Initialize new members
{
    // 初始化计时器
    masterTimer = new QElapsedTimer();
    masterTimer->start();
    
    realTimer = new QElapsedTimer();
    realTimer->start();
    lastTimestamp = realTimer->elapsed();
    
    // 初始化滤波相关变量
    filteredValues.resize(16, 0.0); // 默认支持16个通道
    



}

SnapshotThread::~SnapshotThread()
{
    closeLogFile(); // Ensure file is closed on destruction
    // 清理资源
    if (masterTimer) {
        delete masterTimer;
        masterTimer = nullptr;
    }
    
    if (realTimer) {
        delete realTimer;
        realTimer = nullptr;
    }
}

// 初始化主计时器
void SnapshotThread::setupMasterTimer()
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

// New slot implementation to restart the master timer
void SnapshotThread::resetMasterTimer()
{
    if (masterTimer) {
        masterTimer->restart();
        qDebug() << "[SnapshotThread] Master timer restarted.";
    } else {
        qDebug() << "[SnapshotThread] Warning: resetMasterTimer called but masterTimer is null!";
        // Optionally recreate it if necessary
        setupMasterTimer(); 
    }
    snapshotCount = 0; // Also reset snapshot count
    // Reset data buffers if needed, e.g., snapshotQueue.clear(); - uncomment if desired
    // snapshotQueue.clear(); 
}

// 应用一阶低通滤波器
double SnapshotThread::applyLowPassFilter(double input, double prevOutput, double timeConstant, double deltaT)
{
    // 一阶低通滤波器公式: output = prevOutput + (input - prevOutput) * (deltaT / (timeConstant + deltaT))
    double alpha = deltaT / (timeConstant + deltaT);
    double output = prevOutput + (input - prevOutput) * alpha;
    return output;
}

// 应用滤波到结果数据
QVector<double> SnapshotThread::applyFilterToResults(const QVector<double> &rawData, qint64 deltaT)
{
    QVector<double> filtered = rawData;
    
    // 确保filteredValues大小与rawData大小一致
    if (filteredValues.size() != rawData.size()) {
        filteredValues.resize(rawData.size());
        for (int i = 0; i < filteredValues.size(); i++) {
            filteredValues[i] = 0.0;
        }
    }
    
    // 对每个通道分别应用滤波器
    if (filterEnabled) {
        for (int i = 0; i < rawData.size(); i++) {
            // 计算滤波后的值
            double prevOutput = filteredValues[i];
            double input = rawData[i];
            double output = applyLowPassFilter(input, prevOutput, filterTimeConstant, deltaT / 1000.0);
            
            // 更新滤波后的值
            filtered[i] = output;
            filteredValues[i] = output;
        }
    }
    
    return filtered;
}

// 处理Modbus数据
void SnapshotThread::handleModbusData(QVector<double> resultdata, qint64 readTimeInterval)
{
    // Add check for processing enabled flag
    if (!processingEnabled) {
        qDebug() << "[SnapshotThread] handleModbusData: Processing disabled, skipping.";
        return; // If processing is not enabled, exit early
    }

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
        
        qDebug() << "处理Modbus数据: " << resultdata;
        
        // 确保modbusNumRegs与数据大小一致
        if (modbusNumRegs != resultdata.size()) {
            modbusNumRegs = resultdata.size();
        }
        
        // 应用滤波处理 - 不管UI控制，直接进行滤波
        QVector<double> filteredData = applyFilterToResults(resultdata, readTimeInterval);
        
        // 标记Modbus数据有效
        modbusDataValid = true;
        
        // 更新currentSnapshot使用滤波后的数据
        currentSnapshot.modbusValid = true;
        currentSnapshot.modbusData = filteredData;
        
        // 保持数据到modbusData，用于兼容其他可能使用它的代码
        // 确保modbusData已经初始化为正确大小
        if (modbusData.size() != resultdata.size()) {
            modbusData.resize(resultdata.size());
            for (int i = 0; i < resultdata.size(); i++) {
                modbusData[i].clear();
            }
        }
        
        // 将滤波后的数据添加到数据缓冲区
        for (int i = 0; i < filteredData.size(); i++) {
            // 添加数据到存储
            modbusData[i].append(filteredData[i]);
            
            // 限制数据点数量，避免内存占用过多
            while (modbusData[i].size() > 10000) {
                modbusData[i].removeFirst();
            }
        }
        
        // 如果需要，你可以在这里添加将数据发送到WebSocket的代码
        // 创建JSON对象，包含Modbus数据
        QJsonObject modbusJson;
        modbusJson["type"] = "modbus";
        modbusJson["timestamp"] = QDateTime::currentMSecsSinceEpoch();
        QJsonArray dataArray;
        for (const double &value : filteredData) {
            dataArray.append(value);
        }
        modbusJson["data"] = dataArray;
        emit sendModbusResultToWebSocket(modbusJson, readTimeInterval);
        
    } catch (const std::exception& e) {
        qDebug() << "处理Modbus数据时出错: " << e.what();
    } catch (...) {
        qDebug() << "处理Modbus数据时发生未知错误";
    }
}

// 处理DAQ数据
void SnapshotThread::handleDAQData(const QVector<double> &timeData, const QVector<QVector<double>> &channelData)
{
    try {
        if (timeData.isEmpty() || channelData.isEmpty()) {
            qDebug() << "警告: 收到空的DAQ数据";
            return;
        }
        
        // 更新DAQ状态
        daqIsAcquiring = true;
        daqNumChannels = channelData.size();
        
        // 保存时间数据
        daqTimeData = timeData;
        
        // 确保daqChannelData大小正确
        if (daqChannelData.size() != daqNumChannels) {
            daqChannelData.resize(daqNumChannels);
        }
        
        // 保存通道数据
        for (int i = 0; i < daqNumChannels; i++) {
            // 追加新数据
            daqChannelData[i].append(channelData[i]);
            
            // 限制数据点数量
            const int maxPoints = 10000;
            while (daqChannelData[i].size() > maxPoints) {
                daqChannelData[i].removeFirst();
            }
        }
        
        // 更新当前快照中的DAQ数据
        currentSnapshot.daqValid = true;
        currentSnapshot.daqRunning = true;
        
        // 如果daqChannelData不为空，更新快照中的DAQ数据
        if (!daqChannelData.isEmpty()) {
            currentSnapshot.daqData.resize(daqNumChannels);
            for (int i = 0; i < daqNumChannels; i++) {
                if (!daqChannelData[i].isEmpty()) {
                    // 使用最新的数据点
                    currentSnapshot.daqData[i] = daqChannelData[i].last();
                } else {
                    currentSnapshot.daqData[i] = 0.0;
                }
            }
        }
        
    } catch (const std::exception& e) {
        qDebug() << "处理DAQ数据时出错: " << e.what();
    } catch (...) {
        qDebug() << "处理DAQ数据时发生未知错误";
    }
}

// 处理ECU数据
void SnapshotThread::handleECUData(const ECUData &data)
{
    // 在 snapshotthread.cpp 的 handleECUData 开头
    qDebug() << "[SnapshotThread] handleECUData called. Received data isValid:" << data.isValid;

    if (!data.isValid) { // 保持原有逻辑
        qDebug() << "[SnapshotThread] handleECUData: Received data is invalid, returning.";
        return;
    }


    try {
        // 使用互斥锁保护latestECUData的访问
        QMutexLocker locker(&latestECUDataMutex);

        // 保存最新的ECU数据
        latestECUData = data;
        ecuDataValid = true;  // 设置ECU数据有效标志
        qDebug() << "[SnapshotThread] handleECUData: Updated latestECUData and set ecuDataValid to true.";

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
        

        

        
        // 注意：不再直接更新图表，统一由processDataSnapshots处理
        // 更新当前快照，以便processDataSnapshots能够获取最新数据
        currentSnapshot.ecuValid = true;
        currentSnapshot.ecuData = ecuValues;
        
        // 调试信息
        qDebug() << "收到ECU数据: 节气门=" << data.throttle 
                 << "%, 转速=" << data.engineSpeed << "rpm"
                 << ", ecuDataValid=" << ecuDataValid
                 << ", snapEcuIsConnected=" << snapEcuIsConnected;
        
    } catch (const std::exception& e) {
        qDebug() << "处理ECU数据时出错: " << e.what();
    } catch (...) {
        qDebug() << "处理ECU数据时发生未知错误";
    }
}

// 接收ECU连接状态
void SnapshotThread::handleECUConnectionStatus(bool connected, QString message)
{
    // 添加详细调试信息，帮助追踪函数是否被调用
    qDebug() << "===> [SnapshotThread] handleECUConnectionStatus called: connected=" << connected << ", message=" << message;
    
    // 更新ECU连接状态
    snapEcuIsConnected = connected;
    qDebug() << "===> [SnapshotThread] snapEcuIsConnected set to:" << snapEcuIsConnected;
    
    // 如果连接断开，则标记ECU数据无效
    if (!connected) {
        ecuDataValid = false;
        
        // 更新当前快照中的ECU状态
        currentSnapshot.ecuValid = false;
    }
    
    qDebug() << "===> 更新后的ECU状态: 已连接=" << snapEcuIsConnected << ", 数据有效=" << ecuDataValid;
    
    // 手动触发数据快照处理，确保状态变化立即反映
    QMetaObject::invokeMethod(this, &SnapshotThread::processDataSnapshots, Qt::QueuedConnection);
}

// 处理数据快照
void SnapshotThread::processDataSnapshots()
{
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
        
        // 获取当前的Modbus数据 - 直接从currentSnapshot获取
        snapshot.modbusValid = currentSnapshot.modbusValid;
        if (snapshot.modbusValid) {
            // 直接复制currentSnapshot中已滤波的Modbus数据
            snapshot.modbusData = currentSnapshot.modbusData;
            qDebug() << "快照" << snapshotCount + 1 << "Modbus数据有效，数据:" << snapshot.modbusData;
        } else {
            qDebug() << "快照" << snapshotCount + 1 << "Modbus数据无效";
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
            qDebug() << "快照" << snapshotCount + 1 << "DAQ数据有效，通道数:" << daqNumChannels << "数据:" << snapshot.daqData;
        } else {
            qDebug() << "快照" << snapshotCount + 1 << "DAQ数据无效，采集状态:" << daqIsAcquiring << "通道数:" << daqNumChannels;
        }
        
        // 获取当前的ECU数据 - 使用基于snapEcuIsConnected和ecuDataValid的简化逻辑
      snapshot.ecuValid = snapEcuIsConnected && latestECUData.isValid; // 直接基于连接状态和最新数据有效性

    if (snapshot.ecuValid) { // 如果连接正常且最新数据有效
        snapshot.ecuData.resize(9);
        snapshot.ecuData[0] = latestECUData.throttle;
        snapshot.ecuData[1] = latestECUData.engineSpeed;
        snapshot.ecuData[2] = latestECUData.cylinderTemp;
        snapshot.ecuData[3] = latestECUData.exhaustTemp;
        snapshot.ecuData[4] = latestECUData.axleTemp;
        snapshot.ecuData[5] = latestECUData.fuelPressure;
        snapshot.ecuData[6] = latestECUData.intakeTemp;
        snapshot.ecuData[7] = latestECUData.atmPressure;
        snapshot.ecuData[8] = latestECUData.flightTime;
        qDebug() << "快照" << snapshotCount + 1 << "ECU数据有效，数据:" << snapshot.ecuData;
    } else {
        qDebug() << "快照" << snapshotCount + 1 << "ECU数据无效，ECU连接状态:" << snapEcuIsConnected << "最新数据有效:" << latestECUData.isValid;
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
        
        // --- Logging Logic ---
        if (logFile && logStream && logFile->isOpen()) {
            writeSnapshotToFile(snapshot);
            snapshotsSinceLastWrite++;

            if (snapshotsSinceLastWrite >= writeThreshold) {
                logStream->flush(); // Flush buffer to disk
                snapshotsSinceLastWrite = 0;
                qDebug() << "[SnapshotThread] Flushed log data to disk.";
            }
        }
        // --- End Logging Logic ---
        
        // 发送数据快照到WebSocket (新增)
        // WebSocketThread 会通过信号槽连接接收这个信号
        emit snapshotForWebSocket(snapshot, snapshotCount);
        
        // 发送处理好的快照，让主线程更新UI
        emit snapshotProcessed(snapshot, snapshotCount);
        
    } catch (const std::exception& e) {
        qDebug() << "处理数据快照时出错: " << e.what();
    } catch (...) {
        qDebug() << "处理数据快照时发生未知错误";
    }
}

// 设置滤波状态和参数
void SnapshotThread::setFilterEnabled(bool enabled, double timeConstant)
{
    filterEnabled = enabled;
    
    // 如果禁用滤波器，则清除之前的滤波数据
    if (!filterEnabled) {
        filteredValues.clear();
    }
    
    // 更新滤波时间常数
    if (timeConstant > 0) {
        filterTimeConstant = timeConstant / 1000.0; // 转换为秒
    }
    
    qDebug() << "SnapshotThread: 滤波状态已更新为" << (enabled ? "启用" : "禁用") 
             << "时间常数:" << filterTimeConstant << "秒";
}

// Modified slot to handle processing and logging state
void SnapshotThread::setProcessingEnabled(bool enabled)
{
    processingEnabled = enabled;
    qDebug() << "[SnapshotThread] Processing enabled set to:" << processingEnabled;

    if (enabled) {
        // Start logging when processing starts
        if (!initializeLogFile()) {
            qDebug() << "[SnapshotThread] Failed to initialize log file. Logging disabled.";
            // Optionally handle the error, e.g., disable processing again or notify user
        }
    } else {
        // Stop logging when processing stops
        closeLogFile();
    }
}

// --- Implementation of Logging Helper Methods ---

bool SnapshotThread::initializeLogFile()
{
    if (logFile) { // Already initialized
        return true;
    }

    // Create filename with timestamp
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    logFilePath = QDir::currentPath() + "/" + timestamp + ".csv"; // Store in executable directory

    logFile = new QFile(logFilePath);
    if (!logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append )) {
        qDebug() << "[SnapshotThread] Error: Could not open log file for writing:" << logFilePath << logFile->errorString();
        delete logFile;
        logFile = nullptr;
        return false;
    }

    logStream = new QTextStream(logFile);

    qDebug() << "[SnapshotThread] Log file initialized:" << logFilePath;

    // Generate and write header
    generateCsvHeader();
    writeCsvHeader();
    snapshotsSinceLastWrite = 0; // Reset counter

    return true;
}

void SnapshotThread::generateCsvHeader()
{
    csvHeader.clear();
    csvHeader << "Timestamp" << "SnapshotIndex" << "ModbusValid" << "DAQValid" << "DAQRunning" << "ECUValid";

    // Add headers based on configured counts
    const int MAX_ECU_CH = 9; // Fixed

    for (int i = 0; i < configuredModbusChannels; ++i) { // Use configured count
        csvHeader << QString("Modbus_%1").arg(i);
    }
    for (int i = 0; i < configuredDaqChannels; ++i) { // Use configured count
        csvHeader << QString("DAQ_%1").arg(i);
    }
    QStringList ecuNames = {"Throttle", "EngineSpeed", "CylinderTemp", "ExhaustTemp", "AxleTemp", "FuelPressure", "IntakeTemp", "AtmPressure", "FlightTime"};
    for (int i = 0; i < MAX_ECU_CH; ++i) {
        csvHeader << QString("ECU_%1").arg(ecuNames.value(i, QString::number(i))); // Use names if available
    }
}

void SnapshotThread::writeCsvHeader()
{
    if (logStream && !csvHeader.isEmpty()) {
        *logStream << csvHeader.join(",") << Qt::endl;
        logStream->flush(); // Write header immediately
    }
}

void SnapshotThread::writeSnapshotToFile(const DataSnapshot &snapshot)
{
    if (!logStream) return;

    QStringList dataRow;
    dataRow << QString::number(snapshot.timestamp, 'f', 3); // Timestamp with ms precision
    dataRow << QString::number(snapshot.snapshotIndex);
    dataRow << QString(snapshot.modbusValid ? "1" : "0");
    dataRow << QString(snapshot.daqValid ? "1" : "0");
    dataRow << QString(snapshot.daqRunning ? "1" : "0");
    dataRow << QString(snapshot.ecuValid ? "1" : "0");

    // Add Modbus data based on configured count
    for (int i = 0; i < configuredModbusChannels; ++i) { // Use configured count
        if (snapshot.modbusValid && i < snapshot.modbusData.size()) {
            dataRow << QString::number(snapshot.modbusData[i], 'f', 4); // 4 decimal places for Modbus
        } else {
            dataRow << ""; // Empty string if invalid or channel doesn't exist
        }
    }

    // Add DAQ data based on configured count
    for (int i = 0; i < configuredDaqChannels; ++i) { // Use configured count
        if (snapshot.daqValid && i < snapshot.daqData.size()) {
            dataRow << QString::number(snapshot.daqData[i], 'f', 6); // 6 decimal places for DAQ
        } else {
            dataRow << "";
        }
    }

    // Add ECU data (up to MAX_ECU_CH)
    const int MAX_ECU_CH = 9;
    for (int i = 0; i < MAX_ECU_CH; ++i) {
        if (snapshot.ecuValid && i < snapshot.ecuData.size()) {
            dataRow << QString::number(snapshot.ecuData[i], 'f', 2); // 2 decimal places for ECU
        } else {
            dataRow << "";
        }
    }

    *logStream << dataRow.join(",") << Qt::endl;
}

void SnapshotThread::closeLogFile()
{
    if (logStream) {
        logStream->flush(); // Final flush
        qDebug() << "[SnapshotThread] Final log flush.";
    }
    if (logFile && logFile->isOpen()) {
        logFile->close();
        qDebug() << "[SnapshotThread] Log file closed:" << logFilePath;
    }
    // Safely delete objects
    delete logStream;
    logStream = nullptr;
    delete logFile;
    logFile = nullptr;
    logFilePath.clear();
}

// --- End of Logging Helper Methods ---

// --- New Slot Implementation ---
void SnapshotThread::setupLogging(int modbusCount, int daqCount)
{
    configuredModbusChannels = (modbusCount > 0) ? modbusCount : 0; // Ensure non-negative
    configuredDaqChannels = (daqCount > 0) ? daqCount : 0;       // Ensure non-negative
    qDebug() << "[SnapshotThread] Logging configured for Modbus:" << configuredModbusChannels << "channels, DAQ:" << configuredDaqChannels << "channels.";
}

// ... other existing methods like setFilterEnabled ...
