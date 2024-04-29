#include "MprpcChannel.h"
#include "header.pb.h"
#include "MprpcConfig.h"

#include <google/protobuf/descriptor.h>
#include <mymuduo/Logger.h>
#include <mymuduo/Socket.h>
#include <mymuduo/InetAddress.h>

// header_size(4B)+header_str(service_name method_name)+args_size【header】+args_str
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request,
                              google::protobuf::Message *response,
                              google::protobuf::Closure *done)
{
    // 构建rpc请求头
    auto service = method->service();
    std::string arg_str;
    if (!request->SerializeToString(&arg_str))
    {
        LOG_ERROR("%s:%d serialize request error!", __FILE__, __LINE__);
        return;
    }
    header::rpcHeader rpcHeader;
    rpcHeader.set_service_name(service->name());
    rpcHeader.set_method_name(method->name());
    rpcHeader.set_arg_size(arg_str.size());
    // 构建rpc请求字符串
    std::string header_str;
    if (!rpcHeader.SerializeToString(&header_str))
    {
        LOG_ERROR("%s:%d serialize header_str error!", __FILE__, __LINE__);
        return;
    }
    uint32_t header_size = header_str.size();
    std::string rpc_str;
    rpc_str.insert(0, (char *)&header_size, 4); // 插入四个字节
    rpc_str.append(header_str);
    rpc_str.append(arg_str);
    // 向服务器发送数据后，将返回的数据反序列化给response
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    Socket socket(sockfd);
    // InetAddress peerAddr(atoi(MprpcConfig::instance().load("rpcserverport").c_str()), MprpcConfig::instance().load("rpcserverip"));
    InetAddress peerAddr(8000);
    socket.connect(peerAddr);
    int send_len = -1;
    if ((send_len=socket.send((void *)rpc_str.c_str(), rpc_str.size())) == -1)
    {
        LOG_ERROR("%s:%d socket send error!", __FILE__, __LINE__);
        socket.close();
        return;
    }
    char recv_str[1024] = {0};
    int recv_len = -1;
    if ((recv_len=socket.recv(recv_str, 1024)) == -1)
    {
        LOG_ERROR("%s:%d socket recv error!", __FILE__, __LINE__);
        socket.close();
        return;
    }
    /// fixbug: string写二进制时有\0，字符串读取到时默认停止读取
    if(!response->ParseFromArray(recv_str, recv_len))
    {
        LOG_ERROR("%s:%d parse response error!", __FILE__, __LINE__);
    }
    socket.close();
}