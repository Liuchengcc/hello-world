#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <marcroly.h>

#define OP_CONFIG_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(QString, ip_)\
    OP_MARCRO(QString, port_)\
    OP_MARCRO(QString, lang_)\
    OP_MARCRO(QString, defaultPrjPath_)\
    OP_MARCRO(QString, ftpIp_)\
    OP_MARCRO(QString, ftpPort_)\
    OP_MARCRO(QString, remoteMainPath_)\
    OP_MARCRO(QString, fileFormat_)

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);
    static Config* getInstance();
#define LANG_LIST "Chinese English Japanese Veitnam"
    enum Language
    {
        Chinese = 0,
        English = 1,
        Japanese = 2,
        Veitnam = 3
    };
    void initConfig();
    void loadConfig(QString);
    void saveConfig(QString);

signals:
    OP_CONFIG_ALL_OBJ(DECL_SIGNAL_METADATA);
    OP_CONFIG_ALL_OBJ(REGISTER_METADATA);

private:
    static Config* instance_;
};

#endif // CONFIG_H
