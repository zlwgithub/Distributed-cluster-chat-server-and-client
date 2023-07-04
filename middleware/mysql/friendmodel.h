#pragma once

#include "user.h"
#include <vector>
#include "CommonConnectionPool.h"
using namespace std;

// 维护好友信息的操作接口方法
class FriendModel
{
public:
    static FriendModel &GetInstance();
    // 添加好友关系
    void insert(string userid, string friendid);

    // 返回用户好友列表
    vector<User> query(string userid);

private:
    FriendModel() {}
    FriendModel(const FriendModel &) = delete;
    FriendModel(FriendModel &&) = delete;
};
