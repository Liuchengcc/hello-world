#include "config.h"
#include "json/value.h"
#include <QFile>
#include <globalfunction.h>
#include "bzresources.h"

#define HOST_IS_UNIX 0
#if HOST_IS_UNIX
const QString c_CfgPath = "./config.json";
const QString c_ip = DEFAULT_RC_IP;
const QString c_port = "5001";
const QString c_lang = "Chinese";
const QString c_defaultPath = "";
const QString c_ftpIp = DEFAULT_RC_IP;
const QString c_ftpPort = "9021";
const QString c_remoteMainPath = "/apps/LinkHou/VC/";
const QString c_fileFormat = "Unix";
#else
const QString c_CfgPath = "./config.json";
const QString c_ip = DEFAULT_RC_IP;
const QString c_port = "5001";
const QString c_lang = "Chinese";
const QString c_defaultPath = "";
const QString c_ftpIp = DEFAULT_RC_IP;
const QString c_ftpPort = "21";
const QString c_remoteMainPath = "/ata1:1/VC/";
const QString c_fileFormat = "Dos";
#endif
Config* Config::instance_ = nullptr;
Config* Config::getInstance()
{
    if(!instance_)
    {
        instance_ = new Config;
    }
    return instance_;
}

Config::Config(QObject *parent) : QObject(parent)
{
    if(!QFile::exists(c_CfgPath))
    {
        initConfig();
    }
    else
    {
        loadConfig(c_CfgPath);
    }
}


void Config::initConfig()
{

  ip_ =  c_ip;
  port_ = c_port;
  lang_ = c_lang;
  defaultPrjPath_ = c_defaultPath;
  ftpIp_ = c_ftpIp;
  ftpPort_ = c_ftpPort;
  remoteMainPath_ = c_remoteMainPath;
  fileFormat_ = c_fileFormat;
  saveConfig(c_CfgPath);
}

void Config::loadConfig(QString Path)
{
    Json::Value root = GlobalFunction::ReadJsonFromJsonfile(Path);
#define PARSER_CONFIG_DATA(TYPE, DATA)\
    DATA = QString::fromStdString(root[#DATA].asString());
OP_CONFIG_ALL_OBJ(PARSER_CONFIG_DATA);
}

void Config::saveConfig(QString Path)
{
    Json::Value root;
#define WRITE_CONFIG_DATA(TYPE, DATA)\
    root[#DATA] = DATA.toStdString();
    OP_CONFIG_ALL_OBJ(WRITE_CONFIG_DATA);
    GlobalFunction::WriteJsonToJsonfile(root, Path);
}
