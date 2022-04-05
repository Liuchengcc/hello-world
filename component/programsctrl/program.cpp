#include "program.h"
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <globalfunction.h>
#include <json/value.h>
#include <marcroly.h>

#define AXIS_SUM 32
const QString c_axisFileType("axis");
const QString c_fileSumName("FileNum");

    #define PROGRAM_GROUP "programs"
    #define POINT_GROUP "points"
    #define USER_GROUP "users"
    #define TOOL_GROUP "tools"
    #define INSTANCE_GROUP "instances"
    #define CONFIG_GROUP "configs"
    #define OTHER_GROUP "others"

#define OP_ALL_FILE_TYPE(OP_FUNC) OP_FUNC(PROGRAM_GROUP)\
    OP_FUNC(POINT_GROUP)\
    OP_FUNC(USER_GROUP)\
    OP_FUNC(TOOL_GROUP)\
    OP_FUNC(INSTANCE_GROUP)\
    OP_FUNC(CONFIG_GROUP)\
    OP_FUNC(OTHER_GROUP)

    #define PROGRAM_GROUP_TYPE JobFile
    #define POINT_GROUP_TYPE PointFile
    #define USER_GROUP_TYPE UserFile
    #define TOOL_GROUP_TYPE ToolFile
    #define INSTANCE_GROUP_TYPE InstanceFile
    #define CONFIG_GROUP_TYPE ConfigFile
    #define OTHER_GROUP_TYPE OtherFile

Program::Program(QString path, QObject *parent):
    QObject(parent),
    lhprjPath_(path),
    programFileList_(new QList<ProgramFile*>)
{
    programName_ = path.split("/").last().split(".").at(0);
    AxisVec_ = QVector<Axis>(AXIS_SUM);
    QString programSettingPath = path;
    path_ = path.left(path.size() - path.split("/").last().size());
    if(QFile::exists(programSettingPath))
    {
        searchAndUpdateProgramFilesByHeadFile();
    }
    readAndParserAllParam();
    if(readAndParserAxisesFile())
    {
        initDefaultAxisFile();
    }
}

Program::~Program()
{
    foreach (ProgramFile* file, *programFileList_)
    {
        programFileList_->removeOne(file);
        file->deleteLater();
    }
}

void Program::searchAndUpdateProgramFilesByHeadFile()
{
    programFileList_->clear();
    fileNamesList_.clear();
    Json::Value root = GlobalFunction::ReadJsonFromJsonfile(lhprjPath_);
    auto getProgramName = [this, root](QString type, ProgramFile::FileType filetype)
    {
        int sum = root["project"][type.toStdString()]["files"].size();
        for(int i = 0; i< sum ; i++)
        {
            QString filename = QString::fromStdString(root["project"][type.toStdString()]["files"][i].asString());
            fileNamesList_.append(filename);
            programFileList_->append(new ProgramFile(path_ +"/" + filename, filetype, this));
        }
    };
#define GET_PROGRAM_NAME(NAME) getProgramName(NAME, ProgramFile::NAME##_TYPE);
    OP_ALL_FILE_TYPE(GET_PROGRAM_NAME);
    set_filesSum_(fileNamesList_.size());
}

void Program::readAndParserAllParam()
{
    readAndParserPointsFile();
    readAndParserToolsFile();
    readAndParserUsersFile();
    readAndParserAxisesFile();
}

void Program::saveToProgramHeadFile()
{
    Json::Value root;
    root["verison"] = "1.0";
    auto saveFileWithType = [this, &root]( QString typeName, ProgramFile::FileType type)
    {
        QStringList files;
        int index = 0;
        foreach(ProgramFile* file, *programFileList_)
        {
            if(file->getFileType() == type)
            {
                root["project"][typeName.toStdString()]["files"][index] = file->getFileName().toStdString();
                index++;
            }
        }
        root["project"][typeName.toStdString()]["sums"] = QString::number(index).toStdString();
    };
#define SAVE_FILE_WITH_TYPE(NAME) saveFileWithType(NAME, ProgramFile::NAME##_TYPE);
    OP_ALL_FILE_TYPE(SAVE_FILE_WITH_TYPE);
    GlobalFunction::WriteJsonToJsonfile(root, lhprjPath_);
}

QList<ProgramFile *> *Program::getProgramFileList()
{
    return programFileList_;
}

int Program::readAndParserAxisesFile()
{
    ProgramFile *pgFile = getProgramFile(SETTING_AXIS_FILE);
    if(!pgFile)
    {
        return -1;
    }
    QString axisFilePath = path_ + SETTING_AXIS_FILE;
    Json::Value root = GlobalFunction::ReadJsonFromJsonfile(axisFilePath);
    if(QString::fromStdString(root["type"].asString()) != c_axisFileType)
    {
        return -1;
    }
    axisesSum_ = QString::fromStdString(root["axisesSum"].asString());
#define PARSER_AXIS(AXIS_VEC, data) AXIS_VEC[i].data = QString::fromStdString(root["axises"][i][#data].asString());
    for(int i = 0; i < AXIS_SUM; i++)
    {
        DO_AXIS_STRUCT(PARSER_AXIS, AxisVec_);
    }
#undef PARSER_AXIS
    return 0;
}


void Program::saveAxisDataAndToFile(QVector<Axis> axisVec)
{
    saveAxisDataAndToFile(axisesSum_, axisVec);
}

void Program::saveAxisDataAndToFile(QString axisesSum, QVector<Axis> axisVec)
{

    set_AxisVec_(axisVec);
    set_axisesSum_(axisesSum);
    Json::Value root;
    root["type"]= c_axisFileType.toStdString();
    root["axisesSum"] = axisesSum.toStdString();
#define SAVE_AXIS(AXIS_VEC, data) root["axises"][i][#data] = AXIS_VEC[i].data.toStdString();
    for(int i = 0; i < AXIS_SUM; i++)
    {
        DO_AXIS_STRUCT(SAVE_AXIS, AxisVec_);
    }
    QString axisFilePath = path_ + SETTING_AXIS_FILE;
    GlobalFunction::WriteJsonToJsonfile(root, axisFilePath);
#undef SAVE_AXIS
    return;
}

int Program::readAndParserPointsFile()
{
    ProgramFile *pgFile = getProgramFile(POINTFILE);
    if(!pgFile)
    {
        return -1;
    }
    QString filePath= path_ + POINTFILE;
    Json::Value root = GlobalFunction::ReadJsonFromJsonfile(filePath);
    int sum = root["Robot"]["Points"].size();
    QList<PointValue> ptList;
    for(int i = 0; i < sum; i++)
    {
        QString pointName = QString::fromStdString(root["Robot"]["Points"][i].asString());
        PointValue tempPoint;
        tempPoint.name= pointName;
       #define PARSER_POINT_DOUBLE(ELEMENT)\
        tempPoint.ELEMENT = QString::number(root[pointName.toStdString()][#ELEMENT].asDouble());
        DO_FOREACH_ELEMENT(9, PARSER_POINT_DOUBLE, X, Y, Z, U, V, W, R, S, T);
        tempPoint.description = QString::fromStdString(root[pointName.toStdString()]["Des"].asString());
        tempPoint.robotID = QString::number(root[pointName.toStdString()]["RobotID"].asInt());
        tempPoint.Local = QString::number(root[pointName.toStdString()]["Lo"].asInt());
        tempPoint.Hand = QString::number(root[pointName.toStdString()]["H"].asInt());
        tempPoint.Wrist = QString::number(root[pointName.toStdString()]["Wr"].asInt());
        tempPoint.J1Flag = QString::number(root[pointName.toStdString()]["J1"].asInt());
        tempPoint.J2Flag = QString::number(root[pointName.toStdString()]["J2"].asInt());
        tempPoint.J4Flag = QString::number(root[pointName.toStdString()]["J4"].asInt());
        tempPoint.J6Flag = QString::number(root[pointName.toStdString()]["J6"].asInt());
        tempPoint.J1Angle = QString::number(root[pointName.toStdString()]["J1a"].asInt());
        tempPoint.J4Angle = QString::number(root[pointName.toStdString()]["J4a"].asInt());
        tempPoint.Edit = QString::number(root[pointName.toStdString()]["Edit"].asInt());
        ptList.append(tempPoint);
    }
    set_pointsList_(ptList);
    return 0;
}

#define PARSER_COORD_DOUBLE(ELEMENT)\
    tempValue.ELEMENT = QString::number(root[name.toStdString()][#ELEMENT].asDouble());

#define READ_PARSER_COORD(FILE_NAME, COORD_TYPE, COORD_LIST_OBJ, SET_COORD_LIST_FUNC) \
    if(!getProgramFile(FILE_NAME))return -1;\
    QString filePath = path_ + FILE_NAME;\
     Json::Value root = GlobalFunction::ReadJsonFromJsonfile(filePath);\
    int sum = root["Robot"][#COORD_TYPE].size();\
    QList<CoordValue> coordList;\
    for(int i = 0; i < sum; i++)\
    {\
        QString name = QString::fromStdString(root["Robot"][#COORD_TYPE][i].asString());\
        CoordValue tempValue;\
        tempValue.name = QString::fromStdString(name.toStdString());\
        tempValue.Des = QString::fromStdString(root[name.toStdString()]["Des"].asString());\
        DO_FOREACH_ELEMENT(6, PARSER_COORD_DOUBLE, X, Y, Z, U, V, W);\
        coordList.append(tempValue); \
    }\
    SET_COORD_LIST_FUNC(coordList);\
    return 0;

int Program::readAndParserToolsFile()
{
    READ_PARSER_COORD(TOOLFILE, Tools, toolCoordsList_, set_toolCoordsList_);
}

int Program::readAndParserUsersFile()
{
    READ_PARSER_COORD(USERFILE, Users, userCoordsList_, set_userCoordsList_);
}

bool Program::containFile(QString name)
{
    return fileNamesList_.contains(name);
}

ProgramFile *Program::getProgramFile(QString fileName)
{
    for(QList< ProgramFile* >::iterator it = programFileList_->begin();it != programFileList_->end(); ++it)
    {
        if((*it)->getFileName() == fileName)
        {
            return (*it);
        }
    }
    return NULL;
}

void Program::addOrOverwriteNewProgramFile(QString fileName, QByteArray& data)
{
    GlobalFunction::WriteByteArrayToDocFile(get_path_()+fileName, data);
    if(fileNamesList_.contains(fileName))
    {
        qDebug()<<"over write file:"<<fileName;
    }
    else
    {
        programFileList_->append(new ProgramFile(path_ + fileName, this));
        QStringList tempfileNameList = get_fileNamesList_();
        tempfileNameList.append(fileName);
        set_fileNamesList_(tempfileNameList);
        int fileSum = get_filesSum_()+1;
        set_filesSum_(fileSum);
        saveToProgramHeadFile();
    }
        if(getProgramFile(fileName)->getFileType() == ProgramFile::PointFile){readAndParserPointsFile();}
        if(getProgramFile(fileName)->getFileType() == ProgramFile::ToolFile){readAndParserToolsFile();}
        if(getProgramFile(fileName)->getFileType() == ProgramFile::UserFile){readAndParserUsersFile();}
}

void Program::deleteProgramFile(QString fileName)
{
    ProgramFile* tempFile = getProgramFile(fileName);
    if(!tempFile)
    {
        return;
    }
    programFileList_->removeOne(tempFile);
    tempFile->deleteLater();
    QStringList tempfileNameList = get_fileNamesList_();
    tempfileNameList.removeOne(fileName);
    set_fileNamesList_(tempfileNameList);
    int fileSum = get_filesSum_()-1;
    set_filesSum_(fileSum);
    saveToProgramHeadFile();
    GlobalFunction::rmFile(tempFile->get_filePath_());
}

void Program::initDefaultAxisFile()
{
    QString axisFilePath = path_ + SETTING_AXIS_FILE;
    Json::Value root;
    root["type"] = c_axisFileType.toStdString();
    #define DEFAULT_AXIS(axisVec,data) root["axises"][i][#data] = axisVec[i].data.toStdString();
        for(int i = 0; i < AXIS_SUM; i++)
        {
            DO_AXIS_STRUCT(DEFAULT_AXIS, AxisVec_);
        }
    #undef DEFAULT_AXIS
        QByteArray docData;
        GlobalFunction::WriteJsonToQByteArray(root, docData);
        addOrOverwriteNewProgramFile(SETTING_AXIS_FILE, docData);
}
