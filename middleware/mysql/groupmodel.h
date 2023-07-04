#pragma once

#include "group.h"
#include <string>
#include <vector>
#include <user.h>
#include <unordered_set>
#include <mutex>
#include "CommonConnectionPool.h"
using namespace std;

struct creategroupres
{
    bool success;
    string groupid;
};

// 维护群组信息的操作接口方法
class GroupModel
{
public:
    static GroupModel &GetInstance();
    // 创建群组
    creategroupres createGroup(Group &group);
    // 加入群组
    void addGroup(string userid, string groupid, string role);
    // 查询用户所在群组信息
    vector<Group> queryGroups(string userid);
    // 根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其它成员群发消息
    vector<string> queryGroupUsers(string userid, string groupid);

private:
    GroupModel() {}
    GroupModel(const GroupModel &) = delete;
    GroupModel(GroupModel &&) = delete;

private:
    unordered_set<string> groupidset;
    std::mutex mutex_;
};