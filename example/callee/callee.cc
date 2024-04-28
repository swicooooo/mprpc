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

/// protobuf rpc会生成两个主要的类 Service(服务端)、Service_stub(客户端)
/// Service：内部有virtual void CallMethod()，通过switch索引到具体的方法，
///          通过重写该方法形成多态，在解析完客户端发送的数据后可直接调用callMethod完成业务方法
/// Service_stub：rpcChannel内部有virtual void CallMethod()，需继承rpcChannel重写该方法
///               stub下的rpc方法都统一走rpcChannel，做rpc方法的数据序列化和网络发送
int main(int argc, char *argv[])
{
    int argcc = 3;
    char *argvv[] = {(char *)"callee", (char *)"-i", (char *)"test.conf"};
    MprpcApplication::init(argcc, argvv);

    RpcProvider provider;
    provider.notifyService(new UserService());
    provider.run();
    return 0;
}
