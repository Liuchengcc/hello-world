#ifndef GLOBALFUNCTION_H
#define GLOBALFUNCTION_H

#include <QObject>
#include <json/value.h>
#include <fstream>
#include <QCryptographicHash>

#define SHOW_LOG(MSG) GlobalFunction::showLogMsg(MSG);
class GlobalFunction : public QObject
{
    Q_OBJECT
public:
    explicit GlobalFunction(QObject *parent = 0);
    static Json::Value ReadJsonFromJsonfile(QString);
    static Json::Value ReadJsonFromJsonStr(QString);
    static int WriteJsonToJsonfile(Json::Value, QString);
    static int WriteJsonToQByteArray(Json::Value, QByteArray&);
    static int ReadDocFileToByteArray(QString path, QByteArray&);
    static QString CalFileCrcInUnix(QString path);
    static QString CalFileCrcAuto(QString path);
    static int WriteByteArrayToDocFile(QString path, QByteArray&);
    static bool copyDirectoryFiles(QString from, QString to, bool isOverWrite = true);
    static bool copyFileToPath(QString, QString, bool isOverWrite = true);
    static bool mkdirIfNotExist(QString);
    static bool createFileInPath(QString path, QString fileName);
    static bool rmDir(QString dirPath);
    static bool rmFile(QString filePath);
    static void Delayms(unsigned int);
    static bool isStrMatchReg(QString str, QString reg);
    static bool isFloatParamValid(QString str, bool isPos = false, bool isFloat = true, int leftCount = 4, int rightCount = 6);
    static bool isNameValid(QString str);
    static bool isDescribValid(QString str);
    static bool isIPValid(QString str);
    static QString MD5(const QByteArray&);
    static void showLogMsg(QString);

signals:
    
public slots:
    
};

#endif // GLOBALFUNCTION_H
