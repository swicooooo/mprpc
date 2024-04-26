#include "MprpcApplication.h"
#include "RpcProvider.h"
#include "user.pb.h"

#include <mymuduo/Logger.h>

class UserService : public user::UserServiceRpc
{
public:
    // 本地方法
    bool Login(const std::string &name, const std::string &pwd)
    {
        LOG_INFO("name: %s, pwd: %s", name.c_str(), pwd.c_str());
        return true;
    }

    // 重写UserServiceRpc中的Login方法
    void Login(::google::protobuf::RpcController *controller,
               const ::user::LoginRequest *request,
               ::user::LoginResponse *response,
               ::google::protobuf::Closure *done) override
    {
        // 处理业务并返回response
        Login(request->name(), request->pwd());
        auto code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(true);
        // 执行回调
        done->Run();
    }
};

int main(int argc, char *argv[])
{
    int argcc = 3;
    char *argvv[] = {(char *)"mprpc", (char *)"-i", (char *)"test.conf"};
    MprpcApplication::init(argcc, argvv);

    RpcProvider provider;
    provider.notifyService(new UserService());
    provider.run();
    return 0;
}
