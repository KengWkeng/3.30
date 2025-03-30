#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <QObject>
#include <QHostAddress>

// 前向声明
class WebSocketThread;

/**
 * @brief WebSocketServerManager类
 * 用于管理WebSocket服务器的启动和监控
 * 提供静态方法以确保WebSocket服务器正常运行
 */
class WebSocketServerManager {
public:
    /**
     * @brief 确保WebSocket服务器已启动
     * @param server WebSocketThread对象指针
     * @param port 服务器端口号，默认为8080
     * @return 服务器是否成功运行
     */
    static bool ensureServerStarted(WebSocketThread* server, int port = 8080);

private:
    /**
     * @brief 打印网络信息以帮助调试
     * @param port 服务器端口号
     */
    static void printNetworkInfo(int port);
};

#endif // WEBSOCKETSERVER_H
