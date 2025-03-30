#include "dashboardcalculator.h"

DashboardCalculator::DashboardCalculator(QObject *parent)
    : QThread(parent), m_stopRequested(false)
{
    qDebug() << "DashboardCalculator 创建在线程:" << QThread::currentThread();
}

DashboardCalculator::~DashboardCalculator()
{
    stopThread();
    wait(); // 等待线程结束
}

void DashboardCalculator::stopThread()
{
    m_stopRequested = true;
    // 唤醒线程以便检查停止标志
    if (isRunning()) {
        // 发送一个空请求以唤醒线程
        QMutexLocker locker(&m_queueMutex);
        m_calculationQueue.append(CalculationRequest());
    }
}

void DashboardCalculator::requestCalculation(const QString &dashboardName, const QString &formula, const QMap<QString, double> &variables)
{
    // 将计算请求添加到队列
    QMutexLocker locker(&m_queueMutex);
    
    CalculationRequest request;
    request.dashboardName = dashboardName;
    request.formula = formula;
    request.variables = variables;
    
    m_calculationQueue.append(request);
    
    // 如果线程尚未启动，则启动它
    if (!isRunning()) {
        start();
    }
}

void DashboardCalculator::updateVariables(const QMap<QString, double> &variables)
{
    QMutexLocker locker(&m_variableMutex);
    
    // 合并新变量到全局变量表
    for (auto it = variables.begin(); it != variables.end(); ++it) {
        m_allVariables[it.key()] = it.value();
    }
}

void DashboardCalculator::run()
{
    qDebug() << "DashboardCalculator线程开始执行，线程ID:" << QThread::currentThreadId();
    
    while (!m_stopRequested) {
        CalculationRequest request;
        bool hasRequest = false;
        
        // 从队列中获取计算请求
        {
            QMutexLocker locker(&m_queueMutex);
            if (!m_calculationQueue.isEmpty()) {
                request = m_calculationQueue.takeFirst();
                hasRequest = true;
            }
        }
        
        // 如果没有请求，则等待
        if (!hasRequest) {
            msleep(10); // 短暂休眠以减少CPU使用
            continue;
        }
        
        // 如果是空请求（用于唤醒线程），则跳过处理
        if (request.dashboardName.isEmpty() && request.formula.isEmpty()) {
            continue;
        }
        
        // 处理计算请求
        try {
            // 合并变量，更新到全局变量表
            {
                QMutexLocker locker(&m_variableMutex);
                for (auto it = request.variables.begin(); it != request.variables.end(); ++it) {
                    m_allVariables[it.key()] = it.value();
                }
            }
            
            // 预处理公式，替换变量为数值
            QString processedFormula = request.formula;
            
            // 使用正则表达式查找变量模式（如A_0, B_1等）
            static QRegularExpression varPattern("([A-Z]_[0-9]+)");
            QRegularExpressionMatchIterator matches = varPattern.globalMatch(request.formula);
            
            // 记录变量替换前后的情况，帮助诊断问题
            bool hasUnknownVars = false;
            
            // 获取所有变量值的副本，避免长时间锁定互斥锁
            QMap<QString, double> allVariablesCopy;
            {
                QMutexLocker locker(&m_variableMutex);
                allVariablesCopy = m_allVariables;
            }
            
            while (matches.hasNext()) {
                QRegularExpressionMatch match = matches.next();
                QString varName = match.captured(1);
                
                if (allVariablesCopy.contains(varName)) {
                    // 使用变量表中的值替换变量名
                    double value = allVariablesCopy[varName];
                    processedFormula.replace(varName, QString::number(value));
                } else {
                    // 如果变量未提供，使用0
                    qDebug() << "变量" << varName << "未提供值，使用默认值0";
                    processedFormula.replace(varName, "0");
                    hasUnknownVars = true;
                }
            }
            
            // 输出处理后的公式，帮助调试
            qDebug() << "处理后的公式:" << processedFormula;
            
            // 计算表达式
            double result = evaluateExpression(processedFormula);
            
            // 发出计算结果信号
            emit calculationResult(request.dashboardName, result);
            
            // 记录计算的变量值，以便其他公式使用
            {
                QMutexLocker locker(&m_variableMutex);
                if (!request.dashboardName.isEmpty()) {
                    QString varName = request.dashboardName;
                    if (!varName.contains("_")) {
                        // 如果变量名不是标准格式，则假定为D_(自定义)前缀
                        int index = request.dashboardName.indexOf("-Dashboard");
                        if (index > 0) {
                            varName = "D_" + request.dashboardName.left(index);
                        }
                    }
                    m_allVariables[varName] = result;
                }
            }
        } catch (const std::exception &e) {
            qDebug() << "公式计算错误:" << e.what();
            emit calculationError(request.dashboardName, QString("计算错误: %1").arg(e.what()));
        }
    }
    
    qDebug() << "DashboardCalculator线程结束执行";
}

bool DashboardCalculator::isOperator(const QChar &c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')';
}

int DashboardCalculator::precedence(const QChar &op)
{
    if (op == '+' || op == '-') 
        return 1;
    if (op == '*' || op == '/') 
        return 2;
    return 0;
}

double DashboardCalculator::applyOperator(double a, double b, QChar op)
{
    switch (op.toLatin1()) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return b != 0 ? a / b : 0; // 防止除以零
        default: return 0;
    }
}

double DashboardCalculator::evaluateExpression(const QString &expression)
{
    QStack<double> values;
    QStack<QChar> ops;
    
    for (int i = 0; i < expression.length(); i++) {
        // 跳过空格
        if (expression[i].isSpace())
            continue;
            
        // 处理数字
        if (expression[i].isDigit() || expression[i] == '.') {
            QString numStr;
            while (i < expression.length() && (expression[i].isDigit() || expression[i] == '.')) {
                numStr.append(expression[i]);
                i++;
            }
            i--; // 回退一位，因为循环会自增
            
            bool ok;
            double num = numStr.toDouble(&ok);
            if (ok) {
                values.push(num);
            } else {
                qDebug() << "无法将" << numStr << "转换为数字";
                return 0.0;
            }
        }
        // 处理左括号
        else if (expression[i] == '(') {
            ops.push(expression[i]);
        }
        // 处理右括号
        else if (expression[i] == ')') {
            while (!ops.isEmpty() && ops.top() != '(') {
                QChar op = ops.pop();
                
                // 需要两个操作数
                if (values.size() < 2) {
                    qDebug() << "表达式格式错误：" << expression;
                    return 0.0;
                }
                
                double b = values.pop();
                double a = values.pop();
                
                values.push(applyOperator(a, b, op));
            }
            
            if (!ops.isEmpty())
                ops.pop(); // 弹出左括号
        }
        // 处理操作符
        else if (isOperator(expression[i])) {
            while (!ops.isEmpty() && precedence(ops.top()) >= precedence(expression[i]) && ops.top() != '(') {
                QChar op = ops.pop();
                
                // 需要两个操作数
                if (values.size() < 2) {
                    qDebug() << "表达式格式错误：" << expression;
                    return 0.0;
                }
                
                double b = values.pop();
                double a = values.pop();
                
                values.push(applyOperator(a, b, op));
            }
            
            ops.push(expression[i]);
        }
        // 处理其他字符（可能是错误）
        else {
            qDebug() << "表达式中包含无法识别的字符:" << expression[i];
            return 0.0;
        }
    }
    
    // 处理剩余的操作符
    while (!ops.isEmpty()) {
        QChar op = ops.pop();
        
        // 需要两个操作数
        if (values.size() < 2) {
            qDebug() << "表达式格式错误：" << expression;
            return 0.0;
        }
        
        double b = values.pop();
        double a = values.pop();
        
        values.push(applyOperator(a, b, op));
    }
    
    // 最终结果应该是values栈中唯一的值
    if (values.size() == 1) {
        return values.top();
    } else {
        qDebug() << "表达式计算错误，结果不唯一:" << expression;
        return 0.0;
    }
} 