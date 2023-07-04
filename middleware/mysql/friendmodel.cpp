#include "friendmodel.h"
#include "Connection.h"

FriendModel &FriendModel::GetInstance()
{
    static FriendModel FriendModel_;
    return FriendModel_;
}
// 添加好友关系
void FriendModel::insert(string userid, string friendid)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into friend_ values('%s', '%s')", userid.c_str(), friendid.c_str());

    ConnectionPool::getConnectionPool()->getConnection()->update(sql);
}

// 返回用户好友列表
vector<User> FriendModel::query(string userid)
{
    // 1.组装sql语句
    char sql[1024] = {0};

    sprintf(sql, "select a.userid,a.username,a.state from user_ a inner join friend_ b on b.friendid = a.userid where b.userid='%s'", userid.c_str());

    vector<User> vec;

    MYSQL_RES *res = ConnectionPool::getConnectionPool()->getConnection()->query(sql);

    if (res != nullptr)
    {
        // 把userid用户的所有离线消息放入vec中返回
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            User user;
            user.setUserId(row[0]);
            user.setUserName(row[1]);
            user.setUserState(row[2]);
            vec.push_back(user);
        }
        mysql_free_result(res);
        return vec;
    }

    return vec;
}