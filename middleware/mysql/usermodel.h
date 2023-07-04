#pragma once

#include "user.h"
#include <mutex>
#include <memory>
#include <unordered_set>
#include "CommonConnectionPool.h"
// User表的数据操作类
class UserModel
{
public:
    static UserModel &GetInstance();
    // User表的增加方法
    bool insert(User &user);

    // 根据用户号码查询用户信息
    User query(string userid);

    // 更新用户的状态信息
    bool updateState(User user);

    // 重置用户的状态信息
    void resetState();

private:
    UserModel() {}
    UserModel(const UserModel &) = delete;
    UserModel(UserModel &&) = delete;

private:
    unordered_set<string> useridset;
    std::mutex mutex_;
};