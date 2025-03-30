#include "websocketserver.h"
#include "websocketthread.h"
#include <QNetworkInterface>
#include <QHostInfo>
#include <QDebug>

class WebSocketServerManager {
public:
    static bool ensureServerStarted(WebSocketThread* server, int port = 8080) {
        if (!server) {
            qDebug() << "错误: WebSocket服务器对象为空";
            return false;
        }

        // 确保服务器已启动
        if (!server->isRunning()) {
            server->startServer(port);
            
            // 输出服务器地址信息，帮助调试
            printNetworkInfo(port);
        }

        return server->isRunning();
    }

private:
    static void printNetworkInfo(int port) {
        qDebug() << "WebSocket服务器端口:" << port;
        qDebug() << "HTTP服务器端口:" << (port + 1);
        
        // 打印本地主机名
        QString localHostName = QHostInfo::localHostName();
        qDebug() << "本地主机名:" << localHostName;
        
        // 打印所有网络接口信息
        QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
        qDebug() << "可用网络接口:";
        for (const QHostAddress &address : addresses) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && 
                address != QHostAddress::LocalHost) {
                qDebug() << "  - " << address.toString() 
                         << " (WebSocket: ws://" << address.toString() << ":" << port << ")";
            }
        }
        
        qDebug() << "本地访问地址: http://localhost:" << (port + 1) << "/websocket_client.html";
    }
};

// 下面是你可以在MainWindow类中添加的方法，确保WebSocket服务器启动
/*
void MainWindow::ensureWebSocketServerStarted() {
    if (wsTh) {
        bool success = WebSocketServerManager::ensureServerStarted(wsTh);
        if (success) {
            ui->plainReceive->appendPlainText("WebSocket服务器已启动");
        } else {
            ui->plainReceive->appendPlainText("WebSocket服务器启动失败");
        }
    }
}
*/
