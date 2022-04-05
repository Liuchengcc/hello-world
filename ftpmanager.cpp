#include "ftpmanager.h"
#include "qurl.h"
#include <QEventLoop>
#include <QNetworkReply>
#include <qdir.h>
#include <qftp.h>
#include <QDebug>
#include <globalfunction.h>
#include <QMutexLocker>
#define CLOSE_TIMEOUT 3000


FtpManager* FtpManager::instance_ = nullptr;
FtpManager *FtpManager::getInstance()
{
    if(!instance_)
    {
        instance_ = new FtpManager;
    }
    return instance_;
}

void FtpManager::setFtpHostIPAndPort(QString ip, QString port)
{
    IP_ = ip;
    port_ = port;
}

 QByteArray FtpManager::getFile(QString fileName)
{
     GlobalFunction::showLogMsg(tr("上传文件:%1").arg(fileName));
    ErrFIleName = fileName;
    closeTimer_->stop();
    tempBuf_.clear();
    qDebug()<< fileName;
    if(checkAndConnect())
    {
        qDebug()<< "eventloop exec";
        GetID = ftp_->get(fileName);
        eventLoop_->exec();
        qDebug()<< "eventloop quit";
        if(!isConnected_)
        {
            GlobalFunction::showLogMsg(tr("网络错误：控制器断开连接"));
        }
    }
    closeTimer_->start();
    return tempBuf_;
 }

 QStringList FtpManager::getFileList()
  {
     closeTimer_->stop();
     if(checkAndConnect())
     {
         QStringList fileNameList = fileNameList_;
         fileNameList_.clear();
         ftp_->list("/");
         eventLoop_->exec();
         //qDebug()<<fileNameList_;
         if(!isConnected_)
         {
             fileNameList_ = fileNameList;
           GlobalFunction::showLogMsg(tr("网络错误：控制器断开连接"));
         }
     }
     closeTimer_->start();
     return fileNameList_;
 }

 bool FtpManager::checkAndConnect()
 {

     qDebug() << "ftp_ state:"<<ftp_->state();
     if(ftp_->state() != QFtp::LoggedIn)
     {
         ftpLogin(IP_, port_);
         for(int i = 0; i < 20; i++)
         {
             if(ftp_->state() == QFtp::LoggedIn)
             {
                 break;
             }
             GlobalFunction::Delayms(100);
         }
         if(ftp_->state() != QFtp::LoggedIn)
         {
             qDebug() << "ftp_ error:"<<ftp_->error();
             qDebug() << "ftp_ state:"<<ftp_->state();
             if(ftp_->error() == QFtp::ConnectionRefused)
             {
                 GlobalFunction::showLogMsg(tr("网络错误：控制器拒绝连接"));
             }
             else
             {
                 GlobalFunction::showLogMsg(tr("网络错误：控制器连接失败"));
             }
             return false;
         }
     }
     return true;
 }

int FtpManager::putFile(QByteArray data, QString fileName)
{
    GlobalFunction::showLogMsg(tr("下载文件:%1").arg(fileName));
    if(data.size() < 10)
    {
        GlobalFunction::showLogMsg(tr("文件%1为空，请勿下载！").arg(fileName));
        return -1;
    }

    closeTimer_->stop();
    if(checkAndConnect())
    {
        PutID = ftp_->put(data, fileName);
        ErrFIleName = fileName;
        eventLoop_->exec();
        if(!isConnected_)
        {
            GlobalFunction::showLogMsg(tr("网络错误：控制器断开连接"));
        }
    }
    closeTimer_->start();
    return 0;
}

int FtpManager::delFile(QString fileName)
{
    checkAndConnect();
    ftp_->remove(fileName);
    eventLoop_->exec();
    closeTimer_->start();
    return 0;

}

void FtpManager::ftpLogin(QString ip, QString port, QString name, QString password)
{
    int PORT = port.toInt();
    ftp_->connectToHost(ip, PORT);
    ftp_->login(name, password);
}

FtpManager::FtpManager(QObject *parent) : QObject(parent)
{
    ftp_ = new QFtp(this);
    isConnected_ = true;
    eventLoop_ = new QEventLoop;
    closeTimer_ = new QTimer();
    closeTimer_->setSingleShot(true);
    closeTimer_->setInterval(CLOSE_TIMEOUT);
    connect(closeTimer_, SIGNAL(timeout()), this, SLOT(resetFtp()));
    connect(ftp_, SIGNAL(commandFinished(int,bool)), this, SLOT(cmdFinished(int, bool)));
    connect(ftp_, SIGNAL(listInfo(QUrlInfo)), this, SLOT(getFileList(QUrlInfo)));
    connect(ftp_, SIGNAL(readyRead()), this, SLOT(readFile()));
    connect(ftp_, SIGNAL(stateChanged(int)), this, SLOT(ftpStateChanged(int)));
}


FtpManager::~FtpManager()
{

}

void FtpManager::cmdFinished(int cmd, bool error)
{
    Q_UNUSED(cmd)
   qDebug()<< "current cmd Finished:" <<ftp_->currentCommand() << " " << error;
   if(error)
   {
       qDebug()<<ftp_->errorString();
       GlobalFunction::showLogMsg(tr("ftp error: %1").arg(ftp_->errorString()));
   }
   if(ftp_->currentCommand() == QFtp::Get||
           ftp_->currentCommand() == QFtp::Put||
           ftp_->currentCommand() == QFtp::List ||
           ftp_->currentCommand() == QFtp::Remove)
   {
       eventLoop_->quit();
   }
}

void FtpManager::getFileList(QUrlInfo info)
{
    fileNameList_.append(info.name());
//    qDebug()<<info.name();
}

void FtpManager::readFile()
{
    tempBuf_ .append( ftp_->readAll());
}

void FtpManager::ftpStateChanged(int state)
{
    if(state == QFtp::LoggedIn)
    {
        isConnected_ = true;
    }
    qDebug()<< "ftp state changed:"<<state;
}

void FtpManager::resetFtp()
{
    if(ftp_->error() != 0 )
    {
        return;
    }
    ftp_->close();
    closeTimer_->stop();
    qDebug() << "close ftp_";
    delete ftp_;
//    ftp_->deleteLater();
    ftp_ = new QFtp;
    eventLoop_->quit();
    delete eventLoop_;
    eventLoop_ = new QEventLoop;
    connect(ftp_, SIGNAL(commandFinished(int,bool)), this, SLOT(cmdFinished(int, bool)));
    connect(ftp_, SIGNAL(listInfo(QUrlInfo)), this, SLOT(getFileList(QUrlInfo)));
    connect(ftp_, SIGNAL(readyRead()), this, SLOT(readFile()));
    connect(ftp_, SIGNAL(stateChanged(int)), this, SLOT(ftpStateChanged(int)));
}

void FtpManager::closeFtp()
{
    //GlobalFunction::showLogMsg(tr("示教器正在重置文件链接！"));
    ftp_->close();
    qDebug() << "close ftp_";
    delete ftp_;
//    ftp_->deleteLater();
    ftp_ = new QFtp;
    eventLoop_->quit();
    delete eventLoop_;
    eventLoop_ = new QEventLoop;
    connect(ftp_, SIGNAL(commandFinished(int,bool)), this, SLOT(cmdFinished(int, bool)));
    connect(ftp_, SIGNAL(listInfo(QUrlInfo)), this, SLOT(getFileList(QUrlInfo)));
    connect(ftp_, SIGNAL(readyRead()), this, SLOT(readFile()));
    connect(ftp_, SIGNAL(stateChanged(int)), this, SLOT(ftpStateChanged(int)));
}

void FtpManager::tcpDisconnected()
{
    isConnected_ = false;
    if(eventLoop_->isRunning())
    {
        eventLoop_->quit();
    }
}
