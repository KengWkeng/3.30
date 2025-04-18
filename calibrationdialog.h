#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QTableWidget>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QVector>
#include <QPair>
#include <QString>
#include <QTimer>
#include <QFile>
#include <QSettings>
#include <QMessageBox>
#include <algorithm>
#include <QDateTime>
#include <QDebug>
#include <cmath>
#include <QTime>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMap>

// 包含QCustomPlot图表库
#include "qcustomplot.h"

// 引入数据结构定义
#include "snapshotthread.h"

// 前向声明
class SnapshotThread;

class CalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalibrationDialog(SnapshotThread *thread, QWidget *parent = nullptr);
    ~CalibrationDialog();

    // Public method to update channels when Modbus settings change
    void updateModbusChannels();

signals:
    void startCalibrationRequested();
    void stopCalibrationRequested();

private:
    // UI 元素
    QComboBox *deviceComboBox;          // 设备选择下拉框
    QComboBox *channelComboBox;         // 通道选择下拉框
    QCustomPlot *plot;                  // 图表控件
    QTableWidget *calibrationTable;     // 校准数据表格
    QLabel *currentValueLabel;          // 当前 *原始* 值标签
    QDoubleSpinBox *calibrationInput;   // 校准 *标准* 值输入框
    QPushButton *addPointButton;        // 添加校准点按钮
    QPushButton *removePointButton;     // 移除选中校准点按钮
    QPushButton *calibrateButton;       // 计算校准系数按钮
    QPushButton *closeButton;           // 关闭按钮
    QPushButton *clearButton;           // 清除所有点按钮
    QPushButton *startStopButton;       // 开始/停止校准按钮
    QPushButton *confirmButton;         // 重命名 calibrateButton
    QPushButton *initializeButton;      // 初始化校准参数按钮

    // 数据存储和处理
    QVector<QPair<double, double>> calibrationPoints; // 保存校准点 <平均原始值, 标准值>
    QVector<double> tempMeasurements;   // 临时存储原始数据点以计算平均值
    QVector<DataSnapshot> tempSnapshots; // 临时存储收集期间的快照数据，用于批量校准
    QVector<QColor> channelColors;      // 存储不同通道的颜色
    QTimer dataUpdateTimer;             // 定时器，用于定期更新UI（图表和标签）
    QTimer pointCollectionTimer;        // 5秒校准点采集定时器
    SnapshotThread *snapshotThread;     // 数据采集线程指针

    // 保存当前选择和状态
    QString currentDeviceType;          // 当前选中的设备类型 ("Modbus", "DAQ", "ECU", "Custom")
    int currentChannelIndex;            // 当前选中的通道索引 (0-based)
    bool isCollectingPoints;          // 是否正在进行5秒点采集
    double latestRawValue;             // 最新的原始测量值 (用于无数据时的回退)
    bool isCalibrationRunning;         // 校准运行状态
    bool tableSelectionConnected = false; // 表格选择信号是否已连接

    // 校准系数 (计算结果)
    double coef_a, coef_b, coef_c, coef_d; // y = ax³ + bx² + cx + d

    // 缓存最新的原始快照数据
    DataSnapshot latestRawSnapshot;
    int latestSnapshotIndex = -1;

    // 温度传感器批量校准相关
    bool isBatchCalibration;                 // 是否为批量校准模式
    QVector<int> validChannels;              // 有效通道索引列表
    QMap<int, QVector<QPair<double, double>>> channelCalibrationPoints; // 每个通道的校准点 <key=通道索引, value=校准点数组>
    QMap<int, CalibrationParams> channelCalibrationResults;        // 每个通道的校准结果
    QSet<int> excludedColumns;               // 被排除的标准值列（不用于计算）

    // 方法
    void setupUI();                     // 设置UI布局和控件
    void connectSignalsSlots();         // 连接所有信号和槽
    void updateChannelComboBox();       // 根据设备类型更新通道下拉框
    void initializePlot();              // 初始化图表设置
    void updatePlotData(double time, double value); // 向图表添加数据点
    void updateMultiChannelPlot(double time, const QVector<double>& values); // 向图表添加多通道数据点
    void updateCurrentValueLabel(double value); // 更新当前原始值标签
    void updateMultiChannelValueLabel(const QVector<double>& values); // 更新多通道当前值标签
    double getRawValueFromSnapshot(const DataSnapshot& snapshot); // 从快照获取当前通道的原始值
    QVector<double> getMultiChannelRawValues(const DataSnapshot& snapshot); // 获取多通道原始值
    void performCubicFit();             // 执行三次函数拟合 (输入: calibrationPoints)
    void performMultiChannelCubicFit(); // 为每个通道执行三次函数拟合
    bool saveCalibrationToFile();       // 保存校准系数到 calibration.ini
    bool saveMultiChannelCalibration(); // 保存多通道校准结果到配置文件
    bool initializeCalibrationParams(); // 初始化校准参数为默认值
    int getChannelCount(const QString &deviceType); // 获取指定设备类型的通道数
    QString getChannelName(const QString &deviceType, int index); // 获取通道名称（用于显示）
    void updateCalibrationTable();      // 刷新表格内容
    void updateMultiChannelCalibrationTable(); // 更新多通道校准表格
    void checkCalibrationButtonState(); // 根据校准点数量更新校准按钮状态
    void updateUiForCalibrationState(bool running); // 根据状态更新UI
    void findValidChannels();           // 查找有效的通道
    void useDefaultChannelDetection();  // 使用默认方法检测有效通道
    QString getDeviceDisplayName(const QString &internalName); // 获取设备显示名称
    QString getDeviceInternalName(const QString &displayName); // 获取设备内部名称

private slots:
    // UI事件处理槽
    void onDeviceChanged(int index);
    void onChannelChanged(int index);
    void onAddPointClicked();
    void onRemovePointClicked();
    void onClearClicked();
    void onStartStopClicked();
    void onConfirmClicked();

    // 数据处理和定时器槽
    void onRawSnapshotReceived(const DataSnapshot &rawSnapshot); // 接收原始快照数据
    void onDataUpdateTimerTimeout();  // 定时更新UI（图表，标签）
    void onPointCollectionTimerTimeout(); // 5秒点采集结束
};

#endif // CALIBRATIONDIALOG_H