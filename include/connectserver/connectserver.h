#pragma once

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Timestamp.h>
#include <string>
#include <iostream>
using namespace std;
// 聊天服务器的主类

class ConnectServer
{
public:
    // 初始化聊天服务器对象
    ConnectServer(muduo::net::EventLoop *loop,
                  const muduo::net::InetAddress &listenAddr,
                  const string &nameArg);

    // 启动服务
    void start();

private:
    // 上报链接相关信息的回调函数
    void onConnection(const muduo::net::TcpConnectionPtr &);

    // 上报读写事件相关信息的回调函数
    void onMessage(const muduo::net::TcpConnectionPtr &,
                   muduo::net::Buffer *,
                   muduo::Timestamp);

    muduo::net::TcpServer _server; // 组合的muduo库，实现服务器功能的类对象
    muduo::net::EventLoop *_loop;  // 指向事件循环对象的指针
};