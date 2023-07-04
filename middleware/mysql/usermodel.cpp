#include "usermodel.h"
#include "Connection.h"
using namespace std;

UserModel &UserModel::GetInstance()
{
    static UserModel UserModel_;
    return UserModel_;
}

// User表的增加方法
bool UserModel::insert(User &user)
{
    string randUserId = GenerateQQnumber();
    {
        unique_lock<mutex> lock(mutex_);
        while (useridset.find(randUserId) != useridset.end())
        {
            randUserId = GenerateQQnumber();
        }
        useridset.insert(randUserId);
    }
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user_(userid,username, password, state) values('%s','%s', '%s', '%s')", randUserId.c_str(), user.getUserName().c_str(), user.getPwd().c_str(), user.getUserState().c_str());
    bool ret = ConnectionPool::getConnectionPool()->getConnection()->update(sql);
    if (ret)
    {
        // 获取插入成功的用户数据生成的主键id
        user.setUserId(randUserId);
        return true;
    }

    return false;
}

// 根据用户号码查询用户信息
User UserModel::query(string id)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from user_ where userid = '%s'", id.c_str());

    MYSQL_RES *res = ConnectionPool::getConnectionPool()->getConnection()->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row != nullptr)
        {
            User user;
            user.setUserId(row[0]);
            user.setUserName(row[1]);
            user.setPwd(row[2]);
            user.setUserState(row[3]);
            mysql_free_result(res);
            return user;
        }
    }

    return User();
}

// 更新用户的状态信息
bool UserModel::updateState(User user)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "update user_ set state = '%s' where userid = '%s'", user.getUserState().c_str(), user.getUserId().c_str());

    if (ConnectionPool::getConnectionPool()->getConnection()->update(sql))
    {
        return true;
    }

    return false;
}

// 重置用户的状态信息
void UserModel::resetState()
{
    // 1.组装sql语句
    char sql[1024] = "update user_ set state = 'offline' where state = 'online'";

    ConnectionPool::getConnectionPool()->getConnection()->update(sql);
}