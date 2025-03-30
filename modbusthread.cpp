#include "modbusthread.h"

modbusThread::modbusThread(QObject *parent)
    : QObject{parent}
{
    //qDebug() << "modbusThread created in thread:" << QThread::currentThread();

    realTimer = new QElapsedTimer;
    // 不在构造函数中启动计时器，而是通过resetTimer方法启动
    lastTime = 0;
    currentTime = 0;
    
    // 不在构造函数中创建modbusClient，而是通过信号延迟创建
}

void modbusThread::initModbusClient()
{
    //qDebug() << "Initializing modbusClient in thread:" << QThread::currentThread();
    modbusClient = new QModbusRtuSerialClient(this);
    connect(modbusClient, &QModbusDevice::stateChanged, this, [this](QModbusDevice::State state) {
        if (state == QModbusDevice::ConnectedState) {
            emit modbusConnectionStatus(true, "串口连接状态已变更为已连接");
        } else if (state == QModbusDevice::UnconnectedState) {
            emit modbusConnectionStatus(false, "串口连接状态已变更为未连接");
        }
    });
}

// 实现重置计时器的方法
void modbusThread::resetTimer()
{
    qDebug() << "重置ModBus线程计时器";
    lastTime = 0;
    currentTime = 0;
    
    // 重新启动计时器
    if (realTimer) {
        realTimer->restart();
    } else {
        realTimer = new QElapsedTimer;
        realTimer->start();
    }
}

void modbusThread::getModbusResult(int serverAddr, int startRegister, int registerCount)
{
    //qDebug() << "getModbusResult running in thread:" << QThread::currentThread();
    // 准备存储寄存器值的向量
    QVector<double> resultdata(registerCount);

    // 精密计时器
    currentTime = realTimer->elapsed();
    interval = currentTime - lastTime;

    // 创建读取请求，读取多个寄存器
    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters, startRegister, registerCount);

    if (auto *reply = modbusClient->sendReadRequest(readUnit, serverAddr))
    {
        // 使用信号槽方式处理完成信号
        connect(reply, &QModbusReply::finished, this, [this, reply, registerCount, resultdata = std::move(resultdata)]() mutable {
            if (reply->error() == QModbusDevice::NoError)
            {
                const QModbusDataUnit resultUnit = reply->result();
                
                // 直接将每个寄存器的值存入结果向量
                for (int i = 0; i < resultUnit.valueCount(); i++)
                {
                    // 转换为实际值 (乘以0.01)
                    resultdata[i] = resultUnit.value(i) * 0.01;
                }
            }
            else
            {
                qDebug() << "Modbus读取错误: " << reply->errorString();
                // 错误情况下填充无效数据
                for (int i = 0; i < registerCount; i++) {
                    resultdata[i] = -9999; // 使用一个特殊值表示错误
                }
            }
            
            // 更新时间并发送结果
            lastTime = currentTime;
            emit sendModbusResult(resultdata, interval);
            
            reply->deleteLater();
        });
    }
    else
    {
        qDebug() << "无法发送Modbus请求: " << modbusClient->errorString();
        // 请求失败情况下填充无效数据
        for (int i = 0; i < registerCount; i++) {
            resultdata[i] = -9999; // 使用一个特殊值表示错误
        }
        
        // 更新时间并发送结果
        lastTime = currentTime;
        emit sendModbusResult(resultdata, interval);
    }
}

// 设置串口参数的函数实现
void modbusThread::setModbusPortInfo(QString portName, int baudRateIndex, int stopBitsIndex, int dataBitsIndex, int parityIndex)
{
    //qDebug() << "setModbusPortInfo running in thread:" << QThread::currentThread();
    // 设置串口名称
    modbusClient->setConnectionParameter(QModbusDevice::SerialPortNameParameter, QVariant(portName));
    
    // 设置波特率
    switch (baudRateIndex)
    {
    case 0: modbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, QSerialPort::Baud9600); break;
    case 1: modbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, QSerialPort::Baud19200); break;
    case 2: modbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, QSerialPort::Baud38400); break;
    case 3: modbusClient->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, QSerialPort::Baud57600); break;
    }
    
    // 设置停止位
    switch (stopBitsIndex)
    {
    case 0:  modbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::OneStop); break;
    case 1:  modbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::OneAndHalfStop); break;
    case 2:  modbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::TwoStop); break;
    default: modbusClient->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::OneStop); break;
    }
    
    // 设置数据位
    switch (dataBitsIndex) 
    {
    case 0: modbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8); break;
    case 1: modbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data7); break;
    case 2: modbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data6); break;
    case 3: modbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data5); break;
    default: modbusClient->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8); break;
    }
    
    // 设置校验位
    switch (parityIndex)
    {
    case 0:  modbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::NoParity); break;
    case 1:  modbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::EvenParity); break;
    case 2:  modbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::OddParity); break;
    default: modbusClient->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::NoParity); break;
    }
    
    // 连接设备
    if (modbusClient->connectDevice())
    {
        qDebug() << "串口连接成功";
        // 通过信号发送连接成功的消息
        emit modbusConnectionStatus(true, "串口连接成功");
    }
    else
    {
        qDebug() << "串口连接失败: " << modbusClient->errorString();
        modbusClient->disconnectDevice();
        // 通过信号发送连接失败的消息
        emit modbusConnectionStatus(false, "串口连接失败: " + modbusClient->errorString());
    }
}

// 添加关闭modbus连接的槽函数实现
void modbusThread::closeModbusConnection()
{
    qDebug() << "closeModbusConnection running in thread:" << QThread::currentThread();
    if (modbusClient && modbusClient->state() != QModbusDevice::UnconnectedState) {
        modbusClient->disconnectDevice();
        emit modbusConnectionStatus(false, "串口已关闭");
    }
}

