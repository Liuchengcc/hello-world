#include "taskmonitor.h"
#include "ui_taskmonitor.h"
#include "marcroly.h"
#include "DockManager.h"
#include "controller.h"

const QString c_dockHeadName("任务监控");

#define AXIS_SUM 32
#define COLUMN_HEADER "任务ID 任务状态 任务文件"
INSTANCE_MODE(TaskMonitor, instance_)
TaskMonitor::TaskMonitor(QWidget *parent) :
    BaseWidget(parent),
    ui(new Ui::TaskMonitor)
{
    ui->setupUi(this);
    initDock();
    initUi();
}

TaskMonitor::~TaskMonitor()
{
    delete ui;
}

void TaskMonitor::initDock()
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

void TaskMonitor::initUi()
{
    ui->tableWidget_TaskMonitor->verticalHeader()->setHidden(true);
    ui->tableWidget_TaskMonitor->setRowCount(AXIS_SUM);
    ui->tableWidget_TaskMonitor->setColumnCount(tr(COLUMN_HEADER).split(" ").size());
    ui->tableWidget_TaskMonitor->setHorizontalHeaderLabels(tr(COLUMN_HEADER).split(" "));

}
