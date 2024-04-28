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
    if(!request->SerializeToString(&arg_str))
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
    if(!rpcHeader.SerializeToString(&header_str))
    {
        LOG_ERROR("%s:%d serialize header_str error!", __FILE__, __LINE__);
        return;
    }    
    uint32_t header_size = header_str.size();
    std::string rpc_str;
    rpc_str.insert(0,(char*)&header_size,4);    // 插入四个字节
    rpc_str.append(header_str);
    rpc_str.append(arg_str);
    // 向服务器发送数据后，将返回的数据反序列化给response
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    Socket socket(sockfd);
    /// TODO

}