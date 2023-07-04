#include "offlinemessagemodel.h"
#include "Connection.h"

OfflineMsgModel &OfflineMsgModel::GetInstance()
{
    static OfflineMsgModel OfflineMsgModel_;
    return OfflineMsgModel_;
}

// 存储用户的离线消息
void OfflineMsgModel::insert(string userid, string msg)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage_ values('%s', '%s')", userid.c_str(), msg.c_str());

    ConnectionPool::getConnectionPool()->getConnection()->update(sql);
}

// 删除用户的离线消息
void OfflineMsgModel::remove(string userid)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage_ where userid='%s'", userid.c_str());

    ConnectionPool::getConnectionPool()->getConnection()->update(sql);
}

// 查询用户的离线消息
vector<string> OfflineMsgModel::query(string userid)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage_ where userid = '%s'", userid.c_str());

    vector<string> vec;

    MYSQL_RES *res = ConnectionPool::getConnectionPool()->getConnection()->query(sql);
    if (res != nullptr)
    {
        // 把userid用户的所有离线消息放入vec中返回
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            vec.push_back(row[0]);
        }
        mysql_free_result(res);
        return vec;
    }

    return vec;
}