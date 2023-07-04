#include "groupmodel.h"
#include <memory>
#include "Connection.h"

GroupModel &GroupModel::GetInstance()
{
    static GroupModel GroupModel_;
    return GroupModel_;
}

// 创建群组
creategroupres GroupModel::createGroup(Group &group)
{
    creategroupres res;
    string randGroupId = GenerateQQnumber();
    {
        unique_lock<mutex> lock(mutex_);
        while (groupidset.find(randGroupId) != groupidset.end())
        {
            randGroupId = GenerateQQnumber();
        }
        groupidset.insert(randGroupId);
    }

    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into group_(groupid,groupname, groupdesc) values('%s','%s', '%s')", randGroupId.c_str(),
            group.getGroupName().c_str(), group.getGroupDesc().c_str());

    if (ConnectionPool::getConnectionPool()->getConnection()->update(sql))
    {
        group.setGroupId(randGroupId);
        res.groupid = randGroupId;
        res.success = true;
    }

    return res;
}

// 加入群组
void GroupModel::addGroup(string userid, string groupid, string role)
{
    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser_ values('%s','%s', '%s')",
            groupid.c_str(), userid.c_str(), role.c_str());

    if (!ConnectionPool::getConnectionPool()->getConnection()->update(sql))
    {
    }
}

// 查询用户所在群组信息
vector<Group> GroupModel::queryGroups(string userid)
{
    /*
    1. 先根据userid在groupuser表中查询出该用户所属的群组信息
    2. 在根据群组信息，查询属于该群组的所有用户的userid，并且和user表进行多表联合查询，查出用户的详细信息
    */
    char sql[1024] = {0};
    sprintf(sql, "select a.groupid,a.groupname,a.groupdesc from group_ a inner join groupuser_ b on a.groupid = b.groupid where b.userid='%s'",
            userid.c_str());

    vector<Group> groupVec;

    MYSQL_RES *res = ConnectionPool::getConnectionPool()->getConnection()->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        // 查出userid所有的群组信息
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            Group group;
            group.setGroupId(row[0]);
            group.setGroupName(row[1]);
            group.setGroupDesc(row[2]);
            groupVec.push_back(group);
        }
        mysql_free_result(res);
    }

    // 查询群组的用户信息
    for (Group &group : groupVec)
    {
        sprintf(sql, "select a.userid,a.username,a.state,b.grouprole from user_ a inner join groupuser_ b on b.userid = a.userid where b.groupid='%s'", group.getGroupId().c_str());

        MYSQL_RES *res = ConnectionPool::getConnectionPool()->getConnection()->query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setUserId(row[0]);
                user.setUserName(row[1]);
                user.setUserState(row[2]);
                group.getGroupUsers().emplace(user, "");
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}

// 根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其它成员群发消息
vector<string> GroupModel::queryGroupUsers(string userid, string groupid)
{
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser_ where groupid = '%s' and userid != '%s'", groupid.c_str(), userid.c_str());

    vector<string> idVec;

    MYSQL_RES *res = ConnectionPool::getConnectionPool()->getConnection()->query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            idVec.push_back(row[0]);
        }
        mysql_free_result(res);
    }

    return idVec;
}