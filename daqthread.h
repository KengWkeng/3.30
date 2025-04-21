#ifndef DAQTHREAD_H
#define DAQTHREAD_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include "Art_DAQ.h"

// 定义必要的常量，确保这些常量在Art_DAQ.h中未定义的情况下可用
#ifndef ArtDAQ_Val_Cfg_Default
#define ArtDAQ_Val_Cfg_Default  -1
#endif

#ifndef ArtDAQ_Val_Volts
#define ArtDAQ_Val_Volts     10348
#endif

#ifndef ArtDAQ_Val_Rising
#define ArtDAQ_Val_Rising    10280
#endif

#ifndef ArtDAQ_Val_ContSamps
#define ArtDAQ_Val_ContSamps 10123
#endif

#ifndef ArtDAQ_Val_Acquired_Into_Buffer
#define ArtDAQ_Val_Acquired_Into_Buffer 1
#endif

#ifndef ArtDAQ_Val_GroupByScanNumber
#define ArtDAQ_Val_GroupByScanNumber 1
#endif

// 定义回调函数类型
#ifndef ART_CALLBACK
#ifdef _WIN32
#define ART_CALLBACK __cdecl
#else
#define ART_CALLBACK
#endif
#endif

class DAQThread : public QObject
{
    Q_OBJECT

public:
    explicit DAQThread(QObject *parent = nullptr);
    ~DAQThread();

public slots:
    // 用于从主线程接收命令的槽函数
    void initDAQ(const QString &deviceName, const QString &channelStr, double sampleRate, int samplesPerChannel);
    void startAcquisition();
    void stopAcquisition();

    // 低通滤波器相关槽函数
    void setFilterEnabled(bool enabled);
    void setCutoffFrequency(double frequency);
    bool isFilterEnabled() const;
    double getCutoffFrequency() const;

signals:
    // 将数据发送到主线程的信号
    void dataReady(QVector<double> timeData, QVector<QVector<double>> channelData, int numChannels);
    void acquisitionStatus(bool isRunning, QString message);
    void error(QString errorMessage);

private:
    // 数据采集任务句柄
    TaskHandle taskHandle;
    // 采样率
    double sampleRate;
    // 每次读取的样本数
    int samplesPerChannel;
    // 通道数量
    int numChannels;
    // 是否正在采集
    bool isAcquiring;
    // 设备名称
    QString m_deviceName;
    // 通道字符串
    QString m_channelStr;
    // 解析通道字符串
    QVector<int> parseChannels(const QString &channelStr);
    // 获取设备/通道字符串
    QString getDeviceChannelString(const QString &deviceName, const QVector<int> &channels);

    // 回调函数所需的数据
    QVector<QVector<double>> channelData;
    QVector<double> timeData;

    // 低通滤波器相关变量
    bool filterEnabled;                      // 滤波器启用状态
    double cutoffFrequency;                  // 截止频率（Hz）
    QVector<QVector<double>> filterBuffers;  // 每个通道的滤波缓冲区
    QVector<double> filterCoefficients;      // FIR滤波器系数
    int filterOrder;                         // 滤波器阶数

    // 友元声明以便回调函数可以访问私有成员
    friend int32 ART_CALLBACK EveryNCallbackDAQ(TaskHandle taskHandle, int32 everyNsamplesEventType,
                                       uInt32 nSamples, void *callbackData);
    friend int32 ART_CALLBACK DoneCallbackDAQ(TaskHandle taskHandle, int32 status, void *callbackData);

    // 数据处理方法
    void processData(float64 *data, int32 read);

    // 滤波器相关方法
    void calculateFilterCoefficients();      // 计算滤波器系数
    double applyFilter(double sample, int channelIndex); // 应用滤波器到单个样本
};

// 声明回调函数
int32 ART_CALLBACK EveryNCallbackDAQ(TaskHandle taskHandle, int32 everyNsamplesEventType,
                           uInt32 nSamples, void *callbackData);
int32 ART_CALLBACK DoneCallbackDAQ(TaskHandle taskHandle, int32 status, void *callbackData);

// 全局变量，供回调函数使用
extern DAQThread* g_daqThread;

#endif // DAQTHREAD_H