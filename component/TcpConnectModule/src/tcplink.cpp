#include "tcplink.h"
#include<QDebug>
#include <QDateTime>
#include<QHostAddress>
#include<QMutexLocker>
#include<qdatastream.h>
#include<globalfunction.h>
#include<QTextCodec>
#include <QNetworkProxy>

const int HeartBeat_Interval    = 200; //心跳包的间隔时间:1s
TcpLink* TcpLink::instance = NULL;
TcpLink* TcpLink::getInstance()
{
    if(instance == NULL)
    {
        instance = new TcpLink;
    }
    return instance;
}

TcpLink::TcpLink(QObject *parent) :
    QObject(parent),
    hostIP_(DEFAULT_RC_IP),
    hostPort_("5001"),
    needResetFlag_(true),
    isConnectFlag_ (false),
    socketState_(QAbstractSocket::UnconnectedState)
{

    lastRecvDateTime_ = new QDateTime(QDateTime::currentDateTime());
    currentRecv_Str.clear();

    buffer_.clear();
    isCompleteOneFrame_ = true;

    checkKeepAlive_Timer = new QTimer();                        //心跳包检测
    checkKeepAlive_Timer->setInterval(HeartBeat_Interval);      //间隔时间4000ms
    connect(checkKeepAlive_Timer, SIGNAL(timeout()), this, SLOT(sendHeartBeat()));

    checkConnect_Timer = new QTimer();
    checkConnect_Timer->setInterval(HeartBeat_Interval*5);
    connect(checkConnect_Timer, SIGNAL(timeout()), this, SLOT(checkConnect()));

    tcpClient = QSharedPointer<QTcpSocket>(new QTcpSocket);           //套接字
    connect(tcpClient.data(), SIGNAL(readyRead()), this, SLOT(clientRead()));              //接收下位机的文件与指令消息
    connect(tcpClient.data(), SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(tcpStateChange(QAbstractSocket::SocketState)));
    connect(tcpClient.data(), SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));               //报错
}

TcpLink::~TcpLink()
{
}

void TcpLink::clientRead()
{    
    *lastRecvDateTime_ = QDateTime::currentDateTime();
    if(tcpClient->bytesAvailable() <= 0)
    {
        return;
    }

    buffer_ += tcpClient->readAll();
    if(buffer_.size() < 10)
    {
        if(isCompleteOneFrame_)
        {
            buffer_.clear();
            qDebug()<<"----------tcp socket data error!";
            return;
        }
        else
        {
            currentRecv_Str += buffer_;
            if(currentRecv_FrameHead.datalen == currentRecv_Str.length() + 10)
            {
                buffer_.clear();
                isCompleteOneFrame_ = true;
                addToRecvQueue();
            }
            else
            {
                qDebug()<<"----------tcp data error! file length is: "<<currentRecv_Str.length()<<"receive length is: "<<currentRecv_FrameHead.datalen;
                buffer_.clear();
            }
            return;
        }
    }
    while(buffer_.size() > 10)  //大文件非第一包
    {
        if(!isCompleteOneFrame_)
        {
            if((buffer_.length() + bigFileLength) >= currentRecv_FrameHead.datalen - 10)
            {
                int lastLength = currentRecv_FrameHead.datalen - bigFileLength - 10;
                currentRecv_Str += buffer_.mid(0, lastLength);
                buffer_.remove(0, lastLength);
                QTextCodec* tcd = QTextCodec::codecForName("UTF8");
                currentRecv_Str = tcd->toUnicode(currentRecv_Str.toUtf8());

                isCompleteOneFrame_ = true;
                addToRecvQueue();
                bigFileLength = 0;
                continue;
            }   //大文件非第一包，也没接收完结
            else if((buffer_.length() + bigFileLength) < currentRecv_FrameHead.datalen - 10)
            {
#if DEBUG_MODE
                qDebug()<<__FILE__<<__LINE__;
                qDebug()<<"--------------收到大文件的中间部分----------------";
                qDebug()<<"buffer_.size() = "<< buffer_.size();
                qDebug()<<"bigFileLength = "<<bigFileLength;
                qDebug()<<"currentRecv_FranmeHead.datalen = "<<currentRecv_FrameHead.datalen;
                qDebug()<<"buffer_.data() = "<<QString(buffer_);// buffer_.data();
                qDebug()<<"-----------------------------------------------";
#endif
                currentRecv_Str += buffer_;
                bigFileLength += buffer_.size();
                buffer_.clear();
                continue;
            }
        }
        else    //接收新的帧
        {
            if(buffer_.at(0) != 0 ||  //robotid = 0;  cmd = 0/1/2/3
                    buffer_.at(1) > 3)
            {
                buffer_.clear();
                qDebug()<<"----------tcp socket headData error!";
                return;
            }
            else
            {
                isCompleteOneFrame_ = false;
                currentRecv_FrameHead.robotid   =   buffer_[0];                    //机器人编码-1个字节
                currentRecv_FrameHead.cmd       =   buffer_[1];                    //指令码-1个字
                currentRecv_FrameHead.index     =  buffer_[2]<<8;                 //地址码-2个字节
                currentRecv_FrameHead.index     |=  (uchar) buffer_[3];
                currentRecv_FrameHead.datalen   =   (buffer_[4]<<24)&0xff000000;   //长度-4个字节
                currentRecv_FrameHead.datalen   +=  (buffer_[5] << 16)&0xff0000;
                currentRecv_FrameHead.datalen   +=  (buffer_[6] << 8)&0xff00;
                currentRecv_FrameHead.datalen   +=  buffer_[7]&0xff;
                currentRecv_FrameHead.type      =   buffer_[8]<<8;                 //类型--2个字节-暂未使用
                currentRecv_FrameHead.type      |=  buffer_[9];

                if(buffer_.size() == currentRecv_FrameHead.datalen)//普通通讯数据帧
                {
                    currentRecv_Str = buffer_.remove(0, 10);
                    buffer_.clear();
                    isCompleteOneFrame_ = true;
                    QTextCodec* tcd = QTextCodec::codecForName("UTF8");
                    currentRecv_Str = tcd->toUnicode(currentRecv_Str.toUtf8());
                    addToRecvQueue();
                    continue;
                }
                else if(buffer_.size() < currentRecv_FrameHead.datalen) //large file
                {
#if DEBUG_MODE
                    qDebug()<<__FILE__<<__LINE__;
                    qDebug()<<"--------------开始收到大文件----------------";
                    qDebug()<<"buffer_.size() = "<< buffer_.size();
                    qDebug()<<"bigFileLength = "<< bigFileLength;
                    qDebug()<<"currentRecv_FranmeHead.datalen = "<<currentRecv_FrameHead.datalen;
                    qDebug()<<"buffer_.data() = "<<buffer_.mid(10, -1).data();
                    qDebug()<<"------------------------------------------";
#endif
//                    currentRecv_Str = buffer_.remove(0, 10);
                    currentRecv_Str = buffer_.mid(10, -1).data();
                    bigFileLength = buffer_.size() - 10;
                    buffer_.clear();
                    isCompleteOneFrame_ = false;
                    continue;
                }
                else if(buffer_.size() > currentRecv_FrameHead.datalen) //more than one frame
                {
                    currentRecv_Str = buffer_.remove(0, 10).mid(0, currentRecv_FrameHead.datalen - 10);
                    isCompleteOneFrame_ = true;
                    buffer_.remove(0, currentRecv_FrameHead.datalen - 10);
                    addToRecvQueue();
                    continue;
                }
            }
        }
    }
}
void TcpLink::addToRecvQueue()
{
    emit recvTcpData(currentRecv_FrameHead, currentRecv_Str);
}

/************************************************
 *              示教器发送指令给控制器              *
 * **********************************************/
//frame:id-cmd-index-length-type-saved-data
//bytes:1--1---2-----4-----1------1-----N
//total bytes:10+N bytes
bool TcpLink::clientSendCMD(unsigned char robotid,
                                 unsigned char cmd,
                                 unsigned int index,
                                 int type,
                                 char *str)    //客户端-发送指令字符串
{
    int dataLen = strlen(str) + 1 + FRAME_HEAD_LENGTH;
#if DEBUG_MODE
    qDebug()<<("<--")
            <<QString::number(robotid)
            <<QString::number(cmd)
            <<QString::number(index)
            <<QString::number(dataLen)
            <<QString::number(type)
            <<QString(str);
#endif
    QByteArray outBlock;                //数据缓冲区,即存放每次要发送的数据
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);

    sendOut <<quint8((robotid) & 0xFF)
            <<quint8((cmd) & 0xFF)
            <<quint8(((index) & 0xFF00)>>8) <<quint8((index)&0x00FF)
            <<quint8(((dataLen)>>24)&0xFF)  <<quint8(((dataLen)>>16)&0xFF)
            <<quint8(((dataLen)>>8)&0xFF)   <<quint8((dataLen)&0xFF)
            <<quint8(((type)&0xFF00)>>8)    <<quint8((type)&0x00FF);
    sendOut.writeRawData(str, strlen(str)+1);

    qint64 num = tcpClient->write(outBlock);
#if DEBUG_MODE
    qDebug()<<tr("------")+QString::number(num);
#endif
    if(num < 0){
        clientHandle = CLIENT_ERROR;
        return false;
    } else{
        outBlock.resize(0);//清空发送缓冲区
        clientHandle = CLIENT_SEND_CMD_DONE;
        return true;
    }
}

void TcpLink::displayError(QAbstractSocket::SocketError)
{
    qDebug()<<"displayError:"<<tcpClient->errorString();
}

void TcpLink::setIP(QString IP)
{
    hostIP_ = IP;
}

void TcpLink::setPort(QString port)
{
    hostPort_ = port;
}

void TcpLink::connectServer()
{
    isConnectFlag_ = true;
    checkKeepAlive_Timer->start();
    checkConnect_Timer->start();
    checkConnect();
}

void TcpLink::disconnectServer()
{
    isConnectFlag_ = false;
    checkKeepAlive_Timer->stop();
    checkConnect_Timer->stop();
    checkConnect();
}

void TcpLink::sendHeartBeat()               //示教器发送心跳包给下位机
{
    clientHandle = CLIENT_SEND_HEARTBEATS;
    clientSendCMD(ROBOT_ID, FUNC_SYS, SYS_HEARTBEATS, TYPE, (char*)("0,"));//helloheart
}

void TcpLink::tcpStateChange(QAbstractSocket::SocketState sktState)
{
    socketState_ = sktState;
    emit updateCurrentNetworkState(socketState_);
//#if DEBUG_MODE
    qDebug()<<"tcpStateChange:"<<sktState;
//#endif
    switch(sktState)
    {
    case QAbstractSocket::ConnectedState:
        needResetFlag_ = false;
        isCompleteOneFrame_ = true;
        emit tcpConnected();
        break;
    case QAbstractSocket::UnconnectedState:
    case QAbstractSocket::ClosingState:
        needResetFlag_ = true;
        emit tcpDisconnected();
        break;
    default:
        break;
    }
}

void TcpLink::checkConnect()
{
    if(!isConnectFlag_)
    {
#if DEBUG_MODE
        qDebug()<<tr("clientClose()---thread id is:")<<QThread::currentThreadId();
#endif
        tcpClient.data()->disconnect();
        tcpClient.data()->close();
        tcpStateChange(QAbstractSocket::UnconnectedState);
        return;
        /*********************************************************/
    }
    double num =  QDateTime::currentDateTime().toTime_t() - lastRecvDateTime_->toTime_t();
#if DEBUG_MODE
    qDebug()<<"lastRecvDateTime is:"<< lastRecvDateTime_->toTime_t()
           <<"currentDateTime is:"<<QDateTime::currentDateTime().toTime_t()
          <<"between num is:"<<num;
#endif
    if(num > 4)
    {
        needResetFlag_ = true;
    }
    if(needResetFlag_)
    {
        tcpClient = QSharedPointer<QTcpSocket>( new QTcpSocket );
        connect(tcpClient.data(), SIGNAL(readyRead()), this, SLOT(clientRead()), Qt::DirectConnection);              //接收下位机的文件与指令消息
        connect(tcpClient.data(), SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(displayError(QAbstractSocket::SocketError)), Qt::DirectConnection);               //报错
        connect(tcpClient.data(), SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(tcpStateChange(QAbstractSocket::SocketState)), Qt::DirectConnection);
        tcpClient->setProxy(QNetworkProxy::NoProxy);
        tcpClient->connectToHost(QHostAddress(hostIP_), hostPort_.toInt());//连接到指定主机的指定端口
    }
}

void TcpLink::sendDataByNetWork(TCP_FREAMHEAD header, QByteArray str)
{
    clientSendCMD(header.robotid, header.cmd, header.index, header.type, str.data());
}

