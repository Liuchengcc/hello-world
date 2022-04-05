#include "controlwidget.h"
#include "ui_controlwidget.h"
#include "DockManager.h"
#include "controller.h"
#include "marcroly.h"
#include "commondialog.h"
#include "qmessagebox.h"
#include "globalfunction.h"

const QString c_dockHeadName("示教与控制");
ControlWidget *ControlWidget::instance_ = nullptr;
ControlWidget *ControlWidget::getInstance()
{
    if(!instance_)
    {
        instance_ = new ControlWidget;
    }
    return instance_;
}

ControlWidget::ControlWidget(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::ControlWidget)
{
    ui->setupUi(this);
    ctrller_ = Controller::getInstance();
    initDock();
    initUi();
    initNetRecvAndDataChange();
    connect(ui->pushButton_addPt, &QPushButton::clicked, this, [this]()
    {
        //1. 发送添加指令
        //2. 接收到ok之后，同步下载points.cr文件
        //3. update points 数据
        CommonDialog dia(tr("添加点位"), QStringList(tr("点位名 描述 权限").split(" ")), this);
        if(CommonDialog::Accepted == dia.exec())
        {
            QString name= dia.getDialogData()[0];
            QString des = dia.getDialogData()[1];
            QString edit= dia.getDialogData()[2];
            int user = ui->comboBox_Users->currentIndex();
            GlobalFunction::showLogMsg(tr("添加点位:%1").arg(name));
            ctrller_->buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_ADD_CUR_PT_2_LIST, 0, TYPE, name+";"+des+";"+QString::number(user)+";"+ edit + ";,");
        }
    });
    connect(ui->pushButton_modPt, &QPushButton::clicked, this, [this]()
    {
        QString name = ui->comboBox_Point->currentText();
         CommonDialog dia(tr("修改点位"), QStringList(tr("描述").split(" ")), this);
        if(CommonDialog::Accepted == dia.exec())
        {
            QString des = dia.getDialogData()[0];
            int user = ui->comboBox_Users->currentIndex();
            GlobalFunction::showLogMsg(tr("修改点位:%1").arg(name));
            ctrller_->buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_MODIFY_PT_BY_NAME, 0, TYPE, name+";"+des+";"+QString::number(user)+";0;,");
        }
    });
    connect(ui->pushButton_delPt, &QPushButton::clicked, this, [this]()
    {
        if(QMessageBox::Ok== QMessageBox::information(this,"提示", "确认删除点？", QMessageBox::Ok|QMessageBox::Cancel))
        {
            QString name = ui->comboBox_Point->currentText();
            GlobalFunction::showLogMsg(tr("删除点位:%1").arg(name));
            ctrller_->buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_DEL_PT_BY_NAME,0, TYPE, name+";,");
        }
    });

    connect(ui->comboBox_RobotID, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
    {
        ctrller_->buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_SELECT_ROBOT_ID,0, TYPE, tr("%1;,").arg(index));
    });
    connect(ui->pushButton_teachRun, &QPushButton::clicked, [this]
    {
        if(QMessageBox::Cancel == QMessageBox::information(this, tr("提示"), tr("确定是否执行运动？"), QMessageBox::Ok|QMessageBox::Cancel))
        {
            return;
        }
        QString cmdType = ui->comboBox_teachRunType->currentText();
        QString speed = QString::number(ui->horizontalSlider_TeachVelocity->value());
        QString point1 = ui->comboBox_runPoint1->currentText();
        QString point2 = ui->comboBox_runPoint2->currentText();
        QString offset = ui->lineEdit_zOffset->text();
        QString limit = ui->lineEdit_zLim->text();
        auto checkTypeAndRun = [=](QString checkType,int cmdType1,QString cmd)
        {
            if(cmdType == checkType)
            {
                ctrller_->buildPacketSend(ROBOT_ID, FUNC_TEACH, cmdType1,0, TYPE, cmd);
                GlobalFunction::showLogMsg(tr("发送指令%1，cmd=%2").arg(checkType).arg(cmd));
            }
        };
        checkTypeAndRun("MOVJ", TEACH_ACT_MOVEJ, tr("%1;%2;,").arg(point1).arg(speed));
        checkTypeAndRun("MOVL", TEACH_ACT_MOVEL, tr("%1;%2;,").arg(point1).arg(speed));
        checkTypeAndRun("MOVC", TEACH_ACT_MOVEC, tr("%1;%2;%3;,").arg(point1).arg(point2).arg(speed));
        checkTypeAndRun("JUMP", TEACH_ACT_JUMP, tr("%1;%2;%3;%4;%5;N;,").arg(point1).arg(speed).arg(offset).arg(limit));
    });
}

ControlWidget::~ControlWidget()
{
    delete ui;
}

void ControlWidget::initDock()
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
    connect(dock_, &ads::CDockWidget::closed, []()
    {
        qDebug()<<"dock is closed";
    });
    connect(dock_, &ads::CDockWidget::visibilityChanged, [](bool status)
    {
        qDebug()<<"dock is visibility" << status;
    });
}

void ControlWidget::initUi()
{
    ui->comboBox_Coordinate->addItems(tr("世界$关节$工具$用户$姿态").split("$"));
    ui->comboBox_Users->blockSignals(true);
    ui->comboBox_Tools->blockSignals(true);
    ui->comboBox_Users->addItems(tr("null user1 user2 user3 user4 user5 user6 user7 user8 user9 user10 user11 user12 user13 user14 user15 user16").split(" "));
    ui->comboBox_Tools->addItems(tr("null tool1 tool2 tool3 tool4 tool5 tool6 tool7 tool8 tool9 tool10 tool11 tool12 tool13 tool14 tool15 tool16").split(" "));
    ui->comboBox_Users->blockSignals(false);
    ui->comboBox_Tools->blockSignals(false);
    ui->horizontalSlider_TeachVelocity->setRange(1,100);
    connect(ui->horizontalSlider_TeachVelocity, &QSlider::valueChanged, this, [this](int value){ctrller_->set_teachSpeed_(value);ui->label_TeachSpeedPresent->setText(tr("%1%").arg(value));});

    //bind step or jog distance
#define GET_STEP_DIS_EDIT(token) lineEdit_StepDis##token
#define STEP_DIS_CHANGED(AXIS_TOKEN)\
    connect(ui->GET_STEP_DIS_EDIT(AXIS_TOKEN), &QLineEdit::textChanged, this, [this](QString dis)\
    {\
        Controller::DoubleMap map = ctrller_->get_jogStepDistance_();\
        map[STRING_LATER(AXIS_TOKEN##_VALUE)] = dis.toDouble();\
        ctrller_->set_jogStepDistance_(map);\
    });
    OP_ALL_AXIS(STEP_DIS_CHANGED);

    //bind positive or negtive jog move.
#define GET_POS_TOKEN_BTN(token) pushButton_##token##Positive
#define GET_NEG_TOKEN_BTN(token) pushButton_##token##Negative
#define BOUND_TEACH_AXIS_MOVE(AXIS_TOKEN)\
    connect(ui->GET_POS_TOKEN_BTN(AXIS_TOKEN), &QPushButton::pressed, this, [this]{ctrller_->RobotJogAxis("+",STRING_LATER(AXIS_TOKEN##_VALUE), "s");});\
    connect(ui->GET_POS_TOKEN_BTN(AXIS_TOKEN), &QPushButton::released, this, [this]{ctrller_->RobotJogAxis("+",STRING_LATER(AXIS_TOKEN##_VALUE), "t");});\
    connect(ui->GET_NEG_TOKEN_BTN(AXIS_TOKEN), &QPushButton::pressed, this, [this]{ctrller_->RobotJogAxis("-",STRING_LATER(AXIS_TOKEN##_VALUE), "s");});\
    connect(ui->GET_NEG_TOKEN_BTN(AXIS_TOKEN), &QPushButton::released, this, [this]{ctrller_->RobotJogAxis("-",STRING_LATER(AXIS_TOKEN##_VALUE), "t");});
    OP_ALL_AXIS(BOUND_TEACH_AXIS_MOVE);

    // bind all step or jog distance
#define RESET_ALL_STEP_DIS(DISTANCE)\
    ui->lineEdit_StepDisJ1->setText(DISTANCE);\
    ui->lineEdit_StepDisJ2->setText(DISTANCE);\
    ui->lineEdit_StepDisJ3->setText(DISTANCE);\
    ui->lineEdit_StepDisJ4->setText(DISTANCE);\
    ui->lineEdit_StepDisJ5->setText(DISTANCE);\
    ui->lineEdit_StepDisJ6->setText(DISTANCE);
    connect(ui->radioButton_JogMode, &QRadioButton::clicked, this, [this](){RESET_ALL_STEP_DIS("0.000000");ctrller_->set_jogMode_(Controller::Jog);});
    connect(ui->radioButton_shortStrpMode, &QRadioButton::clicked, this, [this](){RESET_ALL_STEP_DIS("0.100000");ctrller_->set_jogMode_(Controller::Step);});
    connect(ui->radioButton_MidStepMode, &QRadioButton::clicked, this, [this](){RESET_ALL_STEP_DIS("1.000000");ctrller_->set_jogMode_(Controller::Step);});
    connect(ui->radioButton_LongStepMode, &QRadioButton::clicked, this, [this](){RESET_ALL_STEP_DIS("10.000000");ctrller_->set_jogMode_(Controller::Step);});
    // change to teach auto remote mode.
    connect(ui->pushButton_TeachMode, &QPushButton::clicked, this, [this](){ctrller_->changeSysModeState(Controller::mode_teach);});
    connect(ui->pushButton_AutoMode, &QPushButton::clicked, this, [this](){ctrller_->changeSysModeState(Controller::mode_auto);});
    connect(ui->pushButton_RemoteMode, &QPushButton::clicked, this, [this](){ctrller_->changeSysModeState(Controller::mode_remote);});
}

void ControlWidget::initNetRecvAndDataChange()
{
    //update point
    connect(ProgramsCtrl::getInstance(), &ProgramsCtrl::updateActivePoint, this, [&](QList<PointValue> ptList)
    {
        auto setPointComb = [ptList](QComboBox* box)
        {
            QString name = box->currentText();
            box->clear();
            for(int i = 0; i < ptList.size(); i++)
            {
                box->addItem(ptList.at(i).name);
            }
            if(box->findText(name))
            {
                box->setCurrentText(name);
            }
        };
        setPointComb(ui->comboBox_Point) ;
        setPointComb(ui->comboBox_runPoint1) ;
        setPointComb(ui->comboBox_runPoint2) ;
    });
    connect(ctrller_, &Controller::updateRobotState, this, [this]
    {
# define UPDATE_COLLTROLLER_STATE(STATE, MATCH_STATE, DES_LABEL, SHOW_STRING, STRING_COLOR) {\
    if(STATE == MATCH_STATE)\
        {\
            DES_LABEL->setText(SHOW_STRING); \
            DES_LABEL->setStyleSheet(tr("color:%1").arg(STRING_COLOR));\
        }\
    }
        UPDATE_COLLTROLLER_STATE(Controller::mode_teach, ctrller_->get_modeState_(), ui->label_mode, "示教", "green");
        UPDATE_COLLTROLLER_STATE(Controller::mode_auto, ctrller_->get_modeState_(), ui->label_mode, "自动", "green");
        UPDATE_COLLTROLLER_STATE(Controller::mode_remote, ctrller_->get_modeState_(), ui->label_mode, "远程", "green");
        UPDATE_COLLTROLLER_STATE(Controller::mode_unkown, ctrller_->get_modeState_(),  ui->label_mode, "unkown", "gray");
        UPDATE_COLLTROLLER_STATE(Controller::sys_STANDSTILL, ctrller_->get_sysState_(), ui->label_sys, "就绪", "green");
        UPDATE_COLLTROLLER_STATE(Controller::sys_STOP, ctrller_->get_sysState_(), ui->label_sys, "停止", "red");
        auto getError = [this]()
        {
            QString error = "未知错误";
            if(ctrller_->get_servoError_() != "0"){error = ctrller_->get_servoError_();}
            else if(ctrller_->get_ethercatError_() != "0"){error = ctrller_->get_ethercatError_();}
            else if(ctrller_->get_motionError_() != "0"){error = ctrller_->get_motionError_();}
            else if(ctrller_->get_interpreterError_() != "0"){error = ctrller_->get_interpreterError_();}
            QString rightError = QString("0x%1").arg(error.toInt(), 4, 16, QLatin1Char('0'));
            return rightError;
        };
        UPDATE_COLLTROLLER_STATE(Controller::sys_ERROR, ctrller_->get_sysState_(), ui->label_sys, getError(), "red");
        UPDATE_COLLTROLLER_STATE(Controller::sys_NORMAL_RUN, ctrller_->get_sysState_(), ui->label_sys, "自动运行", "green");
        UPDATE_COLLTROLLER_STATE(Controller::sys_DEBUG_RUN, ctrller_->get_sysState_(), ui->label_sys, "调试运行", "green");
        UPDATE_COLLTROLLER_STATE(Controller::sys_DEBUG_PAUSE, ctrller_->get_sysState_(), ui->label_sys, "调试暂停", "green");
        UPDATE_COLLTROLLER_STATE(Controller::sys_NORMAL_PAUSE, ctrller_->get_sysState_(), ui->label_sys, "自动暂停", "green");
        UPDATE_COLLTROLLER_STATE(Controller::sys_unkown, ctrller_->get_sysState_(), ui->label_sys, "unkown", "gray");
        UPDATE_COLLTROLLER_STATE(Controller::servo_on, ctrller_->get_servoState_(), ui->label_servo, "上电", "green");
        UPDATE_COLLTROLLER_STATE(Controller::servo_off, ctrller_->get_servoState_(), ui->label_servo, "下电", "red");
        UPDATE_COLLTROLLER_STATE(Controller::servo_unkown, ctrller_->get_servoState_(), ui->label_servo, "unkown", "gray");
    });
    //update position
    connect(ctrller_, &Controller::change_currentCarPoint_, this, [this](PointValue ptValue)
    {
        if(ui->CarPosition->isChecked())
        {
            ui->J1CurrentPosition->setText(ptValue.X);
            ui->J2CurrentPosition->setText(ptValue.Y);
            ui->J3CurrentPosition->setText(ptValue.Z);
            ui->J4CurrentPosition->setText(ptValue.U);
            ui->J5CurrentPosition->setText(ptValue.V);
            ui->J6CurrentPosition->setText(ptValue.W);
            ui->HandFlag->setText(ptValue.Hand == 0 ? "Right":"Left");
        }
    });
    connect(ctrller_, &Controller::change_currentJointPoint_, this, [this](PointValue ptValue)
    {
        if(ui->JointPosition->isChecked())
        {
            ui->J1CurrentPosition->setText(ptValue.X);
            ui->J2CurrentPosition->setText(ptValue.Y);
            ui->J3CurrentPosition->setText(ptValue.Z);
            ui->J4CurrentPosition->setText(ptValue.U);
            ui->J5CurrentPosition->setText(ptValue.V);
            ui->J6CurrentPosition->setText(ptValue.W);
        }
    });
}


void ControlWidget::on_comboBox_Users_currentIndexChanged(const QString &userName)
{
    QComboBox* userCombo = qobject_cast<QComboBox*>(sender());
    QString num = QString::number((userCombo->findText(userName)));
    QString sendData = "u;";
    if(userName == tr("无"))
    {
        sendData += "null;0;,";
    }
    else
    {
        sendData += tr("%1;%2;,").arg(userName).arg(num);
    }
    ctrller_->buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_SET_COOR, 0, TYPE, sendData);
}

void ControlWidget::on_comboBox_Tools_currentIndexChanged(const QString &toolName)
{
    QComboBox* userCombo = qobject_cast<QComboBox*>(sender());
     QString num = QString::number((userCombo->findText(toolName)));
    QString sendData = "t;";
    if(toolName == tr("无"))
    {
        sendData += "null;0;,";
    }
    else
    {
        sendData += tr("%1;%2;,").arg(toolName).arg(num);
    }
    ctrller_->buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_SET_COOR, 0, TYPE, sendData);
}

void ControlWidget::on_comboBox_Coordinate_currentIndexChanged(const QString &coord)
{
    QMap<QString, QString> tempMap;
    tempMap[tr("世界")] = "w";
    tempMap[tr("关节")] = "j";
    tempMap[tr("工具")] = "t";
    tempMap[tr("用户")] = "u";
    ctrller_->set_coordMode_(tempMap[coord]);
}
