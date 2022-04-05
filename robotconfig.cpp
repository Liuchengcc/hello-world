#include "robotconfig.h"
#include <QFile>
#include <QCoreApplication>
#include <QSettings>
#include <QMap>

QString RobotConfig::g_IP_ = "192.168.2.228";
QString RobotConfig::g_Port_ = "5001";
bool RobotConfig::g_isJsonMode_ = true;
int RobotConfig::g_languageMode_ = 0;
RobotConfig* RobotConfig::instance_ = NULL;
RobotConfig::RobotConfig()
{
    QString RobotConfigFilePath_ = QCoreApplication::applicationDirPath() + "/" + "config.ini";
    if(!QFile::exists(RobotConfigFilePath_))
    {
        initRobotConfigFile();
    }
    else
    {
        getRobotConfigFile();
    }
}

RobotConfig *RobotConfig::getInstance()
{
    if(instance_ == NULL)
    {
        instance_ = new RobotConfig;
    }
    return instance_;
}

void RobotConfig::initRobotConfigFile()
{
    QString RobotConfigFilePath_ = QCoreApplication::applicationDirPath() + "/" + "config.ini";
    QSettings settingRobotConfig(RobotConfigFilePath_, QSettings::IniFormat);
    settingRobotConfig.beginGroup("SYSTEM");
    settingRobotConfig.setValue("IP", g_IP_);
    settingRobotConfig.setValue("Port", g_Port_);
    settingRobotConfig.setValue("isJsonMode", true);
    settingRobotConfig.setValue("languageMode", 0);
    settingRobotConfig.endGroup();
}

void RobotConfig::getRobotConfigFile()
{
    QString RobotConfigFilePath_ = QCoreApplication::applicationDirPath() + "/" + "config.ini";
    QSettings settingRobotConfig(RobotConfigFilePath_, QSettings::IniFormat);
    settingRobotConfig.beginGroup("SYSTEM");
    g_IP_ = settingRobotConfig.value("IP").toString();
    g_Port_ = settingRobotConfig.value("Port").toString();
    g_isJsonMode_ = settingRobotConfig.value("isJsonMode").toBool();
     g_languageMode_ = settingRobotConfig.value("languageMode").toInt();
}

void RobotConfig::saveRobotConfigFile()
{
    QString RobotConfigFilePath_ = QCoreApplication::applicationDirPath() + "/" + "config.ini";
    QSettings settingRobotConfig(RobotConfigFilePath_, QSettings::IniFormat);
    settingRobotConfig.beginGroup("SYSTEM");
    settingRobotConfig.setValue("IP", g_IP_);
    settingRobotConfig.setValue("Port", g_Port_);
    settingRobotConfig.setValue("isJsonMode", g_isJsonMode_);
    settingRobotConfig.setValue("languageMode", g_languageMode_);
    settingRobotConfig.endGroup();
}
