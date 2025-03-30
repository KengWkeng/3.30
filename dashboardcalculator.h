#ifndef DASHBOARDCALCULATOR_H
#define DASHBOARDCALCULATOR_H

#include <QThread>
#include <QString>
#include <QMap>
#include <QVector>
#include <QRegularExpression>
#include <QDebug>
#include <QStack>
#include <QMutex>

class DashboardCalculator : public QThread
{
    Q_OBJECT

public:
    explicit DashboardCalculator(QObject *parent = nullptr);
    ~DashboardCalculator();

    // 启动计算请求
    void requestCalculation(const QString &dashboardName, const QString &formula, const QMap<QString, double> &variables);
    
    // 更新变量值
    void updateVariables(const QMap<QString, double> &variables);
    
    // 线程停止执行
    void stopThread();

protected:
    // 重写run方法，实现线程主循环
    void run() override;

signals:
    // 计算结果信号，发送到主线程
    void calculationResult(const QString &dashboardName, double result);
    // 计算错误信号
    void calculationError(const QString &dashboardName, const QString &errorMessage);

private:
    // 判断字符是否是操作符
    bool isOperator(const QChar &c);
    // 获取操作符优先级
    int precedence(const QChar &op);
    // 应用操作符进行计算
    double applyOperator(double a, double b, QChar op);
    // 计算表达式
    double evaluateExpression(const QString &expression);
    
    // 存储所有仪表盘的变量值
    QMap<QString, double> m_allVariables;
    // 变量锁，确保线程安全
    QMutex m_variableMutex;
    // 计算请求队列锁
    QMutex m_queueMutex;
    
    // 计算请求队列
    struct CalculationRequest {
        QString dashboardName;
        QString formula;
        QMap<QString, double> variables;
    };
    QVector<CalculationRequest> m_calculationQueue;
    
    // 线程停止标志
    bool m_stopRequested;
};

#endif // DASHBOARDCALCULATOR_H 