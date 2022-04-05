#ifndef INSTANCEINTERFACE_H
#define INSTANCEINTERFACE_H

#include <QWidget>
#include <basewidget.h>
#include <qstringlist.h>
#include <axisconfigform.h>

#define DO_INSTANCE_INHERIT_AXIS(FUNC, INS)\
    FUNC(INS, VelLim)/*关节最大速度限制*/\
    FUNC(INS, AccLim)/*关节最大加速度限制*/\
    FUNC(INS, PosPLim)/*关节正极限*/\
    FUNC(INS, PosNLim)/*关节负极限*/

#define DO_INSTANCE_STRUCT(FUNC, FUNC_ARRAY, INS)\
    FUNC(INS, type)/*类型*/\
    FUNC(INS, RobotID)/*实例号*/\
    FUNC(INS, addingAxisSum)/*附加轴个数*/\
    FUNC(INS, RobotDof)/*自由度*/\
    FUNC(INS, RobotType)/*机器人类型*/\
    FUNC(INS, AXISNum)/*轴总数*/\
    FUNC_ARRAY(INS, AxisID)/*轴ID号*/\
    FUNC(INS, Car_MaxPVel)/*笛卡尔位置最大速度 mm/s*/\
    FUNC(INS, Car_MaxGVel)/*笛卡尔姿态最大速度 deg/s*/\
    FUNC(INS, Car_MaxPAcc)/*笛卡尔位置加速度*/\
    FUNC(INS, Car_MaxGAcc)/*笛卡尔姿态加速度*/\
    FUNC_ARRAY(INS, Robot_Link)/*连杆参数*/\
    FUNC(INS, Car_StopPDec)/*笛卡尔位置停止减速度*/\
    FUNC(INS, Car_StopGDec)/*笛卡尔姿态停止减速度*/\
    FUNC_ARRAY(INS, Joint_StopDec)/*关节停止减速度*/\
    FUNC_ARRAY(INS, CoupleRatio)/*耦合比*/\

//    DO_INSTANCE_INHERIT_AXIS(FUNC, INS)/*和Aixs共有的参数*/

#define DECL_STRUCT_VALUE(INIT_PTR, VALUE) QString VALUE;
#define DECL_STRUCT_ARRAY_VALUE(INIT_PTR, VALUE) QStringList VALUE;

struct Puma
{
    DO_INSTANCE_STRUCT(DECL_STRUCT_VALUE, DECL_STRUCT_ARRAY_VALUE, PTR);
};

struct Scara
{
    DO_INSTANCE_STRUCT(DECL_STRUCT_VALUE, DECL_STRUCT_ARRAY_VALUE, PTR);
};

struct Module
{
    DO_INSTANCE_STRUCT(DECL_STRUCT_VALUE, DECL_STRUCT_ARRAY_VALUE, PTR);
};
#undef DECL_STRUCT_VALUE
#undef DECL_STRUCT_ARRAY_VALUE

class BaseWidget;
class InstanceInterface : public QWidget
{
    Q_OBJECT
public:
    explicit InstanceInterface(QWidget *parent = nullptr);

    static BaseWidget* getInstance(QString path);
signals:

};

#endif // INSTANCEINTERFACE_H
