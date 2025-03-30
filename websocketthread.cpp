#include "websocketthread.h"
#include <QNetworkInterface>
#include <QDebug>
#include <QDir>
#include <QMimeDatabase>
#include <QDateTime>
#include <QTimer>

WebSocketThread::WebSocketThread(QObject *parent)
    : QObject(parent)
    , m_server(nullptr)
    , m_httpServer(nullptr)
    , m_serverPort(8080)
    , m_running(false)
{
    // 设置服务器地址为任意地址以便局域网内访问
    m_serverAddress = QHostAddress::Any;
    
    // 默认不启动服务器，由用户通过按钮控制
    qDebug() << "WebSocketThread已创建，等待用户启动";
}

WebSocketThread::~WebSocketThread()
{
    stopServer();
    
    // 清理HTTP服务器
    if (m_httpServer) {
        m_httpServer->close();
        delete m_httpServer;
    }
    
    // 清理客户端连接
    qDeleteAll(m_httpClients);
    m_httpClients.clear();
}

void WebSocketThread::startServer(int port)
{
    if (m_running) {
        qDebug() << "WebSocket服务器已经在运行中";
        return;
    }

    qDebug() << "尝试启动WebSocket服务器，端口:" << port;
    m_serverPort = port;

    // 将所有初始化操作放在当前线程执行，避免跨线程访问
    
    // 初始化WebSocket服务器
    if (!m_server) {
        qDebug() << "创建新的WebSocketServer实例";
        m_server = new QWebSocketServer(QStringLiteral("Modbus Data Server"),
                                        QWebSocketServer::NonSecureMode,
                                        this);
        
        // 连接信号槽
        connect(m_server, &QWebSocketServer::newConnection,
                this, &WebSocketThread::onNewConnection);
        // 添加服务器错误信号连接
        connect(m_server, &QWebSocketServer::serverError,
                this, [this]() {
                    QString errorMsg = m_server->errorString();
                    qDebug() << "WebSocket服务器错误:" << errorMsg;
                    emit serverError("WebSocket服务器错误: " + errorMsg);
                });
                
        qDebug() << "WebSocketServer实例创建成功";
    }

    // 启动WebSocket服务器
    qDebug() << "正在启动WebSocket服务器，地址:" << m_serverAddress.toString() << "端口:" << m_serverPort;
    if (m_server->listen(m_serverAddress, m_serverPort)) {
        m_running = true;
        qDebug() << "WebSocket服务器已启动，地址:" << m_serverAddress.toString() 
                 << "端口:" << m_serverPort;
        
        // 创建和启动HTTP服务器
        if (!m_httpServer) {
            m_httpServer = new QTcpServer(this);
            connect(m_httpServer, &QTcpServer::newConnection, 
                    this, &WebSocketThread::onHttpNewConnection);
        }
        
        // 明确使用QHostAddress::Any确保监听所有网络接口
        if (m_httpServer->listen(QHostAddress::Any, m_serverPort + 1)) {
            qDebug() << "HTTP服务器已启动，地址: 0.0.0.0 (所有网络接口)"
                     << "端口:" << (m_serverPort + 1);
            
            // 打印可访问的URL
            qDebug() << "本地访问地址: http://localhost:" << (m_serverPort + 1) << "/websocket_client.html";
            
            // 显示所有可能的访问地址
            QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
            qDebug() << "可用网络接口:";
            for (const QHostAddress &address : addresses) {
                if (address.protocol() == QAbstractSocket::IPv4Protocol && 
                    address != QHostAddress::LocalHost) {
                    qDebug() << "  - IP:" << address.toString() 
                             << "访问URL: http://" << address.toString() << ":" << (m_serverPort + 1)
                             << "/websocket_client.html";
                }
            }
            
            emit serverStarted(true, QString("服务器已启动: WebSocket(端口%1) 和 HTTP(端口%2)")
                            .arg(m_serverPort)
                            .arg(m_serverPort + 1));
        } else {
            qDebug() << "HTTP服务器启动失败:" << m_httpServer->errorString();
            emit serverStarted(true, QString("WebSocket服务器已启动，但HTTP服务器启动失败: %1")
                            .arg(m_httpServer->errorString()));
        }
    } else {
        qDebug() << "WebSocket服务器启动失败:" << m_server->errorString();
        emit serverStarted(false, QString("服务器启动失败: %1")
                        .arg(m_server->errorString()));
    }
}

void WebSocketThread::stopServer()
{
    if (!m_running)
        return;

    // 关闭所有WebSocket客户端连接
    for (QWebSocket *client : m_clients) {
        client->close();
    }
    qDeleteAll(m_clients);
    m_clients.clear();

    // 关闭所有HTTP客户端连接
    for (QTcpSocket *client : m_httpClients) {
        client->close();
    }
    qDeleteAll(m_httpClients);
    m_httpClients.clear();

    // 关闭WebSocket服务器
    if (m_server) {
        m_server->close();
        m_running = false;
    }
    
    // 关闭HTTP服务器
    if (m_httpServer) {
        m_httpServer->close();
    }
    
    emit serverStopped();
    qDebug() << "服务器已停止";
}

// 处理HTTP新连接
void WebSocketThread::onHttpNewConnection()
{
    QTcpSocket *socket = m_httpServer->nextPendingConnection();
    if (!socket)
        return;
        
    m_httpClients.append(socket);
        
    connect(socket, &QTcpSocket::readyRead, this, &WebSocketThread::onHttpReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, [this, socket]() {
        m_httpClients.removeAll(socket);
        socket->deleteLater();
    });
    
    qDebug() << "HTTP客户端已连接:" << socket->peerAddress().toString() << ":" << socket->peerPort();
}

// 处理HTTP数据
void WebSocketThread::onHttpReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket)
        return;
        
    // 确保读取完整的HTTP请求
    if (socket->bytesAvailable() < 4) { // 至少需要"GET "的长度
        return; // 等待更多数据
    }
        
    QByteArray request = socket->readAll();
    
    // 记录完整的HTTP请求进行调试
    qDebug() << "收到HTTP请求数据长度:" << request.size() << "字节";
    qDebug() << "HTTP请求前100字节:" << request.left(100);
    
    // 处理请求
    handleHttpRequest(socket, QString::fromUtf8(request));
}

// 处理HTTP请求
void WebSocketThread::handleHttpRequest(QTcpSocket *socket, const QString &request)
{
    // 简单解析HTTP请求
    QStringList lines = request.split("\r\n");
    if (lines.isEmpty()) {
        qDebug() << "空HTTP请求";
        sendHttpResponse(socket, "Empty Request", "text/plain", 400);
        return;
    }
        
    QStringList requestLine = lines[0].split(" ");
    if (requestLine.size() < 3) {
        qDebug() << "无效的HTTP请求格式:" << lines[0];
        sendHttpResponse(socket, "Invalid Request", "text/plain", 400);
        return;
    }
        
    QString method = requestLine[0];
    QString path = requestLine[1];
    
    qDebug() << "收到HTTP请求:" << method << path << "从" << socket->peerAddress().toString();
    
    // 只处理GET请求
    if (method != "GET") {
        qDebug() << "不支持的HTTP方法:" << method;
        sendHttpResponse(socket, "Method not allowed", "text/plain", 405);
        return;
    }
    
    // 处理根路径请求，重定向到websocket_client.html
    if (path == "/" || path == "/index.html") {
        path = "/websocket_client.html";
        qDebug() << "将根路径请求重定向到:" << path;
    }
    
    // 移除URL参数
    if (path.contains("?")) {
        path = path.left(path.indexOf("?"));
        qDebug() << "移除URL参数，实际请求路径:" << path;
    }
    
    // 移除开头的斜杠并读取文件
    QString fileName = path.mid(1);
    
    // 如果文件名为空，默认为websocket_client.html
    if (fileName.isEmpty()) {
        fileName = "websocket_client.html";
        qDebug() << "文件名为空，使用默认文件:" << fileName;
    }
    
    // 在日志中显示更多的文件路径信息
    qDebug() << "尝试读取文件:" << fileName;
    qDebug() << "当前工作目录:" << QDir::currentPath();
    qDebug() << "应用程序目录:" << QCoreApplication::applicationDirPath();
    
    QByteArray content = readHtmlFile(fileName);
    
    if (!content.isEmpty()) {
        // 确定内容类型
        QString contentType = "text/html";
        if (fileName.endsWith(".css"))
            contentType = "text/css";
        else if (fileName.endsWith(".js"))
            contentType = "application/javascript";
        else if (fileName.endsWith(".json"))
            contentType = "application/json";
        else if (fileName.endsWith(".png"))
            contentType = "image/png";
        else if (fileName.endsWith(".jpg") || fileName.endsWith(".jpeg"))
            contentType = "image/jpeg";
        
        qDebug() << "发送HTTP响应，内容类型:" << contentType << "内容长度:" << content.size();
        sendHttpResponse(socket, content, contentType);
    } else {
        // 尝试提供一个内嵌的默认HTML页面
        if (fileName == "websocket_client.html") {
            QByteArray defaultHtml = "<!DOCTYPE html><html><head><title>WebSocket测试</title></head><body>"
                                    "<h1>WebSocket测试客户端</h1>"
                                    "<p>状态: <span id='status'>未连接</span></p>"
                                    "<button id='connect'>连接</button>"
                                    "<div id='messages'></div>"
                                    "<script>"
                                    "document.getElementById('connect').onclick = function() {"
                                    "  var ws = new WebSocket('ws://' + window.location.hostname + ':8080');"
                                    "  ws.onopen = function() { document.getElementById('status').textContent = '已连接'; };"
                                    "  ws.onmessage = function(evt) {"
                                    "    var msg = document.createElement('p');"
                                    "    msg.textContent = '收到: ' + evt.data;"
                                    "    document.getElementById('messages').appendChild(msg);"
                                    "  };"
                                    "  ws.onclose = function() { document.getElementById('status').textContent = '已断开'; };"
                                    "};"
                                    "</script></body></html>";
            sendHttpResponse(socket, defaultHtml, "text/html");
            qDebug() << "发送默认HTML页面";
        } else {
            qDebug() << "文件未找到:" << fileName;
            sendHttpResponse(socket, "File not found: " + fileName.toUtf8(), "text/plain", 404);
        }
    }
}

// 发送HTTP响应
void WebSocketThread::sendHttpResponse(QTcpSocket *socket, const QByteArray &content, 
                                      const QString &contentType, int statusCode)
{
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "尝试发送HTTP响应到断开的连接";
        return;
    }

    QByteArray response;
    QString statusMessage = (statusCode == 200) ? "OK" : 
                           (statusCode == 404) ? "Not Found" : 
                           (statusCode == 400) ? "Bad Request" : 
                           (statusCode == 405) ? "Method Not Allowed" : "Error";
    
    // 构建HTTP响应头
    response.append(QString("HTTP/1.1 %1 %2\r\n").arg(statusCode).arg(statusMessage).toUtf8());
    response.append(QString("Content-Type: %1; charset=utf-8\r\n").arg(contentType).toUtf8());
    response.append(QString("Content-Length: %1\r\n").arg(content.size()).toUtf8());
    response.append("Connection: close\r\n");
    // 添加CORS头，允许所有来源
    response.append("Access-Control-Allow-Origin: *\r\n");
    response.append("\r\n");
    response.append(content);
    
    // 写入响应
    socket->write(response);
    socket->flush();
    
    // 不要立即断开连接，让客户端先接收数据
    QTimer::singleShot(100, socket, [socket]() {
        if (socket && socket->state() == QAbstractSocket::ConnectedState) {
            socket->disconnectFromHost();
        }
    });
    
    qDebug() << "HTTP响应已发送，状态码:" << statusCode << "内容长度:" << content.size();
}

// 读取HTML文件
QByteArray WebSocketThread::readHtmlFile(const QString &fileName)
{
    QByteArray content;
    
    // 尝试在不同位置寻找文件
    QStringList searchPaths = {
        QDir::currentPath() + "/" + fileName,
        QCoreApplication::applicationDirPath() + "/" + fileName,
        QCoreApplication::applicationDirPath() + "/Debug/" + fileName,
        QCoreApplication::applicationDirPath() + "/Release/" + fileName,
        // 添加项目源代码目录
        QDir::currentPath() + "/../" + fileName
    };
    
    qDebug() << "查找文件的路径:";
    for (const QString &path : searchPaths) {
        qDebug() << "  " << path;
        QFile file(path);
        if (file.exists()) {
            qDebug() << "  文件存在!";
            if (file.open(QIODevice::ReadOnly)) {
                content = file.readAll();
                file.close();
                qDebug() << "找到并读取文件:" << path << "大小:" << content.size() << "字节";
                break;
            } else {
                qDebug() << "  文件存在但无法打开:" << file.errorString();
            }
        }
    }
    
    if (content.isEmpty()) {
        qDebug() << "未找到文件:" << fileName;
    }
    
    return content;
}

bool WebSocketThread::isRunning() const
{
    return m_running;
}

void WebSocketThread::handleModbusData(const QJsonObject &data, int interval)
{
    if (!m_running || m_clients.isEmpty())
        return;

    // 添加时间间隔信息
    QJsonObject dataWithInterval = data;
    dataWithInterval["interval"] = interval;
    
    // 广播消息给所有客户端
    broadcastMessage(dataWithInterval);
}

void WebSocketThread::handleModbusRawData(QVector<double> resultdata, qint64 readTimeInterval)
{
    // 只有在服务器运行且有客户端连接的情况下才处理数据
    if (!m_running) {
        return;
    }
    
    if (m_clients.isEmpty()) {
        // 没有客户端连接，跳过数据处理
        return;
    }

    // 获取起始地址（这里可以从UI或其他地方获取，暂时使用0）
    int startAddress = 0;
    
    // 转换数据为JSON
    QJsonObject jsonData = convertModbusDataToJson(resultdata, startAddress);
    
    // 添加时间间隔信息
    jsonData["interval"] = (int)readTimeInterval;
    
    // 广播消息给所有客户端
    broadcastMessage(jsonData);
    
    qDebug() << "发送数据到" << m_clients.size() << "个WebSocket客户端";
}

QJsonObject WebSocketThread::convertModbusDataToJson(const QVector<double> &data, int startAddress)
{
    QJsonObject modbusData;
    QJsonArray registersArray;
    
    // 遍历所有寄存器数据
    for (int i = 0; i < data.size(); ++i)
    {
        // 创建寄存器对象
        QJsonObject registerObj;
        int registerAddress = i + startAddress;
        
        registerObj["address"] = registerAddress;
        registerObj["value"] = data[i];
        registerObj["description"] = QString("寄存器 %1").arg(registerAddress);
        
        // 添加到数组
        registersArray.append(registerObj);
    }
    
    // 构建完整的JSON对象
    modbusData["registers"] = registersArray;
    modbusData["timestamp"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    modbusData["data_count"] = data.size();
    
    return modbusData;
}

void WebSocketThread::onNewConnection()
{
    QWebSocket *socket = m_server->nextPendingConnection();
    
    if (socket) {
        connect(socket, &QWebSocket::disconnected, 
                this, &WebSocketThread::onSocketDisconnected);
        connect(socket, &QWebSocket::textMessageReceived,
                this, &WebSocketThread::onTextMessageReceived);
        // 添加错误信号连接
        connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
                this, &WebSocketThread::onSocketError);

        m_clients << socket;
        
        QString clientInfo = QString("%1:%2").arg(socket->peerAddress().toString())
                                            .arg(socket->peerPort());
        emit clientConnected(clientInfo);
        qDebug() << "新的WebSocket客户端连接:" << clientInfo;
    }
}

void WebSocketThread::onSocketDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket*>(sender());
    if (client) {
        QString clientInfo = QString("%1:%2").arg(client->peerAddress().toString())
                                            .arg(client->peerPort());
        
        m_clients.removeAll(client);
        client->deleteLater();
        
        emit clientDisconnected(clientInfo);
        qDebug() << "WebSocket客户端断开连接:" << clientInfo;
    }
}

void WebSocketThread::onTextMessageReceived(const QString &message)
{
    QWebSocket *client = qobject_cast<QWebSocket*>(sender());
    if (client) {
        qDebug() << "收到WebSocket客户端消息:" << message;
        
        // 这里可以处理来自客户端的命令
        // 如果需要的话，可以实现更多的命令处理逻辑
    }
}

// 添加缺失的onSocketError实现
void WebSocketThread::onSocketError(QAbstractSocket::SocketError error)
{
    QWebSocket *client = qobject_cast<QWebSocket*>(sender());
    QString errorString = client ? client->errorString() : "未知错误";
    QString clientInfo = client ? QString("%1:%2").arg(client->peerAddress().toString()).arg(client->peerPort()) : "未知客户端";
    
    qDebug() << "WebSocket错误:" << errorString << "客户端:" << clientInfo << "错误代码:" << error;
    
    // 发送错误信号
    emit serverError("WebSocket连接错误: " + errorString);
}

void WebSocketThread::broadcastMessage(const QJsonObject &data)
{
    QJsonDocument doc(data);
    QString message = doc.toJson(QJsonDocument::Compact);
    
    for (QWebSocket *client : m_clients) {
        client->sendTextMessage(message);
    }
}

void WebSocketThread::sendMessageToAllClients(const QString &message)
{
    if (!m_running || m_clients.isEmpty()) {
        qDebug() << "无法发送消息: 服务器未运行或无客户端连接";
        return;
    }
    
    qDebug() << "向所有客户端(" << m_clients.size() << "个)发送消息:" << message;
    
    foreach(QWebSocket *client, m_clients) {
        if (client && client->isValid()) {
            client->sendTextMessage(message);
        }
    }
}

void WebSocketThread::testConnection()
{
    if (!m_running) {
        qDebug() << "错误: 无法进行连接测试，WebSocket服务器未运行";
        emit serverError("无法进行连接测试，WebSocket服务器未运行");
        return;
    }
    
    qDebug() << "开始WebSocket连接测试...";
    qDebug() << "客户端连接数量:" << m_clients.size();
    
    // 创建测试数据
    QVector<double> testData;
    for (int i = 0; i < 10; i++) {
        testData.append(i * 1.5);
    }
    
    // 发送测试数据
    handleModbusRawData(testData, 500);
    
    // 发送文本测试消息
    sendMessageToAllClients("这是一条WebSocket测试消息 - " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
    
    qDebug() << "WebSocket测试数据已发送";
    
    // 显示服务器信息
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    qDebug() << "可用网络接口:";
    for (const QHostAddress &address : addresses) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && 
            address != QHostAddress::LocalHost) {
            qDebug() << "  - " << address.toString() 
                     << " (WebSocket: ws://" << address.toString() << ":" << m_serverPort << ")";
            qDebug() << "  - " << "HTTP访问: http://" << address.toString() << ":" << (m_serverPort + 1) << "/websocket_client.html";
        }
    }
    
    // 给所有可能的本地连接显示提示
    qDebug() << "本地测试地址:";
    qDebug() << "  - ws://localhost:" << m_serverPort;
    qDebug() << "  - http://localhost:" << (m_serverPort + 1) << "/websocket_client.html";
    qDebug() << "  - ws://127.0.0.1:" << m_serverPort;
    qDebug() << "  - http://127.0.0.1:" << (m_serverPort + 1) << "/websocket_client.html";
}