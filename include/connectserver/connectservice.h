#pragma once

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <mprpc/mprpcchannel.h>
#include <mprpc/mprpcapplication.h>

#include "groupmodel.h"
#include "friendmodel.h"
#include "usermodel.h"
#include "offlinemessagemodel.h"
#include "redis.h"
#include "qq.pb.h"
using namespace muduo;
using namespace muduo::net;
using namespace std;

// 表示处理消息的事件回调方法类型
using MsgHandler = std::function<void(const muduo::net::TcpConnectionPtr &conn, string &, Timestamp)>;

// 聊天服务器业务类
class ConnectService
{
public:
    // 获取单例对象的接口函数
    static ConnectService *instance();
    // 处理注销业务
    void Loginout(const muduo::net::TcpConnectionPtr &conn, string &request, Timestamp time);
    // 处理客户端异常退出
    void clientCloseException(const muduo::net::TcpConnectionPtr &conn);
    // 服务器异常，业务重置方法
    void reset();
    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int, string);

    //=======================================下面由rpc服务器提供=====================================
    // 处理登录业务
    void Login(const muduo::net::TcpConnectionPtr &conn, string &request, Timestamp time);
    // 处理注册业务
    void Register(const muduo::net::TcpConnectionPtr &conn, string &request, Timestamp time);

    // 一对一聊天业务
    void OneChat(const muduo::net::TcpConnectionPtr &conn, string &request, Timestamp time);
    // 群组聊天业务
    void GroupChat(const muduo::net::TcpConnectionPtr &conn, string &request, Timestamp time);
    // 获取当前用户的离线消息  个人聊天信息或者群组消息
    void GetOfflineMsg(const muduo::net::TcpConnectionPtr &conn, string &request, Timestamp time);

    // 添加好友业务
    void AddFriend(const muduo::net::TcpConnectionPtr &conn, string &request, Timestamp time);
    // 获取用户的所有好友信息
    void GetFriendList(const muduo::net::TcpConnectionPtr &conn, string &request, Timestamp time);

    // 创建群组业务
    void CreateGroup(const muduo::net::TcpConnectionPtr &conn, string &request, Timestamp time);
    // 加入群组业务
    void AddGroup(const muduo::net::TcpConnectionPtr &conn, string &request, Timestamp time);
    // 获取用户加入的所有群组信息
    void GetGroupList(const muduo::net::TcpConnectionPtr &conn, string &request, Timestamp time);

private:
    ConnectService();

    // 存储消息id和其对应的业务处理方法
    unordered_map<int, MsgHandler> HandlerMap;
    // 存储在线用户的通信连接
    unordered_map<string, muduo::net::TcpConnectionPtr> _userConnMap;
    // 定义互斥锁，保证_userConnMap的线程安全
    mutex _connMutex;

    // redis操作对象
    Redis _redis;
};