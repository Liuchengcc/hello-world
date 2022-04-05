#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <tcplink.h>
#include <marcroly.h>
#include <program.h>

#define OP_CONTROLLER_ALL_OBJ(OP_MARCRO)\
    OP_MARCRO(NetWorkState, networkState_);\
    OP_MARCRO(SysState, sysState_);\
    OP_MARCRO(ServoState, servoState_);\
    OP_MARCRO(ModeState, modeState_);\
    OP_MARCRO(QString, coordMode_);\
    OP_MARCRO(JogMode, jogMode_);\
    OP_MARCRO(DoubleMap, jogStepDistance_);\
    OP_MARCRO(PointValue, currentCarPoint_);\
    OP_MARCRO(PointValue, currentJointPoint_);\
    OP_MARCRO(QString, ethercatError_);\
    OP_MARCRO(QString, servoError_);\
    OP_MARCRO(QString, motionError_);\
    OP_MARCRO(QString, interpreterError_);\
    OP_MARCRO(int, teachSpeed_);

class Controller : public QObject
{
    Q_OBJECT
public:
    typedef QMap<QString, double> DoubleMap;
    enum NetWorkState
    {
        net_unConnected,
        net_connected,
        net_connectingError
    };
    enum JogMode
    {
        Jog,
        Step
    };
    enum SysState
        {
            sys_TEACH 							=0x00,//示教模式
            sys_STOP                                =0x01,//默认状态
            sys_DEBUG_RUN 						=0x02,//调试运行模式
            sys_DEBUG_PAUSE						=0x03,
            sys_NORMAL_RUN						=0x04,//全速运行模式
            sys_NORMAL_PAUSE					=0x05,
            sys_ERROR							=0x06,//报错
            sys_STANDSTILL                  = 0x07
        };
    enum ModeState
    {
        mode_teach,
        mode_auto,
        mode_remote
    };
    enum ServoState
    {
        servo_off,
        servo_on
    };

    static Controller* getInstance();
    explicit Controller(QObject *parent = nullptr);

    void setIPAndPort(QString ip, QString port);
    void buildPacketSend(uchar id, uchar cmd, uint index, int datalen, int type, QString str);
    void buildPacketSend(uchar cmd, uint index, QString str);

    void handleUpdateRobotState(QString);
    void handleUpdateRobotPositon(QString);
    void handleChangeRobotState(QString);
    void handleUpdateIOState(QString);
    void handleUpdateAxisState(QString);
    void servoOn();
    void servoOff();
    void RobotJogAxis(QString dir, QString aixsName, QString startOrstop);
    void changeSysModeState(ModeState);
    void programStart();
    void programDebugStart();
    void programStop();
    void programPause();
    void getCurrentEncorder();

signals:
    void updateMainWindState();

    //recvHandle
    void recvIOState(QList<bool>& , QList<bool> &);
    void recvAxisState(QStringList&);
    void changeRobotState();
    void updateRobotState();
    void updateRobotPosition();
    void recvErrorCode(QString);
    void receiveFileListInfo(QString);
    void receiveDeleteFile(QString);
    void  getRCFileCRC(QString);
    void receiveClearError(QString);
    void receiveGetControllerVersion(QString);
    void receiveNetMonitorData(QString);
    void receiveMonitorRegData(QString);
    void receiveControllerMac(QString);
    void receiveGetControllerIPAddr(QString);
    void receiveGetLicense(QString);
    void receiveWriteParameter(QString);
    void beginTeachMode(QString);
    void endTeachMode(QString);
    void receiveServoOn(QString);
    void receiveTeachStep(QString);
    void receiveTeachJog(QString);
    void changePointByID(QString);
    void receiveAddUser(QString);
    void receiveDelUser(QString);
    void receiveModifyUser(QString);
    void receiveAddTool(QString);
    void receiveDelTool(QString);
    void receiveModifyTool(QString);
    void receiveSaveCali(QString);
    void receiveCalculateCail(QString);
    void receiveCalculate3ptFinish(QString);
    void receiveSetCoor(QString);
    void receiveGetEncorder(QStringList);
    void receiveGetOuterEncorder(QString);
    void receiveGetExisHome(QString);
    void receiveConveyorFactor(QString);
    void receiveConveySave(QString);
    void receiveGetTeachRun(QString);
    void receiveCalHomeEncorder(QString);
    void receiveGoRefPulse(QString);
    void receiveCameraState(QString);
    void receiveCameraPix(QString);
    void receiveCameraCaliPara(QString);
    void receiveRunProgram(QString);
    void receiveDebugRunProgram(QString);
    void receiveStopProgram(QString);
    void receiveRunningCurrentLine(QString);
    void receiveRunningMovmentLine(QString);
    void receivePauseRunning(QString);
    void receiveResumeRunning(QString);
    void receivePrintLog(QString);
    void receiveRunningError(QString);
public slots:
    void setTcpLink(bool);
    void updateNetworkState(QAbstractSocket::SocketState);
    void handleRecvData(TCP_FREAMHEAD, QString);
signals:
OP_CONTROLLER_ALL_OBJ(DECL_SIGNAL_METADATA);
OP_CONTROLLER_ALL_OBJ(REGISTER_METADATA);
private:
    static Controller *instance_;
    TcpLink *tcpModule_;
    bool isConnecting_;
};

#endif // CONTROLLER_H
