#ifndef WEBSOCKETTHREAD_H
#define WEBSOCKETTHREAD_H

#include <QObject>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>
#include <QHostAddress>
#include <QDateTime>
#include <QVector>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QCoreApplication>
#include <QDebug>
#include "snapshotthread.h"

class WebSocketThread : public QObject
{
    Q_OBJECT

public:
    explicit WebSocketThread(QObject *parent = nullptr);
    ~WebSocketThread();

    void startServer(int port = 8080);
    void stopServer();
    bool isRunning() const;

    // 测试WebSocket连接和数据传输
    Q_INVOKABLE void testConnection();

public slots:
    // 新增：处理完整数据快照的槽函数
    void handleDataSnapshot(const DataSnapshot &snapshot, int snapshotCount);
    
    // 以下旧方法标记为废弃，将在未来版本移除
    void handleModbusData(const QJsonObject &data, int interval); // 废弃
    void handleModbusRawData(QVector<double> resultdata, qint64 readTimeInterval); // 废弃
    
    void sendMessageToAllClients(const QString &message);

signals:
    // 连接状态信号
    void clientConnected(const QString &clientInfo);
    void clientDisconnected(const QString &clientInfo);
    void serverStarted(bool success, const QString &message);
    void serverStopped();
    void serverError(QString errorMessage);
    void messageReceived(QString message);

private slots:
    // WebSocket服务器槽函数
    void onNewConnection();
    void onSocketDisconnected();
    void onTextMessageReceived(const QString &message);
    void onSocketError(QAbstractSocket::SocketError error);
    
    // HTTP服务器槽函数
    void onHttpNewConnection();
    void onHttpReadyRead();

private:
    QWebSocketServer *m_server;
    QList<QWebSocket*> m_clients;
    QHostAddress m_serverAddress;
    int m_serverPort;
    bool m_running;
    
    // HTTP服务器相关成员
    QTcpServer *m_httpServer;
    QList<QTcpSocket*> m_httpClients;
    
    // 发送数据给所有客户端
    void broadcastMessage(const QJsonObject &data);
    
    // 新增：将DataSnapshot转换为JSON格式
    QJsonObject convertSnapshotToJson(const DataSnapshot &snapshot, int snapshotCount);
    
    // 旧方法标记为废弃
    QJsonObject convertModbusDataToJson(const QVector<double> &data, int startAddress); // 废弃
    
    // 处理HTTP请求
    void handleHttpRequest(QTcpSocket *socket, const QString &request);
    void sendHttpResponse(QTcpSocket *socket, const QByteArray &content, 
                          const QString &contentType, int statusCode = 200);
    QByteArray readHtmlFile(const QString &fileName);
};

#endif // WEBSOCKETTHREAD_H