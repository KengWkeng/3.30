#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QPaintEvent>
#include <QTimer>
#include <QMouseEvent>
#include <QMap>
#include <QVariant>
#include <QColor>
#include <QVector>
#include <QMetaType>
// #include "dashboardcalculator.h" // 引入计算线程类

// 为QVector<int>类型声明元类型，以支持属性系统
Q_DECLARE_METATYPE(QVector<int>)

// 前向声明
class QDialog;
class QGroupBox;
class QPushButton;

// 定义指针样式枚举
enum PointerStyle {
    PointerStyle_Circle = 0,
    PointerStyle_Indicator = 1,
    PointerStyle_Triangle = 2,
    PointerStyle_Custom = 3
};

/**
 * @class Dashboard
 * @brief 自定义仪表盘控件
 *
 * 仪表盘使用Qt的角度坐标系（0度在右侧，逆时针为正方向）
 * 刻度起始角度为130度
 * 刻度结束角度为50度
 * 刻度总范围为280度
 */
class Dashboard : public QWidget
{
    Q_OBJECT

    // 使用Qt属性系统
    Q_PROPERTY(double value READ getValue WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double minValue READ getMinValue WRITE setMinValue NOTIFY rangeChanged)
    Q_PROPERTY(double maxValue READ getMaxValue WRITE setMaxValue NOTIFY rangeChanged)
    Q_PROPERTY(int precision READ getPrecision WRITE setPrecision)
    Q_PROPERTY(QString unit READ getUnit WRITE setUnit)
    Q_PROPERTY(QString title READ getTitle WRITE setTitle)
    Q_PROPERTY(QColor titleColor READ getTitleColor WRITE setTitleColor)
    Q_PROPERTY(QColor scaleColor READ getScaleColor WRITE setScaleColor)
    Q_PROPERTY(PointerStyle pointerStyle READ getPointerStyle WRITE setPointerStyle)
    Q_PROPERTY(QColor pointerColor READ getPointerColor WRITE setPointerColor)
    Q_PROPERTY(bool animation READ getAnimation WRITE setAnimation)
    Q_PROPERTY(double animationStep READ getAnimationStep WRITE setAnimationStep)
    Q_PROPERTY(QString variableName READ getVariableName WRITE setVariableName)
    Q_PROPERTY(QString formula READ getFormula WRITE setFormula)
    Q_PROPERTY(bool customVariable READ isCustomVariable WRITE setCustomVariable)

public:
    explicit Dashboard(QWidget *parent = nullptr);
    ~Dashboard();

    // Getter/Setter
    double getValue() const;
    double getMinValue() const;
    double getMaxValue() const;
    int getPrecision() const;
    QString getUnit() const;
    QString getTitle() const;
    QColor getTitleColor() const;
    QColor getScaleColor() const;
    QColor getPointerColor() const;
    PointerStyle getPointerStyle() const;
    bool getAnimation() const;
    double getAnimationStep() const;
    QString getVariableName() const { return m_variableName; }
    QString getFormula() const { return m_formula; }
    bool isCustomVariable() const { return m_isCustomVariable; }

    void setValue(double value);
    void setMinValue(double minValue);
    void setMaxValue(double maxValue);
    void setPrecision(int precision);
    void setUnit(const QString &unit);
    void setTitle(const QString &title);
    void setTitleColor(const QColor &color);
    void setScaleColor(const QColor &color);
    void setPointerStyle(PointerStyle style);
    void setPointerColor(const QColor &color);
    void setAnimation(bool animation);
    void setAnimationStep(double step);
    void setRange(double minValue, double maxValue) { setMinValue(minValue); setMaxValue(maxValue); }
    void setVariableName(const QString &name) { m_variableName = name; }
    void setFormula(const QString &formula) { m_formula = formula; }
    void setCustomVariable(bool isCustom) { m_isCustomVariable = isCustom; }

    // 添加公式计算功能 - 保留公共接口，但修改内部实现使用计算线程
    double calculateFormula(const QString &formula, const QMap<QString, double> &variables);

    // 设置计算线程
    // void setCalculator(DashboardCalculator *calculator);

    // 设置初始化状态
    void setInitializationStatus(bool initialized) { m_initialized = initialized; }

public slots:
    // 设置用于公式计算的变量值
    // void setVariableValues(const QMap<QString, double> &variables);
    // 接收来自计算线程的计算结果
    // void onCalculationResult(const QString &dashboardName, double result);
    // 接收来自计算线程的计算错误
    // void onCalculationError(const QString &dashboardName, const QString &errorMessage);
    // 刷新仪表盘UI
    void refreshUI();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void updateValue();

signals:
    void valueChanged(double value);
    void rangeChanged();
    void dashboardDoubleClicked();
    void dashboardSettingsChanged(const QString &objectName, const QMap<QString, QVariant> &settings);
    void customVariableFormulaChanged(const QString &objectName, const QString &formula);

private:
    // 工具方法
    void updateLabel();
    void drawThemeBackground(QPainter& painter);
    double evaluateExpression(const QString &expression);
    int precedence(const QChar &op);
    bool isOperator(const QChar &c);
    double applyOperator(double a, double b, QChar op);

    // 变量值持久化
    QMap<QString, double> m_lastVariableValues;
    bool m_hasValidResult = false;

    // 计算线程对象指针 - 由外部设置，不由Dashboard管理生命周期
    // DashboardCalculator *m_calculator;
    // 最近一次计算结果
    // double m_lastCalculatedValue;
    // 最近一次计算是否成功
    // bool m_lastCalculationSucceeded;

    // 核心数值属性
    double m_value;           // 当前值
    double m_currentValue;    // 用于动画的当前值
    double m_animationStep;   // 动画步长
    double m_minValue;        // 最小值
    double m_maxValue;        // 最大值
    int m_precision;          // 精度（小数位数）
    QString m_unit;           // 单位
    QString m_title;          // 标题
    QColor m_titleColor;      // 标题颜色

    // 视觉样式属性
    QColor m_backgroundColor; // 背景颜色
    QColor m_foregroundColor; // 前景颜色（外环）
    QColor m_scaleColor;      // 刻度颜色
    QColor m_textColor;       // 文字颜色
    QColor m_pointerColor;    // 指针颜色
    PointerStyle m_pointerStyle; // 指针样式

    // 动画相关
    bool m_animation;         // 是否启用动画
    QTimer *m_timer;          // 动画定时器

    // 刻度相关
    int m_scaleMinor;         // 小刻度数量
    int m_scaleMajor;         // 大刻度数量

    // 自定义变量相关
    QString m_variableName;   // 变量名称 (如 A_0, B_1, C_2, D_0)
    QString m_formula;        // 自定义变量的公式
    bool m_isCustomVariable;  // 是否为自定义变量
    QMap<QString, double> m_variables; // 存储公式中用到的变量值

    // 初始化状态
    bool m_initialized = false; // 默认为未初始化

    // 界面绘制方法
    void drawBackground(QPainter *painter);
    void drawScale(QPainter *painter);
    void drawScaleNum(QPainter *painter);
    void drawTitle(QPainter *painter);
    void drawUnit(QPainter *painter);
    void drawValue(QPainter *painter);
    void drawPointer(QPainter *painter);
    void drawCenter(QPainter *painter);

    // 对话框相关方法
    void createSettingsDialog();
    QGroupBox* createDataSourceGroup(QDialog *dialog);
    QGroupBox* createDisplayGroup(QDialog *dialog);
    void connectDialogButtons(QDialog *dialog, QPushButton *okButton, QPushButton *cancelButton);
};

#endif // DASHBOARD_H