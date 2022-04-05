#include "controller.h"
#include "qdebug.h"
#include "globalfunction.h"
#include "programsctrl.h"
#include "CRC32.h"
#include "programsctrl.h"
#include "config.h"

#define RECVDATA_SPLIT0 ","
#define RECVDATA_SPLIT1 ";"
#define RECVDATA_SPLIT2 ":"

Controller *Controller::instance_ = nullptr;
Controller *Controller::getInstance()
{
    if(!instance_)
    {
        instance_ = new Controller;
    }
    return instance_;
}

Controller::Controller(QObject *parent) : QObject(parent),
    networkState_(net_unConnected),
    sysState_(sys_STOP),
    servoState_(servo_off),
    modeState_(mode_auto),
    coordMode_("j"),
    jogMode_(Jog),
    teachSpeed_(30)
{
    tcpModule_ = TcpLink::getInstance();
    connect(tcpModule_, &TcpLink::updateCurrentNetworkState, this, &Controller::updateNetworkState);
    connect(tcpModule_, &TcpLink::tcpConnected, this, [this]
    {

      ProgramsCtrl::getInstance()->updateControllerFileList();
        qDebug()<< "updateTcpConnected()";
    });
    connect(tcpModule_, &TcpLink::tcpDisconnected, this, []
    {
        qDebug()<<"disConnect from controller";
    });
    connect(tcpModule_, &TcpLink::recvTcpData, this, &Controller::handleRecvData);
}

void Controller::setIPAndPort(QString ip, QString port)
{
    tcpModule_->setIP(ip);
    tcpModule_->setPort(port);

}

void Controller::buildPacketSend(uchar id, uchar cmd, uint index, int datalen, int type, QString str)
{
    if(networkState_ != net_connected)
    {
        qDebug()<<"current tcpSocket is not connect";
        GlobalFunction::showLogMsg(tr("网络未连接，请连接控制器后重试！"));
        return;
    }
    TCP_FREAMHEAD frame_header;
    frame_header.robotid    = id;
    frame_header.cmd        = cmd;
    frame_header.index      = index;
    frame_header.datalen    = datalen;
    frame_header.type       = type;
    tcpModule_->sendDataByNetWork(frame_header, QByteArray().append(str));
}

void Controller::buildPacketSend(uchar cmd, uint index, QString str)
{
   this->buildPacketSend(ROBOT_ID, cmd, index, 0, TYPE, str);
}


void Controller::handleUpdateRobotState(QString recvData)
{
    auto getValueFromRecvData = [&recvData](QString splitChar, int index)
    {
        QStringList recvDataList = recvData.split(splitChar);
        if(recvDataList.size() < index-1)
        {
            return QString();
        }
        return recvDataList.at(index);
    };

    set_sysState_((SysState)getValueFromRecvData(RECVDATA_SPLIT1, 0).toInt());
    set_modeState_((ModeState)getValueFromRecvData(RECVDATA_SPLIT1, 1).toInt());
    set_servoState_((ServoState)getValueFromRecvData(RECVDATA_SPLIT1, 6).toInt());
    set_ethercatError_(getValueFromRecvData(RECVDATA_SPLIT1, 4));
    set_servoError_(getValueFromRecvData(RECVDATA_SPLIT1, 5));
    set_motionError_(getValueFromRecvData(RECVDATA_SPLIT1, 7));
    set_interpreterError_(getValueFromRecvData(RECVDATA_SPLIT1, 8));
}

void Controller::handleUpdateRobotPositon(QString recvData)
{
    auto getValueFromRecvDataDepth2 = [recvData](QString splitChar1, int index1, QString splitChar2, int index2)
    {
        QStringList recvDataList = recvData.split(splitChar1);
        if(recvDataList.size() < index1 - 1)
        {
            return QString();
        }
        QString rightData = recvDataList.at(index1);
        QStringList recvDataList2 = rightData.split(splitChar2);
        if(recvDataList2.size() < index2 - 1)
        {
            return QString();
        }
        return recvDataList2.at(index2);
    };
    PointValue tempCarPt;
#define SET_CAR_PT_VALUE(PT_OBJ, AXIS, INDEX2) PT_OBJ.AXIS = getValueFromRecvDataDepth2(RECVDATA_SPLIT1,3, RECVDATA_SPLIT2, INDEX2);
    SET_CAR_PT_VALUE(tempCarPt, X, 0);
    SET_CAR_PT_VALUE(tempCarPt, Y, 1);
    SET_CAR_PT_VALUE(tempCarPt, Z, 2);
    SET_CAR_PT_VALUE(tempCarPt, U, 3);
    SET_CAR_PT_VALUE(tempCarPt, V, 4);
    SET_CAR_PT_VALUE(tempCarPt, W, 5);
    SET_CAR_PT_VALUE(tempCarPt, R, 6);
    SET_CAR_PT_VALUE(tempCarPt, S, 7);
    SET_CAR_PT_VALUE(tempCarPt, T, 8);
    SET_CAR_PT_VALUE(tempCarPt, Local, 9);
    SET_CAR_PT_VALUE(tempCarPt, Hand, 10);
    SET_CAR_PT_VALUE(tempCarPt, Elbow, 11);
    SET_CAR_PT_VALUE(tempCarPt, Wrist, 12);
    SET_CAR_PT_VALUE(tempCarPt, J1Flag, 13);
    SET_CAR_PT_VALUE(tempCarPt, J2Flag, 14);
    SET_CAR_PT_VALUE(tempCarPt, J4Flag, 15);
    SET_CAR_PT_VALUE(tempCarPt, J6Flag, 16);
    SET_CAR_PT_VALUE(tempCarPt, J1Angle, 17);
    SET_CAR_PT_VALUE(tempCarPt, J4Angle, 18);
    set_currentCarPoint_(tempCarPt);

#define SET_JOINT_PT_VALUE(PT_OBJ, AXIS, INDEX2) PT_OBJ.AXIS = getValueFromRecvDataDepth2(RECVDATA_SPLIT1,4, RECVDATA_SPLIT2, INDEX2);
    SET_JOINT_PT_VALUE(tempCarPt, X, 0);
    SET_JOINT_PT_VALUE(tempCarPt, Y, 1);
    SET_JOINT_PT_VALUE(tempCarPt, Z, 2);
    SET_JOINT_PT_VALUE(tempCarPt, U, 3);
    SET_JOINT_PT_VALUE(tempCarPt, V, 4);
    SET_JOINT_PT_VALUE(tempCarPt, W, 5);
    SET_JOINT_PT_VALUE(tempCarPt, R, 6);
    SET_JOINT_PT_VALUE(tempCarPt, S, 7);
    SET_JOINT_PT_VALUE(tempCarPt, T, 8);
    set_currentJointPoint_(tempCarPt);
}

void Controller::handleChangeRobotState(QString recvData)
{
    Q_UNUSED(recvData);
}

#define JSON_GET_1PARAM_INT(data) QString::fromStdString(root[#data].asString()).toInt()
#define JSON_GET_2PARAM_INT(data1, data2) QString::fromStdString(root[#data1][#data2].asString()).toInt()

void Controller::handleUpdateIOState(QString recvData)
{
//    qDebug()<<recvData;
    if(recvData.isEmpty()){ERROR_OCCUR};
    Json::Value root = GlobalFunction::ReadJsonFromJsonStr(recvData);
    if(!root.isObject()){ERROR_OCCUR};
    int inputSum = QString::fromStdString(root["input"]["ioSum"].asString()).toInt();
    int outputSum = QString::fromStdString(root["output"]["ioSum"].asString()).toInt();
    int inputModuleSum = QString::fromStdString(root["input"]["moduleSum"].asString()).toInt();
    int outputModuleSum = QString::fromStdString(root["output"]["moduleSum"].asString()).toInt();
    int dataFormat = QString::fromStdString(root["dataFormat"].asString()).toInt();
    QList<unsigned int> inputData, outputData;
    for(int i = 0; i <(int) root["input"]["data"].size(); i++)
    {
        inputData.append(QString::fromStdString(root["input"]["data"][i].asString()).toUInt());
    }
    for(int i = 0; i <(int) root["output"]["data"].size(); i++)
    {
        outputData.append(QString::fromStdString(root["output"]["data"][i].asString()).toUInt());
    }
    if(inputModuleSum == inputData.size() && outputModuleSum == outputData.size())
    {
        QList<bool> inputList, outputList;
        foreach (unsigned int tempData, inputData)
        {
            for(int i = 0; i < dataFormat; i++)
            {
                inputList.append((bool)((tempData>>i)&1));
                if(inputList.size() >= inputSum){break;}
            }
        }
        foreach (unsigned int tempData, outputData)
        {
            for(int i = 0; i < dataFormat; i++)
            {
                outputList.append((bool)((tempData>>i)&1));
                if(outputList.size() >= outputSum){break;}
            }
        }
        emit recvIOState(inputList, outputList);
    }
}

void Controller::handleUpdateAxisState(QString recvData)
{
    Json::Value root = GlobalFunction::ReadJsonFromJsonStr(recvData);
    if(!root.isObject()){ERROR_OCCUR};
    int axisSum = JSON_GET_1PARAM_INT(axisSum);
    if((int)root["axises"].size()!=axisSum){ERROR_OCCUR};
    QStringList axisData;
    for(int i = 0; i < axisSum; i++)
    {
        QString tempData = tr("%1;%2;%3;%4;%5").arg(QString::fromStdString(root["axises"][i]["state"].asString()))
                                               .arg(QString::fromStdString(root["axises"][i]["pos"].asString()))
                                               .arg(QString::fromStdString(root["axises"][i]["vel"].asString()))
                                               .arg(QString::fromStdString(root["axises"][i]["acc"].asString()))
                                               .arg(QString::fromStdString(root["axises"][i]["err"].asString()));
        axisData.append(tempData);
    }
    emit recvAxisState(axisData);
}

void Controller::servoOn()
{
    buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_SERVO_ON, 0, TYPE, "63,");
}

void Controller::servoOff()
{
    buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_SERVO_ON, 0, TYPE, "0,");
}

void Controller::RobotJogAxis(QString dir, QString aixsName, QString startOrstop)
{
    if(modeState_ != Controller::mode_teach)return;
    QString cmdBuf;
    cmdBuf = "PtRA";    //点位名
    cmdBuf +=";";
    cmdBuf += QString::number(teachSpeed_);     //速度百分比
    cmdBuf +=";";
    cmdBuf += QString::number(jogStepDistance_[aixsName]);    //寸动距离
    cmdBuf +=";";
    cmdBuf += dir;  //方向:+/-
    cmdBuf += aixsName;      //轴名:x/y/z/u/v/w
    cmdBuf += startOrstop; //起停:s/t
    cmdBuf += coordMode_;      //坐标系:j/w/t/u(关节/世界/工具/用户 )  c(校准)
    cmdBuf += ",";
    if( jogMode_ == Jog)
    {
        buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_JOG_MOV, 0, 1, cmdBuf);
    }
    else if(jogMode_ == Step)
    {
        buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_STEP_MOV, 0, 1, cmdBuf);
    }
}

void Controller::changeSysModeState(ModeState mode)
{
    if(mode == get_modeState_())
    {
        return;
    }
    switch(mode)
    {

    case mode_teach:
    {
        if(!ProgramsCtrl::getInstance()->getActiveProgram())
        {
            GlobalFunction::showLogMsg(tr("请选择活动项目后重试！"));
            return;
        }
        if(get_sysState_() != sys_STANDSTILL)
        {
            GlobalFunction::showLogMsg(tr("当前不在standstill，发送停止指令..."));
            buildPacketSend(ROBOT_ID, FUNC_INTER, INTER_STOP, 0, TYPE, "");
            GlobalFunction::Delayms(200);
        }
        QString cmdBuf;
        cmdBuf = Config::getInstance()->get_remoteMainPath_() + POINTFILE;//ptFilePathName
        cmdBuf += ";";
        cmdBuf += "ROBOT1";//groupName
        cmdBuf += ";";
        cmdBuf += GlobalFunction::CalFileCrcAuto(ProgramsCtrl::getInstance()->getActiveProgram()->get_path_() + POINTFILE);
        cmdBuf += ";";
        cmdBuf += GlobalFunction::CalFileCrcAuto(ProgramsCtrl::getInstance()->getActiveProgram()->get_path_() + TOOLFILE);
        cmdBuf += ";";
        cmdBuf += GlobalFunction::CalFileCrcAuto(ProgramsCtrl::getInstance()->getActiveProgram()->get_path_() + USERFILE);
        cmdBuf += ",";
                //计算点位文件CRC
        GlobalFunction::showLogMsg(tr("切换示教模式，cmd=%1 ...").arg(cmdBuf));
        buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_BEGIN, 0, TYPE, cmdBuf);
        GlobalFunction::Delayms(400);
        if(get_servoState_()== servo_on)
        {
            buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_SERVO_ON, 0, TYPE, "0,");
        }
        break;
    }
    case mode_auto:
    {
        GlobalFunction::showLogMsg(tr("切换自动模式..."));
        buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_END, 0, TYPE, "endPara,");
        break;
    }
    case mode_remote:
    {
        GlobalFunction::showLogMsg(tr("切换远程模式..."));
        buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_REMOTE_BEGIN, 0, TYPE, ",");
        break;
    }
    default:
        break;
    }
}

void Controller::programStart()
{
    if(get_modeState_() != mode_auto){qDebug()<<"mode error change to auto mode!";return;};
    buildPacketSend(ROBOT_ID, FUNC_INTER, INTER_START, 0, TYPE, "");
}

void Controller::programDebugStart()
{
    if(get_modeState_() != mode_auto){qDebug()<<"mode error change to auto mode!";return;};
    buildPacketSend(ROBOT_ID, FUNC_INTER, INTER_DBG_START, 0, TYPE, "");
}

void Controller::programStop()
{
    if(get_modeState_() != mode_auto){qDebug()<<"mode error change to auto mode!";return;};
    buildPacketSend(ROBOT_ID, FUNC_INTER, INTER_STOP, 0, TYPE, "");
}

void Controller::programPause()
{
    buildPacketSend(ROBOT_ID, FUNC_INTER, INTER_PAUSE, 0, TYPE, "");
}

void Controller::getCurrentEncorder()
{
    buildPacketSend(ROBOT_ID, FUNC_TEACH, TEACH_GET_ENCODER, 0, TYPE, "63,");
}

void Controller::setTcpLink(bool linkingController)
{
    if(linkingController)
    {
        isConnecting_ = true;
        tcpModule_->connectServer();
    }
    else
    {
        isConnecting_ = false;
        tcpModule_->disconnectServer();
    }
}

void Controller::updateNetworkState(QAbstractSocket::SocketState state)
{
    if(isConnecting_)
    {
        if(state == QAbstractSocket::ConnectedState)
        {
            set_networkState_(net_connected);
        }
        else
        {
            set_networkState_(net_connectingError);
        }
    }
    else
    {
        set_networkState_(net_unConnected);
    }
    emit updateMainWindState();
}

void Controller::handleRecvData(TCP_FREAMHEAD frameheader, QString str_packet)
{
#if DEBUG_MODE
    qDebug()<<"recv header:"<<frameheader.cmd <<"data:"<< str_packet;
#endif
    if(FUNC_SYS == frameheader.cmd)
    {//系统控制相关命令
        if(frameheader.index !=  SYS_GET_FILE_INFO &&
                frameheader.index != SYS_UPDATE_STATUS &&
                frameheader.index != SYS_UPDATE_POSITION&&
                frameheader.index != SYS_HEARTBEATS&&
                str_packet.split(";").size()>0)
        {
            QString errorCode = str_packet.split(";").at(0);
            if(errorCode != "0" && frameheader.index != SYS_GET_TCP_COM_DATA_REALTIME )
            {
                emit recvErrorCode(errorCode);
            }
        }
        switch(frameheader.index)
        {

        case SYS_GET_FILE_INFO:     //获取文件列表
        {
            emit receiveFileListInfo(str_packet);
            break;
        }

        case SYS_DOWNLOAD_SCRIPT:   //从示教器或上位机下载文件到控制器
        {
            break;
        }

        case SYS_UPLOAD_SCRIPT:     //接收控制器上传的文件,RC-to-TP
        {
            break;
        }
        case SYS_UPDATE_APP:
        case SYS_UPDATE_ETHERCAT_XML:
        {
            qDebug()<<str_packet;
        }
        case SYS_DELETE_FILE:       //删除文件
        {
            emit receiveDeleteFile(str_packet);
            break;
        }

        case SYS_CHANGE_MODE:       //连接成功后，控制器主动发送给示教器
        {
            handleChangeRobotState(str_packet);
            emit changeRobotState();
            break;
        }

        case SYS_UPDATE_STATUS:     //更新系统状态
        {
            handleUpdateRobotState(str_packet);
            emit updateRobotState();
            break;
        }
        case SYS_UPDATE_POSITION:   //更新位置信息状态
        {
            handleUpdateRobotPositon(str_packet);
            emit updateRobotPosition();
            break;
        }
        case SYS_GET_FILE_CRC:
        {
            emit getRCFileCRC(str_packet);
            break;
        }
        case SYS_CLEAR_FAULT:       //清除motion错误
        {
            emit receiveClearError(str_packet);
            break;
        }
        case SYS_READ_HWVERSION:
        {
            emit receiveGetControllerVersion(str_packet);
            break;
        }
        case SYS_SPEED_PERSENT:
        {
            break;
        }
        case SYS_GET_MAC:
        {
            emit receiveControllerMac(str_packet);
            break;
        }
        case SYS_GET_TCP_COM_DATA:
        case SYS_STOP_GET_TCP_COM_DATA:
        {
            break;
        }
        case SYS_GET_TCP_COM_DATA_REALTIME:
        {
            emit receiveNetMonitorData(str_packet);
            break;
        }
        case SYS_GET_REGISTER:
        {
            emit receiveMonitorRegData(str_packet);
            break;
        }

        case SYS_GET_AGENT_SERVER:
        {
            emit receiveGetControllerIPAddr(str_packet);
            break;
        }
        case SYS_GET_LICENSE:
        {
            emit receiveGetLicense(str_packet);
            break;
        }
        case SYS_WRITE_PARAMETER:
        {
            emit receiveWriteParameter(str_packet);
            break;
        }
        case SYS_GET_IO_STATE:
        {
            handleUpdateIOState(str_packet);
            break;
        }
        case SYS_GET_AXIS_STATE:
        {
            handleUpdateAxisState(str_packet);
            break;
        }
        case SYS_HEARTBEATS:
        {
#if DEBUG_MODE
            qDebug()<<"recv heartbeat;";
#endif
            break;
        }
        default:
        {
            qDebug()<<"unknown header:"<<frameheader.index;
            break;
        }
        }
    }
    else if(FUNC_TEACH == frameheader.cmd)
    {//示教器相关命令
        if(str_packet.split(";").size()>0)
        {
            QString errorCode = str_packet.split(";").at(0);
            if(errorCode != "0")
            {
                //                        emit recvErrorCode(errorCode);
            }
        }
        switch(frameheader.index)
        {
        case TEACH_BEGIN:
        {
            emit beginTeachMode(str_packet);
            qDebug()<<str_packet;
            break;
        }

        case TEACH_END:
        {
            emit endTeachMode(str_packet);
            break;
        }

        case TEACH_SET_OUT:         //IO: LED

            break;


        case TEACH_UPDATE_MORNITOR_DATA:

            break;
        case TEACH_SERVO_ON:
        {
            emit receiveServoOn(str_packet);
            break;
        }
        case TEACH_STEP_MOV:
        {
            emit receiveTeachStep(str_packet);
            break;
        }
        case TEACH_JOG_MOV:
        {
            emit receiveTeachJog(str_packet);
            break;
        }
        case TEACH_MODIFY_PT:
        {
            emit changePointByID(str_packet);
            break;
        }
        case TEACH_MODIFY_PT_BY_NAME:
        case TEACH_ADD_CUR_PT_2_LIST:
        case TEACH_DEL_PT_BY_NAME:
        {
            ProgramsCtrl::getInstance()->upLoadFromControllerActivePoints();
            break;
        }
        case TEACH_ADD_NEW_USER_2_LIST:
        {
            emit receiveAddUser(str_packet);
            break;
        }
        case TEACH_DEL_USER_BY_NAME:
        {
            emit receiveDelUser(str_packet);
            break;
        }
        case TEACH_MODIFY_USER_BY_NAME:
        {
            emit receiveModifyUser(str_packet);
            break;
        }
        case TEACH_ADD_NEW_TOOL_2_LIST:
        {
            emit receiveAddTool(str_packet);
            break;
        }
        case TEACH_DEL_TOOL_BY_NAME:
        {
            emit receiveDelTool(str_packet);
            break;
        }
        case TEACH_MODIFY_TOOL_BY_NAME:
        {
            emit receiveModifyTool(str_packet);
            break;
        }
        case TEACH_SAVE_CALI_DATA:
        {
            emit receiveSaveCali(str_packet);
            break;
        }
        case TEACH_CALCULATE_CALI_DATA:
        case TEACH_CALCULATE_CALI_DATA_NEW:
        {
            emit receiveCalculateCail(str_packet);
            break;
        }
        case TEACH_CALCULATE_3_PT_CALI_DATA:
        {
            emit receiveCalculate3ptFinish(str_packet);
            break;
        }
        case TEACH_SET_COOR:
        {
            emit receiveSetCoor(str_packet);
            break;
        }
        case TEACH_GET_ENCODER:
        {
            qDebug()<<str_packet;
            QStringList encorderList =  str_packet.split(RECVDATA_SPLIT0).at(0).split(RECVDATA_SPLIT1);
            if(encorderList.size() > 2)
            {
                encorderList.pop_front();    //error
                encorderList.pop_front();    //sum
                emit receiveGetEncorder(encorderList);
            }
            break;
        }
        case TEACH_GET_ADDI_ENCODER:
        {
            emit receiveGetOuterEncorder(str_packet);
            break;
        }
        case TEACH_EXAXIS_GO_HOME:
        {
            emit receiveGetExisHome(str_packet);
            break;
        }
        case TEACH_SET_CONVEYOR_ENC_FACTOR:
        {
            emit receiveConveyorFactor(str_packet);
            break;
        }
        case TEACH_GET_CONVEYOR_ENC_FACTOR:
        {
            emit receiveConveyorFactor(str_packet);
            break;
        }
        case TEACH_SET_CONVEYOR_PARM:
        {
            emit receiveConveySave(str_packet);
            break;
        }
        case TEACH_ACT_MOVEL:
        case TEACH_ACT_MOVEJ:
        case TEACH_ACT_MOVEC:
        case TEACH_ACT_JUMP:
        {
            emit receiveGetTeachRun(str_packet);
            break;
        }
        case TEACH_SAVE_HOME_CALI_DATA:
        {
            break;
        }
        case TEACH_CALCULATE_HOME_CALI_DATA:
        {
            emit receiveCalHomeEncorder(str_packet);
            break;
        }
        case TEACH_GO_REF_PULSE :
        {
            emit receiveGoRefPulse(str_packet);
            break;
        }
        case TEACH_TM_TURN_RESET:
        {
            qDebug()<<str_packet;
            break;
        }
        case TEACH_GET_CAMERA_CONNECT_STATE:
        {
            emit receiveCameraState(str_packet);
            break;
        }
        case TEACH_GET_CAMERA_PIX_POSITION:
        {
            emit receiveCameraPix(str_packet);
            break;
        }
        case TEACH_CALI_VISION_PARAMETER:
        {
            emit receiveCameraCaliPara(str_packet);
            break;
        }
        case TEACH_SET_CAMERA_PIX_POSITION:
        case  TEACH_CONNECT_CAMERA_SERVER:
        case TEACH_DISCONNECT_CAMERA_SERVER:
        case TEACH_GET_CAMERA_TOOL_CURRENT_POSITION:
        case TEACH_CAL_CAMERA_TOOL:
        case TEACH_SAVE_CAMERA_POSITION:
        {
            qDebug()<< str_packet;
            break;
        }
        default:
        {
            qDebug()<<"unknown header:"<<frameheader.index;
            break;
        }

        }
    } else if(FUNC_INTER == frameheader.cmd)
    {//解释器相关命令/
        switch(frameheader.index)
        {
        case INTER_START:
        {
            emit receiveRunProgram(str_packet);
            break;
        }
        case INTER_DBG_START:   //启动机械手解释器-调试运行
        {
            emit receiveDebugRunProgram(str_packet);
            break;
        }

        case INTER_STOP:        //停止机械手解释器-全速运行
        {
            emit receiveStopProgram(str_packet);
            break;
        }

        case INTER_DBG_PT_LINE: //运行时的当前行号
        {
            emit receiveRunningCurrentLine(str_packet);
            break;
        }
        case DEG_DB_PT_MOVE_LINE://运动行
        {
            emit receiveRunningMovmentLine(str_packet);
            break;
        }

        case INTER_DBG_STEP_INTO://单步运行(调试运行)
            break;
        case INTER_DBG_INS_PT:  //PRINT打印指令
        {
            emit receivePrintLog(str_packet);
            break;
        }
        case INTER_DBG_ROBOT_ERR://ERROR指令使用
        {
            emit receiveRunningError(str_packet);
            break;
        }
        case INTER_PAUSE:   //暂停
        {
            emit receivePauseRunning(str_packet);
            break;
        }
        case INTER_RESUME:  //恢复
        {
            emit receiveResumeRunning(str_packet);
            break;
        }
        default:
        {
            qDebug()<<"unknown header:"<<frameheader.index;
            break;
        }
        }
    }
}
