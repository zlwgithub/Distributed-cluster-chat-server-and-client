#pragma once

#include <string>
#include <stdio.h>
#include <stdlib.h> //包括rand()、srand()、abs()等函数
#include <time.h>
using namespace std;

// 随机生成7-10位的QQ号
static string GenerateQQnumber()
{
    srand((unsigned int)time(NULL));
    string qqnumber;
    int weishu = (rand() % 4) + 2;
    qqnumber.push_back((rand() % 9) + 1 + '0');
    for (int i = 0; i < weishu; ++i)
    {
        qqnumber.push_back((rand() % 10) + '0');
    }
    return qqnumber;
}

class User
{
public:
    User(string userid = "-1", string username = "", string password = "", string userstate = "offline")
    {
        this->userid = userid;
        this->username = username;
        this->password = password;
        this->userstate = userstate;
    }

    void setUserId(string userid) { this->userid = userid; }
    void setUserName(string username) { this->username = username; }
    void setPwd(string password) { this->password = password; }
    void setUserState(string userstate) { this->userstate = userstate; }

    string getUserId() const { return this->userid; }
    string getUserName() const { return this->username; }
    string getPwd() const { return this->password; }
    string getUserState() const { return this->userstate; }
    bool operator==(const User &user1) const
    {
        return user1.getUserId() == this->getUserId();
    }

private:
    string userid;
    string username;
    string password;
    string userstate;
};