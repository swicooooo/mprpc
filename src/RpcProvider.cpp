#include "RpcProvider.h"
#include "MprpcConfig.h"
#include "header.pb.h"
#include "Zookeeper.h"

#include <google/protobuf/descriptor.h>
#include <mymuduo/Logger.h>

void RpcProvider::notifyService(google::protobuf::Service *service) 
{
    ServiceInfo serviceInfo;
    serviceInfo.service_ = service;
    auto descriptor = service->GetDescriptor();
    LOG_INFO("service name: %s", descriptor->name().c_str());
    for (size_t i = 0; i < descriptor->method_count(); i++)
    {
        serviceInfo.methods_.insert({descriptor->method(i)->name(),descriptor->method(i)});
        LOG_INFO("method name: %s", descriptor->method(i)->name().c_str());
    }
    services_.insert({descriptor->name(), serviceInfo});
}

void RpcProvider::run()
{
    std::string ip = MprpcConfig::instance().load("rpcserverip");
    uint16_t port = atoi(MprpcConfig::instance().load("rpcserverport").c_str());

    InetAddress listenAddr(port, ip);
    TcpServer server(&loop_, listenAddr, "RpcProvider");

    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    server.setThreadNum(4);

    // 将rpc节点服务和方法注册到zk上, /service_name/method_name ip:port
    ZkClient zkCli;
    zkCli.start();
    for(auto &service: services_)
    {
        std::string service_path = "/" + service.first;
        zkCli.create(service_path.c_str(), nullptr, 0);
        for(auto &method: service.second.methods_)
        {
            std::string method_path = service_path + "/" + method.first;
            char host[128] = {0};
            sprintf(host, "%s:%d", ip.c_str(), port);
            zkCli.create(method_path.c_str(), host, strlen(host), ZOO_EPHEMERAL); // 临时性节点
        }
    }

    server.start();
    loop_.loop();
}

void RpcProvider::onConnection(const TcpConnectionPtr &conn)
{
    if(!conn->connected()) 
    {
        conn->shutdown();
    }
}

///处理粘包问题 header_size(4B)+header_str(service_name method_name)+args_size【header】+args_str
void RpcProvider::onMessage(const TcpConnectionPtr &conn, Buffer *buffer, Timestamp)
{
    // 反序列化从rpcConsumer发送过来的proto消息
    std::string recv_str = buffer->retrieveAllAsString();
    uint32_t header_size = 0;
    recv_str.copy((char*)&header_size, 4, 0);
    std::string header_str = recv_str.substr(4, header_size);
    
    // 获取rpc header+body发送过来的消息
    header::rpcHeader rpcHeader;
    if(!rpcHeader.ParseFromString(header_str))
    {
        LOG_ERROR("%s:%d parse rpcHeader error!", __FILE__, __LINE__);
        return;
    }
    std::string service_name = rpcHeader.service_name();
    std::string method_name = rpcHeader.method_name();
    uint32_t  arg_size = rpcHeader.arg_size();
    std::string arg_str = recv_str.substr(4+header_size, arg_size); // 获取参数内容

    LOG_INFO("////////////////print/////////////    ////////////////print/////////////");
    LOG_INFO("service_name:%s  method_name:%s  arg_size:%d  arg_str:%s",
        service_name.c_str(), method_name.c_str(), arg_size, arg_str.c_str());
    LOG_INFO("////////////////print/////////////    ////////////////print/////////////");
    
    // 查找service对象和method描述符是否注册
    auto it = services_.find(service_name);
    auto mit = it->second.methods_.find(method_name);
    if(it == services_.end() || mit ==  it->second.methods_.end())
    {
        LOG_ERROR("%s:%d services_ or methods_ is not found!", __FILE__, __LINE__);
        return;
    }
    auto service = it->second.service_;
    auto methodDesc = mit->second;

    // 在框架上根据远端rpc的请求，调用在rpc上发布的方法
    auto request = service->GetRequestPrototype(methodDesc).New();
    if(!request->ParseFromString(arg_str))
    {
        LOG_ERROR("%s:%d parse request error!", __FILE__, __LINE__);
        return;
    }
    auto response = service->GetResponsePrototype(methodDesc).New();
    auto done = google::protobuf::NewCallback<RpcProvider,const TcpConnectionPtr&, google::protobuf::Message*>
                (this, &RpcProvider::onRpcCallMethod, conn, response);  // 绑定closure的回调函数
    service->CallMethod(methodDesc, nullptr, request, response, done);  // 自动调用用户发布的method
}

void RpcProvider::onRpcCallMethod(const TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string resp_str;
    if(!response->SerializeToString(&resp_str))
    {
        LOG_ERROR("%s:%d parse response error!", __FILE__, __LINE__);
    }
    else
    {
        conn->send(resp_str);
    }
    conn->shutdown();   // 短链接服务，由provider断开连接
}
