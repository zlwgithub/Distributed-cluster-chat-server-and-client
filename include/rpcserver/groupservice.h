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

class GroupService : public fixbug::GroupServiceRpc
{
public:
    std::vector<Group> GetGroupList(string userid)
    {
        vector<Group> groupuserVec = GroupModel::GetInstance().queryGroups(userid);
        return groupuserVec;
    }

    void GetGroupList(::google::protobuf::RpcController *controller,
                      const ::fixbug::GetGroupListRequest *request,
                      ::fixbug::GetGroupListResponse *response,
                      ::google::protobuf::Closure *done)
    {
        string userid = request->userid();

        std::vector<Group> groupList = GetGroupList(userid);

        response->mutable_result()->set_success(true);
        response->mutable_result()->set_errmsg("");
        for (auto group_ : groupList)
        {
            auto *p = response->mutable_grouplist()->add_group();
            fixbug::Group grouptemp;
            grouptemp.set_groupid(group_.getGroupId());
            grouptemp.set_groupname(group_.getGroupName());
            grouptemp.set_groupdesc(group_.getGroupDesc());
            for (auto &member : group_.getGroupUsers())
            {
                fixbug::User usertemp;
                usertemp.set_userid(member.first.getUserId());
                usertemp.set_username(member.first.getUserName());
                usertemp.set_userstate(member.first.getUserState());
                *grouptemp.add_groupmember() = usertemp;
                *grouptemp.add_memberidentity() = member.second;
            }
            *p = grouptemp;
        }

        done->Run();
    }

    bool AddGroup(string userid, string groupid)
    {
        GroupModel::GetInstance().addGroup(userid, groupid, "normal");
        return true;
    }

    void AddGroup(::google::protobuf::RpcController *controller,
                  const ::fixbug::AddGroupRequest *request,
                  ::fixbug::AddGroupResponse *response,
                  ::google::protobuf::Closure *done)
    {
        string userid = request->userid();
        string groupid = request->groupid();

        bool ret = AddGroup(userid, groupid);

        if (ret == false)
        {
            response->mutable_result()->set_errmsg("加入群组失败");
        }
        else
        {
            response->mutable_result()->set_errmsg("");
        }
        response->mutable_result()->set_success(ret);

        done->Run();
    }

    creategroupres CreateGroup(string userid, string groupname, string groupdesc)
    {
        Group group("-1", groupname, groupdesc);
        creategroupres res;
        if ((res = GroupModel::GetInstance().createGroup(group)).success)
        {
            // 存储群组创建人信息
            GroupModel::GetInstance().addGroup(userid, group.getGroupId(), "creator");
        }
        return res;
    }

    void CreateGroup(::google::protobuf::RpcController *controller,
                     const ::fixbug::CreateGroupRequest *request,
                     ::fixbug::CreateGroupResponse *response,
                     ::google::protobuf::Closure *done)
    {
        string userid = request->userid();
        string groupname = request->groupname();
        string groupdesc = request->groupdesc();

        creategroupres ret = CreateGroup(userid, groupname, groupdesc);

        if (ret.success == false)
        {
            response->mutable_result()->set_errmsg("创建群组失败");
        }
        else
        {
            response->mutable_result()->set_errmsg("");
        }
        response->mutable_result()->set_success(ret.success);
        *response->mutable_groupid() = ret.groupid;

        done->Run();
    }
};