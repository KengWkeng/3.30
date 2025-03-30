#ifndef ECUTHREAD_H
#define ECUTHREAD_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QElapsedTimer>
#include <QDebug>
#include <QDateTime>
#include <QVector>

// ECU数据结构体
struct ECUData {
    // 时间戳
    QDateTime timestamp;
    QElapsedTimer timer; // 添加计时器用于计算经过的时间
    
    // 原始数据字段
    quint16 throttleRaw;      // 节气门开度原始值
    quint16 engineSpeedRaw;   // 发动机转速原始值
    quint16 cylinderTempRaw;  // 缸温原始值
    quint16 exhaustTempRaw;   // 排温原始值
    quint16 axleTempRaw;      // 轴温原始值
    quint16 fuelPressureRaw;  // 燃油压力原始值
    quint16 intakeTempRaw;    // 进气温度原始值
    quint16 atmPressureRaw;   // 大气压力原始值
    quint16 flightTimeRaw;    // 飞行时间原始值
    
    // 计算后的实际值
    double throttle;          // 节气门开度 (%)
    double engineSpeed;       // 发动机转速 (rpm)
    double cylinderTemp;      // 缸温 (°C)
    double exhaustTemp;       // 排温 (°C)
    double axleTemp;          // 轴温 (°C)
    double fuelPressure;      // 燃油压力 (kPa)
    double intakeTemp;        // 进气温度 (°C)
    double atmPressure;       // 大气压力 (kPa)
    double flightTime;        // 飞行时间 (min)
    
    // 状态标志
    bool throttleError;       // 节气门故障
    bool engineSpeedError;    // 发动机转速故障
    bool cylinderTempError;   // 缸温故障
    bool exhaustTempError;    // 排温故障
    bool axleTempError;       // 轴温故障
    bool fuelPressureError;   // 燃油压力故障
    bool intakeTempError;     // 进气温度故障
    bool atmPressureError;    // 大气压力故障
    bool flightTimeError;     // 飞行时间故障
    
    bool isValid;             // 数据是否有效

    // 默认构造函数
    ECUData() : 
        throttleRaw(0), engineSpeedRaw(0), cylinderTempRaw(0), exhaustTempRaw(0),
        axleTempRaw(0), fuelPressureRaw(0), intakeTempRaw(0), atmPressureRaw(0), flightTimeRaw(0),
        throttle(0), engineSpeed(0), cylinderTemp(0), exhaustTemp(0),
        axleTemp(0), fuelPressure(0), intakeTemp(0), atmPressure(0), flightTime(0),
        throttleError(false), engineSpeedError(false), cylinderTempError(false), exhaustTempError(false),
        axleTempError(false), fuelPressureError(false), intakeTempError(false), atmPressureError(false),
        flightTimeError(false), isValid(false)
    {
        timestamp = QDateTime::currentDateTime();
        timer.start(); // 启动计时器
    }
};

class ECUThread : public QObject
{
    Q_OBJECT
public:
    explicit ECUThread(QObject *parent = nullptr);
    ~ECUThread();

signals:
    // 发送ECU数据到主窗口
    void ecuDataReady(const ECUData &data);
    
    // 发送串口连接状态
    void ecuConnectionStatus(bool connected, QString message);
    
    // 发送错误消息
    void ecuError(QString errorMessage);

public slots:
    // 初始化串口
    void initSerialPort();
    
    // 设置并打开串口
    void openECUPort(const QString &portName);
    
    // 关闭串口
    void closeECUPort();
    
private slots:
    // 处理串口接收到的数据
    void processSerialData();

private:
    QSerialPort *serialPort;
    
    // 缓存接收到的数据
    QByteArray dataBuffer;
    
    // 解析ECU数据帧
    bool parseECUData(const QByteArray &data, ECUData &result);
    
    // 计算校验和
    quint8 calculateChecksum(const QByteArray &data);
    
    // 计算实际值
    double calculateRealValue(quint16 rawValue, double precision, double offset);
};

#endif // ECUTHREAD_H 