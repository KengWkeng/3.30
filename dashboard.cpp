#include "dashboard.h"
#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QDebug>
#include <QtMath>
#include <QMouseEvent>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QTimer>
#include <QMetaObject>
#include <QVariant>
#include <cmath>
#include <QRegularExpression>
#include <QStack>

Dashboard::Dashboard(QWidget *parent) : QWidget(parent),
    m_value(0),
    m_currentValue(0),
    m_animationStep(0.5),
    m_minValue(0),
    m_maxValue(100),
    m_precision(0),
    m_unit(""),
    m_title("仪表盘"),
    m_titleColor(Qt::black),
    m_backgroundColor(Qt::white),
    m_foregroundColor(QColor(72, 72, 72)),
    m_scaleColor(Qt::black),
    m_textColor(Qt::black),
    m_pointerColor(Qt::red),
    m_pointerStyle(PointerStyle_Circle),
    m_animation(true),
    m_scaleMinor(5),
    m_scaleMajor(10),
    m_variableName(""),
    m_formula(""),
    m_isCustomVariable(false)
{
    // 初始化界面和定时器
    setMinimumSize(200, 200);
    
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateValue()));

    // 设置控件尺寸策略和最小尺寸
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

Dashboard::~Dashboard()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    // 注意：不要在这里删除m_calculator，它由外部管理
}

// Getter/Setter 实现
double Dashboard::getValue() const
{
    return m_value;
}

double Dashboard::getMinValue() const
{
    return m_minValue;
}

double Dashboard::getMaxValue() const
{
    return m_maxValue;
}

int Dashboard::getPrecision() const
{
    return m_precision;
}

QString Dashboard::getUnit() const
{
    return m_unit;
}

QString Dashboard::getTitle() const
{
    return m_title;
}

QColor Dashboard::getTitleColor() const
{
    return m_titleColor;
}

QColor Dashboard::getScaleColor() const
{
    return m_scaleColor;
}

PointerStyle Dashboard::getPointerStyle() const
{
    return m_pointerStyle;
}

QColor Dashboard::getPointerColor() const
{
    return m_pointerColor;
}

bool Dashboard::getAnimation() const
{
    return m_animation;
}

double Dashboard::getAnimationStep() const
{
    return m_animationStep;
}

void Dashboard::setValue(double value)
{
    // 确保值在范围内
    if (value < m_minValue) {
        value = m_minValue;
    }

    if (value > m_maxValue) {
        value = m_maxValue;
    }

    // 如果值相同，不处理
    if (m_value == value) {
        return;
    }

    m_value = value;
    emit valueChanged(value);

    // 如果值为最小值，直接设置，不使用动画
    if (m_value == m_minValue) {
        m_currentValue = m_minValue;
        if (m_timer->isActive()) {
            m_timer->stop();
        }
        update();
        return;
    }

    // 使用动画更新值
    if (m_animation) {
        m_timer->start();
    } else {
        m_currentValue = m_value;
        update();
    }
}

void Dashboard::setMinValue(double minValue)
{
    // 避免最小值大于等于最大值
    if (minValue >= m_maxValue) {
        return;
    }

    double oldMinValue = m_minValue;
    m_minValue = minValue;
    emit rangeChanged();

    // 确保当前值不小于最小值
    if (m_value < m_minValue) {
        setValue(m_minValue);
    } else if (oldMinValue != minValue) {
        // 当最小值变化时，需要更新指针位置
        update();
    }
}

void Dashboard::setMaxValue(double maxValue)
{
    // 避免最大值小于等于最小值
    if (maxValue <= m_minValue) {
        return;
    }

    double oldMaxValue = m_maxValue;
    m_maxValue = maxValue;
    emit rangeChanged();

    // 确保当前值不大于最大值
    if (m_value > m_maxValue) {
        setValue(m_maxValue);
    } else if (oldMaxValue != maxValue) {
        // 当最大值变化时，需要更新指针位置
        update();
    }
}

void Dashboard::setPrecision(int precision)
{
    if (precision <= 2 && precision >= 0 && precision != m_precision) {
        m_precision = precision;
        update();
    }
}

void Dashboard::setUnit(const QString &unit)
{
    if (m_unit != unit) {
        m_unit = unit;
        update();
    }
}

void Dashboard::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        update();
    }
}

void Dashboard::setTitleColor(const QColor &color)
{
    if (m_titleColor != color) {
        m_titleColor = color;
        update();
    }
}

void Dashboard::setScaleColor(const QColor &color)
{
    if (m_scaleColor != color) {
        m_scaleColor = color;
        update();
    }
}

void Dashboard::setPointerStyle(PointerStyle style)
{
    if (m_pointerStyle != style) {
        m_pointerStyle = style;
        update();
    }
}

void Dashboard::setPointerColor(const QColor &color)
{
    if (m_pointerColor != color) {
        m_pointerColor = color;
        update();
    }
}

void Dashboard::setAnimation(bool animation)
{
    m_animation = animation;
}

void Dashboard::setAnimationStep(double step)
{
    if (step > 0) {
        m_animationStep = step;
    }
}

// 更新仪表盘值动画
void Dashboard::updateValue()
{
    // 如果目标值是最小值，直接设置为最小值
    if (m_value == m_minValue) {
        m_currentValue = m_minValue;
        m_timer->stop();
        update();
        return;
    }

    if (m_currentValue < m_value) {
        m_currentValue += m_animationStep;
        if (m_currentValue > m_value) {
            m_currentValue = m_value;
            m_timer->stop();
        }
    } else if (m_currentValue > m_value) {
        m_currentValue -= m_animationStep;
        if (m_currentValue < m_value) {
            m_currentValue = m_value;
            m_timer->stop();
        }
    } else {
        m_timer->stop();
    }

    // 确保值在范围内
    if (m_currentValue < m_minValue) {
        m_currentValue = m_minValue;
    }

    if (m_currentValue > m_maxValue) {
        m_currentValue = m_maxValue;
    }

    update();
}

void Dashboard::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    // 创建QPainter并开启抗锯齿
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制各组件
    drawBackground(&painter);
    drawScale(&painter);
    drawScaleNum(&painter);
    drawPointer(&painter);
    drawCenter(&painter);
    drawValue(&painter);
    drawUnit(&painter);
    drawTitle(&painter);
}

void Dashboard::resizeEvent(QResizeEvent *)
{
    refreshUI();
}

void Dashboard::refreshUI()
{
    update();
}

void Dashboard::drawBackground(QPainter *painter)
{
    painter->save();

    // 计算尺寸参数
    const int width = this->width();
    const int height = this->height();
    const int side = qMin(width, height);
    
    // 获取绘制区域
    QRect drawingRect = rect();
    if (width > height) {
        drawingRect.setLeft((width - height) / 2);
        drawingRect.setWidth(height);
    } else {
        drawingRect.setTop((height - width) / 2);
        drawingRect.setHeight(width);
    }
    
    QPointF center = drawingRect.center();
    int radius = side / 2;
    
    // 绘制背景圆
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_backgroundColor);
    painter->drawEllipse(center, radius - 5, radius - 5);
    
    // // 绘制外环
    // painter->setPen(QPen(m_foregroundColor, side * 0.02));
    // painter->setBrush(Qt::NoBrush);
    // painter->drawEllipse(center, radius - 10, radius - 10);

    // // 绘制内环
    // painter->setPen(QPen(m_foregroundColor, side * 0.01));
    // painter->drawEllipse(center, radius * 0.85, radius * 0.85);

    painter->restore();
}

void Dashboard::drawScale(QPainter *painter)
{
    painter->save();

    // 计算尺寸参数
    const int width = this->width();
    const int height = this->height();
    const int side = qMin(width, height);
    
    // 获取绘制区域中心点
    QPointF center = rect().center();
    
    // 计算半径
    const double radius = side / 2.0;
    
    // 设置刻度参数
    int startAngle = 130;   // 起始角度
    int endAngle = 50;      // 结束角度
    int range = 280;        // 角度范围

    int tickCount = m_scaleMajor;        // 主刻度数
    int subTickCount = m_scaleMinor;     // 每个主刻度之间的子刻度数

    // 绘制主刻度和数字
    for (int i = 0; i <= tickCount; ++i) {
        // 计算刻度角度
        double angle = startAngle + range * i / tickCount;
        if (angle > 360) angle -= 360;  // 保持角度在0-360°
        painter->save();
        painter->translate(center);
        painter->rotate(angle);

        // 绘制主刻度线
        painter->setPen(QPen(m_scaleColor, side * 0.012));
        painter->drawLine(radius * 0.8, 0, radius * 0.95, 0);
        painter->restore();
    }
    
    // 绘制副刻度
    for (int i = 0; i < tickCount; ++i) {
        for (int j = 1; j < subTickCount; ++j) {
            double subAngle = startAngle + range * (i + j / static_cast<double>(subTickCount)) / tickCount;
            if (subAngle > 360) subAngle -= 360;
            painter->save();
            painter->translate(center);
            painter->rotate(subAngle);

            painter->setPen(QPen(m_scaleColor, side * 0.005));
            painter->drawLine(radius * 0.85, 0, radius * 0.95, 0);

            painter->restore();
        }
    }

    painter->restore();
}

void Dashboard::drawScaleNum(QPainter *painter)
{
    painter->save();

    // 计算尺寸参数
    const int width = this->width();
    const int height = this->height();
    const int side = qMin(width, height);
    
    // 获取绘制区域中心点
    QPointF center = rect().center();
    
    // 计算半径
    const double radius = side / 2.0 - 10;
    
    // 设置刻度参数
    int startAngle = 130;   // 起始角度
    int endAngle = 50;      // 结束角度
    int range = 280;        // 角度范围
    
    int tickCount = m_scaleMajor;    // 主刻度数
    
    // 设置字体
    QFont font = painter->font();
    font.setPointSize(side * 0.05);
    painter->setFont(font);
    painter->setPen(m_textColor);

    // 绘制主刻度文本
    for (int i = 0; i <= tickCount; ++i) {
        // 计算刻度角度
        double angle = startAngle + range * i / tickCount;
        if (angle > 360) angle -= 360;  // 保持角度在0-360°
        
        // 计算刻度值
        double value = m_minValue + (m_maxValue - m_minValue) * i / tickCount;
        QString text = QString::number(value, 'f', m_precision);
        
        // 获取文本大小
        QFontMetricsF metrics(font);
        double textWidth = metrics.horizontalAdvance(text);
        double textHeight = metrics.height();
        
        // 计算文本位置
        double angleRad = qDegreesToRadians(angle);
        double x = center.x() + radius * 0.65 * qCos(angleRad) - textWidth/2;
        double y = center.y() + radius * 0.65 * qSin(angleRad) + textHeight/4;
        
        // 调整文本位置以提高可读性
        if (angle > 90 && angle < 270) {
            x -= textWidth * 0.1;
        } else {
            x += textWidth * 0.1;
        }
        
        // 绘制文本
        painter->drawText(QPointF(x, y), text);
    }

    painter->restore();
}

void Dashboard::drawTitle(QPainter *painter)
{
    if (m_title.isEmpty()) {
        return;
    }

    painter->save();

    // 计算尺寸参数
    const int width = this->width();
    const int height = this->height();
    const int side = qMin(width, height);

    // 设置字体
    const double fontSize = side / 18;
    QFont font;
    font.setPointSizeF(fontSize);
    painter->setFont(font);
    painter->setPen(m_titleColor);

    // 获取文本度量
    QFontMetricsF fm(font);
    const double textWidth = fm.horizontalAdvance(m_title);

    // 计算文本矩形
    QRectF textRect(width / 2 - textWidth / 2, height * 0.25, textWidth, fontSize * 1.5);
    
    // 绘制标题文本
    painter->drawText(textRect, Qt::AlignCenter, m_title);

    painter->restore();
}

void Dashboard::drawUnit(QPainter *painter)
{
    if (m_unit.isEmpty()) {
        return;
    }

    painter->save();

    // 计算尺寸参数
    const int width = this->width();
    const int height = this->height();
    const int side = qMin(width, height);

    // 设置字体
    const double fontSize = side / 20;
    QFont font;
    font.setPointSizeF(fontSize);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(m_textColor);

    // 获取文本度量
    QFontMetricsF fm(font);
    const double textWidth = fm.horizontalAdvance(m_unit);

    // 计算文本矩形
    QRectF textRect(width / 2 - textWidth / 2, height * 0.6, textWidth, fontSize * 1.5);

    // 绘制单位文本
    painter->drawText(textRect, Qt::AlignCenter, m_unit);

    painter->restore();
}

void Dashboard::drawValue(QPainter *painter)
{
    painter->save();

    // 计算尺寸参数
    const int width = this->width();
    const int height = this->height();
    const int side = qMin(width, height);

    // 设置字体
    const double fontSize = side / 10;
    QFont font;
    font.setPointSizeF(fontSize);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(m_textColor);

    // 创建值文本
    const QString text = QString::number(m_currentValue, 'f', m_precision);
    QFontMetricsF fm(font);
    const double textWidth = fm.horizontalAdvance(text);

    // 绘制值文本
    QRectF textRect(width / 2 - textWidth / 2, height * 0.75 - fontSize/2, textWidth, fontSize);
    painter->drawText(textRect, Qt::AlignCenter, text);

    painter->restore();
}

void Dashboard::drawPointer(QPainter *painter)
{
    painter->save();

    // 计算尺寸参数
    const int width = this->width();
    const int height = this->height();
    const int side = qMin(width, height);
    
    // 获取绘制区域中心点
    QPointF center = rect().center();
    
    // 计算半径
    const double radius = side / 2.0;
    
    // 设置刻度参数
    int startAngle = 130;   // 起始角度
    int endAngle = 50;      // 结束角度
    int range = 280;        // 角度范围
    
    // 确保当前值在范围内
    double currentValue = qBound(m_minValue, m_currentValue, m_maxValue);

    // 计算值比例
    double fraction = 0;
    if (m_maxValue > m_minValue) {
        fraction = (currentValue - m_minValue) / (m_maxValue - m_minValue);
    }
    fraction = qBound(0.0, fraction, 1.0);
    
    // 计算指针角度
    double valueAngle = startAngle + range * fraction;
    
    // 旋转画笔
    painter->translate(center);
    painter->rotate(valueAngle);
    
    // 计算指针参数
    const double pointerWidth = side / 20.0*2;
    
    // 设置画笔和画刷
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_pointerColor);
    
    // 绘制指针 - 将指针长度增加到原来的1.5倍
    QPainterPath needlePath;
    needlePath.moveTo(-pointerWidth / 2, 0);
    needlePath.lineTo(0, -pointerWidth / 4);
    needlePath.lineTo(radius* 0.9, 0); // 增加指针长度为原来的1.5倍
    needlePath.lineTo(0, pointerWidth / 4);
    needlePath.lineTo(-pointerWidth / 2, 0);
    
    painter->drawPath(needlePath);
    
    painter->restore();
}

void Dashboard::drawCenter(QPainter *painter)
{
    painter->save();

    // 计算尺寸参数
    const int width = this->width();
    const int height = this->height();
    const int side = qMin(width, height);
    
    // 获取中心点
    QPointF center = rect().center();

    // 计算中心圆半径
    const double centerRadius = side / 15;

    // 绘制中心圆
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_pointerColor);
    painter->drawEllipse(center, centerRadius, centerRadius);

    // 绘制中心白色圆
    painter->setBrush(m_backgroundColor);
    painter->drawEllipse(center, centerRadius * 0.75, centerRadius * 0.75);

    painter->restore();
}

void Dashboard::mouseDoubleClickEvent(QMouseEvent *event)
{
    // 处理左键双击事件
    if (event->button() == Qt::LeftButton) {
        qDebug() << "Dashboard双击: " << this->objectName();
        emit dashboardDoubleClicked();
        
        createSettingsDialog();
    }

    // 继续处理事件
    QWidget::mouseDoubleClickEvent(event);
}

// 创建仪表盘设置对话框
void Dashboard::createSettingsDialog()
{
    // 创建对话框
    QDialog *dialog = new QDialog(this->parentWidget());
    dialog->setWindowTitle(m_title.isEmpty() ? tr("仪表盘设置") : m_title + tr(" 设置"));
    dialog->resize(500, 400);
    dialog->setMinimumSize(400, 350);

    // 添加主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);

    // 添加数据源设置组
    QGroupBox *dataSourceGroup = createDataSourceGroup(dialog);
    mainLayout->addWidget(dataSourceGroup);

    // 添加仪表盘显示设置组
    QGroupBox *displayGroup = createDisplayGroup(dialog);
    mainLayout->addWidget(displayGroup);

    // 添加按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *okButton = new QPushButton(tr("确定"), dialog);
    QPushButton *cancelButton = new QPushButton(tr("取消"), dialog);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // 连接按钮信号
    connectDialogButtons(dialog, okButton, cancelButton);

    // 显示为模态对话框
    if (dialog->exec() == QDialog::Accepted) {
        qDebug() << "仪表盘设置已更新: " << this->objectName();
    }

    // 释放资源
    dialog->deleteLater();
}

// 创建数据源设置组
QGroupBox* Dashboard::createDataSourceGroup(QDialog *dialog)
{
    QGroupBox *sourceGroup = new QGroupBox(tr("数据源"), dialog);
    QGridLayout *sourceLayout = new QGridLayout(sourceGroup);

    // 设备类型选择
    QLabel *deviceTypeLabel = new QLabel(tr("设备类型:"), dialog);
    QComboBox *deviceTypeCombo = new QComboBox(dialog);
    deviceTypeCombo->setObjectName("deviceTypeCombo");
    deviceTypeCombo->addItem(tr("Modbus"), 0);
    deviceTypeCombo->addItem(tr("DAQ采集卡"), 1);  // 修正名称：DAQ才是数据采集通道
    deviceTypeCombo->addItem(tr("ECU"), 2);
    deviceTypeCombo->addItem(tr("CustomData"), 3); // 修改：自定义变量改为 CustomData
    sourceLayout->addWidget(deviceTypeLabel, 0, 0);
    sourceLayout->addWidget(deviceTypeCombo, 0, 1);

    // 通道选择
    QLabel *channelLabel = new QLabel(tr("通道:"), dialog);
    QComboBox *channelCombo = new QComboBox(dialog);
    channelCombo->setObjectName("channelCombo");
    sourceLayout->addWidget(channelLabel, 1, 0);
    sourceLayout->addWidget(channelCombo, 1, 1);

    // 公式输入框（初始为隐藏状态）
    // QLabel *formulaLabel = new QLabel(tr("公式:"), dialog); // 第十九处屏蔽
    // formulaLabel->setObjectName("formulaLabel"); // 第十九处屏蔽
    // QLineEdit *formulaEdit = new QLineEdit(dialog); // 第十九处屏蔽
    // formulaEdit->setObjectName("formulaEdit"); // 第十九处屏蔽
    // formulaEdit->setPlaceholderText(tr("输入计算公式，如: A_0 + B_1 * 2")); // 第十九处屏蔽
    // formulaLabel->setVisible(false); // 第十九处屏蔽
    // formulaEdit->setVisible(false); // 第十九处屏蔽
    // sourceLayout->addWidget(formulaLabel, 2, 0); // 第十九处屏蔽
    // sourceLayout->addWidget(formulaEdit, 2, 1); // 第十九处屏蔽
    
    // 帮助提示
    // QLabel *helpLabel = new QLabel(tr("变量格式: A_x (Modbus), B_x (DAQ), C_x (ECU), D_x (自定义)"), dialog); // 第二十处屏蔽
    // helpLabel->setObjectName("helpLabel"); // 第二十处屏蔽
    // helpLabel->setVisible(false); // 第二十处屏蔽
    // sourceLayout->addWidget(helpLabel, 3, 0, 1, 2); // 第二十处屏蔽

    // 辅助函数：更新通道列表内容
    auto updateChannelList = [=](int index) {
        channelCombo->clear();
        // formulaLabel->setVisible(false); // 第二十一处屏蔽
        // formulaEdit->setVisible(false); // 第二十一处屏蔽
        // helpLabel->setVisible(false); // 第二十一处屏蔽

        // 添加通道列表，通道名包含变量名便于后续提取
        switch (index) {
            case 0: // Modbus
            {
                // 检查是否有自定义Modbus通道列表
                QVariant modbusChannelsVar = this->property("modbusChannels");
                if (modbusChannelsVar.isValid()) {
                    QVector<int> modbusChannels = modbusChannelsVar.value<QVector<int>>();
                    if (!modbusChannels.isEmpty()) {
                        // 使用自定义通道列表
                        for (int i = 0; i < modbusChannels.size(); i++) {
                            int channelNum = modbusChannels[i];
                            channelCombo->addItem(tr("通道 %1 (A_%2)").arg(channelNum).arg(channelNum));
                        }
                        qDebug() << "使用自定义Modbus通道列表:" << modbusChannels;
                    } else {
                        // 如果列表为空，使用默认通道0-15
                        for (int i = 0; i < 16; i++) {
                            channelCombo->addItem(tr("通道 %1 (A_%2)").arg(i).arg(i));
                        }
                    }
                } else {
                    // 如果没有自定义通道，使用默认通道0-15
                    for (int i = 0; i < 16; i++) {
                        channelCombo->addItem(tr("通道 %1 (A_%2)").arg(i).arg(i));
                    }
                }
            }
            break;
            case 1: // DAQ
            {
                // 检查是否有自定义DAQ通道列表
                QVariant customChannelsVar = this->property("daqChannels");
                if (customChannelsVar.isValid()) {
                    QVector<int> customChannels = customChannelsVar.value<QVector<int>>();
                    if (!customChannels.isEmpty()) {
                        // 使用自定义通道列表
                        for (int i = 0; i < customChannels.size(); i++) {
                            int channelNum = customChannels[i];
                            channelCombo->addItem(tr("通道 %1 (B_%2)").arg(channelNum).arg(channelNum));
                        }
                        qDebug() << "使用自定义DAQ通道列表:" << customChannels;
                    } else {
                        // 如果列表为空，使用默认通道
                        for (int i = 0; i < 8; i++) {
                            channelCombo->addItem(tr("通道 %1 (B_%2)").arg(i).arg(i));
                        }
                    }
                } else {
                    // 如果没有自定义通道，使用默认通道
                    for (int i = 0; i < 8; i++) {
                        channelCombo->addItem(tr("通道 %1 (B_%2)").arg(i).arg(i));
                    }
                }
            }
            break;
            case 2: // ECU
            {
                QStringList ecuFields = {
                    tr("节气门开度 (C_0)"),
                    tr("发动机转速 (C_1)"),
                    tr("缸温 (C_2)"),
                    tr("排温 (C_3)"),
                    tr("轴温 (C_4)"),
                    tr("燃油压力 (C_5)"),
                    tr("进气温度 (C_6)"),
                    tr("大气压力 (C_7)"),
                    tr("飞行时间 (C_8)")
                };
                
                for (int i = 0; i < ecuFields.size(); i++) {
                    channelCombo->addItem(ecuFields[i]);
                }
            }
            break;
            case 3: // CustomData
            {
                /* // 第二十二处屏蔽 - 开始
                for (int i = 0; i < 8; i++) {
                    channelCombo->addItem(tr("变量 %1 (D_%2)").arg(i).arg(i));
                }
                // 显示公式输入框和帮助信息
                formulaLabel->setVisible(true);
                formulaEdit->setVisible(true);
                helpLabel->setVisible(true);
                
                // 如果有已保存的公式，则显示
                if (!m_formula.isEmpty()) {
                    formulaEdit->setText(m_formula);
                }
                */ // 第二十二处屏蔽 - 结束
                 for (int i = 0; i < 5; ++i) { // customData 固定为5个通道
                    channelCombo->addItem(tr("通道 %1 (D_%2)").arg(i).arg(i));
                 }
            }
            break;
        }
    };

    // 设备类型变化时更新通道列表
    QObject::connect(deviceTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), dialog, updateChannelList);

    // 确定初始设备类型
    int deviceType = 0; // 默认为Modbus
    
    // 如果是自定义变量，设置为对应的类型
    // if (m_isCustomVariable) { // 第二十三处屏蔽
    //     deviceType = 3; // 自定义变量
    // } else 
    if (!m_variableName.isEmpty()) { // 修改：不再检查 isCustomVariable
        // 根据变量名前缀设置设备类型
        if (m_variableName.startsWith("A_")) deviceType = 0; // Modbus
        else if (m_variableName.startsWith("B_")) deviceType = 1; // DAQ
        else if (m_variableName.startsWith("C_")) deviceType = 2; // ECU
        else if (m_variableName.startsWith("D_")) deviceType = 3; // 自定义变量
    }
    
    // 先初始化通道列表
    updateChannelList(deviceType);
    
    // 然后设置设备类型（这将触发信号，但由于已经初始化了通道列表，所以不会有问题）
    deviceTypeCombo->setCurrentIndex(deviceType);
    
    // 在对话框显示后，设置正确的通道选择
    QTimer::singleShot(100, dialog, [=]() {
        // 根据当前变量名选择正确的通道索引
        if (!m_variableName.isEmpty()) {
            for (int i = 0; i < channelCombo->count(); i++) {
                QString text = channelCombo->itemText(i);
                if (text.contains(m_variableName)) {
                    channelCombo->setCurrentIndex(i);
                    break;
                }
            }
        }
        
        // 如果是自定义变量，设置公式
        /* // 第二十六处屏蔽 - 开始
        if (deviceType == 3 && !m_formula.isEmpty() && formulaEdit) {
            formulaEdit->setText(m_formula);
        }
        */ // 第二十六处屏蔽 - 结束
    });

    return sourceGroup;
}

// 创建显示设置组
QGroupBox* Dashboard::createDisplayGroup(QDialog *dialog)
{
    // 创建分组框
    QGroupBox *displayGroup = new QGroupBox(tr("仪表盘显示设置"), dialog);
    QGridLayout *displayLayout = new QGridLayout(displayGroup);

    // 标签名称
    QLabel *labelNameLabel = new QLabel(tr("标签名称:"), dialog);
    QLineEdit *labelNameEdit = new QLineEdit(m_title, dialog);
    labelNameEdit->setObjectName("labelNameEdit");
    displayLayout->addWidget(labelNameLabel, 0, 0);
    displayLayout->addWidget(labelNameEdit, 0, 1);

    // 数据单位
    QLabel *unitLabel = new QLabel(tr("数据单位:"), dialog);
    QLineEdit *unitEdit = new QLineEdit(m_unit, dialog);
    unitEdit->setObjectName("unitEdit");
    displayLayout->addWidget(unitLabel, 1, 0);
    displayLayout->addWidget(unitEdit, 1, 1);

    // 最小值
    QLabel *minValueLabel = new QLabel(tr("最小读数:"), dialog);
    QDoubleSpinBox *minValueSpinBox = new QDoubleSpinBox(dialog);
    minValueSpinBox->setObjectName("minValueSpinBox");
    minValueSpinBox->setRange(-10000, 10000);
    minValueSpinBox->setValue(m_minValue);
    displayLayout->addWidget(minValueLabel, 2, 0);
    displayLayout->addWidget(minValueSpinBox, 2, 1);

    // 最大值
    QLabel *maxValueLabel = new QLabel(tr("最大读数:"), dialog);
    QDoubleSpinBox *maxValueSpinBox = new QDoubleSpinBox(dialog);
    maxValueSpinBox->setObjectName("maxValueSpinBox");
    maxValueSpinBox->setRange(-10000, 10000);
    maxValueSpinBox->setValue(m_maxValue);
    displayLayout->addWidget(maxValueLabel, 3, 0);
    displayLayout->addWidget(maxValueSpinBox, 3, 1);

    // 颜色设置
    QLabel *pointerColorLabel = new QLabel(tr("指针颜色:"), dialog);
    QPushButton *pointerColorButton = new QPushButton(dialog);
    pointerColorButton->setObjectName("pointerColorButton");
    pointerColorButton->setStyleSheet(QString("background-color: %1").arg(m_pointerColor.name()));
    pointerColorButton->setText(m_pointerColor.name());
    displayLayout->addWidget(pointerColorLabel, 4, 0);
    displayLayout->addWidget(pointerColorButton, 4, 1);

    // 颜色选择按钮点击处理
    QObject::connect(pointerColorButton, &QPushButton::clicked, [=]() {
        QColor color = QColorDialog::getColor(m_pointerColor, dialog, tr("选择指针颜色"));
        if (color.isValid()) {
            pointerColorButton->setStyleSheet(QString("background-color: %1").arg(color.name()));
            pointerColorButton->setText(color.name());
        }
    });

    // 指针样式选择
    QLabel *pointerStyleLabel = new QLabel(tr("指针样式:"), dialog);
    QComboBox *pointerStyleCombo = new QComboBox(dialog);
    pointerStyleCombo->setObjectName("pointerStyleCombo");
    pointerStyleCombo->addItem(tr("圆形指针"), PointerStyle_Circle);
    pointerStyleCombo->addItem(tr("指示器指针"), PointerStyle_Indicator);
    pointerStyleCombo->addItem(tr("三角形指针"), PointerStyle_Triangle);
    pointerStyleCombo->addItem(tr("自定义指针"), PointerStyle_Custom);
    pointerStyleCombo->setCurrentIndex(m_pointerStyle);
    displayLayout->addWidget(pointerStyleLabel, 5, 0);
    displayLayout->addWidget(pointerStyleCombo, 5, 1);

    return displayGroup;
}

// 连接对话框按钮信号
void Dashboard::connectDialogButtons(QDialog *dialog, QPushButton *okButton, QPushButton *cancelButton)
{
    // 查找对话框中的控件
    QLineEdit *labelNameEdit = dialog->findChild<QLineEdit*>("labelNameEdit");
    QLineEdit *unitEdit = dialog->findChild<QLineEdit*>("unitEdit");
    QDoubleSpinBox *minValueSpinBox = dialog->findChild<QDoubleSpinBox*>("minValueSpinBox");
    QDoubleSpinBox *maxValueSpinBox = dialog->findChild<QDoubleSpinBox*>("maxValueSpinBox");
    QComboBox *pointerStyleCombo = dialog->findChild<QComboBox*>("pointerStyleCombo");
    QPushButton *pointerColorButton = dialog->findChild<QPushButton*>("pointerColorButton");
    QComboBox *deviceTypeCombo = dialog->findChild<QComboBox*>("deviceTypeCombo");
    QComboBox *channelCombo = dialog->findChild<QComboBox*>("channelCombo");
    // QLineEdit *formulaEdit = dialog->findChild<QLineEdit*>("formulaEdit"); // 第二十四处屏蔽

    // 确定按钮点击事件
    QObject::connect(okButton, &QPushButton::clicked, [=]() {
        // 更新仪表盘显示设置
        this->setTitle(labelNameEdit->text());
        this->setUnit(unitEdit->text());
        this->setMinValue(minValueSpinBox->value());
        this->setMaxValue(maxValueSpinBox->value());
        this->setPointerStyle((PointerStyle)pointerStyleCombo->currentData().toInt());
        
        // 刷新UI以显示新标题和单位
        this->refreshUI();

        // 更新颜色
        QColor color(pointerColorButton->text());
        if (color.isValid()) {
            this->setPointerColor(color);
        }
        
        // 获取设备类型和通道
        int sourceType = deviceTypeCombo->currentIndex();
        int sourceChannel = channelCombo->currentIndex();
        QString channelText = channelCombo->currentText();
        
        // 提取变量名
        QString variableName;
        QRegularExpression rx("\\(([A-Z]_[0-9]+)\\)"); // 匹配 (A_0), (B_1), (C_2), (D_0) 等格式
        QRegularExpressionMatch match = rx.match(channelText);
        if (match.hasMatch()) {
            variableName = match.captured(1);
        } else if (sourceType == 3) { // 自定义变量
            // 为新变量生成名称
            variableName = QString("D_%1").arg(channelCombo->currentIndex());
        }
        
        this->setVariableName(variableName);
        
        // 设置自定义变量属性
        // bool isCustomVar = (sourceType == 3); // 第二十五处屏蔽
        // this->setCustomVariable(isCustomVar); // 第二十五处屏蔽
        
        // 如果是自定义变量，获取公式
        /* // 第二十六处屏蔽 - 开始
        if (isCustomVar && formulaEdit) {
            QString formula = formulaEdit->text();
            this->setFormula(formula);
            emit customVariableFormulaChanged(this->objectName(), formula);
        }
        */ // 第二十六处屏蔽 - 结束

        // 创建设置映射对象
        QMap<QString, QVariant> settings;
        settings["title"] = labelNameEdit->text();
        settings["unit"] = unitEdit->text();
        settings["minValue"] = minValueSpinBox->value();
        settings["maxValue"] = maxValueSpinBox->value();
        settings["pointerColor"] = pointerColorButton->text();
        settings["pointerStyle"] = pointerStyleCombo->currentData().toInt();
        settings["deviceType"] = sourceType;
        settings["channelIndex"] = sourceChannel;
        settings["channelName"] = channelText;
        settings["variableName"] = variableName;
        
        /* // 第二十七处屏蔽 - 开始
        if (isCustomVar && formulaEdit) {
            settings["formula"] = formulaEdit->text();
            settings["isCustomVariable"] = true;
        }
        */ // 第二十七处屏蔽 - 结束

        // 发出信号通知主窗口保存设置
        emit dashboardSettingsChanged(this->objectName(), settings);

        // 关闭对话框
        dialog->accept();
    });

    // 取消按钮点击事件
    QObject::connect(cancelButton, &QPushButton::clicked, [=]() {
        dialog->reject();
    });
}

// 保留旧的计算公式实现作为备用
double Dashboard::calculateFormula(const QString &formula, const QMap<QString, double> &variables)
{
    // 否则使用同步计算（老方法）
    // 预处理公式，替换变量为数值
    QString processedFormula = formula;
    
    // 使用正则表达式查找变量模式（如A_0, B_1等）
    static QRegularExpression varPattern("([A-Z]_[0-9]+)");
    QRegularExpressionMatchIterator matches = varPattern.globalMatch(formula);
    
    // 记录变量替换前后的情况，帮助诊断问题
    bool hasUnknownVars = false;
    QMap<QString, double> usedVars;
    
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString varName = match.captured(1);
        
        if (variables.contains(varName)) {
            // 使用变量表中的值替换变量名
            double value = variables[varName];
            processedFormula.replace(varName, QString::number(value));
            usedVars[varName] = value;
        } else {
            // 如果变量未提供，使用上次的值或默认值0
            if (m_lastVariableValues.contains(varName)) {
                double value = m_lastVariableValues[varName];
                processedFormula.replace(varName, QString::number(value));
                usedVars[varName] = value;
                qDebug() << "变量" << varName << "使用上次的值:" << value;
            } else {
                // 如果完全没有这个变量的历史值，才使用0
                qDebug() << "变量" << varName << "未提供值，使用默认值0";
                processedFormula.replace(varName, "0");
                hasUnknownVars = true;
            }
        }
    }
    
    // 输出处理后的公式，帮助调试
    qDebug() << "处理后的公式:" << processedFormula;
    
    try {
        // 保存当前使用的变量值，用于下次计算
        for (auto it = usedVars.begin(); it != usedVars.end(); ++it) {
            m_lastVariableValues[it.key()] = it.value();
        }
        
        // 如果所有变量都是未知的，并且之前没有有效的计算结果，返回0
        if (hasUnknownVars && !m_hasValidResult) {
            return 0;
        }
        
        // 计算表达式
        double result = evaluateExpression(processedFormula);
        
        // 标记有了有效结果
        m_hasValidResult = true;
        
        return result;
    } catch (const std::exception &e) {
        qDebug() << "公式计算错误:" << e.what();
        return 0; // 计算错误时返回0
    }
}

// 使用调度场算法计算表达式
double Dashboard::evaluateExpression(const QString &expression)
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

// 判断字符是否是操作符
bool Dashboard::isOperator(const QChar &c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')';
}

// 获取操作符优先级
int Dashboard::precedence(const QChar &op)
{
    if (op == '+' || op == '-') 
        return 1;
    if (op == '*' || op == '/') 
        return 2;
    return 0;
}

// 应用操作符进行计算
double Dashboard::applyOperator(double a, double b, QChar op)
{
    switch (op.toLatin1()) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return b != 0 ? a / b : 0; // 防止除以零
        default: return 0;
    }
}
