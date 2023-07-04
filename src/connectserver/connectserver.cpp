#include "connectserver.h"
#include "connectservice.h"

#include <iostream>
#include <functional>
using namespace std;
using namespace placeholders;

// 初始化聊天服务器对象
ConnectServer::ConnectServer(EventLoop *loop,
                             const InetAddress &listenAddr,
                             const std::string &nameArg)
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册链接回调
    _server.setConnectionCallback(std::bind(&ConnectServer::onConnection, this, _1));

    // 注册消息回调
    _server.setMessageCallback(std::bind(&ConnectServer::onMessage, this, _1, _2, _3));

    // 设置线程数量
    _server.setThreadNum(4);
}

// 启动服务
void ConnectServer::start()
{
    _server.start();
}

// 上报链接相关信息的回调函数
void ConnectServer::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        cout << "客户端name:" << conn->name() << "连接成功!" << endl;
    }
    // 客户端断开链接
    if (!conn->connected())
    {
        cout << "客户端name:" << conn->name() << "断开连接!" << endl;
        ConnectService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

// 上报读写事件相关信息的回调函数
void ConnectServer::onMessage(const TcpConnectionPtr &conn,
                              Buffer *buffer,
                              Timestamp time)
{
    std::string buf = buffer->retrieveAllAsString();
    fixbug::protobuffer pbuf;
    pbuf.ParseFromString(buf);

    uint32_t msgtype = pbuf.protobuftype();

    // 根据header_size读取数据头的原始字符流，反序列化数据，得到rpc请求的详细信息
    buf = pbuf.protobufstr();

    // 达到的目的：完全解耦网络模块的代码和业务模块的代码
    // 通过js["msgid"] 获取=》业务handler=》conn  js  time
    auto msgHandler = ConnectService::instance()->getHandler(msgtype);
    // 回调消息绑定好的事件处理器，来执行相应的业务处理
    msgHandler(conn, buf, time);
}