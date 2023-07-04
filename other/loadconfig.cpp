#include <mprpc/mprpcapplication.h>
#include <string>
#include <iostream>
using namespace std;

// 聊天客户端程序实现，main线程用作发送线程，子线程用作接收线程
int main(int argc, char **argv)
{

    MprpcApplication::Init(argc, argv);
    string ip = MprpcApplication::GetInstance().GetConfig().Load("chatserverip").c_str();
    cout << ip << endl;
    uint16_t port = stoi(MprpcApplication::GetInstance().GetConfig().Load("chatserverport"));
    cout << ip << endl;

    // 创建client端的socket
    // int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    // if (-1 == clientfd)
    // {
    //     cerr << "socket create error" << endl;
    //     exit(-1);
    // }

    // // 填写client需要连接的server信息ip+port
    // sockaddr_in server;
    // memset(&server, 0, sizeof(sockaddr_in));

    // server.sin_family = AF_INET;
    // server.sin_port = htons(stoi(MprpcApplication::GetInstance().GetConfig().Load("chatserverport")));
    // server.sin_addr.s_addr = inet_addr(MprpcApplication::GetInstance().GetConfig().Load("chatserverip").c_str());
}
