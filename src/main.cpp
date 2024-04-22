#include "MprpcApplication.h"
#include "RpcProvider.h"

int main(int argc, char *argv[])
{
    int argcc = 3;
    char *argvv[] = {(char *)"mprpc", (char *)"-i", (char *)"test.conf"};
    MprpcApplication::init(argcc, argvv);

    RpcProvider provider;
    // provider.notifyService();
    provider.run();
    return 0;
}
