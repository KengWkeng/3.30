#ifndef MODBUSTHREAD_H
#define MODBUSTHREAD_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QModbusRtuSerialClient>
#include <QModbusDataUnit>
#include <QModbusReply>
#include <QCoreApplication>

#include <QThread>
#include <QElapsedTimer>
#include <QDebug>
#include <QVariant>


class modbusThread : public QObject
{
    Q_OBJECT
public:
    explicit modbusThread(QObject *parent = nullptr);

    QElapsedTimer * realTimer;
    
    QModbusRtuSerialClient* getModbusClient() { return modbusClient; }

signals:
    //传递modbus读数 - 改为传递多个寄存器数据
    void sendModbusResult(QVector<double> result, long long readTimeInterval);
    
    // 添加信号用于传递串口连接状态
    void modbusConnectionStatus(bool connected, QString message);

private:
    QModbusRtuSerialClient *modbusClient;

    qint64 currentTime;
    qint64 lastTime;
    qint64 interval;
    qint16 serverNum;




public slots:
    // 添加初始化ModbusClient的槽函数
    void initModbusClient();
    
    //更新函数来读取单一从机的多个寄存器
    void getModbusResult(int serverAddr, int startRegister, int registerCount);
    
    // 添加新的槽函数用于设置串口参数
    void setModbusPortInfo(QString portName, int baudRateIndex, int stopBitsIndex, int dataBitsIndex, int parityIndex);
    
    // 添加新的槽函数用于关闭modbus连接
    void closeModbusConnection();

    // 添加重置计时器的方法
    void resetTimer();
};

#endif // MODBUSTHREAD_H
