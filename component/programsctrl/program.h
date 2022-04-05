#ifndef PROGRAM_H
#define PROGRAM_H

#include <QObject>
#include <programfile.h>
#include <programsctrl.h>
#include <QSettings>
#include <QStringList>
#include <QSharedPointer>
#include <marcroly.h>

class ProgramFile;
#define DO_AXIS_STRUCT(FUNC,AXIS_PTR) \
    FUNC(AXIS_PTR,GearRatio);/*减速比*/\
    FUNC(AXIS_PTR,Precision);/*分辨率*/\
    FUNC(AXIS_PTR,FeedConstant);/*当量比*/\
    FUNC(AXIS_PTR,AbsZero);/*绝对零点*/\
    FUNC(AXIS_PTR,HomeOffset);/*零点偏移*/\
    FUNC(AXIS_PTR,Motor_Dir);/*电机方向*/\
    FUNC(AXIS_PTR,Model_Dir);/*模型方向*/\
    FUNC(AXIS_PTR,PosPLim);/*正极限*/\
    FUNC(AXIS_PTR,PosNLim);/*负极限*/\
    FUNC(AXIS_PTR,VelLim);/*速度极限*/\
    FUNC(AXIS_PTR,AccLim)/*加速度极限*/;

#define OP_PROGRAM_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(QString, path_)\
    OP_MARCRO(QString, lhprjPath_)\
    OP_MARCRO(QString, programName_)\
    OP_MARCRO(int, filesSum_)\
    OP_MARCRO(QVector<Axis>, AxisVec_)\
    OP_MARCRO(QString, axisesSum_)\
    OP_MARCRO(QStringList, fileNamesList_)\
    OP_MARCRO(QList<PointValue>, pointsList_)\
    OP_MARCRO(QList<CoordValue>, userCoordsList_)\
    OP_MARCRO(QList<CoordValue>, toolCoordsList_)

struct Axis
{
public:
    #define DECL_STRUCT_VALUE(INIT_PTR, VALUE) QString VALUE;
    DO_AXIS_STRUCT(DECL_STRUCT_VALUE, PTR);
};

struct CoordValue
{
    QString name;
    QString Des;
    QString X;
    QString Y;
    QString Z;
    QString U;
    QString V;
    QString W;
};

struct PointValue
{
    QString name;
    QString description;
    QString robotID;
    QString X;
    QString Y;
    QString Z;
    QString U;
    QString V;
    QString W;

    QString R;
    QString S;
    QString T;

    QString Local;

    QString Hand;
    QString Elbow;
    QString Wrist;

    QString J1Flag;
    QString J2Flag;
    QString J4Flag;
    QString J6Flag;

    QString J1Angle;
    QString J4Angle;
    QString Edit;
};


class Program : public QObject
{
    Q_OBJECT
public:
    explicit Program(QString path, QObject *parent = nullptr);
    ~Program();
    void searchAndUpdateProgramFilesByHeadFile();
    void readAndParserAllParam();
    void saveToProgramHeadFile();
    void addOrOverwriteNewProgramFile(QString fileName, QByteArray& data);
    void deleteProgramFile(QString fileName);
    void initDefaultAxisFile();
    bool containFile(QString name);
    ProgramFile* getProgramFile(QString fileName);
    QList<ProgramFile*>* getProgramFileList();
    int readAndParserAxisesFile();
    void saveAxisDataAndToFile(QString, QVector<Axis>);
    void saveAxisDataAndToFile(QVector<Axis>);
    int readAndParserPointsFile();
    int readAndParserToolsFile();
    int readAndParserUsersFile();
signals:
    OP_PROGRAM_ALL_OBJ(DECL_SIGNAL_METADATA);
    OP_PROGRAM_ALL_OBJ(REGISTER_METADATA);
public slots:

private:
    QList<ProgramFile*>* programFileList_;
    QSharedPointer<QSettings> programConfig_;
};

#endif // PROGRAM_H
