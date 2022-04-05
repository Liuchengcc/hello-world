#include "logoutform.h"
#include "ui_logoutform.h"
#include "DockManager.h"

const QString c_dockHeadName("log状态输出");
LogoutForm* LogoutForm::instance_ = nullptr;
LogoutForm *LogoutForm::getInstance()
{
    if(!instance_)
    {
        instance_ = new LogoutForm;
    }
    return instance_;
}

LogoutForm::LogoutForm(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::LogoutForm)
{
    ui->setupUi(this);
    ui->plainTextEdit_log->setReadOnly(true);
    connect(ui->pushButton_clean, &QPushButton::clicked, [this]{ui->plainTextEdit_log->clear();});
    initDock();
}
LogoutForm::~LogoutForm()
{
    delete ui;
}

void LogoutForm::initDock()
{
    area_ = ads::BottomDockWidgetArea;
    dock_ = new ads::CDockWidget(c_dockHeadName);
    dock_->setWidget(this);
    dock_->setFeatures(ads::CDockWidget::AllDockWidgetFeatures);
    connect(dock_, &ads::CDockWidget::closeRequested, [this]()
    {
        qDebug()<<"dock requested" ;
        dock_->dockManager()->removeDockWidget(dock_);
    });
}

void LogoutForm::showLogMsg(QString data)
{
    ui->plainTextEdit_log->appendPlainText(data);
}
