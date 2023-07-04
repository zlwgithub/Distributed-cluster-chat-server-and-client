#pragma once

#include "config.h"

// mprpc框架的基础类，负责框架的一些初始化操作
class ClientInit
{
public:
    static void Init(int argc, char **argv);
    static ClientInit &GetInstance();
    static Config &GetConfig();

private:
    static Config m_config;

    ClientInit() {}
    ClientInit(const ClientInit &) = delete;
    ClientInit(ClientInit &&) = delete;
};