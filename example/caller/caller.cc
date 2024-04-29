#include "MprpcApplication.h"
#include "MprpcChannel.h"
#include "user.pb.h"

#include <mymuduo/Logger.h>

int main(int argc, char *argv[])
{
    MprpcApplication::init("./test.conf");

    user::UserServiceRpc_Stub stub(new MprpcChannel());
    user::LoginRequest request;
    request.set_name("zhangsan");
    request.set_pwd("0");

    user::LoginResponse response;
    stub.Login(nullptr, &request, &response, nullptr);

    if(response.result().errcode() == 0) 
    {
        LOG_INFO("%s:%d response success: %d", __FILE__, __LINE__, response.success());
    }
    else
    {
        LOG_ERROR("%s:%d response error: %s", __FILE__, __LINE__, response.result().errmsg().c_str());
    }
    return 0;
}


