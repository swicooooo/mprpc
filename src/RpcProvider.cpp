#include "RpcProvider.h"
#include "MprpcConfig.h"

void RpcProvider::notifyService(google::protobuf::Service *service) 
{
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
    server.start();
    loop_.loop();
}

void RpcProvider::onConnection(const TcpConnectionPtr &)
{
}

void RpcProvider::onMessage(const TcpConnectionPtr &, Buffer *, Timestamp)
{
}
