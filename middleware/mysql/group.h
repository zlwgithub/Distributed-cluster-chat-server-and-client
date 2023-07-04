#pragma once

#include <string>
#include <unordered_map>
#include <user.h>
using namespace std;

struct userHash
{
    size_t operator()(const User &user) const
    {
        return hash<string>()(user.getUserId()) ^ hash<string>()(user.getUserName());
    }
};

class Group
{
public:
    Group(string groupid = "-1", string name = "", string desc = "")
    {
        this->groupid = groupid;
        this->groupname = name;
        this->groupdesc = desc;
    }

    void setGroupId(string groupid) { this->groupid = groupid; }
    void setGroupName(string name) { this->groupname = name; }
    void setGroupDesc(string desc) { this->groupdesc = desc; }

    string getGroupId() const { return this->groupid; }
    string getGroupName() const { return this->groupname; }
    string getGroupDesc() const { return this->groupdesc; }
    unordered_map<User, string, userHash> &getGroupUsers() { return this->useridentity; }

private:
    string groupid;
    string groupname;
    string groupdesc;
    unordered_map<User, string, userHash> useridentity;
};