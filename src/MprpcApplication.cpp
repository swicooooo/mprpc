#include "MprpcApplication.h"
#include "MprpcConfig.h"

#include <unistd.h>
#include <mymuduo/Logger.h>

void MprpcApplication::init(std::string fileName)
{
    // 加载配置文件
    MprpcConfig::instance().loadConfigFile(fileName.c_str());
    LOG_INFO("rpcserverip:%s", MprpcConfig::instance().load("rpcserverip").c_str());
    LOG_INFO("rpcserverport:%s", MprpcConfig::instance().load("rpcserverport").c_str());
    LOG_INFO("zookeeperip:%s", MprpcConfig::instance().load("zookeeperip").c_str());
    LOG_INFO("zookeeperport:%s", MprpcConfig::instance().load("zookeeperport").c_str());
}

MprpcApplication &MprpcApplication::instance()
{
    static MprpcApplication app;
    return app;
}

