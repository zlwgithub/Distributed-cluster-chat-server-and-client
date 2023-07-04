#pragma once

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>

#include "redis.h"
#include "groupmodel.h"
#include "friendmodel.h"
#include "usermodel.h"
#include "offlinemessagemodel.h"
#include "qq.pb.h"
using namespace muduo;
using namespace muduo::net;
using namespace std;

class FriendService : public fixbug::FriendServiceRpc
{
public:
    std::vector<User> GetFriendList(string userid)
    {
        vector<User> userVec = FriendModel::GetInstance().query(userid);
        return userVec;
    }

    void GetFriendList(::google::protobuf::RpcController *controller,
                       const ::fixbug::GetFriendListRequest *request,
                       ::fixbug::GetFriendListResponse *response,
                       ::google::protobuf::Closure *done)
    {
        string userid = request->userid();

        std::vector<User> friendList = GetFriendList(userid);

        response->mutable_result()->set_success(true);
        response->mutable_result()->set_errmsg("");
        for (auto friend_ : friendList)
        {
            auto *p = response->mutable_friendlist()->add_friend_();
            fixbug::User usertemp;
            usertemp.set_userid(friend_.getUserId());
            usertemp.set_username(friend_.getUserName());
            usertemp.set_userstate(friend_.getUserState());
            *p = usertemp;
        }

        done->Run();
    }

    bool AddFriend(string userid, string friendid)
    {
        // 存储好友信息
        FriendModel::GetInstance().insert(userid, friendid);
        return true;
    }

    void AddFriend(::google::protobuf::RpcController *controller,
                   const ::fixbug::AddFriendRequest *request,
                   ::fixbug::AddFriendResponse *response,
                   ::google::protobuf::Closure *done)
    {
        string userid = request->userid();
        string friendid = request->friendid();

        bool ret = AddFriend(userid, friendid);

        if (ret == false)
        {
            response->mutable_result()->set_errmsg("添加好友失败");
        }
        else
        {
            response->mutable_result()->set_errmsg("");
        }
        response->mutable_result()->set_success(ret);

        done->Run();
    }
};