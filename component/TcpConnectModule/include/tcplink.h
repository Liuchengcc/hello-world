#ifndef TCPLINK_H
#define TCPLINK_H

#include<QThread>
#include<QTimer>
#include<QMutex>
#include<QQueue>
#include<QTcpSocket>
#include"bzresources.h"
#include"packethandle.h"
#include <QSharedPointer>
#include <QDateTime>

struct TCP_FREAMHEAD
{  //10 bytes
    uchar robotid;      //1 byte
    uchar cmd;          //1 byte
    uint index;         //2 bytes
    int datalen;        //4 bytes
    int type;           //2 bytes
};

typedef enum _TCP_CLIENT_HANDLE{
    CLIENT_NONE = 1,
    CLIENT_SEND_HEARTBEATS,
    CLIENT_SEND_CMD,
    CLIENT_SEND_CMD_DONE,
    CLIENT_SEND_FILE,
    CLIENT_SEND_FILE_DONE,
    CLIENT_RECV_CMD,
    CLIENT_RECV_FILE,
    CLIENT_RECV_FILE_DONE,
    CLIENT_ERROR
}TCP_CLIENT_HANDLE;

class TcpLink : public QObject
{
     Q_OBJECT
public:
    explicit TcpLink(QObject *parent = 0);
    ~TcpLink();

    static TcpLink* getInstance();
    void setIP(QString);
    void setPort(QString);
    void connectServer();
    void disconnectServer();
    void sendDataByNetWork(TCP_FREAMHEAD, QByteArray);
public slots:

signals:
    void tcpConnected();                        //发出信号-连接成功ed
    void tcpDisconnected();                     //发出信号-连接断开ed
    void recvTcpData(TCP_FREAMHEAD, QString);
    void tcpWritten(qint64 bytes);              //发出信号-写成功ed
    void updateCurrentNetworkState(QAbstractSocket::SocketState sktState);
private:
    /*********************************************************/
    static TcpLink* instance;
    QString hostIP_;
    QString hostPort_;
    bool    needResetFlag_;
    bool    isConnectFlag_;

    /*********************************************************/

    //    qintptr ptr;
    QSharedPointer<QTcpSocket> tcpClient;                  //客户端套接字
    QByteArray buffer_;
    bool isCompleteOneFrame_;

    TCP_FREAMHEAD   currentRecv_FrameHead;       //当前正在接收的数据帧的帧头
    QString         currentRecv_Str;             //当前正在接收的指令字符串
    int             bigFileLength;               //主要考虑到如果是中文，不好检测到数据长度
    TCP_FREAMHEAD   currentSend_FrameHead;       //当前正在发送的数据帧的帧头
    QByteArray     currentSend_Str;             //当前正在发送的指令字符串

    TCP_CLIENT_HANDLE clientHandle;
    /*********************************************************/

    QTimer* checkKeepAlive_Timer;
    QTimer* checkConnect_Timer;
    QDateTime* lastRecvDateTime_;
    QAbstractSocket::SocketState socketState_;
    /*********************************************************/

    bool clientSendCMD(unsigned char robotid,               //示教器发送指令
                       unsigned char cmd,
                       unsigned int index,
                       int type,
                       char *str);
    void addToRecvQueue();
    /*********************************************************/

private slots:
    void clientRead();              //接收下位机的文件与指令消息
    void displayError(QAbstractSocket::SocketError);//报错
    void tcpStateChange(QAbstractSocket::SocketState);
    void sendHeartBeat();           //心跳包检测的槽函数
    void checkConnect();
};

#endif // TCPLINK_H
