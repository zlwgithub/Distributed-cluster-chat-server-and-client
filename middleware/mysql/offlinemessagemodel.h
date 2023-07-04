#pragma once

#include <string>
#include <vector>
#include "CommonConnectionPool.h"
using namespace std;

// 提供离线消息表的操作接口方法
class OfflineMsgModel
{
public:
    static OfflineMsgModel &GetInstance();
    // 存储用户的离线消息
    void insert(string userid, string msg);

    // 删除用户的离线消息
    void remove(string userid);

    // 查询用户的离线消息
    vector<string> query(string userid);

private:
    OfflineMsgModel() {}
    OfflineMsgModel(const OfflineMsgModel &) = delete;
    OfflineMsgModel(OfflineMsgModel &&) = delete;
};