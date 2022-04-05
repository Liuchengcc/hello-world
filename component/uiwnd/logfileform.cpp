#include "logfileform.h"
#include "ui_logfileform.h"
#include "globalfunction.h"
#include "programsctrl.h"
#include "DockManager.h"
#include "qfuture.h"
#include "QtConcurrent/QtConcurrent"
#include "qsettings.h"
#include "qdatetime.h"


#define SYS_LOG_NAME "sys.lhlog"
#define SYS_LOG_PATH  "./" SYS_LOG_NAME
#define ERROR_PATH1 "./res/chs/err.ini"
#define ERROR_PATH1_ORIGIN ":/res/chs/err.ini"
#define ERROR_HEAD1 "ERRCODE"
#define ERROR_PATH2 "./res/chs/event.ini"
#define ERROR_PATH2_ORIGIN ":/res/chs/event.ini"
#define ERROR_HEAD2 "EVENT"
#define ERROR_PATH3 "./res/chs/warning.ini"
#define ERROR_PATH3_ORIGIN ":/res/chs/warning.ini"
#define ERROR_HEAD3 "WARNING"


const QString c_dockHeadName("日志记录");
logFileForm* logFileForm::instance_ = nullptr;
logFileForm *logFileForm::getInstance()
{
    if(!instance_)instance_ = new logFileForm;
    return instance_;
}

logFileForm::logFileForm(QWidget *parent):
    BaseWidget(parent),
    ui(new Ui::logFileForm)
{
    ui->setupUi(this);
    initDock();
    initErrorMap();
    ui->tableWidget_log->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_log->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_log->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_log->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_log->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_log->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    if(QFile::exists(SYS_LOG_PATH))
    {
        parserLogFile();
    }
    connect(ui->pushButton_update, &QPushButton::clicked, [this]
    {
        ui->pushButton_update->setEnabled(false);
        uploadLogFile();
        ui->pushButton_update->setEnabled(true);
    });
}

void logFileForm::initErrorMap()
{
    auto addErrorFromFile = [&](QString originPath, QString filePath, QString errorHeader)
    {
        if(!QFile::exists(filePath))
        {
            QFile::copy(originPath, filePath);
        }
        QSettings* pgconfig = new QSettings(filePath, QSettings::IniFormat);
        pgconfig->setIniCodec("GBK");
        pgconfig->beginGroup(errorHeader);
        QStringList robotErrorKeys = pgconfig->childKeys();
        foreach (QString key, robotErrorKeys){ErrorMap_[key] = pgconfig->value(key).toString();}
    };
    addErrorFromFile(ERROR_PATH1_ORIGIN, ERROR_PATH1, ERROR_HEAD1);
    addErrorFromFile(ERROR_PATH2_ORIGIN, ERROR_PATH2, ERROR_HEAD2);
    addErrorFromFile(ERROR_PATH3_ORIGIN, ERROR_PATH3, ERROR_HEAD3);
}

void logFileForm::uploadLogFile()
{
    ProgramsCtrl::getInstance()->upLoadFromControllerFileToByteArray(SYS_LOG_NAME, logData_);
    GlobalFunction::WriteByteArrayToDocFile(SYS_LOG_NAME, logData_);
    parserLogFile();
}

void logFileForm::parserLogFile()
{
    logDataList_ = QString().append(logData_).split("\n", Qt::SkipEmptyParts);
    int size = logDataList_.size();
    int originSize = ui->tableWidget_log->rowCount();
    if(originSize < size)
    {
        ui->tableWidget_log->setRowCount(size);
        int needAddRowSize = size - originSize;
        for(int i = 0; i < needAddRowSize; i++)
        {
            for(int j = 0; j < 6; j++)
            {
                ui->tableWidget_log->setItem(i + originSize, j, new QTableWidgetItem());
            }
        }
    }
    else
    {
        ui->tableWidget_log->setRowCount(size);
    }
    QMap<QString, QString> typeMap;
    typeMap["W"] = tr("警告");
    typeMap["E"] = tr("事件");
    for(int i = 0 ; i < size; i++)
    {
        QStringList tempLogDataList = logDataList_.at(size - 1 -i).split(",");
        if(tempLogDataList.size() < 3)continue;
        //类型
        QString codeType;
        QString trueLogCode;
        if(tempLogDataList.at(2).left(1) == "E")
        {
            codeType = tr("事件");
            trueLogCode = tempLogDataList.at(2);
        }
        else if(tempLogDataList.at(2).left(1) == "W")
        {
            codeType = tr("警告");
            trueLogCode =tr("w0x") + tr("%1").arg(tempLogDataList.at(2).mid(1).toInt(), 4, 16, QLatin1Char('0')).toUpper();
        }
        else
        {
            codeType = tr("错误");
            trueLogCode =tr("e0x") + tr("%1").arg(tempLogDataList.at(2).toInt(), 4, 16, QLatin1Char('0')).toUpper();
        }
        ui->tableWidget_log->item(i, 0)->setText(tempLogDataList.at(0) + " "+tempLogDataList.at(1));
        ui->tableWidget_log->item(i, 1)->setText(codeType);
        ui->tableWidget_log->item(i, 2)->setText(trueLogCode);//
        ui->tableWidget_log->item(i, 3)->setText(ErrorMap_[trueLogCode]);//
        ui->tableWidget_log->item(i, 4)->setText(tempLogDataList.at(3));//时间
        ui->tableWidget_log->item(i, 5)->setText(tempLogDataList.at(4));//时间
    }
}

void logFileForm::initDock()
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

logFileForm::~logFileForm()
{
    delete ui;
}
