#include "connectservice.h"
#include "public.h"
#include <muduo/base/Logging.h>
#include <vector>
using namespace std;

// 获取单例对象的接口函数
ConnectService *ConnectService::instance()
{
    static ConnectService service;
    return &service;
}

// 注册消息以及对应的Handler回调操作
ConnectService::ConnectService()
{
    // 用户基本业务管理相关事件处理回调注册
    HandlerMap.insert({LOGIN, std::bind(&ConnectService::Login, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    HandlerMap.insert({LOGINOUT, std::bind(&ConnectService::Loginout, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    HandlerMap.insert({REG, std::bind(&ConnectService::Register, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    HandlerMap.insert({ONE_CHAT, std::bind(&ConnectService::OneChat, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    HandlerMap.insert({ADD_FRIEND, std::bind(&ConnectService::AddFriend, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});

    // 群组业务管理相关事件处理回调注册
    HandlerMap.insert({CREATE_GROUP, std::bind(&ConnectService::CreateGroup, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    HandlerMap.insert({ADD_GROUP, std::bind(&ConnectService::AddGroup, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    HandlerMap.insert({GROUP_CHAT, std::bind(&ConnectService::GroupChat, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    HandlerMap.insert({GET_FRIENDLIST, std::bind(&ConnectService::GetFriendList, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    HandlerMap.insert({GET_GROUPLIST, std::bind(&ConnectService::GetGroupList, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});
    HandlerMap.insert({GET_OFFLINEMSG, std::bind(&ConnectService::GetOfflineMsg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)});

    // 连接redis服务器
    if (_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ConnectService::handleRedisSubscribeMessage, this, std::placeholders::_1, std::placeholders::_2));
    }
}

// 服务器异常，业务重置方法
void ConnectService::reset()
{
    // 把online状态的用户，设置成offline
    UserModel::GetInstance().resetState();
}

// 获取消息对应的处理器
MsgHandler ConnectService::getHandler(int msgid)
{
    // 记录错误日志，msgid没有对应的事件处理回调
    auto it = HandlerMap.find(msgid);
    if (it == HandlerMap.end())
    {
        // 返回一个默认的处理器，空操作
        return [=](const TcpConnectionPtr &conn, string &requeststr, Timestamp)
        {
        };
    }
    else
    {
        return HandlerMap[msgid];
    }
}

// 处理客户端异常退出
void ConnectService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                // 从map表删除用户的链接信息
                user.setUserId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(stoi(user.getUserId()));

    // 更新用户的状态信息
    if (user.getUserId() != "-1")
    {
        user.setUserState("offline");
        UserModel::GetInstance().updateState(user);
    }
}

// 从redis消息队列中获取订阅的消息
void ConnectService::handleRedisSubscribeMessage(int useridint, string msg)
{
    string userid = to_string(useridint);
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    OfflineMsgModel::GetInstance().insert(userid, msg);
}

// 处理注销业务
void ConnectService::Loginout(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
{
    fixbug::LoginoutRequest loginoutrequest;
    loginoutrequest.ParseFromString(requeststr);
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(loginoutrequest.userid());
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(stoi(loginoutrequest.userid()));

    // 更新用户的状态信息
    User user(loginoutrequest.userid(), "", "", "offline");
    UserModel::GetInstance().updateState(user);
}

/////////////////////////////////////////////////rpc业务////////////////////////////////////////////////////////////////////

void ConnectService::Login(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
{
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());

    fixbug::LoginRequest loginrequest;
    loginrequest.ParseFromString(requeststr);
    fixbug::LoginResponse loginresponse;

    MprpcController controller;
    stub.Login(&controller, &loginrequest, &loginresponse, nullptr);

    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    if (loginresponse.result().success())
    {
        // 登录成功，记录用户连接信息
        {
            lock_guard<mutex> lock(_connMutex);
            _userConnMap.insert({loginrequest.userid(), conn});
            // LOG_INFO("userid:%s,curmapsize:%s", loginrequest.userid().c_str(), to_string(_userConnMap.size()).c_str());
        }
    }

    fixbug::protobuffer buffer;
    buffer.set_protobuftype(LOGIN_ACK);
    buffer.set_protobufstr(loginresponse.SerializeAsString());
    string responsestr = buffer.SerializeAsString();
    conn->send(responsestr);
}

// 处理注册业务  name  password
void ConnectService::Register(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
{
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());

    fixbug::RegisterRequest registerrequest;
    registerrequest.ParseFromString(requeststr);
    fixbug::RegisterResponse registerresponse;

    MprpcController controller;
    stub.Register(&controller, &registerrequest, &registerresponse, nullptr);

    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }

    fixbug::protobuffer buffer;
    buffer.set_protobuftype(REG_ACK);
    buffer.set_protobufstr(registerresponse.SerializeAsString());
    string responsestr = buffer.SerializeAsString();
    conn->send(responsestr);
}

// 一对一聊天业务
void ConnectService::OneChat(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
{
    fixbug::OneChatRequest onechatrequest;
    onechatrequest.ParseFromString(requeststr);
    fixbug::OneChatResponse onechatresponse;
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(onechatrequest.friendid());
        if (it != _userConnMap.end())
        {
            {  
                fixbug::protobuffer buffer;
                buffer.set_protobuftype(ONE_CHAT_MSG);
                fixbug::ChatMessage cmsg = onechatrequest.chatmsg();
                buffer.set_protobufstr(cmsg.SerializeAsString());
                string responsestr = buffer.SerializeAsString();
                it->second->send(responsestr);
            }

            {
                onechatresponse.mutable_result()->set_success(true);
                fixbug::protobuffer buffer;
                buffer.set_protobuftype(ONE_CHAT_ACK);
                buffer.set_protobufstr(onechatresponse.SerializeAsString());
                string responsestr = buffer.SerializeAsString();
                conn->send(responsestr);
            }

            return;
        }
    }
    fixbug::MsgServiceRpc_Stub stub(new MprpcChannel());

    MprpcController controller;
    stub.OneChat(&controller, &onechatrequest, &onechatresponse, nullptr);

    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    fixbug::protobuffer buffer;
    buffer.set_protobuftype(ONE_CHAT_ACK);
    buffer.set_protobufstr(onechatresponse.SerializeAsString());
    string responsestr = buffer.SerializeAsString();
    conn->send(responsestr);
}

// 群组聊天业务
void ConnectService::GroupChat(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
{
    fixbug::GroupChatRequest groupchatrequest;
    groupchatrequest.ParseFromString(requeststr);
    fixbug::GroupChatResponse groupchatresponse;

    vector<string> useridVec = GroupModel::GetInstance().queryGroupUsers(groupchatrequest.userid(), groupchatrequest.groupid());
    fixbug::MsgServiceRpc_Stub stub(new MprpcChannel());
    lock_guard<mutex> lock(_connMutex);
    for (string id : useridVec)
    {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            // 转发群消息
            fixbug::protobuffer buffer;
            buffer.set_protobuftype(GROUP_CHAT_MSG);
            buffer.set_protobufstr(groupchatrequest.chatmsg().SerializeAsString());
            string responsestr = buffer.SerializeAsString();
            it->second->send(responsestr);
        }
        else
        {
            fixbug::OneChatRequest onechatrequest;
            onechatrequest.set_userid(groupchatrequest.userid());
            onechatrequest.set_friendid(id);
            *onechatrequest.mutable_chatmsg() = groupchatrequest.chatmsg();
            fixbug::OneChatResponse onechatresponse;

            MprpcController controller;
            stub.OneChat(&controller, &onechatrequest, &onechatresponse, nullptr);

            // 一次rpc调用完成，读调用的结果   网络传输出错 序列反序列化出错
            if (controller.Failed())
            {
                std::cout << controller.ErrorText() << std::endl;
            }
        }
    }

    groupchatresponse.mutable_result()->set_success(true);
    groupchatresponse.mutable_result()->set_errmsg("");
    fixbug::protobuffer buffer;
    buffer.set_protobuftype(GROUP_CHAT_ACK);
    buffer.set_protobufstr(groupchatresponse.SerializeAsString());
    string responsestr = buffer.SerializeAsString();
    conn->send(responsestr);
}

// 获取当前用户的离线消息  个人聊天信息或者群组消息
void ConnectService::GetOfflineMsg(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
{
    fixbug::MsgServiceRpc_Stub stub(new MprpcChannel());

    fixbug::GetOfflineMsgRequest request;
    request.ParseFromString(requeststr);
    fixbug::GetOfflineMsgResponse response;

    MprpcController controller;
    stub.GetOfflineMsg(&controller, &request, &response, nullptr);

    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }

    fixbug::protobuffer buffer;
    buffer.set_protobuftype(GET_OFFLINEMSG_ACK);
    buffer.set_protobufstr(response.SerializeAsString());
    string responsestr = buffer.SerializeAsString();
    conn->send(responsestr);
}

// 获取用户的所有好友信息
void ConnectService::GetFriendList(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
{
    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());

    fixbug::GetFriendListRequest request;
    request.ParseFromString(requeststr);
    fixbug::GetFriendListResponse response;

    MprpcController controller;
    stub.GetFriendList(&controller, &request, &response, nullptr);

    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }

    fixbug::protobuffer buffer;
    buffer.set_protobuftype(GET_FRIENDLIST_ACK);
    buffer.set_protobufstr(response.SerializeAsString());
    string responsestr = buffer.SerializeAsString();
    conn->send(responsestr);
}

// 添加好友业务 msgid id friendid
void ConnectService::AddFriend(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
{
    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());

    fixbug::AddFriendRequest request;
    request.ParseFromString(requeststr);
    fixbug::AddFriendResponse response;

    MprpcController controller;
    stub.AddFriend(&controller, &request, &response, nullptr);

    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }

    fixbug::protobuffer buffer;
    buffer.set_protobuftype(ADD_FRIEND_ACK);
    buffer.set_protobufstr(response.SerializeAsString());
    string responsestr = buffer.SerializeAsString();
    conn->send(responsestr);
}

// 创建群组业务
void ConnectService::CreateGroup(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
{
    fixbug::GroupServiceRpc_Stub stub(new MprpcChannel());

    fixbug::CreateGroupRequest request;
    request.ParseFromString(requeststr);
    fixbug::CreateGroupResponse response;

    MprpcController controller;
    stub.CreateGroup(&controller, &request, &response, nullptr);

    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }

    fixbug::protobuffer buffer;
    buffer.set_protobuftype(CREATE_GROUP_ACK);
    buffer.set_protobufstr(response.SerializeAsString());
    string responsestr = buffer.SerializeAsString();
    conn->send(responsestr);
}

// 加入群组业务
void ConnectService::AddGroup(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
{
    fixbug::GroupServiceRpc_Stub stub(new MprpcChannel());

    fixbug::AddGroupRequest request;
    request.ParseFromString(requeststr);
    fixbug::AddGroupResponse response;

    MprpcController controller;
    stub.AddGroup(&controller, &request, &response, nullptr);

    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }

    fixbug::protobuffer buffer;
    buffer.set_protobuftype(ADD_GROUP_ACK);
    buffer.set_protobufstr(response.SerializeAsString());
    string responsestr = buffer.SerializeAsString();
    conn->send(responsestr);
}

// 获取用户加入的所有群组信息
void ConnectService::GetGroupList(const TcpConnectionPtr &conn, string &requeststr, Timestamp time)
{
    fixbug::GroupServiceRpc_Stub stub(new MprpcChannel());

    // 演示调用远程发布的rpc方法Login
    fixbug::GetGroupListRequest request;
    request.ParseFromString(requeststr);

    fixbug::GetGroupListResponse response;

    MprpcController controller;
    // 发起rpc方法的调用 RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送
    stub.GetGroupList(&controller, &request, &response, nullptr);

    // 一次rpc调用完成，读调用的结果   网络传输出错 序列反序列化出错
    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }

    fixbug::protobuffer buffer;
    buffer.set_protobuftype(GET_GROUPLIST_ACK);
    buffer.set_protobufstr(response.SerializeAsString());
    string responsestr = buffer.SerializeAsString();
    conn->send(responsestr);
}