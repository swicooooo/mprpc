#include "Zookeeper.h"
#include "MprpcConfig.h"

#include <mymuduo/Logger.h>
#include <semaphore.h>
#include <sstream>

ZkClient::ZkClient(): zhandle_(nullptr)
{}

ZkClient::~ZkClient()
{
    if(zhandle_ != nullptr)
    {
        zookeeper_close(zhandle_);
    }
}

void global_watcher_fn(zhandle_t *zh, int type, int state, const char *path,void *watcherCtx)
{
    // 回调的消息类型是会话(zkclient和zkserver连接时会建立session)且链接成功
    if(type == ZOO_SESSION_EVENT && state == ZOO_CONNECTED_STATE)
    {   
        sem_t *sem = (sem_t*)zoo_get_context(zh);
        sem_post(sem);
    }
}

void ZkClient::start()
{
    std::string ip = MprpcConfig::instance().load("zookeeperip");
    std::string port = MprpcConfig::instance().load("zookeeperport");
    std::string host = ip + ":" + port; // 直接读取"127.0.0.1\r" 拼接出现错误
    LOG_INFO("host: %s", host.c_str());
    // 这里有三个线程
    // 调用者线程、网络IO线程(负责心跳机制 1/3Timeout)、watcher回调线程(server会设置参数)
    zhandle_ = zookeeper_init(host.c_str(), global_watcher_fn, 30000, nullptr, nullptr, 0);
    if(zhandle_ == nullptr)
    {
        LOG_FATAL("%s:%d zookeeper init error!", __FILE__, __LINE__);
    }
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(zhandle_, &sem);    // 设置上下文额外信息，在其他线程共享使用
    sem_wait(&sem);
}

void ZkClient::create(const char *path, const char *data, int datalen, int state)
{
    char path_buf[128];
    int flag = zoo_exists(zhandle_, path, 0, nullptr);
    if(flag == ZNONODE)
    {
        flag = zoo_create(zhandle_, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, path_buf, sizeof(path_buf));
        if(flag != ZOK)
        {
            LOG_FATAL("%s:%d zookeeper create error!", __FILE__, __LINE__);
        } 
    }
}

std::string ZkClient::get(const char *path)
{
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(zhandle_, path, 0, buffer, &bufferlen, nullptr);
    if(flag != ZOK)
    {
        LOG_ERROR("%s:%d zookeeper get error!", __FILE__, __LINE__);
        return "";
    }     
    return buffer;
}

