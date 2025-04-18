#ifndef SNAPSHOTTHREAD_H
#define SNAPSHOTTHREAD_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <QQueue>
#include <QVector>
#include <QDebug>
#include <QMutex>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMutexLocker>
#include <QDateTime>
#include <cmath> // For round
#include <QFile>         // Added for file logging
#include <QTextStream>   // Added for file logging
#include <QStringList>   // Added for CSV header
#include <QMap>

// 包含ECU数据结构的定义
#include "ecuthread.h"

// Forward declaration or include ECUData definition here
struct ECUData;
class DataSnapshot; // Forward declaration

// +++ 新增: 校准参数结构体 +++
struct CalibrationParams {
    double a = 0.0;
    double b = 0.0;
    double c = 1.0; // 默认 y=x
    double d = 0.0;
};
// +++ 结束新增 +++

// 使用与MainWindow相同的数据快照类
class DataSnapshot {
public: // 公开访问成员
    double timestamp;                   // 时间戳（秒）
    QVector<double> modbusData;         // Modbus数据(一维) - 每个寄存器的值
    QVector<double> daqData;            // DAQ数据(一维) - 修改为一维，每个通道只保留最新值
    QVector<double> ecuData;            // ECU数据(9通道)
    QVector<double> customData;         // 新增：自定义计算数据
    bool modbusValid;                   // Modbus数据有效标志
    bool daqValid;                      // DAQ数据有效标志
    bool ecuValid;                      // ECU数据有效标志
    bool daqRunning;                    // DAQ运行状态标志
    int snapshotIndex;                  // 快照索引（序号）

    // 构造函数，初始化所有数据
    DataSnapshot() {
        timestamp = 0.0;                // 初始化为0秒
        modbusData.resize(16, 0.0);     // 16个Modbus寄存器
        daqData.resize(16, 0.0);        // 16个DAQ通道，每个通道只保存最新值
        ecuData.resize(9, 0.0);         // 9个ECU通道
        customData.resize(5, 0.0);      // 初始化自定义数据，预留5个位置
        modbusValid = false;
        daqValid = false;
        ecuValid = false;
        daqRunning = false;             // 初始化DAQ运行状态为false
        snapshotIndex = 0;              // 初始化索引为0
    }
};

class SnapshotThread : public QObject
{
    Q_OBJECT
public:
    explicit SnapshotThread(QObject *parent = nullptr);
    ~SnapshotThread();

    // 初始化主计时器
    void setupMasterTimer();

    // 处理结果时应用滤波
    QVector<double> applyFilterToResults(const QVector<double> &rawData, qint64 deltaT);

    // 一阶低通滤波函数
    double applyLowPassFilter(double input, double prevOutput, double timeConstant, double deltaT);

    // 设置滤波状态和参数
    void setFilterEnabled(bool enabled, double timeConstant = 100.0);
    
    // 新增: 公共方法用于重新加载校准文件
    void reloadCalibrationSettings(const QString& filePath);

    // +++ 新增: 公开获取校准参数的方法 +++
    CalibrationParams getCalibrationParams(const QString& sourceType, int channelIndex);
    // +++ 结束新增 +++

    // 新增：公共方法设置/获取数据记录状态
    void setDataLoggingEnabled(bool enabled);
    bool isDataLoggingEnabled() const;

public slots:
    // 处理Modbus数据
    void handleModbusData(QVector<double> resultdata, qint64 readTimeInterval);

    // 处理DAQ数据
    void handleDAQData(const QVector<double> &timeData, const QVector<QVector<double>> &channelData);

    // 处理ECU数据
    void handleECUData(const ECUData &data);

    // 处理数据快照
    void processDataSnapshots();

    // 接收ECU连接状态
    void handleECUConnectionStatus(bool connected, QString message);

    // New public slot
    void setProcessingEnabled(bool enabled);

    // New slot to receive config
    void setupLogging(int modbusCount, int daqCount);

    // Public method to reset the master timer
    Q_INVOKABLE void resetMasterTimer();

signals:
    // 发送处理好的数据快照
    void snapshotProcessed(const DataSnapshot &snapshot, int snapshotCount);
    void sendModbusResultToWebSocket(const QJsonObject &data, int interval);
    // 发送给WebSocket线程的统一快照信号 (新增)
    void snapshotForWebSocket(const DataSnapshot &snapshot, int snapshotCount);
    // +++ 新增: 发送原始数据快照信号 +++
    void rawSnapshotReady(const DataSnapshot &rawSnapshot);

private:
    // 数据存储相关变量
    DataSnapshot currentSnapshot;          // 当前数据快照
    QQueue<DataSnapshot> snapshotQueue;    // 数据快照队列
    QElapsedTimer *masterTimer;            // 主计时器，用于同步数据
    int maxQueueSize = 1000;               // 最大队列长度，防止内存占用过多
    int snapshotCount = 0;                 // 快照计数器
    
    // 新增：控制是否记录数据到文件
    bool enableDataLogging = true;         // 默认启用数据记录

    // Modbus相关
    bool modbusDataValid = false;          // Modbus数据有效标志
    int modbusNumRegs = 16;                // Modbus寄存器数量
    QVector<QVector<double>> modbusData;   // Modbus数据缓冲区

    // DAQ相关
    QVector<QVector<double>> daqChannelData; // DAQ通道数据
    QVector<double> daqTimeData;             // DAQ时间数据
    int daqNumChannels = 16;                 // DAQ通道数量
    bool daqIsAcquiring = false;             // DAQ采集状态

    // ECU相关
    QVector<QVector<double>> ecuData;      // ECU数据缓冲区
    QVector<double> customDataBuffer;   // 用于计算customData的临时缓冲区
    ECUData latestECUData;                     // ECU最新数据
    bool snapEcuIsConnected = false;           // ECU连接状态
    bool ecuDataValid = false;             // ECU数据有效标志
    QMutex latestECUDataMutex;             // 保护latestECUData的互斥锁

    // 滤波相关
    bool filterEnabled = true;             // 滤波器使能状态
    QVector<double> filteredValues;        // 存储上一次滤波后的结果
    QElapsedTimer *realTimer;              // 计时器，用于计算deltaT
    qint64 lastTimestamp;                  // 上一次时间戳
    double filterTimeConstant = 0.2;       // 滤波时间常数，可调整

    // Add processing enabled flag
    bool processingEnabled = false;

    // +++ 新增: 存储校准参数 +++
    QMap<QString, QMap<int, CalibrationParams>> allCalibrationParams; // Key1: SourceType ("Modbus", "DAQ", "ECU", "Custom"), Key2: Channel Index
    // +++ 结束新增 +++

    // +++ 新增: 私有辅助函数声明 +++
    void loadCalibrationSettings(const QString& filePath);
    double applyCalibration(double rawValue, const CalibrationParams& params);
    // +++ 结束新增 +++

    // Logging members
    QFile *logFile = nullptr;
    QTextStream *logStream = nullptr;
    QString logFilePath;
    QStringList csvHeader;
    int snapshotsSinceLastWrite = 0;
    const int writeThreshold = 100; // Write to disk every 100 snapshots
    // Store configured channel counts
    int configuredModbusChannels = 0;
    int configuredDaqChannels = 0;

    // Private helper methods for logging
    bool initializeLogFile();
    void generateCsvHeader(); // Helper to create the header string
    void writeCsvHeader();
    void writeSnapshotToFile(const DataSnapshot &snapshot);
    void closeLogFile();

};

#endif // SNAPSHOTTHREAD_H