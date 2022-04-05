#ifndef PROGRAMSCTRL_H
#define PROGRAMSCTRL_H

#include <QObject>
#include <QStringList>
#include "programfile.h"

#define OP_PROGRAM_CTRL_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(QStringList, programNameList_)\
    OP_MARCRO(QStringList, controllerFileList_)


class Program;
struct PointValue;

const QString PROGRAMDIRNAME = "lhPrograms";
class ProgramsCtrl : public QObject
{
    Q_OBJECT
public:
    explicit ProgramsCtrl(QObject *parent = 0);
    static ProgramsCtrl* getInstance();
    int newProgram(QString folderPath, QString PrjName);
    int loadProgram(QString LHPrjPath);

    void operateAddNewProgramInSys(QString programName);
    void operateDeleteProgramInSys(QString programPath);
    void removeProgram(QString programName);
    Program* getActiveProgram();
    void setActiveProgram(QString programName);
    Program* getProgram(QString programName);
    QString& getCopySourcePath();
    QList< Program* >* getProgramList();
    void updateControllerFileList();

    int downLoadToControllerFile(QString program, QString file);
    int downLoadToControllerFile(QString program);
    int upLoadFromControllerFile(QString program, QString file);
    int upLoadFromControllerFile(QString program);
    int upLoadFromControllerFileToByteArray(QString file, QByteArray& data);
    int delControllerFile(QString file);
    int delControllerGarbageFiles();
    int delLocalFile(QString program, QString filename);
    int delLocalGarbageFiles(QString program);
    int upLoadFromControllerActivePoints();
signals:
    void updateActivePoint(QList<PointValue>);
    void updateProgramFileList(QString, QStringList);

    OP_PROGRAM_CTRL_ALL_OBJ(DECL_SIGNAL_METADATA);
    OP_PROGRAM_CTRL_ALL_OBJ(REGISTER_METADATA);
public slots:
private:
    static ProgramsCtrl* instance_;
    QList< Program* >* programList_;
    Program* activeProgram_;


};

#endif // PROGRAMSCTRL_H
