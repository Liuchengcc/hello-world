#include "programfile.h"
#include <QCoreApplication>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QRegExp>
#include <globalfunction.h>

QMap<ProgramFile::FileType, QString> ProgramFile::fileTypeNameMap_;
ProgramFile::ProgramFile(QString path, QObject *parent):
    QObject(parent)
{
    set_filePath_(path);
    if(filePath_.split(".").last() == "lh")
    {
        fileType_ = JobFile;
    }
    else if(filePath_.split(".").last() == "pt")
    {
        fileType_ = PointFile;
    }
    else if(filePath_.right(5) == "ls.cr")
    {
        fileType_ = ToolFile;
    }
    else if(filePath_.right(5) == "rs.cr")
    {
        fileType_ = UserFile;
    }
    else if(filePath_.right(3) == "log")
    {
        fileType_ = LogFile;
    }
    else if(filePath_.right(4) == "json")
    {
        fileType_ = ConfigFile;
    }
    else if(filePath_.right(3) == "ist")
    {
        fileType_ = InstanceFile;
    }
    else
    {
        fileType_ = OtherFile;
    }
    name_ = path.split("/").last();
}

ProgramFile::ProgramFile(QString path, ProgramFile::FileType type, QObject *parent):QObject(parent)
{
    set_filePath_(path);
    fileType_ = type;
    name_ = path.split("/").last();
}

void ProgramFile::saveToFile()
{
    QFile file(filePath_);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug()<<tr("can not open file %1").arg(filePath_);
        return;
    }
    QTextStream out(&file);
    QString writeFile = content_;
    out<<writeFile<<endl;
    qDebug()<<"filePath :"<<filePath_;
    qDebug()<<"content :"<<content_;
    out.flush();
    file.close();
    GlobalFunction::Delayms(200);
//    system("sync");
}

void ProgramFile::openFileToContent()
{
    QFile file(filePath_);
    if(!file.open(QFile::ReadWrite))
    {
        qDebug()<<tr("can not open file %1").arg(filePath_);
        return;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    content_.clear();
    QString fileLine, fileLines;
    while(!in.atEnd())
    {
        fileLine = in.readLine();
        fileLine.replace(QRegExp("\t"), "    ");
        fileLines += (fileLine+"\r\n");
    }
    content_= fileLines;
    file.close();
}

QString ProgramFile::getFileName()
{
    return name_;
}

ProgramFile::FileType ProgramFile::getFileType()
{
    return fileType_;
}

QString* ProgramFile::getFileContent()
{
    return &content_;
}

QMap<ProgramFile::FileType, QString> ProgramFile::getTypeName()
{
    if(fileTypeNameMap_.isEmpty())
    {
        QStringList typeNameList = QString(FILE_TYPE_NAME).split(FILE_TYPE_NAME_SPLIT_TOKEN);
        for(int i = 0; i < typeNameList.size(); i++)
        {
            fileTypeNameMap_[(ProgramFile::FileType)i] = typeNameList.at(i);
        }

    }
    return fileTypeNameMap_;
}
