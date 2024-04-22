#include "MprpcConfig.h"

#include <fstream>

MprpcConfig &MprpcConfig::instance()
{
    static MprpcConfig app;
    return app;
}

void MprpcConfig::loadConfigFile(const char *file)
{
    std::ifstream configFile(file);
    std::string line;
    while (std::getline(configFile,line)) {
        if(line[0] == '#') continue;
        size_t pos = line.find('=');
        if(pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos+1);
            configs_.insert({key, value});
        }
    }
}

std::string MprpcConfig::load(const std::string &key)
{
    auto it = configs_.find(key);
    if(it != configs_.end()) {
        return it->second;
    }
    return "";
}
