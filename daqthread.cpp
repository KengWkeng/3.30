#include "daqthread.h"

// 全局变量，供回调函数使用
DAQThread* g_daqThread = nullptr;

DAQThread::DAQThread(QObject *parent) 
    : QObject(parent)
    , taskHandle(0)
    , sampleRate(10000)
    , samplesPerChannel(1000)
    , numChannels(0)
    , isAcquiring(false)
{
    // 设置全局指针
    g_daqThread = this;
}

DAQThread::~DAQThread()
{
    // 确保任务已停止
    if (isAcquiring) {
        stopAcquisition();
    }
}

void DAQThread::initDAQ(const QString &deviceName, const QString &channelStr, double sampleRate, int samplesPerChannel)
{
    // 存储参数
    this->sampleRate = sampleRate;
    this->samplesPerChannel = samplesPerChannel;
    this->m_deviceName = deviceName;
    this->m_channelStr = channelStr;
    
    // 解析通道
    QVector<int> channels = parseChannels(channelStr);
    if (channels.isEmpty()) {
        emit error("通道格式无效，请使用数字和'/'符号分隔，例如：0/1/2");
        return;
    }
    
    numChannels = channels.size();
    
    // 初始化数据缓冲区
    timeData.clear();
    channelData.clear();
    channelData.resize(numChannels);
    
    qDebug() << "初始化DAQ任务: 设备=" << deviceName << ", 通道=" << channelStr 
             << ", 采样率=" << sampleRate << ", 通道数=" << numChannels;
    
    emit acquisitionStatus(false, "已初始化数据采集任务");
}

void DAQThread::startAcquisition()
{
    if (isAcquiring) {
        emit error("数据采集已在进行中");
        return;
    }
    
    // 使用存储的通道字符串重新解析通道
    QVector<int> channels = parseChannels(m_channelStr);
    if (channels.isEmpty() || numChannels == 0) {
        emit error("未指定通道");
        return;
    }
    
    // 获取设备名称 - 从initDAQ中保存的变量获取
    QString deviceName = m_deviceName;
    if (deviceName.isEmpty()) {
        deviceName = "Dev1"; // 默认设备名
    }
    
    QString deviceChannelStr = getDeviceChannelString(deviceName, channels);
    qDebug() << "使用设备通道字符串: " << deviceChannelStr;
    
    // 设置ArtDAQ任务
    int32 errorCode = 0;
    char errBuff[2048] = {'\0'};
    
    // 创建任务
    errorCode = ArtDAQ_CreateTask("", &taskHandle);
    if (errorCode < 0) {
        ArtDAQ_GetExtendedErrorInfo(errBuff, 2048);
        emit error(QString("创建任务失败: %1").arg(errBuff));
        return;
    }
    
    // 创建模拟输入通道
    errorCode = ArtDAQ_CreateAIVoltageChan(taskHandle, deviceChannelStr.toStdString().c_str(), "", 
                                      ArtDAQ_Val_Cfg_Default, -10.0, 10.0, ArtDAQ_Val_Volts, NULL);
    if (errorCode < 0) {
        ArtDAQ_GetExtendedErrorInfo(errBuff, 2048);
        ArtDAQ_ClearTask(taskHandle);
        taskHandle = 0;
        emit error(QString("创建通道失败: %1").arg(errBuff));
        return;
    }
    
    // 设置采样时钟
    errorCode = ArtDAQ_CfgSampClkTiming(taskHandle, "", sampleRate, ArtDAQ_Val_Rising, 
                                   ArtDAQ_Val_ContSamps, samplesPerChannel);
    if (errorCode < 0) {
        ArtDAQ_GetExtendedErrorInfo(errBuff, 2048);
        ArtDAQ_ClearTask(taskHandle);
        taskHandle = 0;
        emit error(QString("设置采样时钟失败: %1").arg(errBuff));
        return;
    }
    
    // 注册回调函数
    errorCode = ArtDAQ_RegisterEveryNSamplesEvent(taskHandle, ArtDAQ_Val_Acquired_Into_Buffer, 
                                            samplesPerChannel, 0, EveryNCallbackDAQ, NULL);
    if (errorCode < 0) {
        ArtDAQ_GetExtendedErrorInfo(errBuff, 2048);
        ArtDAQ_ClearTask(taskHandle);
        taskHandle = 0;
        emit error(QString("注册回调函数失败: %1").arg(errBuff));
        return;
    }
    
    errorCode = ArtDAQ_RegisterDoneEvent(taskHandle, 0, DoneCallbackDAQ, NULL);
    if (errorCode < 0) {
        ArtDAQ_GetExtendedErrorInfo(errBuff, 2048);
        ArtDAQ_ClearTask(taskHandle);
        taskHandle = 0;
        emit error(QString("注册完成事件失败: %1").arg(errBuff));
        return;
    }
    
    // 开始任务
    errorCode = ArtDAQ_StartTask(taskHandle);
    if (errorCode < 0) {
        ArtDAQ_GetExtendedErrorInfo(errBuff, 2048);
        ArtDAQ_ClearTask(taskHandle);
        taskHandle = 0;
        emit error(QString("启动任务失败: %1").arg(errBuff));
        return;
    }
    
    isAcquiring = true;
    emit acquisitionStatus(true, "数据采集已开始");
}

void DAQThread::stopAcquisition()
{
    if (!isAcquiring) {
        return;
    }
    
    // 停止和清理任务
    if (taskHandle != 0) {
        ArtDAQ_StopTask(taskHandle);
        ArtDAQ_ClearTask(taskHandle);
        taskHandle = 0;
    }
    
    isAcquiring = false;
    emit acquisitionStatus(false, "数据采集已停止");
}

void DAQThread::processData(float64 *data, int32 read)
{
    if (read <= 0 || !isAcquiring) {
        return;
    }
    
    // 计算当前的时间点
    double startTime = 0.0;
    if (!timeData.isEmpty()) {
        startTime = timeData.last() + 1.0 / sampleRate;
    }
    
    // 每个通道的数据是交错存储的，需要解交错
    for (int i = 0; i < read; ++i) {
        double time = startTime + i / sampleRate;
        timeData.append(time);
        
        for (int ch = 0; ch < numChannels; ++ch) {
            channelData[ch].append(data[i * numChannels + ch]);
        }
    }
    
    // 限制数据点数量，避免内存占用过多
    int maxDataPoints = 10000;
    if (timeData.size() > maxDataPoints) {
        int removeCount = timeData.size() - maxDataPoints;
        timeData.remove(0, removeCount);
        for (int ch = 0; ch < numChannels; ++ch) {
            channelData[ch].remove(0, removeCount);
        }
    }
    
    // 将数据发送到主线程
    emit dataReady(timeData, channelData, numChannels);
}

QVector<int> DAQThread::parseChannels(const QString &channelStr)
{
    QVector<int> channels;
    
    // 如果已经初始化过，且当前参数为空，则返回已有的通道设置
    if (channelStr.isEmpty() && numChannels > 0) {
        for (int i = 0; i < numChannels; i++) {
            channels.append(i);
        }
        qDebug() << "使用默认通道序号:" << channels;
        return channels;
    }
    
    QStringList parts = channelStr.split("/", Qt::SkipEmptyParts);
    qDebug() << "解析通道字符串:" << channelStr << "，分割为:" << parts;
    
    for (const QString &part : parts) {
        bool ok;
        int channel = part.toInt(&ok);
        if (ok) {
            channels.append(channel);
        } else {
            qDebug() << "无法解析通道号:" << part;
        }
    }
    
    qDebug() << "解析得到的通道号:" << channels;
    return channels;
}

QString DAQThread::getDeviceChannelString(const QString &deviceName, const QVector<int> &channels)
{
    QStringList channelList;
    
    for (int ch : channels) {
        channelList.append(QString("%1/ai%2").arg(deviceName).arg(ch));
    }
    
    QString result = channelList.join(",");
    qDebug() << "生成设备通道字符串:" << result;
    return result;
}

// 全局回调函数实现
int32 ART_CALLBACK EveryNCallbackDAQ(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void *callbackData)
{
    int32 errorCode = 0;
    char errBuff[2048] = {'\0'};
    int32 read = 0;
    
    // 检查全局指针是否有效
    if (!g_daqThread || !g_daqThread->isAcquiring) {
        qDebug() << "回调函数: g_daqThread无效或非采集状态";
        return -1;
    }
    
    // 创建足够大的缓冲区来存储所有通道的数据
    int numChannels = g_daqThread->numChannels;
    int bufferSize = nSamples * numChannels;
    float64 *data = new float64[bufferSize];
    
    // 读取数据
    errorCode = ArtDAQ_ReadAnalogF64(taskHandle, nSamples, 10.0, ArtDAQ_Val_GroupByScanNumber, 
                               data, bufferSize, &read, NULL);
    
    if (errorCode < 0) {
        ArtDAQ_GetExtendedErrorInfo(errBuff, 2048);
        qDebug() << "读取数据失败: " << errBuff;
    } else if (read > 0) {
        qDebug() << "成功读取" << read << "个样本，" << numChannels << "个通道";
        // 处理数据
        g_daqThread->processData(data, read);
    } else {
        qDebug() << "未读取到数据";
    }
    
    delete[] data;
    return 0;
}

int32 ART_CALLBACK DoneCallbackDAQ(TaskHandle taskHandle, int32 status, void *callbackData)
{
    int32 errorCode = 0;
    char errBuff[2048] = {'\0'};
    
    // 检查是否由于错误停止
    if (status < 0) {
        ArtDAQ_GetExtendedErrorInfo(errBuff, 2048);
        qDebug() << "任务异常终止: " << errBuff;
        if (g_daqThread) {
            g_daqThread->isAcquiring = false;
            emit g_daqThread->acquisitionStatus(false, QString("任务异常终止: %1").arg(errBuff));
        }
    }
    
    return 0;
} 