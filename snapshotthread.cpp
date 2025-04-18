#include "snapshotthread.h"
#include <QDir> // Include for QDir::currentPath()
#include <QSettings>   // +++ 新增 +++
#include <QStringList> // +++ 新增 +++
#include <cmath>       // 用于 std::pow 和 round
#include <QCoreApplication> // <--- 添加头文件
#include <QTextCodec> // <--- 添加头文件
#include <QFile>       // <--- 添加头文件
#include <QTextStream> // <--- 添加头文件

SnapshotThread::SnapshotThread(QObject *parent) : QObject(parent),
    logFile(nullptr), logStream(nullptr), snapshotsSinceLastWrite(0), // Initialize new members
    enableDataLogging(true) // 初始化为true，但会被setProcessingEnabled覆盖
{
    // 初始化计时器
    masterTimer = new QElapsedTimer();
    masterTimer->start();
    
    realTimer = new QElapsedTimer();
    realTimer->start();
    lastTimestamp = realTimer->elapsed();
    
    // 初始化滤波相关变量
    filteredValues.resize(16, 0.0); // 默认支持16个通道
    
    // +++ 新增: 加载校准设置 +++
    QString calibFilePath = QCoreApplication::applicationDirPath() + "/calibration.ini"; // <--- 修改路径获取方式
    loadCalibrationSettings(calibFilePath);
    // +++ 结束新增 +++
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
    if (!processingEnabled) {
        return;
    }
    try {
        if (!masterTimer) {
            qDebug() << "警告: 主计时器未初始化，无法处理数据快照";
            masterTimer = new QElapsedTimer();
            masterTimer->start();
            return;
        }

        double currentTime = (masterTimer->elapsed() / 1000.0);
        double roundedTime = round(currentTime * 10.0) / 10.0;

        // 创建一个快照用于存储原始数据
        DataSnapshot rawSnapshot;
        rawSnapshot.timestamp = roundedTime;
        rawSnapshot.snapshotIndex = snapshotCount + 1; // Use upcoming index

        // 1. 填充 rawSnapshot (在应用滤波和校准之前)
        // Modbus (使用 currentSnapshot 中的滤波后但未校准的数据)
        rawSnapshot.modbusValid = currentSnapshot.modbusValid;
        if(rawSnapshot.modbusValid) {
            rawSnapshot.modbusData = currentSnapshot.modbusData; // Data after filter, before calibration
        } else {
            rawSnapshot.modbusData.fill(0.0, configuredModbusChannels > 0 ? configuredModbusChannels : 16);
        }

        // DAQ (使用 currentSnapshot 中的最新原始数据)
        rawSnapshot.daqValid = daqIsAcquiring && daqNumChannels > 0;
        rawSnapshot.daqRunning = daqIsAcquiring;
        if (rawSnapshot.daqValid && !daqChannelData.isEmpty()) {
            rawSnapshot.daqData.resize(daqNumChannels);
            for (int i = 0; i < daqNumChannels && i < daqChannelData.size(); ++i) {
                if (!daqChannelData[i].isEmpty()) {
                    rawSnapshot.daqData[i] = daqChannelData[i].last(); // Last raw point
                } else {
                    rawSnapshot.daqData[i] = 0.0;
                }
            }
        } else {
            rawSnapshot.daqData.fill(0.0, configuredDaqChannels > 0 ? configuredDaqChannels : 16);
        }

        // ECU (使用 latestECUData 中的原始数据)
        rawSnapshot.ecuValid = snapEcuIsConnected && latestECUData.isValid;
        rawSnapshot.ecuData.resize(9);
        if (rawSnapshot.ecuValid) {
            rawSnapshot.ecuData[0] = latestECUData.throttle;
            rawSnapshot.ecuData[1] = latestECUData.engineSpeed;
            rawSnapshot.ecuData[2] = latestECUData.cylinderTemp;
            rawSnapshot.ecuData[3] = latestECUData.exhaustTemp;
            rawSnapshot.ecuData[4] = latestECUData.axleTemp;
            rawSnapshot.ecuData[5] = latestECUData.fuelPressure;
            rawSnapshot.ecuData[6] = latestECUData.intakeTemp;
            rawSnapshot.ecuData[7] = latestECUData.atmPressure;
            rawSnapshot.ecuData[8] = latestECUData.flightTime;
        } else {
            rawSnapshot.ecuData.fill(0.0);
        }

        // Custom Data (计算基于 *原始* DAQ 和 ECU 数据)
        rawSnapshot.customData.resize(5); 
        rawSnapshot.customData.fill(0.0);
        if (rawSnapshot.daqValid && rawSnapshot.ecuValid && rawSnapshot.daqData.size() >= 3 && rawSnapshot.ecuData.size() >= 9) {
            rawSnapshot.customData[0] = rawSnapshot.daqData[0] * rawSnapshot.ecuData[0]*10.0;
            rawSnapshot.customData[1] = rawSnapshot.daqData[1] * rawSnapshot.ecuData[1];
            rawSnapshot.customData[2] = rawSnapshot.daqData[2] * rawSnapshot.ecuData[2];
            // rawSnapshot.customData[3] = 0.0; // Already filled
            // rawSnapshot.customData[4] = 0.0; // Already filled
        }

        // +++ Emit raw snapshot signal +++
        emit rawSnapshotReady(rawSnapshot);

        // 2. 创建用于处理和发送的快照 (从 rawSnapshot 开始)
        DataSnapshot snapshot = rawSnapshot; // Start with a copy of the raw data

        // --- Apply Calibration --- (Apply to the 'snapshot' object)
        // Modbus Calibration
        if (snapshot.modbusValid) {
            for(int i = 0; i < snapshot.modbusData.size(); ++i) {
                CalibrationParams params = getCalibrationParams("Modbus", i);
                double rawValue = snapshot.modbusData[i]; // Value before calibration
                snapshot.modbusData[i] = applyCalibration(rawValue, params); // Apply calibration
            }
            qDebug() << "快照" << snapshot.snapshotIndex << "Modbus数据有效 (已校准)，数据:" << snapshot.modbusData;
        }

        // DAQ Calibration
        if (snapshot.daqValid) {
            for(int i = 0; i < snapshot.daqData.size(); ++i) {
                 CalibrationParams params = getCalibrationParams("DAQ", i);
                 double rawValue = snapshot.daqData[i];
                 snapshot.daqData[i] = applyCalibration(rawValue, params);
            }
            qDebug() << "快照" << snapshot.snapshotIndex << "DAQ数据有效 (已校准)，通道数:" << daqNumChannels << "数据:" << snapshot.daqData;
        }

        // ECU Calibration
        if (snapshot.ecuValid) {
            for(int i = 0; i < 9; ++i) {
                CalibrationParams params = getCalibrationParams("ECU", i);
                double rawValue = snapshot.ecuData[i];
                snapshot.ecuData[i] = applyCalibration(rawValue, params);
            }
            qDebug() << "快照" << snapshot.snapshotIndex << "ECU数据有效 (已校准)，数据:" << snapshot.ecuData;
        }

        // Custom Data Calibration (Apply to the calculated custom values)
        if (snapshot.daqValid && snapshot.ecuValid) { // Only apply if inputs were valid
            for(int i = 0; i < 5; ++i) {
                 snapshot.customData[i] = applyCalibration(snapshot.customData[i], getCalibrationParams("Custom", i));
            }
            qDebug() << "[SnapshotThread] Calibrated customData:" << snapshot.customData;
        }
        // --- End Apply Calibration ---

        // 增加快照计数 (移到此处，确保在处理完成后增加)
        snapshotCount++; 
        snapshot.snapshotIndex = snapshotCount; // Update index in the final snapshot

        // 将 *校准后* 的快照添加到队列中
        snapshotQueue.enqueue(snapshot);
        while (snapshotQueue.size() > 300) {
            snapshotQueue.dequeue();
        }

        // --- Logging Logic --- (Logs the *calibrated* snapshot)
        if (enableDataLogging && logFile && logStream && logFile->isOpen()) {
            writeSnapshotToFile(snapshot);
            snapshotsSinceLastWrite++;
            if (snapshotsSinceLastWrite >= writeThreshold) {
                logStream->flush();
                snapshotsSinceLastWrite = 0;
            }
        } else {
            if (!enableDataLogging && logFile && logFile->isOpen()) {
                closeLogFile();
            }
        }
        // --- End Logging Logic ---

        // 发送 *校准后* 的数据快照到WebSocket
        emit snapshotForWebSocket(snapshot, snapshotCount);

        // 发送 *校准后* 的处理好的快照，让主线程更新UI
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

    // Link data logging enable state to processing state by default
    // setDataLoggingEnabled(enabled); // Let MainWindow manage logging explicitly for calibration

    if (enabled) {
        // Don't automatically initialize log file here anymore
        // MainWindow/CalibrationDialog will manage the logging state
        // if (!initializeLogFile()) {
        //     qDebug() << "[SnapshotThread] Failed to initialize log file. Logging disabled.";
        // }
    } else {
        // Stop logging ONLY if it was enabled
        if (enableDataLogging) {
             closeLogFile();
        }
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
    // Add Custom Data headers
    for (int i = 0; i < 5; ++i) { // Fixed size of 5
        csvHeader << QString("Custom_%1").arg(i);
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
    // Add Custom data (up to 5)
    for (int i = 0; i < 5; ++i) {
        if (i < snapshot.customData.size()) {
             dataRow << QString::number(snapshot.customData[i], 'f', 4); // 4 decimal places for custom
        } else {
            dataRow << ""; // Should not happen due to fixed size
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

// +++ 新增: 加载校准文件实现 +++
void SnapshotThread::loadCalibrationSettings(const QString& filePath)
{
    qDebug() << "[SnapshotThread] Attempting to load calibration settings from:" << filePath;
    allCalibrationParams.clear(); // 清空旧参数

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) { // Read as raw bytes
        qDebug() << "[SnapshotThread] Warning: Cannot open calibration file:" << filePath << file.errorString() << ". Using default calibration (y=x).";
        return;
    }

    QByteArray fileContent = file.readAll();
    file.close(); // Close file after reading

    // Get UTF-8 codec
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString contentString;
    if(codec) {
        contentString = codec->toUnicode(fileContent);
        qDebug() << "[SnapshotThread] Read calibration file and converted to Unicode using UTF-8 codec.";
    } else {
        qDebug() << "[SnapshotThread] Warning: Could not find UTF-8 codec! Trying Latin1.";
        // Fallback to Latin1 if UTF-8 is somehow unavailable
         contentString = QString::fromLatin1(fileContent);
    }

    QStringList lines = contentString.split('\n', Qt::SkipEmptyParts); // Split content into lines

    QString currentSourceType = "";
    int lineNum = 0;

    // Process lines
    for (const QString &rawLine : lines) {
        lineNum++;
        QString line = rawLine.trimmed(); // Trim whitespace from each line

        // Skip empty lines and comments (lines starting with ';' or '#')
        if (line.isEmpty() || line.startsWith(';') || line.startsWith('#')) {
            continue;
        }

        // Check for section header
        if (line.startsWith('[') && line.endsWith(']')) {
            currentSourceType = line.mid(1, line.length() - 2).trimmed();
            qDebug() << "[SnapshotThread] Parsing section:" << currentSourceType;
            continue;
        }

        // Parse key-value pairs within a section
        if (!currentSourceType.isEmpty()) {
            int equalsPos = line.indexOf('=');
            if (equalsPos != -1) {
                QString key = line.left(equalsPos).trimmed();
                QString valueString = line.mid(equalsPos + 1).trimmed();
                
                // 移除引号（如果存在）
                if (valueString.startsWith('"') && valueString.endsWith('"')) {
                    valueString = valueString.mid(1, valueString.length() - 2);
                }
                
                // 移除最后的时间戳（如果存在）
                // 时间戳格式如：|2023-05-20 14:30:45|
                int timeStampStart = valueString.indexOf('|');
                if (timeStampStart != -1) {
                    valueString = valueString.left(timeStampStart).trimmed();
                }
                
                // Remove potential inline comments
                int commentPos = valueString.indexOf(';');
                if (commentPos != -1) {
                    valueString = valueString.left(commentPos).trimmed();
                }
                int commentPosHash = valueString.indexOf('#');
                 if (commentPosHash != -1) {
                    valueString = valueString.left(commentPosHash).trimmed();
                }

                 qDebug() << "[SnapshotThread Debug] Reading Key:" << currentSourceType << "/" << key << "Raw Value String:'" << valueString << "'";

                if (key.startsWith("Channel_")) {
                    bool okIndex;
                    int channelIndex = key.mid(8).toInt(&okIndex);

                    if (okIndex) {
                        QStringList coeffStrings = valueString.split(',', Qt::SkipEmptyParts);

                        if (coeffStrings.size() >= 4) { // 至少需要4个系数
                            bool okA, okB, okC, okD;
                            CalibrationParams params;
                            params.a = coeffStrings[0].trimmed().toDouble(&okA);
                            params.b = coeffStrings[1].trimmed().toDouble(&okB);
                            params.c = coeffStrings[2].trimmed().toDouble(&okC);
                            params.d = coeffStrings[3].trimmed().toDouble(&okD);

                            if (okA && okB && okC && okD) {
                                // Ensure the map for the sourceType exists
                                if (!allCalibrationParams.contains(currentSourceType)) {
                                    allCalibrationParams[currentSourceType] = QMap<int, CalibrationParams>();
                                }
                                allCalibrationParams[currentSourceType][channelIndex] = params;
                                qDebug() << "[SnapshotThread] Loaded calibration for" << currentSourceType << "Channel" << channelIndex
                                         << ": a=" << params.a << ", b=" << params.b << ", c=" << params.c << ", d=" << params.d;
                            } else {
                                qDebug() << "[SnapshotThread] Warning: Invalid number format in calibration for" << currentSourceType << key << "at line" << lineNum << ". Using default.";
                            }
                        } else {
                            qDebug() << "[SnapshotThread] Warning: Incorrect number of coefficients for" << currentSourceType << key << "at line" << lineNum << "(found" << coeffStrings.size() << ", expected 4). Using default.";
                        }
                    } else {
                         qDebug() << "[SnapshotThread] Warning: Invalid channel index format for" << currentSourceType << key << "at line" << lineNum << ". Skipping.";
                    }
                }
            }
        }
    }

    qDebug() << "[SnapshotThread] Finished loading calibration settings manually.";
}
// +++ 结束新增 +++

// +++ 新增: 获取校准参数实现 +++
CalibrationParams SnapshotThread::getCalibrationParams(const QString& sourceType, int channelIndex)
{
    // 如果映射中不存在该源或通道，.value() 会返回默认构造的 CalibrationParams (a=0,b=0,c=1,d=0)
    return allCalibrationParams.value(sourceType).value(channelIndex, CalibrationParams());
}
// +++ 结束新增 +++

// +++ 新增: 应用校准函数实现 +++
double SnapshotThread::applyCalibration(double rawValue, const CalibrationParams& params)
{
    // 使用直接乘法避免 std::pow 的开销和潜在问题
    double x = rawValue;
    double x2 = x * x;
    double x3 = x2 * x;
    return params.a * x3 + params.b * x2 + params.c * x + params.d;
}
// +++ 结束新增 +++

// 新增：设置是否启用数据记录
void SnapshotThread::setDataLoggingEnabled(bool enabled)
{
    if (enableDataLogging == enabled) return; // No change

    enableDataLogging = enabled;
    qDebug() << "[SnapshotThread] Data logging explicitly set to:" << enableDataLogging;

    if (enableDataLogging) {
        // Only initialize if processing is also enabled
        if (processingEnabled && !logFile) {
             if (!initializeLogFile()) {
                 qDebug() << "[SnapshotThread] Failed to initialize log file when enabling data logging.";
                 // Optionally set enableDataLogging back to false or notify
                 enableDataLogging = false;
             }
        }
    } else {
        // If disabling logging, close the file if it's open
        if (logFile && logFile->isOpen()) {
            closeLogFile();
        }
    }
}

// 新增：实现公共方法检查数据记录状态
bool SnapshotThread::isDataLoggingEnabled() const
{
    return enableDataLogging;
}

// 新增：实现公共方法用于重新加载校准文件
void SnapshotThread::reloadCalibrationSettings(const QString& filePath)
{
    loadCalibrationSettings(filePath);
    qDebug() << "[SnapshotThread] Calibration settings reloaded from:" << filePath;
}

// ... other existing methods like setFilterEnabled ...
