#include "mainwindow.h"

#include <QApplication>
#include <QScreen>
#include <QStyle>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    
    // 设置初始窗口大小为屏幕可用区域的80%
    QScreen* screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int width = screenGeometry.width() * 0.8;
    int height = screenGeometry.height() * 0.8;
    w.resize(width, height);
    
    // 居中显示窗口
    w.setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            w.size(),
            screenGeometry
        )
    );
    
    w.show();
    return a.exec();
}
