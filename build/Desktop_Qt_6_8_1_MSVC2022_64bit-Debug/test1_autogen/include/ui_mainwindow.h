/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "dashboard.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *mainLayout;
    QLabel *labelTitle;
    QHBoxLayout *contentLayout;
    QVBoxLayout *leftLayout;
    QLabel *labelLogo;
    QVBoxLayout *verticalLayout_5;
    QPushButton *btnPageInitial;
    QPushButton *btnPagePlot;
    QPushButton *btnPageData;
    QPushButton *btnStartAll;
    QPushButton *btnSaveData;
    QPushButton *btnReadData;
    QSpacerItem *verticalSpacer;
    QWidget *widgetDashBoard;
    QVBoxLayout *monitorLayout;
    QGroupBox *groupBox_10;
    QVBoxLayout *monitorPanelLayout;
    QCustomPlot *dash1plot;
    QGroupBox *groupBox_11;
    QGridLayout *dashboardLayout;
    QVBoxLayout *verticalLayout_Force;
    Dashboard *dashForce;
    QHBoxLayout *horizontalLayout_Force;
    QLabel *labelForce;
    QLCDNumber *lcdForce;
    QVBoxLayout *verticalLayout_Torque;
    Dashboard *dashTorque;
    QHBoxLayout *horizontalLayout_Torque;
    QLabel *labelTorque;
    QLCDNumber *lcdTorque;
    QVBoxLayout *verticalLayout_RPM;
    Dashboard *dashRPM;
    QHBoxLayout *horizontalLayout_RPM;
    QLabel *labelRPM;
    QLCDNumber *lcdRPM;
    QVBoxLayout *verticalLayout_Thrust;
    Dashboard *dashThrust;
    QHBoxLayout *horizontalLayout_Thrust;
    QLabel *labelThrust;
    QLCDNumber *lcdThrust;
    QVBoxLayout *verticalLayout_FuelConsumption;
    Dashboard *dashFuelConsumption;
    QHBoxLayout *horizontalLayout_FuelConsumption;
    QLabel *labelFuelConsumption;
    QLCDNumber *lcdFuelConsumption;
    QVBoxLayout *verticalLayout_SparkPlugTemp;
    Dashboard *dashSparkPlugTemp;
    QHBoxLayout *horizontalLayout_SparkPlugTemp;
    QLabel *labelSparkPlugTemp;
    QLCDNumber *lcdSparkPlugTemp;
    QVBoxLayout *verticalLayout_Power;
    Dashboard *dashPower;
    QHBoxLayout *horizontalLayout_Power;
    QLabel *labelPower;
    QLCDNumber *lcdPower;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QVBoxLayout *pageLayout;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *labelPort;
    QComboBox *comboPort;
    QLabel *labelBaudRate;
    QComboBox *comboBaudRate;
    QLabel *labelStopBits;
    QComboBox *comboStopBits;
    QLabel *labelDataBits;
    QComboBox *comboDataBits;
    QLabel *labelParity;
    QComboBox *comboParity;
    QRadioButton *radioButton;
    QVBoxLayout *verticalLayout_2;
    QLabel *labelPort_3;
    QLineEdit *lineServerAddress;
    QLabel *labelPort_2;
    QComboBox *comboAction;
    QLabel *labelPort_4;
    QLineEdit *lineSegAddress;
    QLabel *labelPort_5;
    QLineEdit *lineSegNum;
    QLabel *labelStopBits_2;
    QLineEdit *lineTimeLoop;
    QCheckBox *filterEnabledCheckBox;
    QPushButton *btnScanPort;
    QPushButton *btnClearSendArea;
    QPushButton *btnOpenPort;
    QPushButton *btnSend;
    QLabel *labelReceive;
    QPlainTextEdit *plainReceive;
    QPushButton *btnClearReceiveArea;
    QGroupBox *groupBoxCAN;
    QGridLayout *gridLayout;
    QLabel *label_3;
    QComboBox *comboBox;
    QLabel *label_4;
    QComboBox *comboCanID;
    QLabel *label_11;
    QComboBox *comboCanBaud;
    QPushButton *btnCanOpenDevice;
    QPushButton *btnCanReset;
    QPushButton *btnCanInit;
    QPushButton *btnCanStart;
    QGroupBox *groupBox_CAN2;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QLabel *label_14;
    QLineEdit *sendIDEdit;
    QComboBox *comboChannel;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *comboFrameType;
    QComboBox *comboDataFrameType;
    QPushButton *btnCanSend;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_15;
    QLineEdit *sendDataEdit;
    QGroupBox *groupBoxECU;
    QHBoxLayout *ecuLayout;
    QLabel *label_17;
    QComboBox *comboSerialECU;
    QPushButton *btnECUScan;
    QPushButton *btnECUStart;
    QHBoxLayout *horizontalLayout_4;
    QGroupBox *groupBoxDAQ;
    QHBoxLayout *horizontalLayout_daq;
    QHBoxLayout *horizontalLayout_daq2;
    QLabel *labelDeviceName;
    QLineEdit *deviceNameEdit;
    QHBoxLayout *horizontalLayout_daq3;
    QLabel *labelChannels;
    QLineEdit *channelsEdit;
    QHBoxLayout *horizontalLayout_daq4;
    QLabel *labelSampleRate;
    QLineEdit *sampleRateEdit;
    QHBoxLayout *horizontalLayout_daq5;
    QPushButton *startDAQButton;
    QPushButton *stopDAQButton;
    QWidget *page_2;
    QVBoxLayout *pageLayout_2;
    QLabel *labelVisualizer;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_6;
    QCustomPlot *modbusCustomPlot;
    QGroupBox *groupBox_6;
    QVBoxLayout *verticalLayout_7;
    QCustomPlot *daqCustomPlot;
    QGroupBox *groupBoxECUplot;
    QVBoxLayout *verticalLayout_8;
    QCustomPlot *ECUCustomPlot;
    QGroupBox *groupBoxCustomPlot;
    QVBoxLayout *verticalLayout_11;
    QCustomPlot *customVarCustomPlot;
    QWidget *page_4;
    QVBoxLayout *pageLayout_4;
    QLabel *label_16;
    QGroupBox *groupBox_7;
    QVBoxLayout *verticalLayout_81;
    QTableView *tableView;
    QGroupBox *groupBox_8;
    QVBoxLayout *verticalLayout_9;
    QTableView *daqDataTableView;
    QGroupBox *groupBox_9;
    QVBoxLayout *verticalLayout_10;
    QTableView *tableViewECU;
    QGroupBox *groupBoxCustomData;
    QVBoxLayout *verticalLayout_12;
    QTableView *customVarTableView;
    QHBoxLayout *bottomLayout;
    QLabel *labelWebSocketStatus;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnTestWebSocket;
    QPushButton *btnWebSocketControl;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(2560, 1440);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setStyleSheet(QString::fromUtf8("QMainWindow {\n"
"    background-color: #f0f0f0;\n"
"}\n"
"QGroupBox {\n"
"    border: 1px solid #cccccc;\n"
"    border-radius: 5px;\n"
"    margin-top: 10px;\n"
"    font-weight: bold;\n"
"    background-color: #ffffff;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px 0 5px;\n"
"}\n"
"QPushButton {\n"
"    background-color: #2c7ec9;\n"
"    color: white;\n"
"    border-radius: 4px;\n"
"    padding: 5px;\n"
"    min-height: 25px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #3d8dd8;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #1c6eb9;\n"
"}\n"
"QPushButton:disabled {\n"
"    background-color: #a0a0a0;\n"
"    color: #e0e0e0;\n"
"    border: 1px solid #909090;\n"
"}\n"
"QLineEdit, QComboBox, QPlainTextEdit {\n"
"    border: 1px solid #cccccc;\n"
"    border-radius: 3px;\n"
"    padding: 2px;\n"
"    background-color: white;\n"
"}\n"
"QLineEdit:disabled, QComboBox:disabled, QPlainTextEdit:disabled {\n"
"    background-color: #f0f"
                        "0f0;\n"
"    color: #a0a0a0;\n"
"    border: 1px solid #d0d0d0;\n"
"}\n"
"QStackedWidget {\n"
"    background-color: #f5f5f5;\n"
"    border-radius: 5px;\n"
"}\n"
"#btnPageModbus, #btnPageCan, #btnPagePlot, #btnPageData, #btnPageDaq, #btnPageInitial {\n"
"    background-color: #2c7ec9;\n"
"    color: white;\n"
"    border-radius: 6px;\n"
"    font-weight: bold;\n"
"}\n"
"#btnPageModbus:hover, #btnPageCan:hover, #btnPagePlot:hover, #btnPageData:hover, #btnPageDaq:hover, #btnPageInitial:hover {\n"
"    background-color: #3d8dd8;\n"
"}\n"
"#btnPageModbus:pressed, #btnPageCan:pressed, #btnPagePlot:pressed, #btnPageData:pressed, #btnPageDaq:pressed, #btnPageInitial:pressed {\n"
"    background-color: #1c6eb9;\n"
"}\n"
"#btnPageModbus:checked, #btnPageCan:checked, #btnPagePlot:checked, #btnPageData:checked, #btnPageDaq:checked, #btnPageInitial:checked {\n"
"    background-color: #166090;\n"
"    border: 2px solid #0a4060;\n"
"}\n"
"#btnPageModbus:disabled, #btnPageCan:disabled, #btnPagePlot:disabled, #btnPageData:di"
                        "sabled, #btnPageDaq:disabled, #btnPageInitial:disabled {\n"
"    background-color: #a0a0a0;\n"
"    color: #e0e0e0;\n"
"    border: 1px solid #909090;\n"
"}\n"
"#btnSaveData {\n"
"    background-color: #2d904c;\n"
"}\n"
"#btnSaveData:hover {\n"
"    background-color: #34a559;\n"
"}\n"
"#btnSaveData:pressed {\n"
"    background-color: #1f7a3d;\n"
"}\n"
"#btnReadData {\n"
"    background-color: #d67821;\n"
"}\n"
"#btnReadData:hover {\n"
"    background-color: #e58632;\n"
"}\n"
"#btnReadData:pressed {\n"
"    background-color: #bc6a1d;\n"
"}\n"
""));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        mainLayout = new QVBoxLayout(centralwidget);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        mainLayout->setContentsMargins(9, 9, 9, 9);
        labelTitle = new QLabel(centralwidget);
        labelTitle->setObjectName("labelTitle");
        labelTitle->setStyleSheet(QString::fromUtf8("font-size: 16pt; font-weight: bold; color: #2c7ec9;"));
        labelTitle->setAlignment(Qt::AlignmentFlag::AlignCenter);

        mainLayout->addWidget(labelTitle);

        contentLayout = new QHBoxLayout();
        contentLayout->setSpacing(6);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setSizeConstraint(QLayout::SizeConstraint::SetMinimumSize);
        leftLayout = new QVBoxLayout();
        leftLayout->setSpacing(10);
        leftLayout->setObjectName("leftLayout");
        labelLogo = new QLabel(centralwidget);
        labelLogo->setObjectName("labelLogo");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(labelLogo->sizePolicy().hasHeightForWidth());
        labelLogo->setSizePolicy(sizePolicy1);
        labelLogo->setMinimumSize(QSize(100, 80));
        labelLogo->setMaximumSize(QSize(171, 121));
        labelLogo->setPixmap(QPixmap(QString::fromUtf8(":/new/prefix1/logo.png")));
        labelLogo->setScaledContents(true);

        leftLayout->addWidget(labelLogo);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(10);
        verticalLayout_5->setObjectName("verticalLayout_5");
        btnPageInitial = new QPushButton(centralwidget);
        btnPageInitial->setObjectName("btnPageInitial");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(btnPageInitial->sizePolicy().hasHeightForWidth());
        btnPageInitial->setSizePolicy(sizePolicy2);
        btnPageInitial->setMinimumSize(QSize(120, 35));
        QFont font;
        font.setBold(true);
        btnPageInitial->setFont(font);
        btnPageInitial->setCheckable(true);

        verticalLayout_5->addWidget(btnPageInitial);

        btnPagePlot = new QPushButton(centralwidget);
        btnPagePlot->setObjectName("btnPagePlot");
        sizePolicy2.setHeightForWidth(btnPagePlot->sizePolicy().hasHeightForWidth());
        btnPagePlot->setSizePolicy(sizePolicy2);
        btnPagePlot->setMinimumSize(QSize(120, 35));
        btnPagePlot->setCheckable(true);

        verticalLayout_5->addWidget(btnPagePlot);

        btnPageData = new QPushButton(centralwidget);
        btnPageData->setObjectName("btnPageData");
        sizePolicy2.setHeightForWidth(btnPageData->sizePolicy().hasHeightForWidth());
        btnPageData->setSizePolicy(sizePolicy2);
        btnPageData->setMinimumSize(QSize(120, 35));
        btnPageData->setCheckable(true);

        verticalLayout_5->addWidget(btnPageData);

        btnStartAll = new QPushButton(centralwidget);
        btnStartAll->setObjectName("btnStartAll");
        sizePolicy2.setHeightForWidth(btnStartAll->sizePolicy().hasHeightForWidth());
        btnStartAll->setSizePolicy(sizePolicy2);
        btnStartAll->setMinimumSize(QSize(120, 35));
        btnStartAll->setFont(font);
        btnStartAll->setCheckable(true);

        verticalLayout_5->addWidget(btnStartAll);

        btnSaveData = new QPushButton(centralwidget);
        btnSaveData->setObjectName("btnSaveData");
        sizePolicy2.setHeightForWidth(btnSaveData->sizePolicy().hasHeightForWidth());
        btnSaveData->setSizePolicy(sizePolicy2);
        btnSaveData->setMinimumSize(QSize(120, 35));
        btnSaveData->setFont(font);

        verticalLayout_5->addWidget(btnSaveData);

        btnReadData = new QPushButton(centralwidget);
        btnReadData->setObjectName("btnReadData");
        sizePolicy2.setHeightForWidth(btnReadData->sizePolicy().hasHeightForWidth());
        btnReadData->setSizePolicy(sizePolicy2);
        btnReadData->setMinimumSize(QSize(120, 35));
        btnReadData->setFont(font);

        verticalLayout_5->addWidget(btnReadData);


        leftLayout->addLayout(verticalLayout_5);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        leftLayout->addItem(verticalSpacer);


        contentLayout->addLayout(leftLayout);

        widgetDashBoard = new QWidget(centralwidget);
        widgetDashBoard->setObjectName("widgetDashBoard");
        QSizePolicy sizePolicy3(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy3.setHorizontalStretch(10);
        sizePolicy3.setVerticalStretch(1);
        sizePolicy3.setHeightForWidth(widgetDashBoard->sizePolicy().hasHeightForWidth());
        widgetDashBoard->setSizePolicy(sizePolicy3);
        widgetDashBoard->setMinimumSize(QSize(500, 400));
        monitorLayout = new QVBoxLayout(widgetDashBoard);
        monitorLayout->setObjectName("monitorLayout");
        groupBox_10 = new QGroupBox(widgetDashBoard);
        groupBox_10->setObjectName("groupBox_10");
        sizePolicy.setHeightForWidth(groupBox_10->sizePolicy().hasHeightForWidth());
        groupBox_10->setSizePolicy(sizePolicy);
        groupBox_10->setMinimumSize(QSize(300, 150));
        monitorPanelLayout = new QVBoxLayout(groupBox_10);
        monitorPanelLayout->setSpacing(4);
        monitorPanelLayout->setObjectName("monitorPanelLayout");
        monitorPanelLayout->setContentsMargins(4, 4, 4, 4);
        dash1plot = new QCustomPlot(groupBox_10);
        dash1plot->setObjectName("dash1plot");

        monitorPanelLayout->addWidget(dash1plot);


        monitorLayout->addWidget(groupBox_10);

        groupBox_11 = new QGroupBox(widgetDashBoard);
        groupBox_11->setObjectName("groupBox_11");
        sizePolicy.setHeightForWidth(groupBox_11->sizePolicy().hasHeightForWidth());
        groupBox_11->setSizePolicy(sizePolicy);
        groupBox_11->setMinimumSize(QSize(300, 400));
        dashboardLayout = new QGridLayout(groupBox_11);
        dashboardLayout->setSpacing(6);
        dashboardLayout->setObjectName("dashboardLayout");
        dashboardLayout->setContentsMargins(4, 4, 4, 4);
        verticalLayout_Force = new QVBoxLayout();
        verticalLayout_Force->setObjectName("verticalLayout_Force");
        dashForce = new Dashboard(groupBox_11);
        dashForce->setObjectName("dashForce");
        QSizePolicy sizePolicy4(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(1);
        sizePolicy4.setHeightForWidth(dashForce->sizePolicy().hasHeightForWidth());
        dashForce->setSizePolicy(sizePolicy4);
        dashForce->setMinimumSize(QSize(120, 120));

        verticalLayout_Force->addWidget(dashForce);

        horizontalLayout_Force = new QHBoxLayout();
        horizontalLayout_Force->setObjectName("horizontalLayout_Force");
        labelForce = new QLabel(groupBox_11);
        labelForce->setObjectName("labelForce");
        QSizePolicy sizePolicy5(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy5.setHorizontalStretch(1);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(labelForce->sizePolicy().hasHeightForWidth());
        labelForce->setSizePolicy(sizePolicy5);
        labelForce->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_Force->addWidget(labelForce);

        lcdForce = new QLCDNumber(groupBox_11);
        lcdForce->setObjectName("lcdForce");
        QSizePolicy sizePolicy6(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);
        sizePolicy6.setHorizontalStretch(2);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(lcdForce->sizePolicy().hasHeightForWidth());
        lcdForce->setSizePolicy(sizePolicy6);
        lcdForce->setMinimumSize(QSize(60, 20));

        horizontalLayout_Force->addWidget(lcdForce);


        verticalLayout_Force->addLayout(horizontalLayout_Force);


        dashboardLayout->addLayout(verticalLayout_Force, 0, 0, 1, 1);

        verticalLayout_Torque = new QVBoxLayout();
        verticalLayout_Torque->setObjectName("verticalLayout_Torque");
        dashTorque = new Dashboard(groupBox_11);
        dashTorque->setObjectName("dashTorque");
        sizePolicy4.setHeightForWidth(dashTorque->sizePolicy().hasHeightForWidth());
        dashTorque->setSizePolicy(sizePolicy4);
        dashTorque->setMinimumSize(QSize(120, 120));

        verticalLayout_Torque->addWidget(dashTorque);

        horizontalLayout_Torque = new QHBoxLayout();
        horizontalLayout_Torque->setObjectName("horizontalLayout_Torque");
        labelTorque = new QLabel(groupBox_11);
        labelTorque->setObjectName("labelTorque");
        sizePolicy5.setHeightForWidth(labelTorque->sizePolicy().hasHeightForWidth());
        labelTorque->setSizePolicy(sizePolicy5);
        labelTorque->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_Torque->addWidget(labelTorque);

        lcdTorque = new QLCDNumber(groupBox_11);
        lcdTorque->setObjectName("lcdTorque");
        sizePolicy6.setHeightForWidth(lcdTorque->sizePolicy().hasHeightForWidth());
        lcdTorque->setSizePolicy(sizePolicy6);
        lcdTorque->setMinimumSize(QSize(60, 20));

        horizontalLayout_Torque->addWidget(lcdTorque);


        verticalLayout_Torque->addLayout(horizontalLayout_Torque);


        dashboardLayout->addLayout(verticalLayout_Torque, 0, 1, 1, 1);

        verticalLayout_RPM = new QVBoxLayout();
        verticalLayout_RPM->setObjectName("verticalLayout_RPM");
        dashRPM = new Dashboard(groupBox_11);
        dashRPM->setObjectName("dashRPM");
        sizePolicy4.setHeightForWidth(dashRPM->sizePolicy().hasHeightForWidth());
        dashRPM->setSizePolicy(sizePolicy4);
        dashRPM->setMinimumSize(QSize(120, 120));

        verticalLayout_RPM->addWidget(dashRPM);

        horizontalLayout_RPM = new QHBoxLayout();
        horizontalLayout_RPM->setObjectName("horizontalLayout_RPM");
        labelRPM = new QLabel(groupBox_11);
        labelRPM->setObjectName("labelRPM");
        sizePolicy5.setHeightForWidth(labelRPM->sizePolicy().hasHeightForWidth());
        labelRPM->setSizePolicy(sizePolicy5);
        labelRPM->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_RPM->addWidget(labelRPM);

        lcdRPM = new QLCDNumber(groupBox_11);
        lcdRPM->setObjectName("lcdRPM");
        sizePolicy6.setHeightForWidth(lcdRPM->sizePolicy().hasHeightForWidth());
        lcdRPM->setSizePolicy(sizePolicy6);
        lcdRPM->setMinimumSize(QSize(60, 20));

        horizontalLayout_RPM->addWidget(lcdRPM);


        verticalLayout_RPM->addLayout(horizontalLayout_RPM);


        dashboardLayout->addLayout(verticalLayout_RPM, 0, 2, 1, 1);

        verticalLayout_Thrust = new QVBoxLayout();
        verticalLayout_Thrust->setObjectName("verticalLayout_Thrust");
        dashThrust = new Dashboard(groupBox_11);
        dashThrust->setObjectName("dashThrust");
        sizePolicy4.setHeightForWidth(dashThrust->sizePolicy().hasHeightForWidth());
        dashThrust->setSizePolicy(sizePolicy4);
        dashThrust->setMinimumSize(QSize(120, 120));

        verticalLayout_Thrust->addWidget(dashThrust);

        horizontalLayout_Thrust = new QHBoxLayout();
        horizontalLayout_Thrust->setObjectName("horizontalLayout_Thrust");
        labelThrust = new QLabel(groupBox_11);
        labelThrust->setObjectName("labelThrust");
        sizePolicy5.setHeightForWidth(labelThrust->sizePolicy().hasHeightForWidth());
        labelThrust->setSizePolicy(sizePolicy5);
        labelThrust->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_Thrust->addWidget(labelThrust);

        lcdThrust = new QLCDNumber(groupBox_11);
        lcdThrust->setObjectName("lcdThrust");
        sizePolicy6.setHeightForWidth(lcdThrust->sizePolicy().hasHeightForWidth());
        lcdThrust->setSizePolicy(sizePolicy6);
        lcdThrust->setMinimumSize(QSize(60, 20));

        horizontalLayout_Thrust->addWidget(lcdThrust);


        verticalLayout_Thrust->addLayout(horizontalLayout_Thrust);


        dashboardLayout->addLayout(verticalLayout_Thrust, 0, 3, 1, 1);

        verticalLayout_FuelConsumption = new QVBoxLayout();
        verticalLayout_FuelConsumption->setObjectName("verticalLayout_FuelConsumption");
        dashFuelConsumption = new Dashboard(groupBox_11);
        dashFuelConsumption->setObjectName("dashFuelConsumption");
        sizePolicy4.setHeightForWidth(dashFuelConsumption->sizePolicy().hasHeightForWidth());
        dashFuelConsumption->setSizePolicy(sizePolicy4);
        dashFuelConsumption->setMinimumSize(QSize(120, 120));

        verticalLayout_FuelConsumption->addWidget(dashFuelConsumption);

        horizontalLayout_FuelConsumption = new QHBoxLayout();
        horizontalLayout_FuelConsumption->setObjectName("horizontalLayout_FuelConsumption");
        labelFuelConsumption = new QLabel(groupBox_11);
        labelFuelConsumption->setObjectName("labelFuelConsumption");
        sizePolicy5.setHeightForWidth(labelFuelConsumption->sizePolicy().hasHeightForWidth());
        labelFuelConsumption->setSizePolicy(sizePolicy5);
        labelFuelConsumption->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_FuelConsumption->addWidget(labelFuelConsumption);

        lcdFuelConsumption = new QLCDNumber(groupBox_11);
        lcdFuelConsumption->setObjectName("lcdFuelConsumption");
        sizePolicy6.setHeightForWidth(lcdFuelConsumption->sizePolicy().hasHeightForWidth());
        lcdFuelConsumption->setSizePolicy(sizePolicy6);
        lcdFuelConsumption->setMinimumSize(QSize(60, 20));

        horizontalLayout_FuelConsumption->addWidget(lcdFuelConsumption);


        verticalLayout_FuelConsumption->addLayout(horizontalLayout_FuelConsumption);


        dashboardLayout->addLayout(verticalLayout_FuelConsumption, 1, 0, 1, 1);

        verticalLayout_SparkPlugTemp = new QVBoxLayout();
        verticalLayout_SparkPlugTemp->setObjectName("verticalLayout_SparkPlugTemp");
        dashSparkPlugTemp = new Dashboard(groupBox_11);
        dashSparkPlugTemp->setObjectName("dashSparkPlugTemp");
        sizePolicy4.setHeightForWidth(dashSparkPlugTemp->sizePolicy().hasHeightForWidth());
        dashSparkPlugTemp->setSizePolicy(sizePolicy4);
        dashSparkPlugTemp->setMinimumSize(QSize(120, 120));

        verticalLayout_SparkPlugTemp->addWidget(dashSparkPlugTemp);

        horizontalLayout_SparkPlugTemp = new QHBoxLayout();
        horizontalLayout_SparkPlugTemp->setObjectName("horizontalLayout_SparkPlugTemp");
        labelSparkPlugTemp = new QLabel(groupBox_11);
        labelSparkPlugTemp->setObjectName("labelSparkPlugTemp");
        sizePolicy5.setHeightForWidth(labelSparkPlugTemp->sizePolicy().hasHeightForWidth());
        labelSparkPlugTemp->setSizePolicy(sizePolicy5);
        labelSparkPlugTemp->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_SparkPlugTemp->addWidget(labelSparkPlugTemp);

        lcdSparkPlugTemp = new QLCDNumber(groupBox_11);
        lcdSparkPlugTemp->setObjectName("lcdSparkPlugTemp");
        sizePolicy6.setHeightForWidth(lcdSparkPlugTemp->sizePolicy().hasHeightForWidth());
        lcdSparkPlugTemp->setSizePolicy(sizePolicy6);
        lcdSparkPlugTemp->setMinimumSize(QSize(60, 20));

        horizontalLayout_SparkPlugTemp->addWidget(lcdSparkPlugTemp);


        verticalLayout_SparkPlugTemp->addLayout(horizontalLayout_SparkPlugTemp);


        dashboardLayout->addLayout(verticalLayout_SparkPlugTemp, 1, 1, 1, 1);

        verticalLayout_Power = new QVBoxLayout();
        verticalLayout_Power->setObjectName("verticalLayout_Power");
        dashPower = new Dashboard(groupBox_11);
        dashPower->setObjectName("dashPower");
        sizePolicy4.setHeightForWidth(dashPower->sizePolicy().hasHeightForWidth());
        dashPower->setSizePolicy(sizePolicy4);
        dashPower->setMinimumSize(QSize(120, 120));

        verticalLayout_Power->addWidget(dashPower);

        horizontalLayout_Power = new QHBoxLayout();
        horizontalLayout_Power->setObjectName("horizontalLayout_Power");
        labelPower = new QLabel(groupBox_11);
        labelPower->setObjectName("labelPower");
        sizePolicy5.setHeightForWidth(labelPower->sizePolicy().hasHeightForWidth());
        labelPower->setSizePolicy(sizePolicy5);
        labelPower->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_Power->addWidget(labelPower);

        lcdPower = new QLCDNumber(groupBox_11);
        lcdPower->setObjectName("lcdPower");
        sizePolicy6.setHeightForWidth(lcdPower->sizePolicy().hasHeightForWidth());
        lcdPower->setSizePolicy(sizePolicy6);
        lcdPower->setMinimumSize(QSize(60, 20));

        horizontalLayout_Power->addWidget(lcdPower);


        verticalLayout_Power->addLayout(horizontalLayout_Power);


        dashboardLayout->addLayout(verticalLayout_Power, 1, 2, 1, 1);


        monitorLayout->addWidget(groupBox_11);


        contentLayout->addWidget(widgetDashBoard);

        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        sizePolicy3.setHeightForWidth(stackedWidget->sizePolicy().hasHeightForWidth());
        stackedWidget->setSizePolicy(sizePolicy3);
        stackedWidget->setMinimumSize(QSize(600, 400));
        page = new QWidget();
        page->setObjectName("page");
        sizePolicy.setHeightForWidth(page->sizePolicy().hasHeightForWidth());
        page->setSizePolicy(sizePolicy);
        pageLayout = new QVBoxLayout(page);
        pageLayout->setObjectName("pageLayout");
        groupBox_5 = new QGroupBox(page);
        groupBox_5->setObjectName("groupBox_5");
        sizePolicy.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy);
        gridLayout_2 = new QGridLayout(groupBox_5);
        gridLayout_2->setObjectName("gridLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        labelPort = new QLabel(groupBox_5);
        labelPort->setObjectName("labelPort");

        verticalLayout->addWidget(labelPort);

        comboPort = new QComboBox(groupBox_5);
        comboPort->setObjectName("comboPort");
        comboPort->setMinimumSize(QSize(0, 22));

        verticalLayout->addWidget(comboPort);

        labelBaudRate = new QLabel(groupBox_5);
        labelBaudRate->setObjectName("labelBaudRate");

        verticalLayout->addWidget(labelBaudRate);

        comboBaudRate = new QComboBox(groupBox_5);
        comboBaudRate->addItem(QString());
        comboBaudRate->addItem(QString());
        comboBaudRate->addItem(QString());
        comboBaudRate->addItem(QString());
        comboBaudRate->setObjectName("comboBaudRate");
        comboBaudRate->setMinimumSize(QSize(0, 22));

        verticalLayout->addWidget(comboBaudRate);

        labelStopBits = new QLabel(groupBox_5);
        labelStopBits->setObjectName("labelStopBits");

        verticalLayout->addWidget(labelStopBits);

        comboStopBits = new QComboBox(groupBox_5);
        comboStopBits->addItem(QString());
        comboStopBits->addItem(QString());
        comboStopBits->addItem(QString());
        comboStopBits->setObjectName("comboStopBits");
        comboStopBits->setMinimumSize(QSize(0, 22));

        verticalLayout->addWidget(comboStopBits);

        labelDataBits = new QLabel(groupBox_5);
        labelDataBits->setObjectName("labelDataBits");

        verticalLayout->addWidget(labelDataBits);

        comboDataBits = new QComboBox(groupBox_5);
        comboDataBits->addItem(QString());
        comboDataBits->addItem(QString());
        comboDataBits->addItem(QString());
        comboDataBits->addItem(QString());
        comboDataBits->setObjectName("comboDataBits");
        comboDataBits->setMinimumSize(QSize(0, 22));

        verticalLayout->addWidget(comboDataBits);

        labelParity = new QLabel(groupBox_5);
        labelParity->setObjectName("labelParity");

        verticalLayout->addWidget(labelParity);

        comboParity = new QComboBox(groupBox_5);
        comboParity->addItem(QString());
        comboParity->addItem(QString());
        comboParity->addItem(QString());
        comboParity->setObjectName("comboParity");
        comboParity->setMinimumSize(QSize(0, 22));

        verticalLayout->addWidget(comboParity);

        radioButton = new QRadioButton(groupBox_5);
        radioButton->setObjectName("radioButton");
        radioButton->setEnabled(true);

        verticalLayout->addWidget(radioButton);


        gridLayout_2->addLayout(verticalLayout, 0, 0, 2, 2);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        labelPort_3 = new QLabel(groupBox_5);
        labelPort_3->setObjectName("labelPort_3");

        verticalLayout_2->addWidget(labelPort_3);

        lineServerAddress = new QLineEdit(groupBox_5);
        lineServerAddress->setObjectName("lineServerAddress");

        verticalLayout_2->addWidget(lineServerAddress);

        labelPort_2 = new QLabel(groupBox_5);
        labelPort_2->setObjectName("labelPort_2");

        verticalLayout_2->addWidget(labelPort_2);

        comboAction = new QComboBox(groupBox_5);
        comboAction->addItem(QString());
        comboAction->addItem(QString());
        comboAction->addItem(QString());
        comboAction->addItem(QString());
        comboAction->addItem(QString());
        comboAction->addItem(QString());
        comboAction->setObjectName("comboAction");

        verticalLayout_2->addWidget(comboAction);

        labelPort_4 = new QLabel(groupBox_5);
        labelPort_4->setObjectName("labelPort_4");

        verticalLayout_2->addWidget(labelPort_4);

        lineSegAddress = new QLineEdit(groupBox_5);
        lineSegAddress->setObjectName("lineSegAddress");

        verticalLayout_2->addWidget(lineSegAddress);

        labelPort_5 = new QLabel(groupBox_5);
        labelPort_5->setObjectName("labelPort_5");

        verticalLayout_2->addWidget(labelPort_5);

        lineSegNum = new QLineEdit(groupBox_5);
        lineSegNum->setObjectName("lineSegNum");

        verticalLayout_2->addWidget(lineSegNum);

        labelStopBits_2 = new QLabel(groupBox_5);
        labelStopBits_2->setObjectName("labelStopBits_2");

        verticalLayout_2->addWidget(labelStopBits_2);

        lineTimeLoop = new QLineEdit(groupBox_5);
        lineTimeLoop->setObjectName("lineTimeLoop");

        verticalLayout_2->addWidget(lineTimeLoop);

        filterEnabledCheckBox = new QCheckBox(groupBox_5);
        filterEnabledCheckBox->setObjectName("filterEnabledCheckBox");

        verticalLayout_2->addWidget(filterEnabledCheckBox);


        gridLayout_2->addLayout(verticalLayout_2, 0, 2, 3, 2);

        btnScanPort = new QPushButton(groupBox_5);
        btnScanPort->setObjectName("btnScanPort");

        gridLayout_2->addWidget(btnScanPort, 3, 0, 1, 2);

        btnClearSendArea = new QPushButton(groupBox_5);
        btnClearSendArea->setObjectName("btnClearSendArea");

        gridLayout_2->addWidget(btnClearSendArea, 3, 2, 1, 2);

        btnOpenPort = new QPushButton(groupBox_5);
        btnOpenPort->setObjectName("btnOpenPort");

        gridLayout_2->addWidget(btnOpenPort, 4, 0, 1, 2);

        btnSend = new QPushButton(groupBox_5);
        btnSend->setObjectName("btnSend");

        gridLayout_2->addWidget(btnSend, 4, 2, 1, 2);

        labelReceive = new QLabel(groupBox_5);
        labelReceive->setObjectName("labelReceive");

        gridLayout_2->addWidget(labelReceive, 5, 0, 1, 1);

        plainReceive = new QPlainTextEdit(groupBox_5);
        plainReceive->setObjectName("plainReceive");

        gridLayout_2->addWidget(plainReceive, 6, 0, 1, 4);

        btnClearReceiveArea = new QPushButton(groupBox_5);
        btnClearReceiveArea->setObjectName("btnClearReceiveArea");

        gridLayout_2->addWidget(btnClearReceiveArea, 7, 0, 1, 2);


        pageLayout->addWidget(groupBox_5);

        groupBoxCAN = new QGroupBox(page);
        groupBoxCAN->setObjectName("groupBoxCAN");
        sizePolicy.setHeightForWidth(groupBoxCAN->sizePolicy().hasHeightForWidth());
        groupBoxCAN->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(groupBoxCAN);
        gridLayout->setObjectName("gridLayout");
        label_3 = new QLabel(groupBoxCAN);
        label_3->setObjectName("label_3");
        label_3->setEnabled(false);
        label_3->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_3, 0, 0, 1, 1);

        comboBox = new QComboBox(groupBoxCAN);
        comboBox->addItem(QString());
        comboBox->setObjectName("comboBox");
        comboBox->setEnabled(false);

        gridLayout->addWidget(comboBox, 0, 1, 1, 1);

        label_4 = new QLabel(groupBoxCAN);
        label_4->setObjectName("label_4");
        label_4->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_4, 1, 0, 1, 1);

        comboCanID = new QComboBox(groupBoxCAN);
        comboCanID->addItem(QString());
        comboCanID->setObjectName("comboCanID");

        gridLayout->addWidget(comboCanID, 1, 1, 1, 1);

        label_11 = new QLabel(groupBoxCAN);
        label_11->setObjectName("label_11");
        label_11->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_11, 2, 0, 1, 1);

        comboCanBaud = new QComboBox(groupBoxCAN);
        comboCanBaud->addItem(QString());
        comboCanBaud->setObjectName("comboCanBaud");

        gridLayout->addWidget(comboCanBaud, 2, 1, 1, 1);

        btnCanOpenDevice = new QPushButton(groupBoxCAN);
        btnCanOpenDevice->setObjectName("btnCanOpenDevice");

        gridLayout->addWidget(btnCanOpenDevice, 3, 0, 1, 1);

        btnCanReset = new QPushButton(groupBoxCAN);
        btnCanReset->setObjectName("btnCanReset");
        btnCanReset->setEnabled(false);

        gridLayout->addWidget(btnCanReset, 3, 1, 1, 1);

        btnCanInit = new QPushButton(groupBoxCAN);
        btnCanInit->setObjectName("btnCanInit");
        btnCanInit->setEnabled(false);

        gridLayout->addWidget(btnCanInit, 4, 0, 1, 1);

        btnCanStart = new QPushButton(groupBoxCAN);
        btnCanStart->setObjectName("btnCanStart");
        btnCanStart->setEnabled(false);

        gridLayout->addWidget(btnCanStart, 4, 1, 1, 1);


        pageLayout->addWidget(groupBoxCAN);

        groupBox_CAN2 = new QGroupBox(page);
        groupBox_CAN2->setObjectName("groupBox_CAN2");
        sizePolicy.setHeightForWidth(groupBox_CAN2->sizePolicy().hasHeightForWidth());
        groupBox_CAN2->setSizePolicy(sizePolicy);
        verticalLayout_3 = new QVBoxLayout(groupBox_CAN2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_14 = new QLabel(groupBox_CAN2);
        label_14->setObjectName("label_14");

        horizontalLayout->addWidget(label_14);

        sendIDEdit = new QLineEdit(groupBox_CAN2);
        sendIDEdit->setObjectName("sendIDEdit");

        horizontalLayout->addWidget(sendIDEdit);

        comboChannel = new QComboBox(groupBox_CAN2);
        comboChannel->addItem(QString());
        comboChannel->addItem(QString());
        comboChannel->setObjectName("comboChannel");

        horizontalLayout->addWidget(comboChannel);


        verticalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        comboFrameType = new QComboBox(groupBox_CAN2);
        comboFrameType->addItem(QString());
        comboFrameType->addItem(QString());
        comboFrameType->setObjectName("comboFrameType");
        comboFrameType->setEnabled(true);

        horizontalLayout_2->addWidget(comboFrameType);

        comboDataFrameType = new QComboBox(groupBox_CAN2);
        comboDataFrameType->addItem(QString());
        comboDataFrameType->addItem(QString());
        comboDataFrameType->setObjectName("comboDataFrameType");
        comboDataFrameType->setEnabled(true);

        horizontalLayout_2->addWidget(comboDataFrameType);

        btnCanSend = new QPushButton(groupBox_CAN2);
        btnCanSend->setObjectName("btnCanSend");
        btnCanSend->setEnabled(false);

        horizontalLayout_2->addWidget(btnCanSend);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_15 = new QLabel(groupBox_CAN2);
        label_15->setObjectName("label_15");

        horizontalLayout_3->addWidget(label_15);

        sendDataEdit = new QLineEdit(groupBox_CAN2);
        sendDataEdit->setObjectName("sendDataEdit");

        horizontalLayout_3->addWidget(sendDataEdit);


        verticalLayout_3->addLayout(horizontalLayout_3);


        pageLayout->addWidget(groupBox_CAN2);

        groupBoxECU = new QGroupBox(page);
        groupBoxECU->setObjectName("groupBoxECU");
        QSizePolicy sizePolicy7(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(groupBoxECU->sizePolicy().hasHeightForWidth());
        groupBoxECU->setSizePolicy(sizePolicy7);
        groupBoxECU->setMinimumSize(QSize(0, 50));
        ecuLayout = new QHBoxLayout(groupBoxECU);
        ecuLayout->setObjectName("ecuLayout");
        label_17 = new QLabel(groupBoxECU);
        label_17->setObjectName("label_17");

        ecuLayout->addWidget(label_17);

        comboSerialECU = new QComboBox(groupBoxECU);
        comboSerialECU->setObjectName("comboSerialECU");

        ecuLayout->addWidget(comboSerialECU);

        btnECUScan = new QPushButton(groupBoxECU);
        btnECUScan->setObjectName("btnECUScan");

        ecuLayout->addWidget(btnECUScan);

        btnECUStart = new QPushButton(groupBoxECU);
        btnECUStart->setObjectName("btnECUStart");

        ecuLayout->addWidget(btnECUStart);


        pageLayout->addWidget(groupBoxECU);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");

        pageLayout->addLayout(horizontalLayout_4);

        groupBoxDAQ = new QGroupBox(page);
        groupBoxDAQ->setObjectName("groupBoxDAQ");
        sizePolicy.setHeightForWidth(groupBoxDAQ->sizePolicy().hasHeightForWidth());
        groupBoxDAQ->setSizePolicy(sizePolicy);
        horizontalLayout_daq = new QHBoxLayout(groupBoxDAQ);
        horizontalLayout_daq->setObjectName("horizontalLayout_daq");
        horizontalLayout_daq2 = new QHBoxLayout();
        horizontalLayout_daq2->setObjectName("horizontalLayout_daq2");
        labelDeviceName = new QLabel(groupBoxDAQ);
        labelDeviceName->setObjectName("labelDeviceName");

        horizontalLayout_daq2->addWidget(labelDeviceName);

        deviceNameEdit = new QLineEdit(groupBoxDAQ);
        deviceNameEdit->setObjectName("deviceNameEdit");

        horizontalLayout_daq2->addWidget(deviceNameEdit);


        horizontalLayout_daq->addLayout(horizontalLayout_daq2);

        horizontalLayout_daq3 = new QHBoxLayout();
        horizontalLayout_daq3->setObjectName("horizontalLayout_daq3");
        labelChannels = new QLabel(groupBoxDAQ);
        labelChannels->setObjectName("labelChannels");

        horizontalLayout_daq3->addWidget(labelChannels);

        channelsEdit = new QLineEdit(groupBoxDAQ);
        channelsEdit->setObjectName("channelsEdit");

        horizontalLayout_daq3->addWidget(channelsEdit);


        horizontalLayout_daq->addLayout(horizontalLayout_daq3);

        horizontalLayout_daq4 = new QHBoxLayout();
        horizontalLayout_daq4->setObjectName("horizontalLayout_daq4");
        labelSampleRate = new QLabel(groupBoxDAQ);
        labelSampleRate->setObjectName("labelSampleRate");

        horizontalLayout_daq4->addWidget(labelSampleRate);

        sampleRateEdit = new QLineEdit(groupBoxDAQ);
        sampleRateEdit->setObjectName("sampleRateEdit");

        horizontalLayout_daq4->addWidget(sampleRateEdit);


        horizontalLayout_daq->addLayout(horizontalLayout_daq4);

        horizontalLayout_daq5 = new QHBoxLayout();
        horizontalLayout_daq5->setObjectName("horizontalLayout_daq5");
        startDAQButton = new QPushButton(groupBoxDAQ);
        startDAQButton->setObjectName("startDAQButton");

        horizontalLayout_daq5->addWidget(startDAQButton);

        stopDAQButton = new QPushButton(groupBoxDAQ);
        stopDAQButton->setObjectName("stopDAQButton");
        stopDAQButton->setEnabled(false);

        horizontalLayout_daq5->addWidget(stopDAQButton);


        horizontalLayout_daq->addLayout(horizontalLayout_daq5);


        pageLayout->addWidget(groupBoxDAQ);

        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        pageLayout_2 = new QVBoxLayout(page_2);
        pageLayout_2->setObjectName("pageLayout_2");
        labelVisualizer = new QLabel(page_2);
        labelVisualizer->setObjectName("labelVisualizer");
        labelVisualizer->setAlignment(Qt::AlignmentFlag::AlignCenter);

        pageLayout_2->addWidget(labelVisualizer);

        groupBox_4 = new QGroupBox(page_2);
        groupBox_4->setObjectName("groupBox_4");
        sizePolicy4.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy4);
        groupBox_4->setMinimumSize(QSize(0, 150));
        verticalLayout_6 = new QVBoxLayout(groupBox_4);
        verticalLayout_6->setObjectName("verticalLayout_6");
        modbusCustomPlot = new QCustomPlot(groupBox_4);
        modbusCustomPlot->setObjectName("modbusCustomPlot");
        modbusCustomPlot->setMinimumSize(QSize(0, 100));

        verticalLayout_6->addWidget(modbusCustomPlot);


        pageLayout_2->addWidget(groupBox_4);

        groupBox_6 = new QGroupBox(page_2);
        groupBox_6->setObjectName("groupBox_6");
        sizePolicy4.setHeightForWidth(groupBox_6->sizePolicy().hasHeightForWidth());
        groupBox_6->setSizePolicy(sizePolicy4);
        groupBox_6->setMinimumSize(QSize(0, 150));
        verticalLayout_7 = new QVBoxLayout(groupBox_6);
        verticalLayout_7->setObjectName("verticalLayout_7");
        daqCustomPlot = new QCustomPlot(groupBox_6);
        daqCustomPlot->setObjectName("daqCustomPlot");
        daqCustomPlot->setMinimumSize(QSize(0, 100));

        verticalLayout_7->addWidget(daqCustomPlot);


        pageLayout_2->addWidget(groupBox_6);

        groupBoxECUplot = new QGroupBox(page_2);
        groupBoxECUplot->setObjectName("groupBoxECUplot");
        sizePolicy4.setHeightForWidth(groupBoxECUplot->sizePolicy().hasHeightForWidth());
        groupBoxECUplot->setSizePolicy(sizePolicy4);
        groupBoxECUplot->setMinimumSize(QSize(0, 150));
        verticalLayout_8 = new QVBoxLayout(groupBoxECUplot);
        verticalLayout_8->setObjectName("verticalLayout_8");
        ECUCustomPlot = new QCustomPlot(groupBoxECUplot);
        ECUCustomPlot->setObjectName("ECUCustomPlot");
        ECUCustomPlot->setMinimumSize(QSize(0, 100));

        verticalLayout_8->addWidget(ECUCustomPlot);


        pageLayout_2->addWidget(groupBoxECUplot);

        groupBoxCustomPlot = new QGroupBox(page_2);
        groupBoxCustomPlot->setObjectName("groupBoxCustomPlot");
        sizePolicy4.setHeightForWidth(groupBoxCustomPlot->sizePolicy().hasHeightForWidth());
        groupBoxCustomPlot->setSizePolicy(sizePolicy4);
        groupBoxCustomPlot->setMinimumSize(QSize(0, 150));
        verticalLayout_11 = new QVBoxLayout(groupBoxCustomPlot);
        verticalLayout_11->setObjectName("verticalLayout_11");
        customVarCustomPlot = new QCustomPlot(groupBoxCustomPlot);
        customVarCustomPlot->setObjectName("customVarCustomPlot");
        customVarCustomPlot->setMinimumSize(QSize(0, 100));

        verticalLayout_11->addWidget(customVarCustomPlot);


        pageLayout_2->addWidget(groupBoxCustomPlot);

        stackedWidget->addWidget(page_2);
        page_4 = new QWidget();
        page_4->setObjectName("page_4");
        sizePolicy.setHeightForWidth(page_4->sizePolicy().hasHeightForWidth());
        page_4->setSizePolicy(sizePolicy);
        pageLayout_4 = new QVBoxLayout(page_4);
        pageLayout_4->setObjectName("pageLayout_4");
        label_16 = new QLabel(page_4);
        label_16->setObjectName("label_16");

        pageLayout_4->addWidget(label_16);

        groupBox_7 = new QGroupBox(page_4);
        groupBox_7->setObjectName("groupBox_7");
        sizePolicy4.setHeightForWidth(groupBox_7->sizePolicy().hasHeightForWidth());
        groupBox_7->setSizePolicy(sizePolicy4);
        verticalLayout_81 = new QVBoxLayout(groupBox_7);
        verticalLayout_81->setObjectName("verticalLayout_81");
        tableView = new QTableView(groupBox_7);
        tableView->setObjectName("tableView");

        verticalLayout_81->addWidget(tableView);


        pageLayout_4->addWidget(groupBox_7);

        groupBox_8 = new QGroupBox(page_4);
        groupBox_8->setObjectName("groupBox_8");
        sizePolicy4.setHeightForWidth(groupBox_8->sizePolicy().hasHeightForWidth());
        groupBox_8->setSizePolicy(sizePolicy4);
        verticalLayout_9 = new QVBoxLayout(groupBox_8);
        verticalLayout_9->setObjectName("verticalLayout_9");
        daqDataTableView = new QTableView(groupBox_8);
        daqDataTableView->setObjectName("daqDataTableView");

        verticalLayout_9->addWidget(daqDataTableView);


        pageLayout_4->addWidget(groupBox_8);

        groupBox_9 = new QGroupBox(page_4);
        groupBox_9->setObjectName("groupBox_9");
        sizePolicy4.setHeightForWidth(groupBox_9->sizePolicy().hasHeightForWidth());
        groupBox_9->setSizePolicy(sizePolicy4);
        verticalLayout_10 = new QVBoxLayout(groupBox_9);
        verticalLayout_10->setObjectName("verticalLayout_10");
        tableViewECU = new QTableView(groupBox_9);
        tableViewECU->setObjectName("tableViewECU");

        verticalLayout_10->addWidget(tableViewECU);


        pageLayout_4->addWidget(groupBox_9);

        groupBoxCustomData = new QGroupBox(page_4);
        groupBoxCustomData->setObjectName("groupBoxCustomData");
        sizePolicy4.setHeightForWidth(groupBoxCustomData->sizePolicy().hasHeightForWidth());
        groupBoxCustomData->setSizePolicy(sizePolicy4);
        verticalLayout_12 = new QVBoxLayout(groupBoxCustomData);
        verticalLayout_12->setObjectName("verticalLayout_12");
        customVarTableView = new QTableView(groupBoxCustomData);
        customVarTableView->setObjectName("customVarTableView");

        verticalLayout_12->addWidget(customVarTableView);


        pageLayout_4->addWidget(groupBoxCustomData);

        stackedWidget->addWidget(page_4);

        contentLayout->addWidget(stackedWidget);


        mainLayout->addLayout(contentLayout);

        bottomLayout = new QHBoxLayout();
        bottomLayout->setObjectName("bottomLayout");
        labelWebSocketStatus = new QLabel(centralwidget);
        labelWebSocketStatus->setObjectName("labelWebSocketStatus");
        labelWebSocketStatus->setStyleSheet(QString::fromUtf8("color: red; font-weight: bold;"));

        bottomLayout->addWidget(labelWebSocketStatus);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        bottomLayout->addItem(horizontalSpacer);

        btnTestWebSocket = new QPushButton(centralwidget);
        btnTestWebSocket->setObjectName("btnTestWebSocket");
        btnTestWebSocket->setMinimumSize(QSize(150, 35));
        btnTestWebSocket->setStyleSheet(QString::fromUtf8("background-color: #2c7ec9;"));

        bottomLayout->addWidget(btnTestWebSocket);

        btnWebSocketControl = new QPushButton(centralwidget);
        btnWebSocketControl->setObjectName("btnWebSocketControl");
        btnWebSocketControl->setMinimumSize(QSize(180, 35));
        btnWebSocketControl->setStyleSheet(QString::fromUtf8("background-color: #2d904c;"));

        bottomLayout->addWidget(btnWebSocketControl);


        mainLayout->addLayout(bottomLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 2560, 18));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\351\207\207\351\233\206\344\270\216\347\233\221\346\216\247\347\263\273\347\273\237", nullptr));
        labelTitle->setText(QCoreApplication::translate("MainWindow", "\345\267\245\344\270\232\346\225\260\346\215\256\351\207\207\351\233\206\344\270\216\347\233\221\346\216\247\347\263\273\347\273\237", nullptr));
        btnPageInitial->setText(QCoreApplication::translate("MainWindow", "\351\200\232\344\277\241\345\210\235\345\247\213\345\214\226", nullptr));
        btnPagePlot->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\345\217\257\350\247\206\345\214\226", nullptr));
        btnPageData->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\345\233\276\350\241\250", nullptr));
        btnStartAll->setText(QCoreApplication::translate("MainWindow", "\344\270\200\351\224\256\345\274\200\345\247\213", nullptr));
        btnSaveData->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\346\225\260\346\215\256", nullptr));
        btnReadData->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226\346\225\260\346\215\256", nullptr));
        groupBox_10->setTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\347\233\221\346\216\247\351\235\242\346\235\277", nullptr));
        groupBox_11->setTitle(QCoreApplication::translate("MainWindow", "\344\273\252\350\241\250\347\233\230", nullptr));
        labelForce->setText(QCoreApplication::translate("MainWindow", "\346\213\211\345\212\233", nullptr));
        labelTorque->setText(QCoreApplication::translate("MainWindow", "\346\211\255\347\237\251", nullptr));
        labelRPM->setText(QCoreApplication::translate("MainWindow", "\350\275\254\351\200\237", nullptr));
        labelThrust->setText(QCoreApplication::translate("MainWindow", "\346\216\250\345\212\233", nullptr));
        labelFuelConsumption->setText(QCoreApplication::translate("MainWindow", "\346\262\271\350\200\227", nullptr));
        labelSparkPlugTemp->setText(QCoreApplication::translate("MainWindow", "\347\201\253\350\212\261\345\241\236\346\270\251\345\272\246", nullptr));
        labelPower->setText(QCoreApplication::translate("MainWindow", "\345\212\237\347\216\207", nullptr));
        groupBox_5->setTitle(QCoreApplication::translate("MainWindow", "Modbus\351\200\232\344\277\241\345\217\202\346\225\260\351\205\215\347\275\256", nullptr));
        labelPort->setText(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243\345\217\267:", nullptr));
        labelBaudRate->setText(QCoreApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207:", nullptr));
        comboBaudRate->setItemText(0, QCoreApplication::translate("MainWindow", "9600", nullptr));
        comboBaudRate->setItemText(1, QCoreApplication::translate("MainWindow", "19200", nullptr));
        comboBaudRate->setItemText(2, QCoreApplication::translate("MainWindow", "38400", nullptr));
        comboBaudRate->setItemText(3, QCoreApplication::translate("MainWindow", "115200", nullptr));

        labelStopBits->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242\344\275\215:", nullptr));
        comboStopBits->setItemText(0, QCoreApplication::translate("MainWindow", "1", nullptr));
        comboStopBits->setItemText(1, QCoreApplication::translate("MainWindow", "1.5", nullptr));
        comboStopBits->setItemText(2, QCoreApplication::translate("MainWindow", "2", nullptr));

        labelDataBits->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\344\275\215:", nullptr));
        comboDataBits->setItemText(0, QCoreApplication::translate("MainWindow", "8", nullptr));
        comboDataBits->setItemText(1, QCoreApplication::translate("MainWindow", "7", nullptr));
        comboDataBits->setItemText(2, QCoreApplication::translate("MainWindow", "6", nullptr));
        comboDataBits->setItemText(3, QCoreApplication::translate("MainWindow", "5", nullptr));

        labelParity->setText(QCoreApplication::translate("MainWindow", "\345\245\207\345\201\266\346\240\241\351\252\214:", nullptr));
        comboParity->setItemText(0, QCoreApplication::translate("MainWindow", "None", nullptr));
        comboParity->setItemText(1, QCoreApplication::translate("MainWindow", "Even", nullptr));
        comboParity->setItemText(2, QCoreApplication::translate("MainWindow", "Odd", nullptr));

        radioButton->setText(QCoreApplication::translate("MainWindow", "\344\270\262\345\217\243\347\212\266\346\200\201", nullptr));
        labelPort_3->setText(QCoreApplication::translate("MainWindow", "\344\273\216\346\234\272\345\234\260\345\235\200\357\274\232", nullptr));
        labelPort_2->setText(QCoreApplication::translate("MainWindow", "\345\221\275\344\273\244\357\274\232", nullptr));
        comboAction->setItemText(0, QCoreApplication::translate("MainWindow", "1", nullptr));
        comboAction->setItemText(1, QCoreApplication::translate("MainWindow", "2", nullptr));
        comboAction->setItemText(2, QCoreApplication::translate("MainWindow", "3", nullptr));
        comboAction->setItemText(3, QCoreApplication::translate("MainWindow", "4", nullptr));
        comboAction->setItemText(4, QCoreApplication::translate("MainWindow", "5", nullptr));
        comboAction->setItemText(5, QCoreApplication::translate("MainWindow", "6", nullptr));

        labelPort_4->setText(QCoreApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250\350\265\267\345\247\213\345\234\260\345\235\200\357\274\232", nullptr));
        labelPort_5->setText(QCoreApplication::translate("MainWindow", "\345\257\204\345\255\230\345\231\250\346\225\260\351\207\217\357\274\232", nullptr));
        labelStopBits_2->setText(QCoreApplication::translate("MainWindow", "\345\276\252\347\216\257\346\227\266\351\227\264\357\274\210ms\357\274\211", nullptr));
        filterEnabledCheckBox->setText(QCoreApplication::translate("MainWindow", "\345\220\257\347\224\250\344\275\216\351\200\232\346\273\244\346\263\242", nullptr));
        btnScanPort->setText(QCoreApplication::translate("MainWindow", "\346\211\253\346\217\217\344\270\262\345\217\243", nullptr));
        btnClearSendArea->setText(QCoreApplication::translate("MainWindow", "\346\270\205\351\231\244", nullptr));
        btnOpenPort->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        btnSend->setText(QCoreApplication::translate("MainWindow", "\350\257\273\345\217\226", nullptr));
        labelReceive->setText(QCoreApplication::translate("MainWindow", "\346\216\245\346\224\266\345\214\272:", nullptr));
        btnClearReceiveArea->setText(QCoreApplication::translate("MainWindow", "\346\270\205\347\251\272\346\216\245\346\224\266\345\214\272", nullptr));
        groupBoxCAN->setTitle(QCoreApplication::translate("MainWindow", "CAN\351\200\232\344\277\241\350\256\276\347\275\256", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207\347\261\273\345\236\213\357\274\232", nullptr));
        comboBox->setItemText(0, QCoreApplication::translate("MainWindow", "CANalyst-II", nullptr));

        label_4->setText(QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207\347\264\242\345\274\225\357\274\232", nullptr));
        comboCanID->setItemText(0, QCoreApplication::translate("MainWindow", "0", nullptr));

        label_11->setText(QCoreApplication::translate("MainWindow", "\346\263\242\347\211\271\347\216\207\357\274\232", nullptr));
        comboCanBaud->setItemText(0, QCoreApplication::translate("MainWindow", "10Kbps", nullptr));

        btnCanOpenDevice->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\350\256\276\345\244\207", nullptr));
        btnCanReset->setText(QCoreApplication::translate("MainWindow", "\345\244\215\344\275\215", nullptr));
        btnCanInit->setText(QCoreApplication::translate("MainWindow", "\345\210\235\345\247\213\345\214\226CAN", nullptr));
        btnCanStart->setText(QCoreApplication::translate("MainWindow", "\345\220\257\345\212\250CAN", nullptr));
        groupBox_CAN2->setTitle(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\345\217\221\351\200\201", nullptr));
        label_14->setText(QCoreApplication::translate("MainWindow", "ID(0x):", nullptr));
        sendIDEdit->setText(QCoreApplication::translate("MainWindow", "00000000", nullptr));
        comboChannel->setItemText(0, QCoreApplication::translate("MainWindow", "\351\200\232\351\201\2230", nullptr));
        comboChannel->setItemText(1, QCoreApplication::translate("MainWindow", "\351\200\232\351\201\2231", nullptr));

        comboFrameType->setItemText(0, QCoreApplication::translate("MainWindow", "\346\240\207\345\207\206\345\270\247", nullptr));
        comboFrameType->setItemText(1, QCoreApplication::translate("MainWindow", "\346\211\251\345\261\225\345\270\247", nullptr));

        comboDataFrameType->setItemText(0, QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\345\270\247", nullptr));
        comboDataFrameType->setItemText(1, QCoreApplication::translate("MainWindow", "\350\277\234\347\250\213\345\270\247", nullptr));

        btnCanSend->setText(QCoreApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        label_15->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256(0x,\344\273\245\347\251\272\346\240\274\345\210\206\351\232\224):", nullptr));
        sendDataEdit->setText(QCoreApplication::translate("MainWindow", "00 11 22 33 44 55 66 77", nullptr));
        groupBoxECU->setTitle(QCoreApplication::translate("MainWindow", "ECU\351\207\207\351\233\206", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "ECU\344\270\262\345\217\243\345\220\215\347\247\260", nullptr));
        btnECUScan->setText(QCoreApplication::translate("MainWindow", "\346\211\253\346\217\217\344\270\262\345\217\243", nullptr));
        btnECUStart->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\351\207\207\351\233\206", nullptr));
        groupBoxDAQ->setTitle(QCoreApplication::translate("MainWindow", "DAQ\346\225\260\346\215\256\351\207\207\351\233\206\345\217\202\346\225\260\350\256\276\347\275\256", nullptr));
        labelDeviceName->setText(QCoreApplication::translate("MainWindow", "\350\256\276\345\244\207\345\220\215\347\247\260\357\274\232", nullptr));
        deviceNameEdit->setText(QCoreApplication::translate("MainWindow", "Dev1", nullptr));
        labelChannels->setText(QCoreApplication::translate("MainWindow", "\351\200\232\351\201\223\345\210\227\350\241\250\357\274\232", nullptr));
        channelsEdit->setText(QCoreApplication::translate("MainWindow", "0/1", nullptr));
        labelSampleRate->setText(QCoreApplication::translate("MainWindow", "\351\207\207\346\240\267\347\216\207(Hz)\357\274\232", nullptr));
        sampleRateEdit->setText(QCoreApplication::translate("MainWindow", "10000", nullptr));
        startDAQButton->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\351\207\207\351\233\206", nullptr));
        stopDAQButton->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242\351\207\207\351\233\206", nullptr));
        labelVisualizer->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256\345\217\257\350\247\206\345\214\226", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "Modbus\345\233\276\350\241\250", nullptr));
        groupBox_6->setTitle(QCoreApplication::translate("MainWindow", "DAQ\345\233\276\350\241\250", nullptr));
        groupBoxECUplot->setTitle(QCoreApplication::translate("MainWindow", "ECU\345\233\276\350\241\250", nullptr));
        groupBoxCustomPlot->setTitle(QCoreApplication::translate("MainWindow", "\350\207\252\345\256\232\344\271\211\345\217\230\351\207\217\345\233\276\350\241\250", nullptr));
        label_16->setText(QCoreApplication::translate("MainWindow", "\346\225\260\346\215\256", nullptr));
        groupBox_7->setTitle(QCoreApplication::translate("MainWindow", "Modbus\346\225\260\346\215\256", nullptr));
        groupBox_8->setTitle(QCoreApplication::translate("MainWindow", "DAQ\346\225\260\346\215\256", nullptr));
        groupBox_9->setTitle(QCoreApplication::translate("MainWindow", "ECU\346\225\260\346\215\256", nullptr));
        groupBoxCustomData->setTitle(QCoreApplication::translate("MainWindow", "\350\207\252\345\256\232\344\271\211\345\217\230\351\207\217\346\225\260\346\215\256", nullptr));
        labelWebSocketStatus->setText(QCoreApplication::translate("MainWindow", "WS: \347\246\273\347\272\277", nullptr));
        btnTestWebSocket->setText(QCoreApplication::translate("MainWindow", "\346\265\213\350\257\225WebSocket", nullptr));
        btnWebSocketControl->setText(QCoreApplication::translate("MainWindow", "\345\220\257\345\212\250WebSocket\346\234\215\345\212\241\345\231\250", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
