#pragma once

#include <mprpc/mprpcapplication.h>
#include <mprpc/mprpcprovider.h>

#include "userservice.h"
#include "friendservice.h"
#include "groupservice.h"
#include "msgservice.h"

// #ifdef FriendService
// #include "FriendService.h"
// #endif

// #ifdef MsgService
// #include "MsgService.h"
// #endif

// #ifdef GroupService
// #include "GroupService.h"
// #endif

class RpcServer
{
public:
        void start(int argc, char **argv)
        {
                MprpcApplication::Init(argc, argv);

                RpcProvider provider;
                provider.NotifyService(new UserService());
                provider.NotifyService(new FriendService());
                provider.NotifyService(new GroupService());
                provider.NotifyService(new MsgService());
                provider.Run();
        }
};