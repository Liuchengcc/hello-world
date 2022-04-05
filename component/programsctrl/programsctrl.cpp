#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include "programsctrl.h"
#include <QDebug>
#include <QDateTime>
#include <globalfunction.h>
#include <ftpmanager.h>
#include <controller.h>
#include <config.h>

#define UPLOAD_IGNORE_TYPE "log LOG lhlog LHLOG dat DAT ff txt config bin bmp xml"
ProgramsCtrl* ProgramsCtrl::instance_ = NULL;
ProgramsCtrl* ProgramsCtrl::getInstance()
{
    if(instance_ == NULL)
    {
        instance_ = new ProgramsCtrl;
    }
    return instance_;
}

int ProgramsCtrl::newProgram(QString folderPath, QString PrjName)
{
    operateAddNewProgramInSys(folderPath+PrjName);
    QDir prjDir(folderPath + PrjName);
    prjDir.setNameFilters(QStringList()<<"*.lhprj");
    QString prjFileName;
    if(prjDir.count() > 0)
    {
        prjFileName = prjDir.entryList().at(0);
        QFile::rename(folderPath +PrjName + "/" + prjFileName, folderPath + PrjName+ "/" + PrjName+".lhprj");
    }
    return 0;
}

int ProgramsCtrl::loadProgram(QString LHPrjPath)
{
    if(programNameList_.contains(LHPrjPath.split("/").last().split(".").at(0)))
    {
        return -1;
    }
    Program* program = new Program(LHPrjPath);
    connect(program, &Program::change_pointsList_, this, [this, program](QList<PointValue> ptList)
    {
        qDebug()<<__FILE__<<__LINE__ << "change point List_";
        if(activeProgram_ == program)
        {
            emit updateActivePoint(ptList);
        }
    });
    connect(program, &Program::change_fileNamesList_, this, [this, program](QStringList fileList)
    {
        QString pgName = program->get_programName_();
        emit updateProgramFileList(pgName, fileList);
    });
    programList_->append(program);
    QStringList tempPgNames = get_programNameList_();
    tempPgNames.append(program->get_programName_());
    set_programNameList_(tempPgNames);
    if(programList_->size() == 1)
    {
        setActiveProgram(program->get_programName_());
    }
    return 0;
}

ProgramsCtrl::ProgramsCtrl(QObject *parent) :
    QObject(parent),
    programList_(new QList< Program* >),
    activeProgram_(nullptr)
{

}

#define DEFAULT_PATH_1 "/res/default/"
#define DEFAULT_PATH_2 ":/res/default/"
void ProgramsCtrl::operateAddNewProgramInSys(QString prjPath)
{
    QString path = QDir::currentPath()+DEFAULT_PATH_1;
    GlobalFunction::mkdirIfNotExist(prjPath);
    QDir dir1 = QDir(path);
    if(dir1.exists())
    {
        GlobalFunction::copyDirectoryFiles(path, prjPath);
    }
    else
    {
        GlobalFunction::copyDirectoryFiles(DEFAULT_PATH_2, prjPath);
    }
}

void ProgramsCtrl::operateDeleteProgramInSys(QString programPath)
{
    GlobalFunction::rmDir(programPath);
}

void ProgramsCtrl::removeProgram(QString programName)
{
    Program* curProgram = getProgram(programName);
    programList_->removeOne(curProgram);
    QStringList pgnameList = get_programNameList_();
    pgnameList.removeOne(programName);
    set_programNameList_(pgnameList);
    curProgram->deleteLater();
}

Program *ProgramsCtrl::getActiveProgram()
{
    return activeProgram_;
}

void ProgramsCtrl::setActiveProgram(QString programName)
{
    activeProgram_ = getProgram(programName);
    activeProgram_->change_pointsList_(activeProgram_->get_pointsList_());
}

Program* ProgramsCtrl::getProgram(QString programName)
{
    qDebug()<<"programName = "<<programName;
    for(QList< Program* >::iterator it = (*programList_).begin(); it != (*programList_).end(); it++)
    {
        if((*it)->get_programName_() == programName)
        {
            return *it;
        }
    }
    //return NULL;
    qDebug()<<"return NULL";
    return (*programList_)[0];
}

QList<Program *> *ProgramsCtrl::getProgramList()
{
    return programList_;
}

void ProgramsCtrl::updateControllerFileList()
{
    set_controllerFileList_(FtpManager::getInstance()->getFileList());
}

int ProgramsCtrl::downLoadToControllerFile(QString program, QString file)
{
    QString filepath = getProgram(program)->getProgramFile(file)->get_filePath_();
    QByteArray contents;
    GlobalFunction::ReadDocFileToByteArray(filepath, contents);
    if(contents.size() < 10)
    {
        qDebug()<<"file is empty";
        return -1;
    }
    FtpManager::getInstance()->putFile(contents, file);
    updateControllerFileList();
    return 0;
}

int ProgramsCtrl::downLoadToControllerFile(QString program)
{
    QStringList fileList = getProgram(program)->get_fileNamesList_();
    foreach (QString file, fileList)
    {
        if(QString(UPLOAD_IGNORE_TYPE).split(" ").contains(file.split(".").last())){continue;}
        downLoadToControllerFile(program, file);
    }
    return 0;
}

int ProgramsCtrl::upLoadFromControllerFile(QString program, QString file)
{
    QByteArray contents = FtpManager::getInstance()->getFile(file);
    if(contents.size() < 10)
    {
        qDebug()<<"upload file is empty";
        return -1;
    }
    getProgram(program)->addOrOverwriteNewProgramFile(file, contents);
    return 0;
}

int ProgramsCtrl::upLoadFromControllerFile(QString program)
{
    foreach (QString file, controllerFileList_)
    {
        if(QString(UPLOAD_IGNORE_TYPE).split(" ").contains(file.split(".").last())){continue;}
        upLoadFromControllerFile(program, file);
    }
    return 0;
}

int ProgramsCtrl::upLoadFromControllerFileToByteArray(QString file, QByteArray &data)
{
    data = FtpManager::getInstance()->getFile(file);
    return 0;
}

int ProgramsCtrl::delControllerFile(QString fileName)
{
    FtpManager::getInstance()->delFile(fileName);
    updateControllerFileList();
    return 0;
}

int ProgramsCtrl::delControllerGarbageFiles()
{
     foreach (QString file, controllerFileList_)
    {
        if(QString(UPLOAD_IGNORE_TYPE).split(" ").contains(file.split(".").last()))
        {
            delControllerFile(file);
        }
    }
     return 0;
}

int ProgramsCtrl::delLocalFile(QString program, QString filename)
{
    getProgram(program)->deleteProgramFile(filename);
    return 0;
}

int ProgramsCtrl::delLocalGarbageFiles(QString program)
{
   QStringList fileList = getProgram(program)->get_fileNamesList_();
   foreach(QString file, fileList)
   {
         if(QString(UPLOAD_IGNORE_TYPE).split(" ").contains(file.split(".").last()))
        {
            delLocalFile(program, file);
        }
   }
   return 0;
}

int ProgramsCtrl::upLoadFromControllerActivePoints()
{
    if(!activeProgram_)
    {
        return -1;
    }
    QString pgName = activeProgram_->get_programName_();
    QString filename = POINTFILE;
    upLoadFromControllerFile(pgName, filename);
    return 0;
}


