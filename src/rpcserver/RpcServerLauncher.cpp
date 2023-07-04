#include "rpcserver.h"

int main(int argc, char **argv)
{
    RpcServer rpcserver;
    rpcserver.start(argc, argv);
    return 0;
}