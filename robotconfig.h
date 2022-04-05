#ifndef ROBOT_CONFIG_H
#define ROBOT_CONFIG_H
#include <QString>
#include <QMap>
struct RobotConfig
{
public:
    RobotConfig();
    static RobotConfig* getInstance();
    static void initRobotConfigFile();
    static void getRobotConfigFile();
    static void saveRobotConfigFile();
    static QString g_IP_;
    static QString g_Port_;
    static bool g_isJsonMode_;
    static int g_languageMode_;//1中文 2英文 3日文
private:
    static RobotConfig* instance_;
};

#endif // ROBOT_CONFIG_H
