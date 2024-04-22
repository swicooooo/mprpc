#include "MprpcApplication.h"
#include "MprpcConfig.h"

#include <unistd.h>
#include <mymuduo/Logger.h>

void MprpcApplication::init(int argc, char *argv[])
{
    // 解析命令行输入的参数
    if (argc < 2) {
        LOG_FATAL("format: command -i <configfile>!\n");
    }
    int c = 0;
    std::string fileName;
    while ((c = getopt(argc, argv, "i:") != -1)) {
        switch (c)
        {
        case 'i':
            fileName = optarg;
            break;
        case '?':
        case ':':
            LOG_FATAL("format: command -i <configfile>!\n");
            break;
        default:
            break;
        }
    }

    // 加载配置文件
    MprpcConfig::instance().loadConfigFile(fileName.c_str());
    LOG_INFO("success to loadConfigFile!\n");
}

MprpcApplication &MprpcApplication::instance()
{
    static MprpcApplication app;
    return app;
}

MprpcApplication::MprpcApplication()
{
}
