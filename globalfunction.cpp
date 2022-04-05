#include "globalfunction.h"
#include "json/value.h"
#include "json/reader.h"
#include "json/writer.h"
#include "unistd.h"
#include <fstream>
#include <assert.h>
#include <QTime>
#include <QApplication>
#include <QFontDatabase>
#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QRegExp>
#include <CRC32.h>
#include <logoutform.h>
#include <config.h>

const QString defaultLHPath =  ":/pic/fileRes/lh.json";

GlobalFunction::GlobalFunction(QObject *parent) :
    QObject(parent)
{

}

Json::Value GlobalFunction::ReadJsonFromJsonfile(QString filePath)
{
    std::ifstream fin(filePath.toStdString());
    if(!fin.is_open())
    {
        showLogMsg(tr("文件%1无法打开！").arg(filePath));
        return Json::Value();
    }
    Json::Reader reader;
    Json::Value root;
    reader.parse(fin, root);
    if(root.type() != Json::objectValue)
    {
        Json::Value temproot;
        root = temproot;
    }
    fin.close();
    return root;
}

Json::Value GlobalFunction::ReadJsonFromJsonStr(QString data)
{
    Json::Reader reader;
    Json::Value root;
    reader.parse(data.toStdString(), root);
    return root;
}

int GlobalFunction::WriteJsonToJsonfile(Json::Value root, QString filePath)
{
    Json::StyledWriter sw;
    std::string sOut = sw.write(root);
    std::ofstream fout(filePath.toStdString().c_str());
    fout<<sOut;
    fout.close();
//    system("sync");
    return root.size();
}

int GlobalFunction::WriteJsonToQByteArray(Json::Value root, QByteArray &arrayData)
{
    Json::StyledWriter sw;
    std::string sOut = sw.write(root);
    arrayData = QByteArray(sOut.c_str(), sOut.size());
    return arrayData.size();
}

int GlobalFunction::ReadDocFileToByteArray(QString path, QByteArray& text)
{
    if(!QFile::exists(path))
    {
        return -1;
    }
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        return -2;
    }
    text = file.readAll();
    file.close();
    return 0;
}

QString GlobalFunction::CalFileCrcInUnix(QString path)
{
     QString crc;
    if(!QFile::exists(path))
    {
        return crc;
    }
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        return crc;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString fileContents;
    fileContents = in.readAll();
    file.close();
    char CrcRstBuf[64];
    memset(CrcRstBuf, 0, 64);
    DoCrcStatic(fileContents.toLatin1().data(), fileContents.size(), CrcRstBuf);//get crcRstPoint
    crc = CrcRstBuf;
    return crc;

}

QString GlobalFunction::CalFileCrcAuto(QString path)
{
    QString sys = Config::getInstance()->get_fileFormat_();
    return CalFileCrcInUnix(path);
}

int GlobalFunction::WriteByteArrayToDocFile(QString path, QByteArray &data)
{
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly))
        {
            showLogMsg(tr("打开文件失败!"));
        }
        if(file.write(data) < 0)
        {
            showLogMsg(tr("写入文件失败!"));
        }
        return 0;
}

bool GlobalFunction::copyDirectoryFiles
(QString fromDir, QString toDir, bool coverFileIfExist)
{
    QDir sourceDir(fromDir);
        QDir targetDir(toDir);
        if(!targetDir.exists()){
            if(!targetDir.mkdir(targetDir.absolutePath()))
                return false;
        }

        QFileInfoList fileInfoList = sourceDir.entryInfoList();
        foreach(QFileInfo fileInfo, fileInfoList){
            if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
                continue;

            if(fileInfo.isDir()){
                if(!copyDirectoryFiles(fileInfo.filePath(),
                    targetDir.filePath(fileInfo.fileName()),
                    coverFileIfExist))
                    return false;
            }
            else{
                if(coverFileIfExist && targetDir.exists(fileInfo.fileName())){
                    targetDir.remove(fileInfo.fileName());
                }

                if(!QFile::copy(fileInfo.filePath(),
                    targetDir.filePath(fileInfo.fileName()))){
                        return false;
                }
            }
        }
        return true;
}

bool GlobalFunction::copyFileToPath(QString sourceDir, QString desDir, bool isOverWrite)
{
    desDir.replace("\\", "/");
    if(sourceDir == desDir)
    {
        return true;
    }
    if(!QFile::exists(sourceDir))
    {
        return false;
    }
    QDir createfile;
    if(createfile.exists(desDir))
    {
        if(isOverWrite)
        {
            createfile.remove(desDir);
        }
    }
    if(!QFile::copy(sourceDir, desDir))
    {
        return false;
    }
    QFile::setPermissions(desDir, QFile::WriteUser|QFile::ReadUser|QFile::WriteGroup|QFile::ReadGroup);
    return true;
}

bool GlobalFunction::mkdirIfNotExist(QString path)
{
    QDir dir(path);
    if(!dir.exists())
    {
        dir.mkdir(path);
    }
    return true;
}

bool GlobalFunction::createFileInPath(QString path, QString fileName)
{
    QDir tempDir;
    if(!tempDir.exists(path))
    {
        tempDir.mkpath(path);
    }
    QFile file(path + "/" + fileName);
    file.open(QIODevice::ReadWrite );
    file.close();
    return true;
}

bool GlobalFunction::rmDir(QString dirPath)
{
    if(dirPath.isEmpty())
    {
        return false;
    }
    QDir dir(dirPath);
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    foreach (QFileInfo file, fileList)
    {
        if (file.isFile())
        {
            file.dir().remove(file.fileName());
        }
        else
        {
            rmDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath());
}

bool GlobalFunction::rmFile(QString filePath)
{
    return QFile::remove(filePath);
}

void GlobalFunction::Delayms(unsigned int msec)
{
    QTime reachTime = QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime() < reachTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

bool GlobalFunction::isStrMatchReg(QString str, QString reg)
{
    QRegExp rx(reg);
    return rx.exactMatch(str);
}

bool GlobalFunction::isFloatParamValid(QString str, bool isPos, bool isFloat, int leftCount, int rightCount)
{
    QString symbol;
        if(isPos)
        {
            symbol = "";
        }
        else
        {
            symbol = "(-)?";
        }

        QString symbolFloat;
        if(isFloat)
        {
            symbolFloat = "0,1";
        }
        else
        {
            symbolFloat = "0";
        }
        QString exp = "^"+symbol+"[1-9]\\d{0,"+QString::number((leftCount - 1),10)+"}(\\."+"{"+symbolFloat+"}"+"\\d{"+"0,"+QString::number((rightCount),10)+"})?|" + "^"+symbol + "0(\\."+"{"+symbolFloat+"}"+"\\d{0,"+QString::number((rightCount),10)+"})?";
        return isStrMatchReg(str, exp);
}

bool GlobalFunction::isNameValid(QString str)
{
    return isStrMatchReg(str,"[a-zA-Z][0-9a-zA-Z]{0,31}");
}

bool GlobalFunction::isDescribValid(QString str)
{
    return isStrMatchReg(str,"[_0-9a-zA-Z]{1,32}");
}

bool GlobalFunction::isIPValid(QString str)
{
    return isStrMatchReg(str,"^([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])\\.([1]?\\d\\d?|2[0-4]\\d|25[0-5])$");
}

QString GlobalFunction::MD5(const QByteArray &data)
{
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(data);
    return hash.result().toHex();
}
void GlobalFunction::showLogMsg(QString logData)
{
    LogoutForm::getInstance()->showLogMsg(logData);
}
