#include "daqthread.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 全局变量，供回调函数使用
DAQThread* g_daqThread = nullptr;

DAQThread::DAQThread(QObject *parent)
    : QObject(parent)
    , taskHandle(0)
    , sampleRate(10000)
    , samplesPerChannel(1000)
    , numChannels(0)
    , isAcquiring(false)
    , filterEnabled(false)
    , cutoffFrequency(50.0)   // 默认截止频率设置为50Hz，适合低频滤波
    , filterOrder(128)        // 默认滤波器阶数增加到128，提高低频滤波效果
{
    // 设置全局指针
    g_daqThread = this;

    // 初始化滤波器系数
    calculateFilterCoefficients();

    qDebug() << "[DAQThread] 初始化完成，默认滤波器设置: 截止频率=" << cutoffFrequency << "Hz, 阶数=" << filterOrder;
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

    // 确保滤波缓冲区大小正确
    if (filterBuffers.size() != numChannels) {
        filterBuffers.resize(numChannels);
        for (int ch = 0; ch < numChannels; ++ch) {
            filterBuffers[ch].resize(filterOrder + 1); // 确保缓冲区大小与滤波器阶数匹配
            filterBuffers[ch].fill(0.0); // 初始化为0
        }
        qDebug() << "[DAQThread] 初始化滤波缓冲区: " << numChannels << "个通道, 每通道" << (filterOrder + 1) << "个样本";
    }

    // 每个通道的数据是交错存储的，需要解交错
    for (int i = 0; i < read; ++i) {
        double time = startTime + i / sampleRate;
        timeData.append(time);

        for (int ch = 0; ch < numChannels; ++ch) {
            double rawValue = data[i * numChannels + ch];

            // 如果滤波器启用，应用滤波
            if (filterEnabled) {
                double filteredValue = applyFilter(rawValue, ch);
                channelData[ch].append(filteredValue);
            } else {
                channelData[ch].append(rawValue);
            }
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

// 设置滤波器启用状态
void DAQThread::setFilterEnabled(bool enabled)
{
    filterEnabled = enabled;
    qDebug() << "[DAQThread] 滤波器状态已设置为:" << (enabled ? "启用" : "禁用");
}

// 设置截止频率
void DAQThread::setCutoffFrequency(double frequency)
{
    // 防止设置无效的截止频率
    if (frequency <= 0 || frequency >= sampleRate / 2.0) {
        qDebug() << "[DAQThread] 无效的截止频率:" << frequency << "必须在(0," << sampleRate/2.0 << ")范围内";
        return;
    }

    cutoffFrequency = frequency;
    qDebug() << "[DAQThread] 截止频率已设置为:" << frequency << "Hz";

    // 重新计算滤波器系数
    calculateFilterCoefficients();
}

// 获取滤波器启用状态
bool DAQThread::isFilterEnabled() const
{
    return filterEnabled;
}

// 获取截止频率
double DAQThread::getCutoffFrequency() const
{
    return cutoffFrequency;
}

// 计算FIR滤波器系数 - 使用Hamming窗函数
void DAQThread::calculateFilterCoefficients()
{
    // 使用更高的滤波器阶数来提高低频滤波效果
    // 对于50Hz以下的低频滤波，需要更高的阶数
    filterOrder = 128; // 增加滤波器阶数以提高低频滤波效果

    // 重置滤波器系数
    filterCoefficients.resize(filterOrder + 1);

    // 归一化截止频率
    double normalizedCutoff = cutoffFrequency / sampleRate;

    // 限制截止频率在合理范围内
    if (normalizedCutoff > 0.45) {
        normalizedCutoff = 0.45; // 防止截止频率过高
        cutoffFrequency = normalizedCutoff * sampleRate;
        qDebug() << "[DAQThread] 截止频率过高，已调整为:" << cutoffFrequency << "Hz";
    }

    // 计算滤波器系数
    double sum = 0.0;
    for (int i = 0; i <= filterOrder; i++) {
        // 计算滤波器系数
        double coef;
        if (i == filterOrder / 2) {
            // 中心点
            coef = 2.0 * normalizedCutoff;
        } else {
            // 其他点
            double x = 2.0 * M_PI * normalizedCutoff * (i - filterOrder / 2.0);
            coef = qSin(x) / x;
        }

        // 应用Hamming窗函数 - 比汉宁窗有更好的侧带抑制
        // Hamming窗函数: 0.54 - 0.46 * cos(2πn/N)
        double hammingWindow = 0.54 - 0.46 * qCos(2.0 * M_PI * i / filterOrder);
        filterCoefficients[i] = coef * hammingWindow;
        sum += filterCoefficients[i];
    }

    // 归一化系数，确保增益为1
    for (int i = 0; i <= filterOrder; i++) {
        filterCoefficients[i] /= sum;
    }

    qDebug() << "[DAQThread] 已计算" << (filterOrder + 1) << "阶FIR滤波器系数，使用Hamming窗函数";
    qDebug() << "[DAQThread] 截止频率:" << cutoffFrequency << "Hz，采样率:" << sampleRate << "Hz";
}

// 应用滤波器到单个样本 - 优化版本
double DAQThread::applyFilter(double sample, int channelIndex)
{
    // 检查通道索引是否有效
    if (channelIndex < 0 || channelIndex >= filterBuffers.size()) {
        return sample; // 如果无效，返回原始样本
    }

    // 确保缓冲区大小正确
    if (filterBuffers[channelIndex].size() != filterOrder + 1) {
        filterBuffers[channelIndex].resize(filterOrder + 1);
        filterBuffers[channelIndex].fill(sample); // 初始化为当前样本值，减少初始瞬态
    }

    // 移动缓冲区数据 - 使用更高效的方式
    memmove(&filterBuffers[channelIndex][1], &filterBuffers[channelIndex][0], filterOrder * sizeof(double));

    // 将新样本添加到缓冲区开头
    filterBuffers[channelIndex][0] = sample;

    // 应用FIR滤波器 - 使用向量化计算提高效率
    double result = 0.0;

    // 将卷积计算分成多个块以提高缓存命中率
    const int blockSize = 16; // 适合现代CPU缓存线的块大小

    for (int i = 0; i <= filterOrder; i += blockSize) {
        double blockSum = 0.0;
        int blockEnd = qMin(i + blockSize, filterOrder + 1);

        for (int j = i; j < blockEnd; j++) {
            blockSum += filterBuffers[channelIndex][j] * filterCoefficients[j];
        }

        result += blockSum;
    }

    return result;
}