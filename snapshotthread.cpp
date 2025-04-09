#include "snapshotthread.h"

SnapshotThread::SnapshotThread(QObject *parent) : QObject(parent)
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
    try {
        // 使用互斥锁保护ECU数据访问
        QMutexLocker locker(&latestECUDataMutex);
        
        // 保存最新的ECU数据
        latestECUData = data;
        ecuDataValid = data.isValid;
        ecuIsConnected = true;
        
        // 确保ecuData大小正确
        if (ecuData.size() != 9) {
            ecuData.resize(9);
        }
        
        // 如果数据有效，将数据添加到ecuData中
        if (data.isValid) {
            // 添加数据
            ecuData[0].append(data.throttle);
            ecuData[1].append(data.engineSpeed);
            ecuData[2].append(data.cylinderTemp);
            ecuData[3].append(data.exhaustTemp);
            ecuData[4].append(data.axleTemp);
            ecuData[5].append(data.fuelPressure);
            ecuData[6].append(data.intakeTemp);
            ecuData[7].append(data.atmPressure);
            ecuData[8].append(data.flightTime);
            
            // 限制数据点数量
            const int maxPoints = 10000;
            for (int i = 0; i < 9; i++) {
                while (ecuData[i].size() > maxPoints) {
                    ecuData[i].removeFirst();
                }
            }
            
            // 更新当前快照中的ECU数据
            currentSnapshot.ecuValid = true;
            currentSnapshot.ecuData.resize(9);
            currentSnapshot.ecuData[0] = data.throttle;
            currentSnapshot.ecuData[1] = data.engineSpeed;
            currentSnapshot.ecuData[2] = data.cylinderTemp;
            currentSnapshot.ecuData[3] = data.exhaustTemp;
            currentSnapshot.ecuData[4] = data.axleTemp;
            currentSnapshot.ecuData[5] = data.fuelPressure;
            currentSnapshot.ecuData[6] = data.intakeTemp;
            currentSnapshot.ecuData[7] = data.atmPressure;
            currentSnapshot.ecuData[8] = data.flightTime;
        }
        
    } catch (const std::exception& e) {
        qDebug() << "处理ECU数据时出错: " << e.what();
    } catch (...) {
        qDebug() << "处理ECU数据时发生未知错误";
    }
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
            
            // 同时从latestECUData获取最新数据，确保没有遗漏
            QMutexLocker locker(&latestECUDataMutex);
            if (latestECUData.isValid) {
                // 使用最新的ECU数据，可能会比列表中的数据更新
                snapshot.ecuData[0] = latestECUData.throttle;
                snapshot.ecuData[1] = latestECUData.engineSpeed;
                snapshot.ecuData[2] = latestECUData.cylinderTemp;
                snapshot.ecuData[3] = latestECUData.exhaustTemp;
                snapshot.ecuData[4] = latestECUData.axleTemp;
                snapshot.ecuData[5] = latestECUData.fuelPressure;
                snapshot.ecuData[6] = latestECUData.intakeTemp;
                snapshot.ecuData[7] = latestECUData.atmPressure;
                snapshot.ecuData[8] = latestECUData.flightTime;
            }
            qDebug() << "快照" << snapshotCount + 1 << "ECU数据有效，数据:" << snapshot.ecuData;
        } else if (latestECUData.isValid && ecuIsConnected) {
            // 如果ecuData为空但有最新的ECU数据，直接使用latestECUData
            QMutexLocker locker(&latestECUDataMutex);
            snapshot.ecuValid = true;
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
            qDebug() << "快照" << snapshotCount + 1 << "ECU数据有效(从latestECUData)，数据:" << snapshot.ecuData;
        } else {
            qDebug() << "快照" << snapshotCount + 1 << "ECU数据无效，ECU连接状态:" << ecuIsConnected;
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