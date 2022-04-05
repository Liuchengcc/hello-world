#ifndef FTPMANAGER_H
#define FTPMANAGER_H

#include <QObject>
#include "qnetworkaccessmanager.h"
#include <qftp.h>
#include <qeventloop.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <bzresources.h>
#include <qmutex.h>

class FtpManager : public QObject
{
    Q_OBJECT
public:
    explicit FtpManager(QObject *parent = 0);
    static FtpManager* getInstance();
    void setFtpHostIPAndPort(QString, QString);
    QByteArray getFile(QString);
    QStringList getFileList();
    bool checkAndConnect();
    int putFile(QByteArray, QString);
    int delFile(QString);
    void ftpLogin(QString ip = DEFAULT_RC_IP, QString port = "21", QString name = "admin", QString password = "robot@123.com");
    void tcpDisconnected();
    ~FtpManager();

signals:

public slots:
    void cmdFinished(int,bool);
    void getFileList(QUrlInfo);
    void readFile();
    void ftpStateChanged(int);
    void resetFtp();
    void closeFtp();
private:
    static FtpManager* instance_;
    QEventLoop* eventLoop_;
    QFtp *ftp_;
    QByteArray tempBuf_;
    QStringList fileNameList_;
    QString IP_;
    QString port_;
    QTimer * closeTimer_;
    int PutID;
    int GetID;
    QString ErrFIleName;
    bool isConnected_;

};

#endif // FTPMANAGER_H
