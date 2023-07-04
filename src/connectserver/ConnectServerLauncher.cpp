#include "connectserver.h"
#include "connectservice.h"
#include <iostream>
#include <muduo/net/InetAddress.h>
#include <muduo/base/Timestamp.h>
#include <mprpc/mprpcapplication.h>
#include <signal.h>
using namespace std;

// 处理服务器ctrl+c结束后，重置user的状态信息
void resetHandler(int)
{
    ConnectService::instance()->reset();
    exit(0);
}

int main(int argc, char **argv)
{

    MprpcApplication::Init(argc, argv);

    signal(SIGINT, resetHandler);

    EventLoop loop;

    InetAddress addr(MprpcApplication::GetInstance().GetConfig().Load("connectserverip").c_str(), stoi(MprpcApplication::GetInstance().GetConfig().Load("connectserverport")));

    ConnectServer server(&loop, addr, "ConnectServer");

    server.start();
    loop.loop();

    return 0;
}