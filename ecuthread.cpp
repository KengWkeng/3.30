#include "ecuthread.h"

ECUThread::ECUThread(QObject *parent)
    : QObject{parent}
{
    qDebug() << "ECUThread 创建在线程:" << QThread::currentThread();
    
    // 创建串口对象，但不在构造函数中初始化
    serialPort = nullptr;
}

ECUThread::~ECUThread()
{
    // 确保在析构时关闭串口并释放资源
    if (serialPort) {
        if (serialPort->isOpen()) {
            serialPort->close();
        }
        delete serialPort;
        serialPort = nullptr;
    }
}

void ECUThread::initSerialPort()
{
    qDebug() << "初始化ECU串口，线程:" << QThread::currentThread();
    
    // 确保在初始化前释放旧的串口资源
    if (serialPort) {
        if (serialPort->isOpen()) {
            serialPort->close();
        }
        delete serialPort;
    }
    
    // 创建新的串口对象
    serialPort = new QSerialPort(this);
    
    // 连接readyRead信号到处理函数
    connect(serialPort, &QSerialPort::readyRead, this, &ECUThread::processSerialData);
    
    // 连接错误信号
    connect(serialPort, &QSerialPort::errorOccurred, this, [=](QSerialPort::SerialPortError error) {
        if (error != QSerialPort::NoError) {
            emit ecuError(QString("串口错误: %1").arg(serialPort->errorString()));
        }
    });
    
    // 清空数据缓冲区
    dataBuffer.clear();
}

void ECUThread::openECUPort(const QString &portName)
{
    qDebug() << "打开ECU串口:" << portName;
    
    // 如果串口未初始化，先进行初始化
    if (!serialPort) {
        initSerialPort();
    }
    
    // 如果串口已经打开，先关闭
    if (serialPort->isOpen()) {
        serialPort->close();
    }
    
    // 设置串口名称
    serialPort->setPortName(portName);
    
    // 配置串口参数 - 根据ECU通信协议配置
    serialPort->setBaudRate(QSerialPort::Baud115200);   // 波特率115200
    serialPort->setDataBits(QSerialPort::Data8);        // 数据位8位
    serialPort->setParity(QSerialPort::NoParity);       // 无校验
    serialPort->setStopBits(QSerialPort::OneStop);      // 1位停止位
    serialPort->setFlowControl(QSerialPort::NoFlowControl); // 无流控制
    
    // 尝试打开串口
    if (serialPort->open(QIODevice::ReadOnly)) {
        emit ecuConnectionStatus(true, QString("ECU串口 %1 已成功打开").arg(portName));
    } else {
        emit ecuConnectionStatus(false, QString("无法打开ECU串口 %1: %2").arg(portName).arg(serialPort->errorString()));
    }
}

void ECUThread::closeECUPort()
{
    qDebug() << "关闭ECU串口";
    
    // 检查串口是否存在并已打开
    if (serialPort && serialPort->isOpen()) {
        serialPort->close();
        emit ecuConnectionStatus(false, "ECU串口已关闭");
    }
}

void ECUThread::processSerialData()
{
    // 检查串口状态
    if (!serialPort || !serialPort->isOpen()) {
        return;
    }
    
    // 读取可用数据
    dataBuffer.append(serialPort->readAll());
    
    // 处理缓冲区中的数据
    while (dataBuffer.size() >= 23) { // 一个完整的ECU数据帧有23个字节
        // 寻找帧头 0x80 0x80
        int startIndex = -1;
        for (int i = 0; i <= dataBuffer.size() - 23; ++i) {
            if ((quint8)dataBuffer.at(i) == 0x80 && (quint8)dataBuffer.at(i+1) == 0x80) {
                startIndex = i;
                break;
            }
        }
        
        // 如果找不到帧头，清除部分数据并返回
        if (startIndex == -1) {
            if (dataBuffer.size() > 100) {
                // 保留最后100个字节，避免缓冲区过大
                dataBuffer = dataBuffer.right(100);
            }
            return;
        }
        
        // 如果找到帧头但数据不够一帧，等待下次接收
        if (startIndex + 23 > dataBuffer.size()) {
            return;
        }
        
        // 提取一帧数据
        QByteArray frame = dataBuffer.mid(startIndex, 23);
        
        // 解析数据帧
        ECUData ecuData;
        if (parseECUData(frame, ecuData)) {
            // 设置时间戳
            ecuData.timestamp = QDateTime::currentDateTime();
            
            // 发送解析成功的数据
            emit ecuDataReady(ecuData);
        } else {
            qDebug() << "ECU数据帧解析失败";
        }
        
        // 从缓冲区中移除已处理的数据
        dataBuffer.remove(0, startIndex + 23);
    }
}

bool ECUThread::parseECUData(const QByteArray &data, ECUData &result)
{
    // 检查数据长度
    if (data.size() != 23) {
        return false;
    }
    
    // 检查帧头
    if ((quint8)data.at(0) != 0x80 || (quint8)data.at(1) != 0x80) {
        return false;
    }
    
    // 检查帧尾
    if ((quint8)data.at(21) != 0x0D || (quint8)data.at(22) != 0x0A) {
        return false;
    }
    
    // 计算校验和
    quint8 calculatedChecksum = calculateChecksum(data.mid(0, 20));
    quint8 receivedChecksum = (quint8)data.at(20);
    
    // 校验
    if (calculatedChecksum != receivedChecksum) {
        qDebug() << "ECU数据校验失败，计算=" << calculatedChecksum << "接收=" << receivedChecksum;
        return false;
    }
    
    // 解析各个数据字段
    quint16 throttleRaw = ((quint8)data.at(2) << 8) | (quint8)data.at(3);
    quint16 engineSpeedRaw = ((quint8)data.at(4) << 8) | (quint8)data.at(5);
    quint16 cylinderTempRaw = ((quint8)data.at(6) << 8) | (quint8)data.at(7);
    quint16 exhaustTempRaw = ((quint8)data.at(8) << 8) | (quint8)data.at(9);
    quint16 axleTempRaw = ((quint8)data.at(10) << 8) | (quint8)data.at(11);
    quint16 fuelPressureRaw = ((quint8)data.at(12) << 8) | (quint8)data.at(13);
    quint16 intakeTempRaw = ((quint8)data.at(14) << 8) | (quint8)data.at(15);
    quint16 atmPressureRaw = ((quint8)data.at(16) << 8) | (quint8)data.at(17);
    quint16 flightTimeRaw = ((quint8)data.at(18) << 8) | (quint8)data.at(19);
    
    // 保存原始值
    result.throttleRaw = throttleRaw;
    result.engineSpeedRaw = engineSpeedRaw;
    result.cylinderTempRaw = cylinderTempRaw;
    result.exhaustTempRaw = exhaustTempRaw;
    result.axleTempRaw = axleTempRaw;
    result.fuelPressureRaw = fuelPressureRaw;
    result.intakeTempRaw = intakeTempRaw;
    result.atmPressureRaw = atmPressureRaw;
    result.flightTimeRaw = flightTimeRaw;
    
    // 检查故障状态并计算实际值
    // 节气门开度：精度0.1，偏移量0
    result.throttleError = (throttleRaw == 0xFFFF);
    result.throttle = result.throttleError ? 0 : calculateRealValue(throttleRaw, 0.1, 0);
    
    // 发动机转速：精度1，偏移量0
    result.engineSpeedError = (engineSpeedRaw == 0xFFFF);
    result.engineSpeed = result.engineSpeedError ? 0 : calculateRealValue(engineSpeedRaw, 1, 0);
    
    // 缸温：精度2，偏移量-30
    result.cylinderTempError = (cylinderTempRaw == 0xFFFF);
    result.cylinderTemp = result.cylinderTempError ? 0 : calculateRealValue(cylinderTempRaw, 2, -30);
    
    // 排温：精度5，偏移量-30
    result.exhaustTempError = (exhaustTempRaw == 0xFFFF);
    result.exhaustTemp = result.exhaustTempError ? 0 : calculateRealValue(exhaustTempRaw, 5, -30);
    
    // 轴温：精度2，偏移量-40
    result.axleTempError = (axleTempRaw == 0xFFFF);
    result.axleTemp = result.axleTempError ? 0 : calculateRealValue(axleTempRaw, 2, -40);
    
    // 燃油压力：精度2，偏移量0
    result.fuelPressureError = (fuelPressureRaw == 0xFFFF);
    result.fuelPressure = result.fuelPressureError ? 0 : calculateRealValue(fuelPressureRaw, 2, 0);
    
    // 进气温度：精度2，偏移量-40
    result.intakeTempError = (intakeTempRaw == 0xFFFF);
    result.intakeTemp = result.intakeTempError ? 0 : calculateRealValue(intakeTempRaw, 2, -40);
    
    // 大气压力：精度1，偏移量0
    result.atmPressureError = (atmPressureRaw == 0xFFFF);
    result.atmPressure = result.atmPressureError ? 0 : calculateRealValue(atmPressureRaw, 1, 0);
    
    // 飞行时间：精度1，偏移量0
    result.flightTimeError = (flightTimeRaw == 0xFFFF);
    result.flightTime = result.flightTimeError ? 0 : calculateRealValue(flightTimeRaw, 1, 0);
    
    // 数据解析完成，设置isValid为true
    result.isValid = true;
    result.timer.restart(); // 重置计时器
    
    return true;
}

quint8 ECUThread::calculateChecksum(const QByteArray &data)
{
    quint8 sum = 0;
    // 累加前20个字节
    for (int i = 0; i < data.size(); ++i) {
        sum += (quint8)data.at(i);
    }
    return sum; // 返回最低8位
}

double ECUThread::calculateRealValue(quint16 rawValue, double precision, double offset)
{
    // 实际值 = 原始数据 × 精度 + 偏移量
    return rawValue * precision + offset;
} 