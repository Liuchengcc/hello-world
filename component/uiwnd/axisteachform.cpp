#include "axisteachform.h"
#include "ui_axisteachform.h"
#include "DockManager.h"
#include "qlineedit.h"
#include "qpushbutton.h"
#include "qcheckbox.h"
#include "qcombobox.h"
#include "qlabel.h"
#include "controller.h"
#include "marcroly.h"

const QString c_dockHeadName("轴控制");

#define AXIS_SUM 32
#define TIMEOUT_UPDATEDATA 300
#define COLUMN_HEADER "轴状态 当前位置 当前速度 当前加速度 错误码 上电 下电 速度\n(mm/s) 加速度\n(mm/s2) 减速度\n(mm/s2) 运动方式 步进距离\n(mm) 正向运动 反向运动 目标位置\n(mm) 启动 停止 清错"
#define OP_ONE_AXIS(FUNC)

INSTANCE_MODE(AxisTeachForm, instance_)
AxisTeachForm::AxisTeachForm(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::AxisTeachForm)
{
    ui->setupUi(this);
    initDock();
    initUi();
    startTimer(TIMEOUT_UPDATEDATA);
}

AxisTeachForm::~AxisTeachForm()
{
    delete ui;
}

void AxisTeachForm::initDock()
{
    area_ = ads::CenterDockWidgetArea;
    dock_ = new ads::CDockWidget(c_dockHeadName);
    dock_->setWidget(this);
    dock_->setFeatures(ads::CDockWidget::AllDockWidgetFeatures);
    connect(dock_, &ads::CDockWidget::closeRequested, [this]()
    {
        qDebug()<<"dock requested" ;
        dock_->dockManager()->removeDockWidget(dock_);
    });
    connect(dock_, &ads::CDockWidget::closed, [](){qDebug()<<"dock is closed";});
    connect(dock_, &ads::CDockWidget::visibilityChanged, [](bool status){qDebug()<<"dock is visibility" << status;});
}

void AxisTeachForm::initUi()
{
    ui->tableWidget_axisTeach->setRowCount(AXIS_SUM);
    ui->tableWidget_axisTeach->setColumnCount(tr(COLUMN_HEADER).split(" ").size());
    ui->tableWidget_axisTeach->setHorizontalHeaderLabels(tr(COLUMN_HEADER).split(" "));
    auto InitOneAxis = [this](int num)
    {
        int columnCount = 0;
#define NEW_OBJ(OBJ_TYPE, OBJ_NAME, OBJ_TEXT) OBJ_TYPE* OBJ_NAME = new OBJ_TYPE(this);\
    SET_TEXT(OBJ_NAME, OBJ_TEXT);\
        ui->tableWidget_axisTeach->setCellWidget(num, columnCount, OBJ_NAME);\
        columnCount++;
        NEW_OBJ(QLabel, curState, "");
        NEW_OBJ(QLabel, curPos, "");
        NEW_OBJ(QLabel, curV, "");
        NEW_OBJ(QLabel, curAcc, "");
        NEW_OBJ(QLabel, curErr, "0");
        NEW_OBJ(QPushButton, servoOn, "上电");
        NEW_OBJ(QPushButton, servoOff, "下电");
        NEW_OBJ(QLineEdit, v, "100");
        NEW_OBJ(QLineEdit, acc, "500");
        NEW_OBJ(QLineEdit, dec, "500");
        NEW_OBJ(QComboBox, runType, "连续 步进 运动到点");
        NEW_OBJ(QLineEdit, jogDst, "10");
        NEW_OBJ(QPushButton, jogPos, "正向运动");
        NEW_OBJ(QPushButton, jogNeg, "反向运动");
        NEW_OBJ(QLineEdit, runDst, "0");
        NEW_OBJ(QPushButton, runStart, "运行");
        NEW_OBJ(QPushButton, runStop, "停止");
        NEW_OBJ(QPushButton, clearFault, "clear");
#undef NEW_OBJ
        connect(servoOn, &QPushButton::clicked, [num]{Controller::getInstance()->buildPacketSend(FUNC_TEACH, TEACH_AXIS_SERVO_ON, tr("%1;1;,").arg(num));});
        connect(servoOff, &QPushButton::clicked, [num]{Controller::getInstance()->buildPacketSend(FUNC_TEACH, TEACH_AXIS_SERVO_ON, tr("%1;0;,").arg(num));});
        connect(runType, QOverload<int>::of(&QComboBox::currentIndexChanged), [jogPos, jogNeg, jogDst, runDst, runStart](int index)
        {
            if(index == 2)
            {
                jogDst->setEnabled(false);jogNeg->setEnabled(false);jogPos->setEnabled(false);
                runDst->setEnabled(true);runStart->setEnabled(true);
            }
            else
            {
                jogDst->setEnabled(true);jogNeg->setEnabled(true);jogPos->setEnabled(true);
                runDst->setEnabled(false);runStart->setEnabled(false);
            }
        });
        runType->setCurrentIndex(2);//默认运动到点
#define BOUND_AXIS_RUN(OBJ, DST_OBJ, RUN_DIR) \
        connect(OBJ, &QPushButton::clicked, [v, runType, acc, dec, DST_OBJ, num]\
        {\
        QStringList tempList = tr("jog;step;abs").split(";");\
           QString sendData = tr("%1;%2;%3;%4;%5;%6;%7;,").arg(tempList.at(runType->currentIndex()))\
                   .arg(num)\
                   .arg(v->text())\
                   .arg(acc->text())\
                   .arg(dec->text())\
                   .arg(RUN_DIR)\
                   .arg(DST_OBJ->text());\
           Controller::getInstance()->buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_AXIS_MOVE, 0 , TYPE, sendData);\
        });
        BOUND_AXIS_RUN(jogPos, jogDst, "+");
        BOUND_AXIS_RUN(jogNeg, jogDst, "-");
        BOUND_AXIS_RUN(runStart, runDst, "+");
#undef BOUND_AXIS_RUN
        connect(runStop, &QPushButton::clicked, [v, acc, dec, num]
        {
            QString sendData = tr("stop;%1;%2;%3;%4;+;100;,").arg(num).arg(v->text()).arg(acc->text()).arg(dec->text());
            Controller::getInstance()->buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_AXIS_MOVE, 0 , TYPE, sendData);
        });
        connect(clearFault, &QPushButton::clicked, [num]
        {
            QString sendData = tr("%1;,").arg(num);
            Controller::getInstance()->buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_AXIS_CLEAR_ERROR, 0 , TYPE, sendData);
        });
    };
    for(int i = 0; i < AXIS_SUM; i++)
    {
        InitOneAxis(i);
    }
    connect(Controller::getInstance(), &Controller::recvAxisState, [this](QStringList &axisDataList)
    {
#define GET_AXIS_DATA(INDEX) qobject_cast<QLabel*>(ui->tableWidget_axisTeach->cellWidget(i, INDEX))->setText(axisDataList.at(i).split(";").at(INDEX));
        if(AXIS_SUM != axisDataList.size()){ERROR_OCCUR}
        for(int i = 0; i < AXIS_SUM; i++)
        {
            DO_FOREACH_ELEMENT(5,GET_AXIS_DATA,0,1,2,3,4);
        }
    });
}

void AxisTeachForm::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    if(!isVisible())return;
    if(Controller::getInstance()->get_networkState_() == Controller::net_connected)
    {
       Controller::getInstance()->buildPacketSend(FUNC_SYS, SYS_GET_AXIS_STATE, tr("-1;,"));
    }
}
