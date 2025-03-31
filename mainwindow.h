#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
// #include <QSerialPort>
// #include <QSerialPortInfo>
// #include <QModbusRtuSerialClient>
// #include <QModbusDataUnit>
// #include <QModbusReply>

#include <QThread>
#include <QTimer>
#include <QDebug>
#include <QVariant>
#include <QResizeEvent>
#include <QElapsedTimer>
#include <QString>
#include <QSettings>
#include <QApplication>
#include <QStatusBar>
#include <QJsonObject>
#include <QQueue>

#include <qcustomplot.h>
#include <modbusthread.h>
#include <canthread.h>
#include <daqthread.h>
//#include <plotthread.h>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QList>
#include <QStandardItemModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>

// 引入仪表盘控件
#include "dashboard.h"

// 添加ECUThread相关头文件
#include "ecuthread.h"

// 修改为只引入WebSocketThread
#include "websocketthread.h"

// 前向声明 DashboardCalculator
class DashboardCalculator;

// 定义数据源类型枚举
enum DataSourceType {
    DataSource_Modbus = 0,
    DataSource_DAQ = 1,
    DataSource_ECU = 2,
    DataSource_Custom = 3 // 添加自定义数据源类型
};

// 定义仪表盘数据源映射结构体
struct DashboardMapping {
    DataSourceType sourceType;   // 数据源类型
    int channelIndex;           // 通道索引
    QString dashboardName;      // 仪表盘对象名称
    QString labelText;          // 标签文本
    QString unit;               // 单位
    double minValue;            // 最小值
    double maxValue;            // 最大值
    QColor pointerColor;        // 指针颜色
    int pointerStyle;           // 指针样式
    QString variableName;       // 变量名称，如 A_0, B_1, C_2, D_0 等
    QString formula;            // 自定义变量公式
};

// 定义数据快照结构体
struct DataSnapshot {
    double timestamp;                   // 时间戳（秒）
    QVector<double> modbusData;         // Modbus数据(一维) - 每个寄存器的值
    QVector<QVector<double>> daqData;   // DAQ数据(16通道)
    QVector<double> ecuData;            // ECU数据(9通道)
    bool modbusValid;                   // Modbus数据有效标志
    bool daqValid;                      // DAQ数据有效标志
    bool ecuValid;                      // ECU数据有效标志
    bool daqRunning;                    // DAQ运行状态标志
    int snapshotIndex;                  // 快照索引（序号）
    
    // 构造函数，初始化所有数据
    DataSnapshot() {
        timestamp = 0.0;                // 初始化为0秒
        modbusData.resize(16, 0.0);     // 16个Modbus寄存器
        daqData.resize(16);             // 16个DAQ通道
        for (auto &channel : daqData) {
            channel.clear();
        }
        ecuData.resize(9, 0.0);         // 9个ECU通道
        modbusValid = false;
        daqValid = false;
        ecuValid = false;
        daqRunning = false;             // 初始化DAQ运行状态为false
        snapshotIndex = 0;              // 初始化索引为0
    }
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 添加公开函数，供Dashboard类获取当前仪表盘映射
    QMap<QString, DashboardMapping>* getDashboardMappings() { return &dashboardMappings; }

    void updateECUPlot(const ECUData &data);
    void addECUDataPoint(double time, const QVector<double> &values);
    void setupECUDataTable();

protected:
    // 添加窗口尺寸调整事件处理函数
    void resizeEvent(QResizeEvent *event) override;
    
    // 添加事件过滤器，处理图表大小变化
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void on_btnScanPort_clicked();

    void on_btnOpenPort_clicked();

    void on_btnClearReceiveArea_clicked();

    void on_btnClearSendArea_clicked();

    void on_btnSend_clicked();

    void showModbusResult(QVector<double> result, qint64 readTimeInterval);

    //void getPortInfo(QVariant portName);

    void on_btnCanOpenDevice_clicked();

    void on_btnCanSend_clicked();

    void on_btnCanStart_clicked();

    void on_btnCanReset_clicked();

    void on_btnCanInit_clicked();

    // void on_btnPageModbus_clicked();

    // void on_btnPageCan_clicked();

    void on_btnPagePlot_clicked();

    void on_btnPageData_clicked();
    
    // 添加保存数据按钮槽函数
    void on_btnSaveData_clicked();
    
    // 添加读取数据按钮槽函数
    void on_btnReadData_clicked();
    
    // 导出表格数据到CSV文件
    bool exportTableToCSV(const QString &filePath);
    
    // 从CSV文件导入数据到表格和曲线
    bool importDataFromCSV(const QString &filePath);

    // void on_btnPageDaq_clicked();
    
    // 添加DAQ相关槽函数
    void on_startDAQButton_clicked();
    void on_stopDAQButton_clicked();
    void handleDAQData(const QVector<double> &timeData, const QVector<QVector<double>> &channelData);
    void handleDAQStatus(bool isRunning, QString message);
    void handleDAQError(QString errorMessage);

    // 滤波开关状态改变槽函数
    void on_filterEnabledCheckBox_stateChanged(int state);

    // void on_btnPageECU_clicked();

    // ECU相关槽函数
    void on_btnECUScan_clicked();
    void on_btnECUStart_clicked();
    void handleECUData(const ECUData &data);
    void handleECUStatus(bool connected, QString message);
    void handleECUError(QString errorMessage);

    void on_btnPageInitial_clicked();
    
    // 添加启用/禁用初始化页面控件的辅助函数
    void enableInitialPage(bool enable);

    void onRandomNumberGenerated(int number);
    
    // WebSocket相关槽函数
    void handleWebSocketServerStarted(bool success, QString message);
    void handleWebSocketClientConnected(const QString &clientInfo);
    void handleWebSocketClientDisconnected(const QString &clientInfo);
    void handleWebSocketMessage(QString message);
    void testWebSocketConnection();
    
    // 添加WebSocket控制按钮的槽函数
    void on_btnWebSocketControl_clicked();

    // WebSocket相关槽函数
    void on_btnWebSocketTest_clicked();
    
    // 主计时器超时槽函数
    void onMainTimerTimeout();
    
    // 新增菜单项槽函数
    void on_actionSetupInitial_triggered();
    void on_actionLoadInitial_triggered();
    void on_actionSaveInitial_triggered();
    
    // 新增：处理仪表盘设置变更的槽函数
    void handleDashboardSettingsChanged(const QString &dashboardName, const QMap<QString, QVariant> &settings);
    
    // 专门处理dashForce仪表盘设置变更
    void handleDashForceSettingsChanged(const QString &dashboardName, const QMap<QString, QVariant> &settings);

    // 新增：数据监控表格相关函数
    void initMonitorTable();
    void updateMonitorTable();

    // 新增：应用仪表盘设置到界面
    void applyDashboardMappings();
    
    // 新增：保存和加载仪表盘映射关系
    void saveDashboardMappings(QSettings &settings);
    void loadDashboardMappings(QSettings &settings);
    
    // 新增：初始化默认的仪表盘映射关系
    void initDefaultDashboardMappings();

    // 新增：根据DAQ通道设置更新仪表盘通道选项
    void updateDashboardDAQChannels(const QString &channelsStr);

    // 新增：根据Modbus寄存器地址和数量更新Modbus通道
    void updateModbusChannels();

    // 新增：根据映射关系更新仪表盘显示
    void updateDashboardByMapping(const QVector<double> &modbusData, 
                                 const QVector<QVector<double>> &daqData, 
                                 const ECUData &ecuData);

    // 添加从数据快照更新各种UI的函数
    void updateDashboardData(const QVector<double> &timeData, const DataSnapshot &snapshot);
    void updateECUDataDisplay(const QVector<double> &timeData, const DataSnapshot &snapshot);
    void updateModbusTable(const QVector<double> &timeData, const QVector<QVector<double>> &modbusData, int numRegs);

    // DAQ相关函数
    void setupDAQPlot();
    void updateDAQPlot();
    void updateDAQPlot(const QVector<double> &timeData, const DataSnapshot &snapshot);
    void setupDAQTable();
    void updateDAQTable(const QVector<double> &timeData, const QVector<QVector<double>> &channelData, int numChannels);
    // void setupDashboardUpdateTimer(); // 设置仪表盘更新定时器

    // 添加用于更新布局的函数
    void updateLayout();

    // ECU相关
    void ECUPlotInit();
    
    // dash1plot相关方法
    void setupDash1Plot();                 // 初始化dash1plot
    void updateDash1Plot(double value);    // 更新dash1plot数据
    
    // 新增：数据快照相关槽函数
    void processDataSnapshots();           // 处理数据快照队列的槽函数

    // 添加setupMasterTimer函数声明
    void setupMasterTimer();

private:

    Ui::MainWindow *ui;
    QThread *SubThread_Modbus;
    QThread *SubThread_Can;

    //采集线程
    modbusThread *mbTh;
    CANThread *canTh;

    // 添加Modbus数据相关变量
    bool modbusDataValid = false;      // Modbus数据有效标志
    int modbusNumRegs = 16;            // Modbus寄存器数量
    QVector<QVector<double>> modbusData;  // Modbus数据缓冲区
    
    // 添加ECU数据相关变量
    bool ecuDataValid = false;         // ECU数据有效标志
    QVector<double> ecudataMap;        // ECU数据映射

    //绘图控件指针
    QCustomPlot *myPlot;
    QVector<QCustomPlot*> myPlots;
    QScrollArea *myScrollArea;
    QVector<QCPGraph*> graphs;
    //状态栏指针
    QStatusBar *sBar;
    //曲线指针
    QCPGraph *pGraph1;

    double realPlotTime;

    void myplotInit(QCustomPlot *customplot);
    
    // 更新仪表盘数据
    void updateDashboards(const QVector<double> &data);

    void switchPage();

    int channelNum;

    // 表格模型
    QStandardItemModel *tableModel;
    // 最大列数（保留最新的数据列）
    const int maxColumns = 100;
    // 初始化表格
    void initTableView();
    // 更新表格数据
    void updateTableData(double time, const QVector<double> &data);

    // 解析CSV行（处理引号内的逗号等）
    QStringList parseCSVLine(const QString &line);
    
    // 根据表格数据重绘曲线
    void redrawPlotsFromTableData();

    // 添加DAQ相关成员
    QThread *daqThread;
    DAQThread *daqTh;
    // QTimer *daqUpdateTimer;
    // QTimer *dashboardUpdateTimer = nullptr; // 用于低频率更新仪表盘
    
    // 数据缓冲区
    QVector<QVector<double>> daqChannelData;
    QVector<double> daqTimeData;
    int daqNumChannels;
    double daqSampleRate;
    bool daqIsAcquiring;
    QStandardItemModel *daqDataModel;
    
    // 保存初始窗口大小和控件位置
    QSize initialSize;
    QMap<QWidget*, QRect> initialGeometries;
    
    // 标志是否初始化了控件位置信息
    bool geometriesInitialized;

    // 添加一阶低通滤波器相关变量
    bool filterEnabled;                  // 滤波器使能状态
    QVector<double> filteredValues;      // 存储上一次滤波后的结果
    QElapsedTimer *realTimer;            // 计时器，用于计算deltaT
    qint64 lastTimestamp;                // 上一次时间戳

    // 一阶低通滤波函数
    double applyLowPassFilter(double input, double prevOutput, double timeConstant, double deltaT);
    
    // 处理结果的时候应用滤波
    QVector<double> applyFilterToResults(const QVector<double> &rawData, qint64 deltaT);

    // ECU相关变量
    QThread *ecuThread;
    ECUThread *ecuTh;
    QStandardItemModel *ecuDataModel;
    bool ecuIsConnected;
    
    // 设置ECU表格模型
    void setupECUTable();

    // 修改WebSocket相关成员
    QThread *webSocketThread;
    WebSocketThread *wsTh;

    // 新增保存和加载初始化文件的函数
    bool saveInitialSettings(const QString &filename);
    bool loadInitialSettings(const QString &filename);
    
    // 新增：获取仪表盘对象列表的辅助函数
    QList<Dashboard*> getAllDashboards();
    
    // 新增：仪表盘与数据源映射
    QMap<QString, DashboardMapping> dashboardMappings;

    // 仪表盘计算器
    DashboardCalculator *dashboardCalculator;

    // ECU图表相关
    QVector<double> ecuTimeData; // ECU时间数据
    QVector<QVector<double>> ecuData; // ECU多通道数据，使用二维向量替代静态数组
    QVector<QCustomPlot*> ecuPlots; // 每个参数的单独图表
    QVector<QLabel*> ecuArrows;     // ECU箭头标签
    QVector<QLabel*> ecuValueLabels;     // ECU值标签
    ECUData latestECUData;                     // ECU最新数据

    // 添加dash1plot相关成员变量
    QCPGraph *dashForceGraph;              // dashForce图表对象
    QVector<double> dashForceTimeData;     // dashForce时间数据
    QVector<double> dashForceValueData;    // dashForce值数据
    QLabel *dashForceValueLabel;           // 右上角值显示标签
    QLabel *dashForceArrowLabel;           // 右侧游标标签
    double dashPlotTimeCounter;            // 时间计数器
    
    // 新增：数据快照相关成员
    DataSnapshot currentSnapshot;          // 当前数据快照
    QQueue<DataSnapshot> snapshotQueue;    // 数据快照队列
    QTimer *snapshotTimer;                 // 数据快照定时器
    QElapsedTimer *masterTimer;            // 主计时器，用于同步数据
    int maxQueueSize = 1000;               // 最大队列长度，防止内存占用过多

    // 添加统一定时器相关变量
    QTimer *mainTimer;          // 主定时器
    bool modbusReadRequested;   // 是否请求了Modbus读取
    bool modbusReading;         // 是否正在读取Modbus数据
    qint64 lastModbusReadTime;  // 上次Modbus读取时间
    qint64 lastSnapshotTime;    // 上次快照时间
    qint64 lastPlotUpdateTime;  // 上次绘图更新时间

signals:

    void sendModbusCommand(int serverAddress, int startAddress, int length);
    
    // 添加新的信号用于发送串口参数
    void sendModbusInfo(QString portName, int baudRateIndex, int stopBitsIndex, int dataBitsIndex, int parityIndex);
    
    // 添加新的信号用于关闭modbus连接
    void closeModbusConnection();

    // 添加重置ModBus计时器的信号
    void resetModbusTimer();

    // ECU相关信号
    void openECUPort(const QString &portName);
    void closeECUPort();

    // 添加Modbus数据转发的信号
    void sendModbusResultToWebSocket(const QJsonObject &data, int interval);

};


#endif // MAINWINDOW_H
